#ifndef SEMAPHORE_H
#define SEMAPHORE_H
#define KEY_SEM 0xB40703

#include <sys/types.h>

union semun {
	int val;
	struct semid_ds * buf;
	unsigned short * array;
	struct seminfo *__buf;
};

class Semaphore {
	private:
		int id;
		int pid;
	public:
		Semaphore(key_t = KEY_SEM, int = 0);
		~Semaphore();
		void wait();
		void signal();

};

#endif
