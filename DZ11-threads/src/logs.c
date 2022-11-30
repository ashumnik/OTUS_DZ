#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "logs.h"
#include "combined_log.h"

#define LOGS_BUFFER_LIMIT 1000

char* copy_string(char* src) 
{
    char* result = NULL;
    result = (char*)(malloc(strlen(src) + 1));
    strncpy(result, src, strlen(src) + 1);
    return result;
}

void init_logs_statistics(LogsStatistics* stats) 
{
    stats->total_logs_processed = 0;
    stats->total_bytes_send = 0;
    stats->url_counter = create_hash_map(kDefaultHashMapSize);
    stats->referers_counter = create_hash_map(kDefaultHashMapSize);

    if (stats->url_counter == NULL || stats->referers_counter == NULL) 
	{
        fprintf(stderr, "Failed to initialize LogsStatistics\n");
        exit(1);
    }
}

void destroy_logs_statistics(LogsStatistics* stats) 
{
    destruct_hash_map(stats->url_counter);
    destruct_hash_map(stats->referers_counter);
}

void combine_two_logs_statistics(LogsStatistics* fir, const LogsStatistics* sec) 
{
    fir->total_logs_processed += sec->total_logs_processed;
    fir->total_bytes_send += sec->total_bytes_send;
    merge_hash_maps(fir->url_counter, sec->url_counter);
    merge_hash_maps(fir->referers_counter, sec->referers_counter);
}

void update_process_stats(LogsStatistics* processor_results, CombinedLog* log) 
{
    long long return_size = string_to_integer(log->return_size);

    processor_results->total_logs_processed++;
    processor_results->total_bytes_send += return_size;

    char* url = copy_string(log->request_line);
    insert_word(processor_results->url_counter, url, return_size);

    char* referer = copy_string(log->referer);
    insert_word(processor_results->referers_counter, referer, 1);
}

void process_logs(LogsStatistics* processor_results, char** lines_to_process, size_t lines_cnt) 
{
    for (size_t i = 0; i < lines_cnt; ++i) 
	{
        char* current_line = lines_to_process[i];
        CombinedLog* log = parse_combined_log(current_line);
        free(current_line);

        if (log == NULL) 
		{
            fprintf(stderr, "Failed to parse log from file\n");
            continue;
        }

        update_process_stats(processor_results, log);

        free_combined_log(log);
    }
}

bool read_logs_package(FILE* input_file, char* buffer[], size_t* buffer_length, size_t buffer_limit) 
{
    bool file_has_finished = false;
    while ((*buffer_length) < buffer_limit) 
	{
        size_t tmp = 0;
        buffer[*buffer_length] = NULL;
        ssize_t status = getline(&buffer[*buffer_length], &tmp, input_file);
        if (status < 0) 
		{
            free(buffer[*buffer_length]);
            file_has_finished = true;
            break;
        }
        ++(*buffer_length);
    }

    return file_has_finished;
}

void* single_thread_process(void* arguments) 
{
    ThreadArgs* thread_args = (ThreadArgs*)arguments;
    LogsStatistics* processor_results = thread_args->processor_results;
    FileWithMutex* files = thread_args->files;
    size_t files_n = thread_args->files_n;
    free(thread_args);

    char* buffer[LOGS_BUFFER_LIMIT];
    size_t buffer_length = 0;
    while (true) 
	{
        int file_id = find_ready_file_and_lock(files, files_n);
        if (file_id < 0) 
		{
            break;
        }
        bool file_has_finished = read_logs_package(files[file_id].file, buffer, &buffer_length, LOGS_BUFFER_LIMIT);
        if (file_has_finished) 
		{
            atomic_store(&files[file_id].done, true);
        }
        pthread_mutex_unlock(files[file_id].mutex);

        process_logs(processor_results, buffer, buffer_length);
        buffer_length = 0;
    }

    pthread_exit(NULL);
}
