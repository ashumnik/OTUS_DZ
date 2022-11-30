#include <string.h>
#include "hash.h"

size_t get_char_code(char c) 
{
    if ('a' <= c && c <= 'z') return c - 'a' + 1;
	else if ('A' <= c && c <= 'Z') return c - 'A' + 27;
	else return 53;
}

size_t get_hash(const char* const word) 
{
    const size_t len = strlen(word);
    size_t result = 0;
    size_t p = 1;
    for (size_t i = 0; i < len; ++i) 
	{
        char cur_c = word[i];
        p *= kHashKey;
        p %= kHashMod;
        result += p * get_char_code(cur_c);
        result %= kHashMod;
    }
    return result;
}
