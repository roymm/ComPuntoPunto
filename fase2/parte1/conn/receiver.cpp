#include "socket.h"
#include "error_handler.h"
#include "message.h"
#include "semaphore.h"

#include <cstdio>
#include <cstdlib>
#include <thread>

#define DEFPORT 2909

/**
	reconstruct	 Reconstruye un archivo nuevo con las piezas que envió el emisor.
	in_queue	En este queue vienen las piezas que van llegando del emisor
	socket_sem	Semáforo que sincroniza las escrituras al socket entre los hilos
	conn_sock		Socket conectada al emisor
	type				El identificador del mensaje, con esto es que se multiplexan los paquetes para que cada thread tome las piezas de un archivo particular.
**/
void reconstruct(Message * in_queue, Semaphore * socket_sem, Socket * conn_sock, long type){
	FILE * fd;
	bool finished = false;
	struct my_msgbuf buf;
	while(!finished){
		in_queue->receive(&buf, type);
		if(buf.piece_number == 0){
			if((fd = fopen(buf.mtext, "wb")) == NULL){
				error_exit(errno, "Error creating file");
			}
		} else if(buf.bytes == 0) {
				fclose(fd);
				finished = true;
				socket_sem->wait();
				if(conn_sock->Write((const char *) &buf, sizeof(struct my_msgbuf)) == -1){ //no importa leer y escribir al mismo tiempo 
					perror("ERROR WRITING ON SOCKET!\n");																							//en el mismo socket porque TCP es full-duplex (vs half-duplex o simplex)
				}
				socket_sem->signal();
		} else {
			fwrite((void *) buf.mtext, 1, buf.bytes, fd);
		}
	}
}
/**
 * Gets the arguments from argv, after making some basic error checking
 * @param argc How many arguments are in argv
 * @param argv The array where the arguments are stored
 */
long get_args(int argc, char ** argv){
	long port;
	if(argc == 1){
		port = DEFPORT;
	} else {
		if((port = strtol(argv[1], NULL, 0)) == 0){ // Si el argumento no es un número válido el programa se cae aquí
			perror("Invalid argument!\n");
			printf("Format: %s <port>\n", argv[0]);
			exit(EXIT_FAILURE);	
		}
	}
	return port;
}

/**
	receiver	Recibe los mensajes del emisor y multiplexa en distintos threads para que los archivos originales sean reensamblados
	argv[1]		Puerto en el que se va a escuchar
**/
int main(int argc, char * argv[]){
	long port = get_args(argc, argv);
	Message in_queue(KEY_SEB); 							//Queue en el que se van a poner las piezas que van llegando del emisor
	Semaphore socket_sem(KEY_ROY, 1); 			//Semáforo que va a sincronizar las escrituras al socket entre los hilos
	Socket sock; 														//Socket que va a aceptar conecciones
	struct my_msgbuf * received_packet;	 		//Estructura donde vamos a guardar lo que nos envía el emisor
	char buffer[sizeof(struct my_msgbuf)]; 	//Buffer en el que se recibirán los bytes del socket
	Socket * conn_sock; 										//Socket que va a estar conectada al emisor

	sock.Bind(port); 												//Si el puerto no es válido se cae aquí adentro
	sock.Listen();
	conn_sock = sock.Accept();

	printf("success\n");
	while(true){
		if(conn_sock->Read((char *) buffer, sizeof(struct my_msgbuf)) == -1){
			printf("ERROR READING ON SOCKET!\n");
		}
		received_packet = (struct my_msgbuf *) &buffer;
		if(received_packet->piece_number == 0){
			std::thread(reconstruct, &in_queue, &socket_sem, conn_sock, received_packet->mtype).detach();
		}
		in_queue.send(received_packet);
	}
}
