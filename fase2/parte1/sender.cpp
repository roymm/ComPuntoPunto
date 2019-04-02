#include "message.h"
#include "shared.h"
#include "semaphore.h"
#include "error_handler.h"

#include <map>
#include <cstdio>
#include <errno.h>

int main(){
	std::map<int, FILE *> rel11;
	Message my_msgq;
	SharedMem shared_mem(DEFAULT_SIZE, KEY_SEB);
	Semaphore messages_sem(KEY_CRIS, 0);
	long* sender_messages = (long*) shared_mem.attach();
	struct my_msgbuf buf;
	while(true){
	 	my_msgq.receive(&buf, 0);
		if(buf.piece_number == 0){
			if((rel11[buf.mtype] = fopen(buf.mtext, "wb")) == NULL){
				error_exit(errno, "Error creating file");
			}
		} 
		else {
			if(buf.bytes == 0){
				fclose(rel11[buf.mtype]);
				sender_messages[0] = buf.mtype;
				messages_sem.signal();
				rel11.erase(buf.mtype);
			} else {
				fwrite((void *) buf.mtext, 1, buf.bytes, rel11[buf.mtype]);
			}
		}
	}
}
