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

/*
* Hay que cambiar el contratista para que vaya leyendo del archivo (hasta 1280 bytes) mientras lo va enviando (tal vez con threads?)
* Para salirse limpiamente: usar memoria compartida
*/

 int Size = 512;
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
	char aux[Size];
	do{
		read_result = fread((void *) aux, 1, Size , source);
		for(int i=0; i<read_result; i++){
			if((i%128)==0 && i!=0){
			buf.mtype = pid;
			buf.piece_number = ++current_piece;
			buf.bytes = read_result;
			my_msgq.send(&buf);
			}
		buf.mtext[i%128] = aux[i];
		}	
		if(read_result < Size){
			buf.mtype = pid;
			buf.piece_number = ++current_piece;
			buf.bytes = read_result;
			my_msgq.send(&buf);	
		}
		buf.mtype = pid;
		buf.piece_number = ++current_piece;
		buf.bytes = read_result;
		my_msgq.send(&buf);	
		
	} while(read_result);

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
	if(argc < 2)
		return printf("Missing arguments!\n"), 1;
	Semaphore my_sem(0xb40703, MAX_PROCESSES);
	DIR * dir = opendir(argv[1]);
	if(!dir)
		return printf("Cannot open directory \"%s\"!\n", argv[1]);
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
