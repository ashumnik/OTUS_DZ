#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>

#include "../src/log_lib.h"

int verbose = 1;
char *log_filename;
FILE *log_fp;

void usage(int argc)
{
	if (argc > 2)
		printf("Too many arguments!\n");
	
	printf("usage: main [filename]\n");
}

void bar()
{
	log_error("log_error");
}

void foo()
{
	bar();
}

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		usage(argc);
		return 1;
	}
	
	log_init((char *)argv[1]);
	
	log_info("log_info");
	
	verbose = 1;
	log_debug("log_debug");
	
	log_warning("log_warning");
	
	log_lib("log_lib");
	
	foo();
	
	return 0;
}