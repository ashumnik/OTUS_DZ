#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>

#include "crc32.h"

void usage(int argc)
{
	if (argc > 2)
		printf("Too many arguments!\n");
	
	printf("usage: main [filename]\n");
}

int main(int argc, char *argv[]) 
{
	if (argc != 2) 
	{
        usage(argc);
        return 1;
    }

    const char* filename = argv[1];
    int fd = -1;
	size_t file_size = 0;
	int32_t crc32_sum = 0;
	
    if ((fd = open(filename, O_RDONLY)) < 0) 
    {
        printf("Error: could not open file %s", filename);
        return 1;
    }

    file_size = get_file_size(filename);
    printf("File size = %ld\n", file_size);

    crc32_sum = find_crc32_sum(fd, file_size);
    printf("CRC32 sum = %u\n", crc32_sum);

    close(fd);
    return 0;
}
