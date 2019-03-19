#include "semaphore.h"
#include <sys/types.h>
#include <sys/sem.h>
#include <unistd.h>
#include <errno.h>
#include <cstdio>
#include <cstdlib>

Semaphore::Semaphore(key_t key, int v){
	id = semget(key, 1, IPC_CREAT|0600);
	union semun u;
	if(-1 == id){
		int exval = errno;
		perror("Error creating the semaphore\n");	
		exit(exval);
	}

	u.val = v;
	int st = semctl(id, 0, SETVAL, u);
	if(-1 == st){
		int exval = errno;
		perror("Error setting the semaphore\n");	
		exit(exval);
	}
	pid = getpid();
}

Semaphore::~Semaphore(){
	if(getpid() == pid){
		int st = semctl(id, 1, IPC_RMID);
		if(-1 == st){
			int exval = errno;
			printf("%d\n", getpid());
			perror("Error deleting the semaphore\n");	
			exit(exval);
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
		int exval = errno;
		perror("Error waiting on the semaphore\n");	
		exit(exval);
	}
}

void Semaphore::signal(){
	struct sembuf z;
	z.sem_num = 0;
	z.sem_op = 1;
	z.sem_flg = 0;
	int st = semop(id, &z, 1);
	if(-1 == st){
		int exval = errno;
		perror("Error signaling the semaphore\n");	
		exit(exval);
	}
}
