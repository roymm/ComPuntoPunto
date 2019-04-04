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

void contractor(const char * directory, const char * filename, Semaphore * my_sem){
	Message my_msgq;
	SharedMem shared_mem(DEFAULT_SIZE, KEY_SEB);
	Semaphore messages_sem(KEY_CRIS, 0);
	long* sender_messages = (long*) shared_mem.attach();
	memset((void*) sender_messages, 0, DEFAULT_SIZE);
	struct my_msgbuf buf;
	char full_path[FILENAME_MAX];
	strcat(full_path, directory);
	int i;
	for(i = 0; full_path[i] != '\0'; ++i);
	if(full_path[i-1] != '/'){
		full_path[i] = '/';
		full_path[i+1] = '\0';
	}
	strcat(full_path, filename);

	FILE * source = fopen(full_path, "rb");
	if(source == NULL){
		error_exit(errno, "Error opening file\n");
	}

	buf.mtype = getpid();
	buf.piece_number = 0;
	buf.bytes = 0;
	strcpy(buf.mtext, filename);

	my_msgq.send(&buf);

	int read_result;
	int full_groups;
	int remainder_bytes;
	char aux[SIZE];
	do{
		read_result = fread((void *) aux, 1, SIZE, source);
		full_groups = read_result/MSGSIZE;
		remainder_bytes = read_result%MSGSIZE;

		for(int i = 0; i < full_groups; ++i){
			memcpy((void *) buf.mtext, (void *) (aux + i*MSGSIZE), MSGSIZE);
			++buf.piece_number;
			buf.bytes = MSGSIZE;
			my_msgq.send(&buf);
		}
		if(remainder_bytes > 0){
			memcpy((void *) buf.mtext, (void *) (aux+full_groups*MSGSIZE), remainder_bytes);
			++buf.piece_number;
			buf.bytes = remainder_bytes;
			my_msgq.send(&buf);
		}

	} while(read_result);
	++buf.piece_number;
	buf.bytes = 0;
	my_msgq.send(&buf);	
		

	fclose(source);
	while(true){
		messages_sem.wait();
		if(sender_messages[0] == buf.mtype){
			printf("Contractor with id %ld is going to meet the Lord!\n", buf.mtype);
			printf("File: %s has been successfully transfered\n", filename);
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
		error_exit(errno, "ERROR: Cannot open directory");
	}
	struct dirent * entry = NULL; 
	int status = 0;
	int forked_processes = 0;
	int number_of_files = 0;
	while((entry = readdir(dir))){
		if(entry->d_type == (DT_REG|DT_UNKNOWN) && entry->d_name[0] != '.'){
			number_of_files++;
			my_sem.wait();
			if(!fork()){
				contractor(argv[1], (const char *) entry->d_name, &my_sem);
			}else{
				++forked_processes;
			}
		}
	}
	if(number_of_files == 0){
		printf("ERROR: Path is empty\n");
	}
	for(int zombies = 0; zombies < forked_processes; ++zombies)
		wait(&status);
	closedir(dir);
}
