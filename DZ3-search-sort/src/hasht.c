#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "hash_table.h"

int usage(int argc)
{
	if (argc > 2)
		printf("Too many arguments!\n");
	
	printf("usage: main [filename]\n");
	
	return 1;
}

int main (int argc, char * argv[]) 
{
	if (argc != 2){
		return usage(argc);
	}
	
	int table_size = 100;
	char *filename = argv[1];
	struct DataItem *hasht;
	FILE *fp = fopen(filename,"r");
	
	if (fp == NULL)
    {
        printf("Error: could not open file %s\n", filename);
        return 1;
    }
	
	hasht = calloc(table_size, sizeof(struct DataItem));
	
	if (hasht == NULL)
	{
		printf("Memory allocation error\n");
        return 1;
	}

	hasht = words_num_count(fp, hasht, &table_size);
	print_item(hasht, &table_size);
	
    fclose(fp);
	free(hasht);

    return 0;
}