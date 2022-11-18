#ifndef _CRC32_H
#define _CRC32_H

#define PAGE_SIZE 0x1000

size_t get_file_size(const char* file_path);
uint32_t find_crc32_sum(int fd, size_t file_size); 

#endif //_CRC32_H