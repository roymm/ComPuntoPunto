#include <assert.h>
#include <dirent.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <wchar.h>

#include "dir.h"
#define READ "r"

int dir_list_files_in_dir(queue_t* queue, const char* path, bool recursive)
{
	assert(queue);
	assert(path);
	assert(*path);

	// Try to open the directory
	DIR* dir = opendir(path);
	if ( dir == NULL ){
		struct stat fileStat;
		// Checks if path is a file
		if(stat(path, &fileStat) < 0)
			return fprintf(stderr, "Error: could not open directory or file \"%s\"\n", path), 1;
		else
			return dir_list_file(queue, path);
	}

	// Load the directory entries (contents) one by one
	struct dirent* entry;
	while ( (entry = readdir(dir)) != NULL )
	{
		// If the entry is a directory
		if ( entry->d_type == DT_DIR )
		{
			// Ignore hidden files and directories
			if ( *entry->d_name == '.' )
				continue;

			// If files should be listed recursively
			if ( recursive )
			{
				// Concatenate the directory to the path
				char relative_path[PATH_MAX];
				sprintf(relative_path, "%s/%s", path, entry->d_name);

				// Load files in the subdirectory
				dir_list_files_in_dir(queue, relative_path, recursive);
			}
		}
		else // The entry is not a directory
		{
			// Concatenate the directory to the path
			char relative_path[PATH_MAX];
			sprintf(relative_path, "%s/%s", path, entry->d_name);
			// Append the file or symlink to the queue
			queue_append(queue, strdup(relative_path));
		}
	}

	// Sucess
	closedir(dir);
	return 0;
}

int dir_list_file(queue_t* queue, const char *file_name)
{
	assert(queue);
	assert(file_name);
	assert(*file_name);
	queue_append(queue, strdup(file_name));
	return 0;
}
