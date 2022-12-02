#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <syslog.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <errno.h>

#include "CommonData.h"
#include "Daemonizer.h"
#include "FileInfoMonitoring.h"

void PrintHelp(FILE* output) {
    fprintf(output, "Program requires at least one argument in the following order:\n"
                    "1. Path to the input file for monitoring;\n"
                    "2. -d option to daemonize the program;\n");
}

void InitializeLogging(const char* program_name) {
    openlog(program_name, LOG_CONS, LOG_DAEMON);
}

int ServerInit(char* kSocketPath) {
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) {
        syslog(LOG_ERR, "Failed to create a socket");
        return errno;
    }
    syslog(LOG_INFO, "Successfully created a socket.\n");

    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, kSocketPath);
    size_t addr_length = sizeof(addr.sun_family) + strlen(kSocketPath);

    unlink(kSocketPath);
    int bind_status = bind(fd, (struct sockaddr*)&addr, addr_length);
    if (bind_status < 0) {
        close(fd);
        syslog(LOG_ERR, "Failed to bind address to socket");
        return errno;
    }
    syslog(LOG_INFO, "Successfully binded address.\n");

    int listen_status = listen(fd, 1);
    if (listen_status < 0) {
        close(fd);
        syslog(LOG_ERR, "Failed to start listening the socket");
        return errno;
    }
    syslog(LOG_INFO, "Successfully started to listen input connections.\n");
	syslog(LOG_INFO, "Waiting for the client to connect...\n");

    return fd;
}

int main(int argc, char** argv) {
    if (argc != 2 && argc != 3) {
        fprintf(stderr, "Wrong number of input arguments.\n");
        PrintHelp(stderr);
        return 1;
    }
	
	int daemonize = 0;
    if (argc == 3) {
        if (strcmp(argv[2], "-d") != 0) {
            fprintf(stderr, "Invalid format of program arguments.\n");
            PrintHelp(stderr);
            return 1;
        }
        daemonize = 1;
    }

	char kSocketPath[MAX_BUFF_SIZE];
	char* filename = CONF_FILENAME;
	FILE* fp = fopen(filename,"r");
	
	if (fp == NULL)
    {
        printf("Error: could not open file %s\n", filename);
        return 1;
    }
	
    fgets(kSocketPath, MAX_BUFF_SIZE, fp);
    fclose(fp);

    InitializeLogging(argv[0]);
    if (daemonize) {
        Daemonize();
        syslog(LOG_INFO, "MyDaemon has started in daemon mode on PID: %d", getpid());
    } else {
        syslog(LOG_INFO, "MyDaemon has started in simple mode on PID: %d", getpid());
    }

    int server_fd = ServerInit(kSocketPath);
    const char* file_path = argv[1];
    StartMonitoring(file_path, server_fd);

    close(PIDFileFD);
    close(server_fd);
    return 0;
}
