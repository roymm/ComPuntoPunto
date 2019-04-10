				#ifndef SEMAPHORE_H
#define SEMAPHORE_H
#define KEY_ASCH 0xB40703
#define KEY_CRIS 0xB55049
#define KEY_SEB 0xB72458
#define KEY_ROY 0xB54911

#include <sys/types.h>

union semun { /** El system call semctl necesita que definamos esta estructura */
	int val; /** Este es el único valor que utilizamos, que corresponde al valor inicial del semáforo */
	struct semid_ds * buf;
	unsigned short * array;
	struct seminfo *__buf;
};

class Semaphore {
	private:
		int id; /** Este es el identificador del semáforo en el sistema operativo */
		int pid; /** Este es el pid del proceso que mandó a crear al semáforo la primera vez */
	public:
		/**
		 * Pide un nuevo semáforo al sistema operativo
		 * Esta función puede fallar, en cuyo caso el proceso que la invoca se cae
		 * @param key El "nombre" del semáforo
		 * @param v El valor inicial del semáforo
		 */
		Semaphore(key_t key = KEY_ASCH, int v = 1);
		/**
		 * Destruye el semáforo, si el pid de quien llama a esta función es igual al pid del que mandó a crear el semáforo
		 */
		~Semaphore();
		/**
		 * Si el valor del semáforo es mayor o igual a 1 deja pasar a quién llame a esta función y le resta 1 al valor
		 * Si el valor es 0, deja esperando a quién llamó a esta función hasta que el valor sea 1.
		 */
		void wait();
		/**
		 * Le suma 1 al valor del semáforo
		 */
		void signal();

};

#endif
