#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <limits.h>

#include "file_list.h"
#include "parallel_logs.h"

#define TOP_N 10

FileNode* get_list_of_files_in_directory(const char* input_dir_path) 
{
    DIR* dir = NULL;
    dir = opendir(input_dir_path);
    if (dir == NULL) 
	{
        perror("Failed to open input directory");
        exit(errno);
    }

    const size_t prefix_len = strlen(input_dir_path) + 2;
    char* prefix_str = (char*)(malloc(prefix_len));
    snprintf(prefix_str, prefix_len, "%s%s", input_dir_path, "/");

    FileNode* last_file_node = NULL;
    struct dirent* node;
    while ((node = readdir(dir)) != NULL) 
	{
        if (node->d_type == DT_REG) 
		{
            const size_t full_path_len = prefix_len + strlen(node->d_name) + 1;
            char* full_path = (char*)(malloc(full_path_len));
            snprintf(full_path, full_path_len, "%s%s", prefix_str, node->d_name);
            last_file_node = insert_node(last_file_node, full_path);
        }
    }

    free(prefix_str);

    closedir(dir);

    return last_file_node;
}

int top_instances_comparator(const void *arg1, const void *arg2) 
{
    const Bucket** lhs_p = (const Bucket**)(arg1);
    const Bucket** rhs_p = (const Bucket**)(arg2);
    const Bucket* lhs = *lhs_p;
    const Bucket* rhs = *rhs_p;

    if (lhs == NULL) return rhs == NULL ? 0 : 1;
    if (rhs == NULL) return -1;
    if (lhs->cnt > rhs->cnt) return -1;
	else if (lhs->cnt == rhs->cnt) return 0;
	else return 1;
}

void sort_top_instances(Bucket* top[], size_t length) 
{
    qsort(top, length, sizeof(Bucket*), top_instances_comparator);
}

void get_top_instances(Bucket* top[], size_t length, HashMap* hash_map) 
{
    BucketsListNode* buckets;
    buckets = get_all_words(hash_map);
    while (buckets != NULL) 
	{
        long long min_cnt = LLONG_MAX;
        int pos = -1;
        for (size_t i = 0; i < length; ++i) 
		{
            if (top[i] == NULL) 
			{
                pos = (int)i;
                break;
            } else if (top[i]->cnt < min_cnt) 
			{
                min_cnt = top[i]->cnt;
                pos = (int)i;
            }
        }

        if (pos != -1) 
		{
            Bucket* bucket_copy = (Bucket*)malloc(sizeof(Bucket));
            bucket_copy->cnt = buckets->bucket->cnt;
            char* word = buckets->bucket->word;
            bucket_copy->word = (char*)(malloc(strlen(word) + 1));
            strncpy(bucket_copy->word, word, strlen(word) + 1);

            if (top[pos] != NULL) 
			{
                destruct_bucket(top[pos]);
            }
            top[pos] = bucket_copy;
        }

        BucketsListNode* next = buckets->next;
        free(buckets);
        buckets = next;
    }

    sort_top_instances(top, length);
}

void print_results(const LogsStatistics* result) 
{
    printf("Program has processed %zu logs\n", result->total_logs_processed);
    printf("Total number of returned bytes: %zu\n", result->total_bytes_send);

    Bucket* top_urls[TOP_N];
    for (size_t i = 0; i < TOP_N; ++i) 
	{
        top_urls[i] = NULL;
    }
    get_top_instances(top_urls, TOP_N, result->url_counter);
    printf("Top URLs:\n");
    for (size_t i = 0; i < TOP_N; ++i) 
	{
        if (top_urls[i] != NULL) 
		{
            printf("- %s [total weight = %lld]\n", top_urls[i]->word, top_urls[i]->cnt);
            destruct_bucket(top_urls[i]);
            free(top_urls[i]);
        }
    }

    Bucket* top_referers[TOP_N];
    for (size_t i = 0; i < TOP_N; ++i) 
	{
        top_referers[i] = NULL;
    }
    get_top_instances(top_referers, TOP_N, result->referers_counter);
    printf("Top referers:\n");
    for (size_t i = 0; i < TOP_N; ++i) 
	{
        if (top_referers[i] != NULL) 
		{
            printf("- %s [total mentions = %lld]\n", top_referers[i]->word, top_referers[i]->cnt);
            destruct_bucket(top_referers[i]);
            free(top_referers[i]);
        }
    }
}

void process(const char* input_dir_path, size_t n_threads) 
{
    FileNode* list_of_files = get_list_of_files_in_directory(input_dir_path);
    size_t files_n = count_list_size(list_of_files);

    if (files_n == 0) 
	{
        printf("Input directory is empty.\n");
        destroy_list(list_of_files);
        return;
    }

    printf("Input directory has %zu files:\n", files_n);
    print_list_of_files(list_of_files);

    LogsStatistics* result = NULL;
    result = run_parallel_logs(list_of_files, files_n, n_threads);

    print_results(result);

    destroy_logs_statistics(result);
    free(result);
    destroy_list(list_of_files);
}

int usage()
{
	fprintf(stderr, "Wrong number of input arguments.\n"
                    "usage: <path> <num of threads> \n\n");	
	return 1;
}

int main(int argc, char** argv) 
{
    if (argc != 3) 
	{
        return usage();
    }

    const char* input_dir_path = argv[1];
    printf("Input directory path: %s\n", input_dir_path);

    const int n_threads = (int)strtol(argv[2], NULL, 0);
    printf("Number of threads: %d\n", n_threads);
    if (n_threads < 1) 
	{
        fprintf(stderr, "Wrong number of input threads. It must be integer value, not less than 1.\n");
        return 1;
    }

    struct timespec start, finish;
    clock_gettime(CLOCK_MONOTONIC, &start);

    process(input_dir_path, (size_t)n_threads);

    clock_gettime(CLOCK_MONOTONIC, &finish);

    double time_taken = (double)(finish.tv_sec - start.tv_sec);
    time_taken += (double)(finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    printf("The program took %.5lf seconds to execute\n", time_taken);
    return 0;
}
