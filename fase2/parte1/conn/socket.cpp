#include "socket.h"
#include "error_handler.h"

#include <cstdio>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>

Socket::Socket(char t, bool ipv6){
	int domain = (ipv6 ? AF_INET6 : AF_INET);
	int type = (t == 's' ? SOCK_STREAM : SOCK_DGRAM);
	sfd = socket(domain, type, 0);
	if(sfd == -1){
		error_exit(errno, "Error creating socket\n");
	}
}

Socket::Socket(int sfd){
	this->sfd = sfd;
}

Socket::~Socket(){
	if(close(sfd) == -1){
		error_exit(errno, "Error closing socket\n");
	}
}

int Socket::Connect(const char * host, int port){
	int status;
	struct sockaddr_in client_addr;
	struct in_addr addr;
	if(inet_aton(host, &addr) == 0){
		error_exit(errno, "Error parsing the IP address socket\n");
	}
	client_addr.sin_addr = addr;
	client_addr.sin_port = htons(port);
	client_addr.sin_family = AF_INET;
	if((status = connect(sfd, (const struct sockaddr *) &client_addr, sizeof(struct sockaddr_in))) == -1){
		perror("Error connecting!\n");
	}
	return status;
}

int Socket::Read(char * text, int len){
	int bytes;
	if((bytes = recv(sfd, text, len, MSG_WAITALL)) == -1){
		perror("Error reading!\n");
	}
	return bytes;
}

int Socket::Write(const char * text, int len){
	int bytes;
	if((bytes = write(sfd, text, len)) == -1){
		perror("Error writing!\n");
	}
	return bytes;
}

int Socket::Listen(int queue){
	int status;
	if((status = listen(sfd, queue)) == -1){
		perror("Error listening!\n");
	}
	return status;
}

int Socket::Bind(long port){
	int status;
	struct sockaddr_in server_addr;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(port);
	server_addr.sin_family = AF_INET;
	if((status = bind(sfd, (const struct sockaddr *) &server_addr, sizeof(struct sockaddr_in))) == -1){
		perror("Error connecting!\n");
	}
	return status;
}

Socket * Socket::Accept(){
	Socket * accepted_socket = nullptr;
	int accepted_sfd;
	unsigned int add_len;
	struct sockaddr_in incoming_addr;
	if((accepted_sfd = accept(sfd, (struct sockaddr *) &incoming_addr, &add_len)) == -1){
		perror("Error accepting!\n");
	} else {
		accepted_socket = new Socket(accepted_sfd);
	}
	return accepted_socket;
}

int Socket::Shutdown(int how){
	int status;
	if((status = shutdown(sfd, how)) == -1){
		perror("Error shutting down!\n");
	}
	return status;
}

int Socket::Close(){
	int status;
	if((status = close(sfd)) == -1){
		perror("Error closing socket file descriptor!\n");
	}
	return status;
}
