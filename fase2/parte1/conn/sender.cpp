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
	int result;

	while(true){
	 	my_msgq.receive(&buf, 0);
		if((result = sock.Write((const char *) &buf, sizeof(struct my_msgbuf))) == -1){
			printf("oh no\n");
		}
	}
}
