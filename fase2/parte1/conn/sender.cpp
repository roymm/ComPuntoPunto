#include "message.h"
#include "error_handler.h"
#include "socket.h"

#include <map>
#include <thread>
#include <cstdio>
#include <errno.h>

void pass_final_status(Message * out_queue, Socket * conn_sock){
	char buffer[sizeof(struct my_msgbuf)];
	struct my_msgbuf * received_packet;	
	int result; 
	while(true){
		if((result = conn_sock->Read((char *) buffer, sizeof(struct my_msgbuf))) == -1){
			perror("ERROR READING FROM SOCKET!\n");
		}
		received_packet = (struct my_msgbuf *) buffer;
		out_queue->send(received_packet);
	}
}

int main(int argc, char * argv[]){
	Message in_queue;
	Message out_queue(0xA0000);
	struct my_msgbuf buf;

	Socket sock;
	sock.Connect(argv[1], atoi(argv[2]));
	printf("success\n");
	std::thread(pass_final_status, &out_queue, &sock).detach();
	while(true){
	 	in_queue.receive(&buf, 0);
		if(sock.Write((const char *) &buf, sizeof(struct my_msgbuf)) == -1){
			perror("ERROR WRITING ON SOCKET\n");
		}
	}
}
