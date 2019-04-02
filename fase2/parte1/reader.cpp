#include "semaphore.h"
#include "message.h"
#include "error_handler.h"
#include "shared.h"

#include <cstdio>
#include <cstring>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

#define MAX_PROCESSES 2
#define SIZE 512

void contractor(const char * filename, Semaphore * my_sem){
	Message my_msgq;
	SharedMem shared_mem(DEFAULT_SIZE, KEY_SEB);
	Semaphore messages_sem(KEY_CRIS, 0);
	long* sender_messages = (long*) shared_mem.attach();
	memset((void*) sender_messages, 0, DEFAULT_SIZE);
	struct my_msgbuf buf;

	FILE * source = fopen(filename, "rb");
	if(source == NULL){
		error_exit(errno, "Error opening file\n");
	}
	long pid = getpid();
	int current_piece = 0;

	buf.mtype = pid;
	buf.piece_number = current_piece;
	buf.bytes = 0;
	strcpy(buf.mtext, filename);

	my_msgq.send(&buf);

	int read_result;
	int full_groups;
	int remainder_bytes;
	char aux[SIZE];
	do{
		read_result = fread((void *) aux, 1, SIZE, source);
		full_groups = read_result/MSG_SIZE;
		remainder_bytes = read_result%MSG_SIZE;

		for(int i = 0; i < full_groups; ++i){
			memcpy((void *) buf.mtext, (void *) (aux + i*MSG_SIZE), MSG_SIZE);
			buf.mtype = pid;
			buf.piece_number = ++current_piece;
			buf.bytes = MSG_SIZE;
			my_msgq.send(&buf);
		}
		if(remainder_bytes > 0){
			memcpy((void *) buf.mtext, (void *) (aux+full_groups*MSG_SIZE), remainder_bytes);
			buf.mtype = pid;
			buf.piece_number = ++current_piece;
			buf.bytes = remainder_bytes;
			my_msgq.send(&buf);
		}

	} while(read_result);
	buf.mtype = pid;
	buf.piece_number = ++current_piece;
	buf.bytes = 0;
	my_msgq.send(&buf);	
		

	fclose(source);
	while(true){
		messages_sem.wait();
		if(sender_messages[0] == pid){
			printf("Contractor with id %ld is going to meet the Lord!\n", pid);
			break;
		}
		messages_sem.signal();
	}
	shared_mem.detach(sender_messages);
	my_sem->signal();
	_exit(0);
}

int main(int argc, char * argv[]){
	if(argc < 2){
		printf("Missing arguments!\n");
		return 1;
	}
	Semaphore my_sem(0xb40703, MAX_PROCESSES);
	DIR * dir = opendir(argv[1]);
	if(!dir){
		exit_error("Cannot open directory");
	}
	struct dirent * entry = NULL; 
	char relative_path[NAME_MAX];
	strcat(relative_path, argv[1]);
	int status = 0;
	int forked_processes = 0;
	while((entry = readdir(dir))){
		if(entry->d_type == (DT_REG|DT_UNKNOWN) && entry->d_name[0] != '.'){
			my_sem.wait();
			if(!fork()){
				strcat(relative_path, (const char *) entry->d_name);
				contractor((const char *) relative_path, &my_sem);
			}else{
				++forked_processes;
			}
		}
	}
	for(int zombies = 0; zombies < forked_processes; ++zombies)
		wait(&status);
	closedir(dir);
}
