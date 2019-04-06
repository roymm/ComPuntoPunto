#ifndef LECTOR_H
#define LECTOR_H

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "dir.h"
#include "queue.h"

int main(int argc, char* argv[])
{
	if(argc == 1)
		return printf("Usage: %s path [-r|--recursive]\n", argv[0]), 1;
		
	bool recursive = false;
	if(argc > 2 && (strcmp(argv[2], "--recursive") == 0 || strcmp(argv[2], "-r") == 0))
		recursive = true;
		
	queue_t* filename_queue = queue_create();
	dir_list_files_in_dir(filename_queue, argv[1], recursive);
	
	for(queue_iterator_t it = queue_begin(filename_queue); it != queue_end(filename_queue); it = queue_next(it)){
		printf("%s\n", (const char *) queue_data(it));
	}
	
	queue_destroy(filename_queue, true);
	
	return 0;
}

#endif
