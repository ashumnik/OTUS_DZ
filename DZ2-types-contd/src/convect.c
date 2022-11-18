#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

long fsize;

int koi_8[64] = {
0xfe, 0xe0, 0xe1, 0xf6, 0xe4, 0xe5, 0xf4, 0xe3,
0xf5, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee,
0xef, 0xff, 0xf0, 0xf1, 0xf2, 0xf3, 0xe6, 0xe2,
0xfc, 0xfb, 0xe7, 0xf8, 0xfd, 0xf9, 0xf7, 0xfa,
0xde, 0xc0, 0xc1, 0xd6, 0xc4, 0xc5, 0xd4, 0xc3,
0xd5, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce,
0xcf, 0xdf, 0xd0, 0xd1, 0xd2, 0xd3, 0xc6, 0xc2,
0xdc, 0xdb, 0xc7, 0xd8, 0xdd, 0xd9, 0xd7, 0xda
};

int usage(int argc)
{
	if (argc > 4)
		printf("Too many arguments!\n");
	
	printf("usage: main [filename] [encoding type] [fout]\n");
	
	return 1;
}

char * read_file(FILE* file) 
{
	fseek(file, 0, SEEK_END);
	fsize = ftell(file);
	fseek(file, 0, SEEK_SET); 
	
	char *string = malloc(fsize + 1);
	fread(string, fsize, 2, file);
	
	return string;
}

void cp_to_utf (uint8_t *from_str, char *filename)
{
	FILE* to_file = fopen(filename, "wa");
	char byte;
	
	for (int i = 0; i < fsize; i++) {
		if (from_str[i] > 0x7f) {
			byte = (6 << 5) | ((from_str[i] + 0x350) >> 6);
			fwrite(&byte, 1, 1, to_file);
			byte = (2 << 6) | ((from_str[i] + 0x350) & 0x3f);
			fwrite(&byte, 1, 1, to_file);
		} else {
			fwrite(&from_str[i], 1, 1, to_file);
		}
	}
	
	fclose(to_file);
}

void iso_to_utf(uint8_t *from_str, char *filename)
{
	FILE* to_file = fopen(filename, "wa");
	char byte;
	
	for (int i = 0; i < fsize; i++) {
		if (from_str[i] > 0x7f) {
			byte = (6 << 5) | ((from_str[i] + 0x360) >> 6);
			fwrite(&byte, 1, 1, to_file);
			byte = (2 << 6) | ((from_str[i] + 0x360) & 0x3f);
			fwrite(&byte, 1, 1, to_file);
		} else {
			fwrite(&from_str[i], 1, 1, to_file);
		}
	}
	
	fclose(to_file);
}

void koi_to_utf(uint8_t *from_str, char *filename)
{
	FILE* to_file = fopen(filename, "wa");
	char byte;
	
	for (int i = 0; i < fsize; i++) {
		if (from_str[i] > 0x7f) {
			byte = (6 << 5) | ((koi_8[from_str[i] - 0xc0] + 0x350) >> 6);
			fwrite(&byte, 1, 1, to_file);
			byte = (2 << 6) | ((koi_8[from_str[i] - 0xc0] + 0x350) & 0x3f);
			fwrite(&byte, 1, 1, to_file);
		} else {
			fwrite(&from_str[i], 1, 1, to_file);
		}
	}
	
	fclose(to_file);
}

int main (int argc, char * argv[]) 
{
	if (argc != 4)
		return usage(argc);
		
	char *rfilename = argv[1];
	char *wfilename = argv[3];
	char *func = argv[2];
	FILE* file = fopen(rfilename, "r");
	char *string;
	
	if (file == NULL)
       return 1;
	
	string = read_file(file);
	fclose(file);
	
	if (!memcmp(func, "koi8", sizeof("koi8"))) {
		koi_to_utf((uint8_t *)string, wfilename);
	} else if (!memcmp(func, "cp1251", sizeof("cp1251"))) {
		cp_to_utf((uint8_t *)string, wfilename);
	} else if (!memcmp(func, "iso-8859", sizeof("iso-8859"))) {
		iso_to_utf((uint8_t *)string, wfilename);
	} else {
		printf("Only: koi8, cp1251, iso-8859\n");
		return 1;
	}
	
	free(string);	
	
	return 0;
}
