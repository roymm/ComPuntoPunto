#ifndef MESSAGE_H
#define MESSAGE_H
#define KEY_MESSAGE 0xB40703
#define MSGSIZE 128

#include <sys/types.h>

struct my_msgbuf{
	long mtype;
	int bytes;
	int piece_number;
	char mtext[MSGSIZE];
};

class Message{
	private:
		int id;
		int pid;
		int bufsize;
	public:
		Message(key_t = KEY_MESSAGE);
		~Message();
		void send(struct my_msgbuf *);
		void receive(struct my_msgbuf *, long = 0);

};

#endif
