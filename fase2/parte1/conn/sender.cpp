#include "message.h"
#include "error_handler.h"
#include "socket.h"

#include <thread>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <errno.h>

/**
	pass_final_status Pasa el mensaje que viene del receptor al contratista
	out_queue	Cola de mensajes que vienen del receptor al contratista
	conn_sock Socket contectada con el receptor
**/
void pass_final_status(Message * out_queue, Socket * conn_sock){
	char buffer[sizeof(struct my_msgbuf)];
	struct my_msgbuf * received_packet;	
	int result; 
	while(true){
		if((result = conn_sock->Read((char *) buffer, sizeof(struct my_msgbuf))) == -1){ //Mientras no haya un mensaje del emisor el thread se va a bloquear aquí.
			perror("ERROR READING FROM SOCKET!\n");
		}
		received_packet = (struct my_msgbuf *) buffer;
		out_queue->send(received_packet);
	}
}

/**
 * Gets the arguments from argv, after making some basic error checking
 * @param ip Where the IP argument will be stored
 * @param port Where the port argument will be stored
 * @param argc How many arguments are in argv
 * @param argv The array where the arguments are stored
 */
void get_args(char * ip, long &port, int argc, char ** argv){
	switch(argc){
		case 1:
		case 2:
			perror("You need more arguments!\n");
			printf("Format: %s <IP address> <port>\n", argv[0]);
			exit(EXIT_FAILURE);
		default:
			strcpy(ip, argv[1]);	
			if((port = strtol(argv[2], NULL, 0)) == 0){ // Si el argumento no es un número válido el programa se cae aquí
				perror("Invalid argument!\n");
				printf("Format: %s <IP address> <port>\n", argv[0]);
				exit(EXIT_FAILURE);
			}
	}
}

/**
	emisor Pasa los mensajes que vienen de los contratistas al receptor
	argv[1] IP del receptor
	argv[2] Número de puerto del receptor
**/
int main(int argc, char * argv[]){
	char ip[15];
	long port;
	get_args(ip, port, argc, argv);
	Message in_queue; //Este es el queue en el que los contratistas van a enviar sus mensajes
	Message out_queue(0xA0000); //Este queue es para comunicar los mensajes que vienen del receptor a los contratistas
	struct my_msgbuf buf; //Aquí vamos a recibir los paquetes de los contratistas.

	Socket sock;
	sock.Connect(ip, port);
	printf("success\n");
	std::thread(pass_final_status, &out_queue, &sock).detach();
	while(true){
	 	in_queue.receive(&buf, 0); //Mientras no haya un mensaje en el in_queue el thread se va a bloquear aquí.
		if(sock.Write((const char *) &buf, sizeof(struct my_msgbuf)) == -1){
			perror("ERROR WRITING ON SOCKET\n");
		}
	}
}
