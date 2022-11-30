#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#include "hash_map.h"
#include "hash.h"

bool is_prime(size_t n) 
{
    for (size_t i = 2; i * i <= n; ++i) 
	{
        if (n % i == 0) 
		{
            return false;
        }
    }
    return true;
}

size_t generate_step() 
{
    size_t step = (size_t)rand();
    while (!is_prime(step)) 
	{
        ++step;
    }
    return step;
}

void destruct_bucket(Bucket* bucket) 
{
    free(bucket->word);
}

void destruct_table(Bucket* table, size_t size) 
{
    for (size_t i = 0; i < size; ++i) 
	{
        Bucket* bucket = &table[i];
        if (bucket->cnt != -1) 
		{
            destruct_bucket(bucket);
        }
    }
    free(table);
}

HashMap* create_hash_map(const size_t size) 
{
    HashMap* hash_map = (HashMap*)malloc(sizeof(HashMap));
    hash_map->size = size;
    hash_map->used_nodes = 0u;
    hash_map->table = (Bucket*)malloc(sizeof(Bucket) * size);
    hash_map->step = generate_step();
    for (size_t i = 0; i < size; ++i) 
	{
        Bucket* cur_node = &hash_map->table[i];
        cur_node->cnt = -1;
    }
    return hash_map;
}

HashMap* merge_hash_maps(HashMap* fir, HashMap* sec) 
{
    BucketsListNode* words_list;
    words_list = get_all_words(sec);

    while (words_list != NULL) 
	{
        char* word = words_list->bucket->word;
        char* word_copy = NULL;
        word_copy = (char*)(malloc(strlen(word) + 1));
        strncpy(word_copy, word, strlen(word) + 1);

        insert_word(fir, word_copy, words_list->bucket->cnt);

        BucketsListNode* next = words_list->next;
        free(words_list);
        words_list = next;
    }

    return fir;
}

void expand_hash_map(HashMap* hash_map) 
{
    Bucket* old_table = hash_map->table;
    const size_t old_size = hash_map->size;

    hash_map->size *= 2;
    hash_map->used_nodes = 0u;

    hash_map->table = (Bucket*)malloc(sizeof(Bucket) * hash_map->size);
    for (size_t i = 0; i < hash_map->size; ++i) 
	{
        Bucket* cur_node = &hash_map->table[i];
        cur_node->cnt = -1;
    }

    for (size_t i = 0; i < old_size; ++i) 
	{
        Bucket* cur_node = &old_table[i];
        if (cur_node->cnt > 0) 
		{
            insert_word(hash_map, cur_node->word, cur_node->cnt);
        } else if (cur_node->cnt == 0) 
		{
            destruct_bucket(cur_node);
        }
    }

    free(old_table);
}

void destruct_hash_map(HashMap* hash_map) 
{
    destruct_table(hash_map->table, hash_map->size);
    free(hash_map);
}

Bucket* find_word(HashMap* hash_map, char* word) 
{
    const size_t start = get_hash(word) % hash_map->size;
    size_t pos = start;
    size_t step = hash_map->step;
    while (true) 
	{
        if (hash_map->table[pos].cnt == -1 || strcmp(word, hash_map->table[pos].word) == 0) 
		{
            return &hash_map->table[pos];
        }

        pos = (pos + step) % hash_map->size;
        if (pos == start) 
		{
            break;
        }
    }
    return NULL;
}

void insert_word(HashMap* hash_map, char* word, long long cnt) 
{
    Bucket* bucket = find_word(hash_map, word);
    if (bucket == NULL) 
	{
        printf("Can't insert new word in HashTable\n");
        exit(1);
    }
    if (bucket->cnt == -1) 
	{
        bucket->cnt = cnt;
        bucket->word = word;
        hash_map->used_nodes++;
    } 
	else 
	{
        bucket->cnt += cnt;
        free(word);
    }

    if (hash_map->used_nodes > (hash_map->size / 2)) 
	{
        expand_hash_map(hash_map);
    }
}

void remove_word(HashMap* hash_map, char* word) 
{
    Bucket* bucket = find_word(hash_map, word);
    if (bucket == NULL || bucket->cnt <= 0) 
	{
        printf("Instance of the word '%s' is not found\n", word);
        return;
    }
    bucket->cnt--;
}

BucketsListNode* get_all_words(HashMap* hash_map) 
{
    BucketsListNode* head = NULL;
    BucketsListNode* tail = NULL;
    for (size_t i = 0; i < hash_map->size; ++i) 
	{
        Bucket* bucket = &hash_map->table[i];
        if (bucket->cnt > 0) 
		{
            BucketsListNode* new_node = (BucketsListNode*)(malloc(sizeof(BucketsListNode)));
            new_node->bucket = bucket;
            new_node->next = NULL;
            if (head == NULL) 
			{
                head = new_node;
                tail = new_node;
            } 
			else 
			{
                tail->next = new_node;
                tail = new_node;
            }
        }
    }

    return head;
}

