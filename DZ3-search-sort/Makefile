CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c11


all: src/hasht.c src/hash_table.c
	$(CC) $(CFLAGS) src/hasht.c src/hash_table.c -o main

clean:
	rm main