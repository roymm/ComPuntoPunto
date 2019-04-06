#ifndef DIR_H
#define DIR_H

#include "queue.h"

/// Add file names in the content of the given directory to the given list
int dir_list_files_in_dir(queue_t* queue, const char* path, bool recursive);
/// Add file name directly to the given list
int dir_list_file(queue_t* queue, const char *file_name);
#endif // DIR_H
