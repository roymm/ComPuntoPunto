#include "error_handler.h"
#include "semaphore.h"
#include <sys/types.h>
#include <sys/sem.h>
#include <unistd.h>
#include <errno.h>
#include <cstdlib>
#include <string.h>
#include <stdio.h>

Semaphore::Semaphore(key_t key, int v){
	id = semget(key, 1, IPC_CREAT|0600);
	union semun u;
	if(-1 == id){
		error_exit(errno, "Error creating the semaphore\n");	
	}

	u.val = v;
	int st = semctl(id, 0, SETVAL, u);
	if(-1 == st){
		error_exit(errno, "Error setting the semaphore\n");	
	}
	pid = getpid();
}

Semaphore::~Semaphore(){
	if(getpid() == pid){
		int st = semctl(id, 1, IPC_RMID);
		if(-1 == st){
			char error_msg[128];
			memset((void*) error_msg, '\0', 128); 
			sprintf(error_msg, "Error deleting the semaphore with id %d\n", id);
			error_exit(errno, error_msg);	
		}
	}
}

void Semaphore::wait(){
	struct sembuf z;
	z.sem_num = 0;
	z.sem_op = -1;
	z.sem_flg = 0;
	int st = semop(id, &z, 1);
	if(-1 == st){
		error_exit(errno, "Error waiting on the semaphore\n");	
	}
}

void Semaphore::signal(){
	struct sembuf z;
	z.sem_num = 0;
	z.sem_op = 1;
	z.sem_flg = 0;
	int st = semop(id, &z, 1);
	if(-1 == st){
		char error_msg[128];
		memset((void*) error_msg, '\0', 128); 
		sprintf(error_msg, "Error signaling the semaphore with id %d\n", id);
		error_exit(errno, error_msg);		
	}
}
