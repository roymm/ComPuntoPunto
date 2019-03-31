#include "semaphore.h"
#include "message.h"
#include "error_handler.h"

#include <cstdio>
#include <cstring>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>

/*
* Falta hacer una despedida entre los contratistas y el emisor
* '->Falta avisar cuando se termina de escribir el archivo enviado
* Hay que cambiar el contratista para que vaya leyendo del archivo (hasta 1280 bytes) mientras lo va enviando (tal vez con threads?)
*/


void contractor(const char * filename, Semaphore * my_sem){
	Message my_msgq;
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

	do{
		read_result = fread((void *) buf.mtext, 1, MSGSIZE, source);
		buf.mtype = pid;
		buf.piece_number = ++current_piece;
		buf.bytes = read_result;
		my_msgq.send(&buf);
	} while(read_result);

	fclose(source);
	my_sem->signal();
	_exit(0);
}

int main(int argc, char * argv[]){
	Semaphore my_sem(0xb40703, 2);
	DIR * dir = opendir(argv[1]);
	struct dirent * entry = NULL; 
	char relative_path[256];
	strcat(relative_path, argv[1]);

	while(entry = readdir(dir)){
		if(entry->d_type == DT_REG|DT_UNKNOWN && entry->d_name[0] != '.'){
			my_sem.wait();
			if(fork()){
				strcat(relative_path, (const char *) entry->d_name);
				contractor((const char *) relative_path, &my_sem);
			}
		}
	}
	closedir(dir);
}
