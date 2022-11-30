#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "combined_log.h"

char* read_until(char** p, char stop_symbol, int skip_first) 
{
    if (p == NULL || (*p == NULL)) 
	{
        return NULL;
    }

    while (skip_first--) 
	{
        if (**p == '\0') 
		{
            return NULL;
        }
        (*p)++;
    }

    size_t len = 0;
    {
        char* pp = *p;
        while ((*pp != '\0') && (*pp != stop_symbol)) 
		{
            ++len;
            pp++;
        }
    }

    char* result = (char*)(malloc(len + 1));

    for (size_t i = 0; i < len; ++i, ++(*p)) 
	{
        result[i] = (**p);
    }
    result[len] = '\0';

    return result;
}

void init_combined_log(CombinedLog* log) 
{
    log->ip = NULL;
    log->l = NULL;
    log->user = NULL;
    log->time = NULL;
    log->request_line = NULL;
    log->status = NULL;
    log->return_size = NULL;
    log->referer = NULL;
    log->user_agent = NULL;
}

void free_combined_log(CombinedLog* log) 
{
    free(log->ip);
    free(log->l);
    free(log->user);
    free(log->time);
    free(log->request_line);
    free(log->status);
    free(log->return_size);
    free(log->referer);
    free(log->user_agent);
    free(log);
}

CombinedLog* parse_combined_log(char* str) 
{
    CombinedLog* result = NULL;
    result = (CombinedLog*)malloc(sizeof(CombinedLog));
    init_combined_log(result);

    char* p = str;

    result->ip = read_until(&p, ' ', 0);
    result->l = read_until(&p, ' ', 1);
    result->user = read_until(&p, ' ', 1);
    result->time = read_until(&p, ']', 2);
    result->request_line = read_until(&p, '"', 3);
    result->status = read_until(&p, ' ', 2);
    result->return_size = read_until(&p, ' ', 1);
    result->referer = read_until(&p, '\"', 2);
    result->user_agent = read_until(&p, '\"', 3);

    return result;
}

long long string_to_integer(const char* str) 
{
    if (str == NULL) 
	{
        fprintf(stderr, "Failed to convert string to integer: empty string");
        exit(0);
    }
    long long result;
    result = strtol(str, NULL, 0);
    return result;
}

void print_combined_log(const CombinedLog* log) 
{
    printf("{");
    printf("ip: %s", log->ip ? log->ip : "-");
    printf("; l: %s", log->l ? log->l : "-");
    printf("; user: %s", log->user ? log->user : "-");
    printf("; time: %s", log->time ? log->time : "-");
    printf("; request_line: \"%s\"", log->request_line ? log->request_line : "-");
    printf("; status: %s", log->status ? log->status : "-");
    printf("; return_size: %s", log->return_size ? log->return_size : "-");
    printf("; referer: \"%s\"", log->referer ? log->referer : "-");
    printf("; user_agent: \"%s\"", log->user_agent ? log->user_agent : "-");
    printf("}\n");
}
