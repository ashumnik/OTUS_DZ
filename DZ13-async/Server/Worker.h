#ifndef _WORKER_H
#define _WORKER_H

#include <sys/types.h>

typedef struct {
    int fd;
    const char* directory_path;
    int* finish_flag;
} WorkerArgs;

void InitWorkers(size_t n);
void DestroyWorkers();
void RunWorker(int fd, const char* directory_path);

#endif //_WORKER_H
