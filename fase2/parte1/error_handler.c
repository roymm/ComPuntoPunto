#include "error_handler.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

void error_exit(int my_error, const char * message){
		perror(message);
		exit(my_error);
}
