#pragma once

#include <stdio.h>

static const size_t kHashKey = 113;
static const size_t kHashMod = (size_t)1e9 + 7;

size_t get_char_code(char c);
size_t get_hash(const char* const word);