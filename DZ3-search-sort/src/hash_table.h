#ifndef HASH_TABLE_HEADER
#define HASH_TABLE_HEADER

#define WSIZE 30
#define CONST_K 3

struct DataItem {
   char key[WSIZE];   
   int data;
};

unsigned hash_str(const char* s, int *size);

void add_item(struct DataItem *item, char *word);

void increase_item(struct DataItem *item);

struct DataItem *words_num_count(FILE *fp, struct DataItem *hasht, int *TABLE_SIZE);

void print_item(struct DataItem *hasht, int *TABLE_SIZE);

struct DataItem *add_or_inc_item(struct DataItem *hasht, char *word, int *TABLE_SIZE);


#endif