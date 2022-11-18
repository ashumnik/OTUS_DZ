CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c11


all: src/log_lib.c
	$(CC) $(CFLAGS) -c src/log_lib.c -o src/log_lib.o
	ar cr src/liblog_lib.a src/log_lib.o
	$(CC) $(CFLAGS) test/test.c -L. src/liblog_lib.a -o main

clean:
	rm main src/log_lib.o src/liblog_lib.a
