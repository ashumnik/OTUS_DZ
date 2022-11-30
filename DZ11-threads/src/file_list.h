#ifndef _FILELIST_H
#define _FILELIST_H

#include <stdlib.h>

typedef struct FileNode FileNode;

struct FileNode 
{
    char* file_path;
    FileNode* next;
};

FileNode* insert_node(FileNode* list_head, char* file_path);
void destroy_list(FileNode* list_head);
size_t count_list_size(FileNode* list_head);
void print_list_of_files(FileNode* list_head);

#endif //_FILELIST_H
