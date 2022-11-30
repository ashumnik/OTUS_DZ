#ifndef _UTILS_H
#define _UTILS_H

#include <stdint.h>

void ParseAddressAndPort(const char* str, char** address, uint32_t* port);

char* ConcatenateStrings(const char* lhs, const char* rhs);

#endif //_UTILS_H
