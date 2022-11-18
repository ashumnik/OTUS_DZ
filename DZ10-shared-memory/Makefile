CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c11


all: src/main.c src/crc32.c
	$(CC) $(CFLAGS) src/main.c src/crc32.c -o main

clean:
	rm main