#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>

#include "file_with_mutex.h"

void file_with_mutex_init(FileWithMutex* file_with_mutex, char* file_path) 
{
    file_with_mutex->file = NULL;
    file_with_mutex->mutex = NULL;
    atomic_store(&file_with_mutex->done, true);

    if (!file_path) 
	{
        return;
    }

    file_with_mutex->file = fopen(file_path, "r");
    if (file_with_mutex->file == NULL) 
	{
        perror("Failed to open input file");
        exit(errno);
    }

    file_with_mutex->mutex = (pthread_mutex_t*)(malloc(sizeof(pthread_mutex_t)));
    if (pthread_mutex_init(file_with_mutex->mutex, NULL)) 
	{
        perror("Failed to initialize mutex");
        exit(errno);
    }

    atomic_store(&file_with_mutex->done, false);
}

void file_with_mutex_destroy(FileWithMutex* file_with_mutex) 
{
    if (file_with_mutex->file) 
	{
        if (fclose(file_with_mutex->file)) 
		{
            perror("Failed to close file");
            exit(errno);
        }
    }

    if (file_with_mutex->mutex) 
	{
        pthread_mutex_destroy(file_with_mutex->mutex);
        free(file_with_mutex->mutex);
    }

    atomic_store(&file_with_mutex->done, true);
}

int find_ready_file_and_lock(FileWithMutex* files, size_t files_n) 
{
    while (true) 
	{
        size_t not_finished_cnt = 0;

        for (size_t i = 0; i < files_n; ++i) 
		{
            if (!atomic_load(&files[i].done)) 
			{
                not_finished_cnt++;
                if (!pthread_mutex_trylock(files[i].mutex)) 
				{
                    if (atomic_load(&files[i].done)) 
					{
                        pthread_mutex_unlock(files[i].mutex);
                        continue;
                    }
                    return (int)i;
                }
            }
        }

        if (not_finished_cnt == 0) 
		{
            break;
        }
    }

    return -1;
}
