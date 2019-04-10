#include "message.h"
#include "error_handler.h"
#include "socket.h"

#include <thread>
#include <cstdio>
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
	emisor Pasa los mensajes que vienen de los contratistas al receptor
	argv[1] IP del receptor
	argv[2] Número de puerto del receptor
**/
int main(int argc, char * argv[]){
	Message in_queue; //Este es el queue en el que los contratistas van a enviar sus mensajes
	Message out_queue(0xA0000); //Este queue es para comunicar los mensajes que vienen del receptor a los contratistas
	struct my_msgbuf buf; //Aquí vamos a recibir los paquetes de los contratistas.

	Socket sock;
	sock.Connect(argv[1], atoi(argv[2]));
	printf("success\n");
	std::thread(pass_final_status, &out_queue, &sock).detach();
	while(true){
	 	in_queue.receive(&buf, 0); //Mientras no haya un mensaje en el in_queue el thread se va a bloquear aquí.
		if(sock.Write((const char *) &buf, sizeof(struct my_msgbuf)) == -1){
			perror("ERROR WRITING ON SOCKET\n");
		}
	}
}
