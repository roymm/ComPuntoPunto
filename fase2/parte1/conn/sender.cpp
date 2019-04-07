#include "message.h"
#include "shared.h"
#include "semaphore.h"
#include "error_handler.h"
#include "socket.h"

#include <map>
#include <cstdio>
#include <errno.h>

int main(int argc, char * argv[]){
	Message my_msgq;
	SharedMem shared_mem(DEFAULT_SIZE, KEY_SEB);
	Semaphore messages_sem(KEY_CRIS, 0);
	long* sender_messages = (long*) shared_mem.attach();
	struct my_msgbuf buf;

	Socket sock;
	sock.Connect(argv[1], atoi(argv[2]));
	printf("success\n");
	

	while(true){
	 	my_msgq.receive(&buf, 0);
		printf("Received!\n");
		/*
		if(buf.piece_number != 0 && buf.bytes == 0){
			messages_sem.wait();
			sender_messages[0] = buf.mtype;
			messages_sem.signal();
		}
		*/
		sock.Write((const char *) &buf, sizeof(struct my_msgbuf));
		printf("Wrote!\n");
	}
}
