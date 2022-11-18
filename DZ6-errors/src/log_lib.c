#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>

#include "log_lib.h"

#define BT_BUF_SIZE 100

void backtrace_log()
{
    int nptrs;
    void *buffer[BT_BUF_SIZE];
    char **strings;
	
	if ((log_fp = fopen(log_filename, "a")) == NULL) 
	{
		log_lib("Cannot open file\n");
		exit(EXIT_FAILURE);
	}

    nptrs = backtrace(buffer, BT_BUF_SIZE);
    fprintf(log_fp, "backtrace() returned %d addresses\n", nptrs);

    strings = backtrace_symbols(buffer, nptrs);
    if (strings == NULL) {
        log_lib("backtrace_symbols");
        exit(EXIT_FAILURE);
    }

    for (int j = 0; j < nptrs; j++)
        fprintf(log_fp, "%s\n", strings[j]);

    free(strings);
}

void log_init(char *filename)
{
	char *log_type = ".log";
	
	log_filename = filename;
	
	if (strstr(filename, log_type) == NULL)
		log_filename = strncat(filename, log_type, 
								strlen(log_type));
	
	if ((log_fp = fopen(log_filename, "a")) == NULL) 
	{
		log_lib("Cannot open file.\n");
		exit(EXIT_FAILURE);
	}
	fclose(log_fp);
}

void log_x(int level, const char *file,
			size_t filelen, const char *func,
			size_t funclen, long line,
			const char *format, ...)
{
	va_list va;
	char *l;
	char buffer[256];
	
	switch (level)
	{
		case LOG_LIB:
			l = "[LOG_LIB]";
			break;
		case LOG_DEBUG:
			l = "[ DEBUG ]";
			break;
		case LOG_WARNING:
			l = "[WARNING]";
			break;
		case LOG_INFO:
			l = "[ INFO  ]";
			break;
		case LOG_ERROR:
			l = "[ ERROR ]";
			break;
		default:
			l = "";
			break;
	}
	 
	
	if ((log_fp = fopen(log_filename, "a")) != NULL) 
	{
		va_start(va, format);
		vsnprintf(buffer, 256, format, va);
		va_end(va);
		fprintf(log_fp, "%s%s[%ld]: %s[%ld]: %ld: %s\n", 
				l, file, filelen, func, 
				funclen, line, buffer);
		fclose(log_fp);
	}
	else 
	{	
		log_lib("Cannot open file.\n");
	}
}
