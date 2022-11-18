//		раскидать по файлам
//		парсер первых битов фото поправить
//		
// похоже память луше алоцировать потихоньку а не сразу всю строку
// обработку ошибок добавить везде и в ДЗ2
//

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>

#include "zip.h"

int usage(int argc)
{
	switch (argc) 
	{
	case 1:
		printf("The filename is not specified!\n");
		break;
	default:
		printf("Too many arguments!\n");
		break;
	}
	printf("usage: main <filename>\n");
	
	return 1;
}

int not_image(unsigned char *bytes, FILE* file, int *j)
{
	*j = 3;
	fread(bytes, 1, *j, file);
  
    if (bytes[0] != 0xff && bytes[1] != 0xd8
        && bytes[2] != 0xff) 
	{
		return 1;
	}
	return 0;
}

void find_end_of_image(unsigned char *bytes, FILE* file, char *flag, int *j)
{
	while (*flag) {
		fread(bytes, 1, 1, file);
		++*j;
		if (bytes[0] == 0xff) {
			fread(bytes, 1, 1, file);
			++*j;
			if (bytes[0] == 0xd9)
				*flag = 0;
		}
	}	
}

int rar_file_list(FILE* file, int *j)
{
	int jpeg_len = *j;
	uint8_t *string, *str;
	long fsize;
	uint32_t *sign;
	struct LocalFileHeader *localFH;
	int f = -1;
	
	fseek(file, 0, SEEK_END);
	fsize = ftell(file);
	fseek(file, 0, SEEK_SET);
	
	string = malloc(fsize + 1);
	str = string + jpeg_len;
	fread(string, fsize, 1, file);
	fclose(file);
	
	string[fsize] = 0;
	
	if (fsize == jpeg_len) {
		printf("This is just a picture!\n");
		return 0;
	} else {
		printf("This is rarjpeg!\nFiles:\n");
	}
	
	while(1) 
	{
		sign = (uint32_t*) (str);
		switch (*sign) {
			case LFH_SIG:
				localFH = (struct LocalFileHeader *) (str);
				for (int i = 0; i < localFH->filenameLength; i++)
					printf("%c", str[offsetof(struct LocalFileHeader, filename) + i]);
				printf("\n");
				str = str + offsetof(struct LocalFileHeader, filename) + 
								localFH->filenameLength + localFH->extraFieldLength 
								+ localFH->compressedSize;
				f++;
				break;
			// case OTHER_SIG:
			// ...
			default:
				free(string);
				printf("Number of files = %d\n", f);
				return 0;
		}
	}
}

int main (int argc, char *argv[]) 
{
	unsigned char bytes[3];
	int j = 3;
	char flag = 1;
	FILE* file;
	
	if (argc != 2)
		return usage(argc);

	file = fopen(argv[1], "r");
	
	if (file == NULL)
        return 1;
	
	if (not_image(bytes, file, &j))
	{
		printf("This is not image!\n");
		return 1;
	}
	
	find_end_of_image(bytes, file, &flag, &j);
	
	rar_file_list(file, &j);
	
	return 0;
}
