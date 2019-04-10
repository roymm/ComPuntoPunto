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
	Message in_queue; 		//Cola en la que se ponen los pedazos de archivo
	Message out_queue(0xA0000); 		//Cola en la que se reciben los mensajes de confirmación
	struct my_msgbuf buf;	//En este struct se pone el mensaje junto a su encabezado
	char full_path[FILENAME_MAX]; //Aquí va la ruta del archivo + el nombre del archivo
	strcat(full_path, directory); 
	int i;
	for(i = 0; full_path[i] != '\0'; ++i); //Buscamos la longitud de la ruta del archivo
	if(full_path[i-1] != '/'){ //Si no tiene el / final se lo agregamos
		full_path[i] = '/';
		full_path[i+1] = '\0';
	}
	strcat(full_path, filename); //Y finalmente lo juntamos con el nombre de archivo

	FILE * source = fopen(full_path, "rb"); //Aquí tendremos el puntero a FILE del archivo
	if(source == NULL){
		error_exit(errno, "Error opening file\n"); //Si hubo algún error le avisamos al usuario y nos salimos del proceso
	}

	buf.mtype = getpid(); //Tomamos como ID el PID del proceso para no tener que hacer que el emisor nos asigne un ID.
	buf.piece_number = 0;
	buf.bytes = 0;
	strcpy(buf.mtext, filename);

	in_queue.send(&buf); //Enviamos el primer paquete con piece_number = 0 para que el receptor sepa que viene un nuevo archivo y cree un thread.

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
			in_queue.send(&buf);
		}
		if(remainder_bytes > 0){
			memcpy((void *) buf.mtext, (void *) (aux+full_groups*MSGSIZE), remainder_bytes);
			++buf.piece_number;
			buf.bytes = remainder_bytes;
			in_queue.send(&buf);
		}

	} while(read_result);
	++buf.piece_number;
	buf.bytes = 0;
	in_queue.send(&buf); //Enviamos el último paquete con bytes = 0 (y mtype distinto de 0) para que el receptor sepa que ya se terminó el archivo

	fclose(source);
	out_queue.receive(&buf, buf.mtype); //jl proceso se va a bloquear hasta que reciba un mensaje
	my_sem->signal(); //Avisamos al lector que ya terminamos para que pueda crear otro proceso
	printf("%s has finished transfering!\n", filename);
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
		error_exit(errno, "Cannot open directory");
	}
	struct dirent * entry = NULL; 
	int status = 0;
	int forked_processes = 0;
	while((entry = readdir(dir))){
		if(entry->d_type == (DT_REG) && entry->d_name[0] != '.'){
			my_sem.wait();
			if(!fork()){
				contractor(argv[1], (const char *) entry->d_name, &my_sem);
			}else{
				++forked_processes;
			}
		}
	}
	for(int zombies = 0; zombies < forked_processes; ++zombies)
		wait(&status);
	closedir(dir);
}
