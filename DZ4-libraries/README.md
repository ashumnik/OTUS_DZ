# DZ4-libraries

To build the weather program, you need to install the following libraries

## libcurl

$ apt-get install libcurl4-gnutls-dev

## libjson-c

### Build instructions:
$ git clone https://github.com/json-c/json-c.git
$ mkdir json-c-build
$ cd json-c-build
$ cmake ../json-c   # See CMake section below for custom arguments

$ make
$ make test
$ make USE_VALGRIND=0 test   # optionally skip using valgrind
$ make install

### Linking to libjson-c

JSON_C_DIR=/path/to/json_c/install
CFLAGS += -I$(JSON_C_DIR)/include/json-c
LDFLAGS+= -L$(JSON_C_DIR)/lib -ljson-c

### build

cd weather
make