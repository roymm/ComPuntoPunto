#include "message.h"
#include "error_handler.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <unistd.h>
#include <cstdio>

Message::Message(key_t key){
	pid = getpid();	
	id = msgget(key, IPC_CREAT|0600);
	bufsize = sizeof(my_msgbuf) - sizeof(long);
	if(-1 == id){
		error_exit(errno, "Error creating message queue\n");
	}
}

Message::~Message(){
/*
	if(getpid() == pid){
		int st = msgctl(id, IPC_RMID, nullptr);
		if(-1 == st){
			error_exit(errno, "Error deleting message queue\n");
		}
	}
*/
}

void Message::send(struct my_msgbuf * buf){
	int st = msgsnd(id, (const void *) buf, bufsize, 0);
	if(-1 == st){
		error_exit(errno, "Error sending message\n");
	}
}

void Message::receive(struct my_msgbuf * buf, long type){
	int st = msgrcv(id, (void *) buf, bufsize, type, 0);
	if(-1 == st){
		error_exit(errno, "Error recieving message\n");
	}
}
