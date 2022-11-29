CC ?= gcc
CFLAGS += -Wall -Wextra -Wpedantic -std=c11
LDFLAGS += -I/usr/include/postgresql -lpq -lm
RM += 

all: main
	
main: src/main.c
	$(CC) $(CFLAGS) src/main.c -o main $(LDFLAGS)

.PHONY: clean
clean:
	$(RM) main