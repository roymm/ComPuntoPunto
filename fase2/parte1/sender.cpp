#include "message.h"
#include "shared.h"
#include "semaphore.h"
#include "error_handler.h"

#include <map>
#include <cstdio>
#include <errno.h>

void get_filename(char * filename, const char * relative_path){
	int final_slash = 0;
	for(int i = 0; relative_path[i] != '\0'; ++i){
		if(relative_path[i] == '/'){
			final_slash = i;
		}
	}
	int count = 0;
	for(int i = final_slash+1; relative_path[i] != '\0'; ++i){
		filename[i - (final_slash+1)] = relative_path[i];
		++count;
	}

	filename[count] = '\0';
}

int main(){
	std::map<int, FILE *> rel11;
	Message my_msgq;
	SharedMem shared_mem(DEFAULT_SIZE, KEY_SEB);
	Semaphore messages_sem(KEY_CRIS, 0);
	long* sender_messages = (long*) shared_mem.attach();
	struct my_msgbuf buf;
	char filename[FILENAME_MAX];
	while(true){
	 	my_msgq.receive(&buf, 0);
		if(buf.piece_number == 0){
			get_filename((char *) filename, (const char *) buf.mtext);
			if((rel11[buf.mtype] = fopen(filename, "wb")) == NULL){
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
