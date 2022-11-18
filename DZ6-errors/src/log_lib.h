#ifndef _LOGLIB_H
#define _LOGLIB_H

#include <stdint.h>
#include <stdlib.h>

#define LOG_LIB			0
#define LOG_DEBUG		1
#define LOG_INFO		2
#define LOG_WARNING		3
#define LOG_ERROR		4

#define log_lib(...) 							\
		log_x(LOG_LIB, __FILE__, 				\
			sizeof(__FILE__)-1, __func__, 		\
			sizeof(__func__)-1, __LINE__,		\
			__VA_ARGS__);			 			\

#define log_debug(...)							\
	do {										\
		if (verbose)							\
			log_x(LOG_DEBUG, __FILE__, 			\
				sizeof(__FILE__)-1, __func__, 	\
				sizeof(__func__)-1, __LINE__,	\
				__VA_ARGS__);			 		\
	} while(0)

#define log_info(...) 							\
		log_x(LOG_INFO, __FILE__, 				\
			sizeof(__FILE__)-1, __func__, 		\
			sizeof(__func__)-1, __LINE__,		\
			__VA_ARGS__);			 			\
			
#define log_warning(...) 						\
		log_x(LOG_WARNING, __FILE__, 			\
			sizeof(__FILE__)-1, __func__, 		\
			sizeof(__func__)-1, __LINE__,		\
			__VA_ARGS__);			 			\

#define log_error(...) 							\
	do {										\
		log_x(LOG_ERROR, __FILE__, 				\
			sizeof(__FILE__)-1, __func__, 		\
			sizeof(__func__)-1, __LINE__,		\
			__VA_ARGS__);			 			\
		backtrace_log();						\
		exit(EXIT_FAILURE);						\
	} while(0)									\
	
extern int verbose;
extern char *log_filename;
extern FILE *log_fp;

void log_x(int level, const char *file,
			size_t filelen, const char *func,
			size_t funclen, long line,
			const char *format, ...);
void log_init(char *filename);
void backtrace_log();

#endif // _LOGLIB_H