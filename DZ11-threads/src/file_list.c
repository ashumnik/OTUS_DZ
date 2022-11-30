#include <stdio.h>

#include "file_list.h"

FileNode* insert_node(FileNode* list_head, char* file_path) 
{
    FileNode* new_node = (FileNode*)(malloc(sizeof(FileNode)));
    new_node->next = list_head;
    new_node->file_path = file_path;
    return new_node;
}

void destroy_list(FileNode* list_head) 
{
    while (list_head != NULL) 
	{
        FileNode* next = list_head->next;
        free(list_head->file_path);
        free(list_head);
        list_head = next;
    }
}

size_t count_list_size(FileNode* list_head) 
{
    size_t res = 0;
    while (list_head != NULL) 
	{
        ++res;
        list_head = list_head->next;
    }
    return res;
}

void print_list_of_files(FileNode* list_head) 
{
    while (list_head != NULL) 
	{
        printf("- %s\n", list_head->file_path);
        list_head = list_head->next;
    }
}
