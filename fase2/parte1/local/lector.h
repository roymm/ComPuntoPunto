#ifndef LECTOR_H
#define LECTOR_H

#include <assert.h>
#include <dir.h>
#include <queue.h>
#include <stdio.h>

int main()
{
	char[FILENAME_MAX] path;
	memset(path, '\0', FILENAME_MAX);	//cleans the array
	printf("Enter the files path: ");
	scanf("%s\n", path);
	queue_t* filename_queue = queue_create();
	dir_list_files_in_dir(filename_queue, (const char*) path, true);
	
	for(queue_iterator_t it = queue_begin(filename_queue); it != queue_end(filename_queue); it = queue_next(it)){
		printf((const char *) queue_data(it));
	}
	
	queue_destroy(filename_queue, true);
	
	return 0;
}

#endif
