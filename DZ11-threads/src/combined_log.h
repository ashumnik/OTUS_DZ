#ifndef _COMBINEDLOGFORMAT_H
#define _COMBINEDLOGFORMAT_H

#include <stdio.h>

typedef struct 
{
    char* ip;
    char* l;
    char* user;
    char* time;
    char* request_line;
    char* status;
    char* return_size;
    char* referer;
    char* user_agent;
} CombinedLog;

void init_combined_log(CombinedLog* log);
void free_combined_log(CombinedLog* log);
CombinedLog* parse_combined_log(char* str);
long long string_to_integer(const char* return_size);
void print_combined_log(const CombinedLog* log);

#endif //_COMBINEDLOGFORMAT_H
