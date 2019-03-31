#ifndef SHMEM_H
#define SHMEM_H

#define KEY_MEM 0xB40703
#define DEFAULT_SIZE 1024

#include <sys/types.h>
#include <stddef.h>

class SharedMem{
	private:
		int id;
		int pid;
	public:
		SharedMem(size_t = DEFAULT_SIZE, key_t = KEY_MEM);
		~SharedMem();
		void * attach();
		void detach(const void *);
};

#endif
