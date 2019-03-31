#include "shared.h"

#include <sys/types.h>
#include <sys/shm.h>
#include <unistd.h>
#include <errno.h>
#include <stddef.h>
#include <cstdio>
#include <cstdlib>


SharedMem::SharedMem(size_t size, key_t key){
	pid = getpid();
	id = shmget(key, size, IPC_CREAT|0600);
	if(-1 == id){
		int err = errno;
		perror("Error creating shared memory\n");
		exit(err);
	}
}

SharedMem::~SharedMem(){
	if(getpid() == pid){
		int st = shmctl(id, IPC_RMID, 0);
		if(-1 == st){
			int err = errno;
			perror("Error deleting shared memory\n");
			exit(err);
		}
	}
}

void * SharedMem::attach(){
	void * memloc = shmat(id, NULL, 0);
	return memloc;
}

void SharedMem::detach(const void * memloc){
	int st = shmdt(memloc);
	if(-1 == st){
		int err = errno;
		perror("Error detaching shared memory\n");
		exit(err);
	}
}
