#ifndef _SERVER_H
#define _SERVER_H

#include <stdint.h>

typedef struct {
    const char* directory_path;
} ServerArgs;

void RunServer(const char* address, uint32_t port, int backlog, ServerArgs* args);

#endif //_SERVER_H
