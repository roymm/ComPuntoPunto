#ifndef SEMAPHORE_H
#define SEMAPHORE_H
#define KEY_ASCH 0xB40703
#define KEY_CRIS 0xB55049
#define KEY_SEB 0xB72458
#define KEY_ROY 0xB54911

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
		Semaphore(key_t = KEY_ASCH, int = 0);
		~Semaphore();
		void wait();
		void signal();

};

#endif
