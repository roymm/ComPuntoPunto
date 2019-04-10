#ifndef MESSAGE_H
#define MESSAGE_H
#define KEY_ASCH 0xB40703
#define KEY_CRIS 0xB55049
#define KEY_SEB 0xB72458
#define KEY_ROY 0xB54911
#define MSGSIZE 128

#include <sys/types.h>

struct my_msgbuf{
	long mtype; /** El tipo de mensaje, sirve para identificar quién envió un mensaje */
	int bytes;  /** El tamaño en bytes del mensaje que se envio y es menor o igual a MSGSIZE */
	int piece_number; /** El número de mensaje que se ha enviado */
	char mtext[MSGSIZE]; /** El mensaje que se desea enviar, puede ser un string o simplemente un mensaje binario */
};

class Message{
	private:
		int id; /** El id que identifica a la cola de mensajes en el sistema operativo */
		int pid; /** El pid del proceso que mandó a crear el buzón */
		int bufsize; /** El tamaño de los "sobres" que se van a mandar, es igual al tamaño de my_msgbuf sin el mtype */
	public:
		/**
		 * Manda a crear un nuevo buzón e inicializa las variables privadas
		 * Esta función puede fallar, en cuyo caso el proceso que la invoca se cae
		 * @param key El "nombre" del buzón
		 */
		Message(key_t key = KEY_ASCH);

		/**
		 * Destruye la cola de mensajes, si el pid de quien llama a esta función es igual al pid del que mandó a crear el buzón
		 */
		~Message();

		/**
		 * Envía un mensaje empaquetado en un my_msgbuf
		 * Esta función puede fallar, en cuyo caso el proceso que la invoca se cae
		 * Esta función puede bloquear si la cola está llena
		 * @param buf Una referencia al paquete que se desea enviar
		 */
		void send(struct my_msgbuf * buf);

		/**
		 * Recibe un mensaje empaquetado en un my_msgbuf
		 * Esta función puede fallar, en cuyo caso el proceso que la invoca se cae
		 * Esta función puede bloquear si la cola está vacía
		 * @param buf Una referencia en la cuál se guardará el mensaje recibido
		 * @param type El tipo de mensaje que se desea recibir, si es 0 se recibe cualquier tipo de mensaje
		 */
		void receive(struct my_msgbuf * buf, long type = 0);

};

#endif
