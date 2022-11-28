project(telnet C)

set(SRC src/telnet.h)

add_executable(telnet src/main.c ${SRC})
