#include "socket.h"
#include "error_handler.h"
#include "message.h"

#include <cstdio>
#include <thread>

void reconstruct(Message * in_queue, Message * out_queue, long type){
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
				out_queue->send(&buf);
				finished = true;
		} else {
			fwrite((void *) buf.mtext, 1, buf.bytes, fd);
		}
	}
}

void send_final_status(Message * out_queue, Socket * conn_sock){
	struct my_msgbuf final_status;
	while(true){
		out_queue->receive(&final_status);
		if(conn_sock->Write((const char *) &final_status, sizeof(struct my_msgbuf)) == -1){ //no importa leer y escribir al mismo tiempo porque TCP es full duplex
			perror("ERROR WRITING ON SOCKET!\n");
		}
	}
}

int main(int argc, char * argv[]){
	Message in_queue(KEY_SEB);
	Message out_queue(KEY_ROY);
	Socket sock;
	struct my_msgbuf * received_packet;	
	char buffer[sizeof(struct my_msgbuf)];
	Socket * conn_sock;

	sock.Bind(atoi(argv[1]));
	sock.Listen();
	conn_sock = sock.Accept();

	printf("success\n");
	std::thread(send_final_status, &out_queue, conn_sock).detach();
	while(true){
		if(conn_sock->Read((char *) buffer, sizeof(struct my_msgbuf)) == -1){
			printf("ERROR READING ON SOCKET!\n");
		}
		received_packet = (struct my_msgbuf *) &buffer;
		if(received_packet->piece_number == 0){
			std::thread(reconstruct, &in_queue, &out_queue, received_packet->mtype).detach();
		}
		in_queue.send(received_packet);
	}
}
