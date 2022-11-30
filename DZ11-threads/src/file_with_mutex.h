#ifndef _FILEWITHMUTEX_H
#define _FILEWITHMUTEX_H

#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>

typedef struct 
{
    FILE* file;
    pthread_mutex_t* mutex;
    atomic_bool done;
} FileWithMutex;

void file_with_mutex_init(FileWithMutex* file_with_mutex, char* file_path);
void file_with_mutex_destroy(FileWithMutex* file_with_mutex);
int find_ready_file_and_lock(FileWithMutex* files, size_t files_n);

#endif //_FILEWITHMUTEX_H
