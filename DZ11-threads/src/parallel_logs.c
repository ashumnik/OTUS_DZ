#include "parallel_logs.h"

LogsStatistics* run_parallel(size_t n_threads, FileWithMutex* files, size_t files_n) 
{
    LogsStatistics* processors_results = NULL;
    processors_results = (LogsStatistics*)malloc(sizeof(LogsStatistics) * n_threads);
    for (size_t i = 0; i < n_threads; ++i) 
	{
        init_logs_statistics(&processors_results[i]);
    }

    pthread_t* threads = (pthread_t*)(malloc(sizeof(pthread_t) * n_threads));
    for (size_t i = 0; i < n_threads; ++i) 
	{
        ThreadArgs* thread_args = (ThreadArgs*)(malloc(sizeof(ThreadArgs)));
        thread_args->processor_results = &processors_results[i];
        thread_args->files = files;
        thread_args->files_n = files_n;
        thread_args->thread_id = i;

        if (pthread_create(&threads[i], NULL, single_thread_process, (void*)thread_args)) 
		{
            fprintf(stderr, "Failed to create thread");
            exit(1);
        }
    }

    for (size_t i = 0; i < n_threads; ++i) 
	{
        pthread_join(threads[i], NULL);
        printf("Thread %zu has processed %zu logs\n", i, processors_results[i].total_logs_processed);
    }
    free(threads);

    LogsStatistics* total_result = (LogsStatistics*)(malloc(sizeof(LogsStatistics)));
    init_logs_statistics(total_result);
    for (size_t i = 0; i < n_threads; ++i) 
	{
        combine_two_logs_statistics(total_result, &processors_results[i]);
        destroy_logs_statistics(&processors_results[i]);
    }
    free(processors_results);

    return total_result;
}

LogsStatistics* run_parallel_logs(FileNode* head, size_t files_n, size_t n_threads) 
{
    FileWithMutex* files = (FileWithMutex*)(malloc(sizeof(FileWithMutex) * files_n));
    for (size_t i = 0; i < files_n; ++i, head = head->next) 
	{
        FileWithMutex* cur = &files[i];
        file_with_mutex_init(cur, head->file_path);
    }

    LogsStatistics* result = run_parallel(n_threads, files, files_n);

    for (size_t i = 0; i < files_n; ++i) 
	{
        FileWithMutex* cur = &files[i];
        file_with_mutex_destroy(cur);
    }
    free(files);

    return result;
}
