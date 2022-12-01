#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "hash_table.h"

int NUM_WORDS = 0;

size_t get_char_code(char c) 
{
    if ('a' <= c && c <= 'z') return c - 'a' + 1;
	else if ('A' <= c && c <= 'Z') return c - 'A' + 27;
	else return 53;
}

size_t hash_str(const char* word, int *size) 
{
    const size_t len = strlen(word);
    size_t result = 0;
    size_t p = 1;
    for (size_t i = 0; i < len; ++i) 
	{
        char cur_c = word[i];
        p *= hash_key;
        p %= *size;
        result += p * get_char_code(cur_c);
        result %= *size;
    }
    return result;
}

struct DataItem *words_num_count(FILE *fp, struct DataItem *hasht, int *size)
{
	char buf[WSIZE];
	
	while(fscanf(fp, "%s", buf) != EOF)
    {
		char *token;
		char *t = ",.(){}/[]-!;?";
		token = strtok(buf, t);
		
		while(token != NULL) {
			hasht = add_or_inc_item(hasht, token, size, 1);
			token = strtok(NULL, t);
		}
	}
	
	return hasht;
}

struct DataItem *rehashing(struct DataItem *hasht_from, int *size)
{
	int new_size = *size + INC_SIZE;
	NUM_WORDS = 0;
	struct DataItem *hasht_to = calloc(new_size, sizeof(struct DataItem));
	if (hasht_to == NULL)
	{
		printf("Memory allocation error\n");
        exit(1);
	}
	
	for (int i = 0; i < *size; ++i) {
		if (hasht_from[i].data != 0) {
			add_or_inc_item(hasht_to, (char*)hasht_from[i].key, &new_size, hasht_from[i].data);
		}
	}
	free(hasht_from);
	*size = new_size;
	return hasht_to;
}

struct DataItem *add_or_inc_item(struct DataItem *hasht, char *word, int *size, int data)
{
	int n = 0;	
	n = hash_str(word, size);
	if (hasht[n].data != 0)
	{
		if (!strncmp(hasht[n].key, word, WSIZE))
		{
			increase_item(&hasht[n]);
		}
		else 
		{
			do 
			{
				n = (n + CONST_K) % *size;
			} while (strncmp(hasht[n].key, word, WSIZE) && hasht[n].data);
			
			if (!hasht[n].data) 
			{
				add_item(&hasht[n], word, data);
				++NUM_WORDS;
			} 
			else
			{
				increase_item(&hasht[n]);
			}
		}
	}
	else
	{
		add_item(&hasht[n], word, data);
		++NUM_WORDS;
	}
	
	if (NUM_WORDS >= *size) {
		hasht = rehashing(hasht, size);
	}
	
	return hasht;
}

void add_item(struct DataItem *item, char *word, int data) 
{
	item->data = data;
	memcpy(item->key, word, WSIZE);
}

inline void increase_item(struct DataItem *item) 
{
	item->data++;
}

void print_item(struct DataItem *hasht, int *size)
{
	for (int i = 0; i < *size; ++i) {
		if (hasht[i].data != 0) {
			printf("%30s : %d\n", hasht[i].key, hasht[i].data);
		}
	}
}