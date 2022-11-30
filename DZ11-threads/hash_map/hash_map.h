#ifndef _HASHTABLE_H
#define _HASHTABLE_H

#include <stdlib.h>
#include <stdint.h>

static const int kDefaultHashMapSize = 100;

typedef struct 
{
    char* word;
    long long cnt;
} Bucket;

typedef struct 
{
    size_t size;
    size_t used_nodes;
    Bucket* table;
    size_t step;
} HashMap;

typedef struct BucketsListNode 
{
    struct BucketsListNode* next;
    Bucket* bucket;
} BucketsListNode;

void destruct_bucket(Bucket* cell);
HashMap* create_hash_map(size_t size);
HashMap* merge_hash_maps(HashMap* fir, HashMap* sec);
void expand_hash_map(HashMap* hash_map);
void destruct_hash_map(HashMap* hash_map);
Bucket* find_word(HashMap* hash_map, char* word);
void insert_word(HashMap* hash_map, char* word, long long cnt);
void remove_word(HashMap* hash_map, char* word);
BucketsListNode* get_all_words(HashMap* hash_map);

#endif //_HASHTABLE_H
