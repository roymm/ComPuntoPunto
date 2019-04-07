#include "socket.h"
#include "message.h"
#include "shared.h"
#include "semaphore.h"
#include "error_handler.h"

#include <cstdio>
#include <map>

int main(int argc, char * argv[]){
	std::map<int, FILE *> rel11;
	Socket sock;
	struct my_msgbuf * received_packet;	
	char buffer[sizeof(struct my_msgbuf)];
	Socket * conn_sock;
	sock.Bind(atoi(argv[1]));
	sock.Listen();
	conn_sock = sock.Accept();
	printf("success\n");
	while(true){
		if(conn_sock->Read((char *) buffer, sizeof(struct my_msgbuf)) == -1){
			printf("oh no\n");
		}
		printf("Read!\n");
		received_packet = (struct my_msgbuf *) &buffer;
		if(received_packet->piece_number == 0){
			if((rel11[received_packet->mtype] = fopen(received_packet->mtext, "wb")) == NULL){
				error_exit(errno, "Error creating file");
			}
			printf("Wrote!\n");
		} else {
			if(received_packet->bytes == 0){
				fclose(rel11[received_packet->mtype]);
				rel11.erase(received_packet->mtype);
			} else {
				fwrite((const void *) received_packet->mtext, 1, received_packet->bytes, rel11[received_packet->mtype]);
				printf("Wrote!\n");
			}
		}
	}
}
