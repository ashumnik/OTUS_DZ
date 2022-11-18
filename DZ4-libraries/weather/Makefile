CC= gcc
CFLAGS= -Wall -Wextra -Wpedantic -std=c11
LDFLAGS+= -lcurl

JSON_C_DIR=/usr/local
CFLAGS += -I$(JSON_C_DIR)/include/json-c
LDFLAGS+= -L$(JSON_C_DIR)/lib -ljson-c

all: src/weather.c
	$(CC) $(CFLAGS) src/weather.c $(LDFLAGS) -o main

clean:
	rm main