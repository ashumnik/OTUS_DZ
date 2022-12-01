#ifndef HASH_TABLE_HEADER
#define HASH_TABLE_HEADER

#define WSIZE 30
#define CONST_K 3
#define INC_SIZE 100

static const size_t hash_key = 113;

struct DataItem {
   char key[WSIZE];   
   int data;
};

size_t get_char_code(char c);

size_t hash_str(const char* word, int *size);

void add_item(struct DataItem *item, char *word, int data);

void increase_item(struct DataItem *item);

struct DataItem *words_num_count(FILE *fp, struct DataItem *hasht, int *TABLE_SIZE);

void print_item(struct DataItem *hasht, int *TABLE_SIZE);

struct DataItem *add_or_inc_item(struct DataItem *hasht, char *word, int *TABLE_SIZE, int data);


#endif