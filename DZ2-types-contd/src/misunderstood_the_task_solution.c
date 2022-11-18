#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>

long fsize;

int koi_8[64] = {
0xe1, 0xe2, 0xf7, 0xe7, 0xe4, 0xe5, 0xf6, 0xfa, 
0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef, 0xf0, 
0xf2, 0xf3, 0xf4, 0xf5, 0xe6, 0xe8, 0xe3, 0xfe,
0xfb, 0xfd, 0xff, 0xf9, 0xf8, 0xfc, 0xe0, 0xf1, 
0xc1, 0xc2, 0xd7, 0xc7, 0xc4, 0xc5, 0xd6, 0xda, 
0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xd0, 
0xd2, 0xd3, 0xd4, 0xd5, 0xc6, 0xc8, 0xc3, 0xde,
0xdb, 0xdd, 0xdf, 0xd9, 0xd8, 0xdc, 0xc0, 0xd1
};

int koi_8_r[64] = {
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
	switch (argc) {
	case 1:
		printf("The filename is not specified!\n");
		break;
	case 2:
		printf("The filename is not specified!\n");
		break;
	case 3:
		printf("The filename is not specified!\n");
		break;
	default:
		printf("Too many arguments!\n");
		break;
	}
	printf("usage: main [filename] [encoding type] [fout]\n");
	
	return 1;
}

uint8_t * read_file(FILE* file) 
{
	fseek(file, 0, SEEK_END);
	fsize = ftell(file);
	fseek(file, 0, SEEK_SET); 
	
	// printf("filesize = %ld\n", fsize);
	
	uint8_t *string = malloc(fsize + 1);
	fread(string, fsize, 2, file);
	
	return string;
}

void write_file(uint8_t *string)
{
	FILE* to_file = fopen("test_cp.txt", "wa");
	fwrite(string, fsize, 1, to_file);
	fclose(to_file);
}

uint8_t * cp_to_iso(uint8_t *from_str) // + 
{
	uint8_t *to_str = malloc(fsize + 1);
	
	for(int i = 0; i < fsize; i++) {
		if (from_str[i] > 0x7f)
			to_str[i] = from_str[i] - 16;
		else
			to_str[i] = from_str[i];
	}
	
	return to_str;
}

uint8_t * iso_to_cp(uint8_t *from_str) // +
{
	uint8_t *to_str = malloc(fsize + 1);
	
	for(int i = 0; i < fsize; i++) {
		if (from_str[i] > 0x7f)
			to_str[i] = from_str[i] + 16;
		else
			to_str[i] = from_str[i];
	}
	
	return to_str;
}

uint8_t * cp_to_koi(uint8_t *from_str) // +
{
	uint8_t *to_str = malloc(fsize + 1);
	
	for(int i = 0; i < fsize; i++) {
		if (from_str[i] > 0x7f)
			to_str[i] = koi_8[from_str[i] - 0xc0];
		else
			to_str[i] = from_str[i];
	}
	
	return to_str;
}

uint8_t * koi_to_cp(uint8_t *from_str) // +
{
	uint8_t *to_str = malloc(fsize + 1);
	
	for(int i = 0; i < fsize; i++) {
		if (from_str[i] > 0x7f)
			to_str[i] = koi_8_r[from_str[i] - 0xc0];
		else
			to_str[i] = from_str[i];
	}
	
	return to_str;
}

uint8_t * koi_to_eng(uint8_t *from_str) // +
{
	uint8_t *to_str = malloc(fsize + 1);
	
	for(int i = 0; i < fsize; i++) {
		if (from_str[i] > 0x7f)
			to_str[i] = from_str[i] & ~(1 << 7);
		else
			to_str[i] = from_str[i];
	}
	
	return to_str;
}

uint8_t * koi_to_iso(uint8_t *from_str) // +
{
	uint8_t *to_str = malloc(fsize + 1);
	
	for(int i = 0; i < fsize; i++) {
		if (from_str[i] > 0x7f)
			to_str[i] = koi_8_r[from_str[i] - 0xc0] - 16;
		else
			to_str[i] = from_str[i];
	}
	
	return to_str;
}

uint8_t * iso_to_koi(uint8_t *from_str) // +
{
	uint8_t *to_str = malloc(fsize + 1);
	
	for(int i = 0; i < fsize; i++) {
		if (from_str[i] > 0x7f)
			to_str[i] = koi_8[from_str[i] - 0xb0];
		else
			to_str[i] = from_str[i];
	}
	
	return to_str;
}

uint8_t *cp_to_utf (uint8_t *from_str) // +
{
	FILE* to_file = fopen("test_cp.txt", "wa");
	uint8_t byte;
	
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
	return 0;
}

uint8_t *iso_to_utf (uint8_t *from_str) // +
{
	FILE* to_file = fopen("test_cp.txt", "wa");
	uint8_t byte;
	
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
	return 0;
}

uint8_t *koi_to_utf (uint8_t *from_str) // +
{
	FILE* to_file = fopen("test_cp.txt", "wa");
	uint8_t byte;
	
	for (int i = 0; i < fsize; i++) {
		if (from_str[i] > 0x7f) {
			byte = (6 << 5) | ((koi_8_r[from_str[i] - 0xc0] + 0x360) >> 6);
			fwrite(&byte, 1, 1, to_file);
			byte = (2 << 6) | ((koi_8_r[from_str[i] - 0xc0] + 0x360) & 0x3f);
			fwrite(&byte, 1, 1, to_file);
		} else {
			fwrite(&from_str[i], 1, 1, to_file);
		}
	}
	
	fclose(to_file);
	return 0;
}

int main (int argc, char * argv[]) 
{
	if (argc != 2) // 4
		return usage(argc);

	FILE* file = fopen(argv[1], "r");
	
	if (file == NULL)
       return 1;
	
	uint8_t *string = read_file(file);
	// uint16_t *string = read_file(file);
	fclose(file);

	uint8_t *string2 = koi_to_utf(string);
	
	// write_file(string2);
	
	// for (int i = 0; i < fsize; i++) {
		// printf("%lx\n", string[i]);
	// }
	
	free(string);
	free(string2);
	

	// printf("%x\n", 0x420 >> 6);
	
	
	return 0;
}
