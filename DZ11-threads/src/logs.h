#ifndef _LOGS_H
#define _LOGS_H

#include "file_with_mutex.h"
#include "../hash_map/hash_map.h"

typedef struct 
{
    size_t total_logs_processed;
    size_t total_bytes_send;
    HashMap* url_counter;
    HashMap* referers_counter;
} LogsStatistics;

typedef struct 
{
    LogsStatistics* processor_results;
    FileWithMutex* files;
    size_t files_n;
    size_t thread_id;
} ThreadArgs;

void init_logs_statistics(LogsStatistics* result);
void destroy_logs_statistics(LogsStatistics* result);
void combine_two_logs_statistics(LogsStatistics* fir, const LogsStatistics* sec);
void* single_thread_process(void* arguments);

#endif //_LOGS_H
