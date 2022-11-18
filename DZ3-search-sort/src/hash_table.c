#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "hash_table.h"

int NUM_WORDS = 0;

unsigned hash_str(const char* s, int *size)
{
	const int p = 3;
	long long hash = 0, p_pow = 1;
	int len = strlen(s);
	int retw;
	
	for (int i = 0; i < len; ++i)
	{
		hash += (s[i] - 'a' + 1) * p_pow;
		p_pow *= p;
	}
	retw = hash % *size;
	
	return retw < 0 ? -retw : retw;
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
			hasht = add_or_inc_item(hasht, token, size);
			token = strtok(NULL, t);
		}
	}
	
	return hasht;
}

struct DataItem *rehashing(struct DataItem *hasht_from, int *size)
{
	int new_size = *size * 2;
	NUM_WORDS = 0;
	struct DataItem *hasht_to = calloc(new_size, sizeof(struct DataItem));
	for (int i = 0; i < *size; ++i) {
		if (hasht_from[i].data != 0) {
			add_or_inc_item(hasht_to, (char*)hasht_from[i].key, &new_size);
		}
	}
	free(hasht_from);
	*size = new_size;
	return hasht_to;
}

struct DataItem *add_or_inc_item(struct DataItem *hasht, char *word, int *size)
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
				add_item(&hasht[n], word);
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
		add_item(&hasht[n], word);
		++NUM_WORDS;
	}
	
	if (NUM_WORDS >= *size) {
		hasht = rehashing(hasht, size);
	}
	
	return hasht;
}

void add_item(struct DataItem *item, char *word) 
{
	item->data = 1;
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