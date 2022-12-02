#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>

#include "CommonData.h"

int main(int argc, char** argv) {
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
	
    if (fd < 0) {
        perror("Failed to create a socket");
        return errno;
    }
	
	char kSocketPath[MAX_BUFF_SIZE];
	char *filename = CONF_FILENAME;
	FILE *fp = fopen(filename,"r");
	
	if (fp == NULL)
    {
        printf("Error: could not open file %s\n", filename);
        return 1;
    }

    fgets(kSocketPath, MAX_BUFF_SIZE, fp);
    fclose(fp);

    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, kSocketPath, strlen(kSocketPath));
    size_t addr_length = sizeof(addr.sun_family) + strlen(kSocketPath);

    int status = connect(fd, (struct sockaddr*)&addr, addr_length);
    if (status < 0) {
        close(fd);
        perror("Failed to connect to server");
        return errno;
    }

    printf("Successfully connected to server.\n");

    char buffer[MAX_MSG_LENGTH];
    file_size_t sample_file_size = 0;

	int bytes_received = recv(fd, buffer, sizeof(buffer), 0);
	if (bytes_received < 0) {
		close(fd);
		perror("Failed to receive the message from client");
		return errno;
	}

	if (bytes_received == 0) {
		printf("Connection to client was closed.\n");
	}

	int bytes_read = 0;
	while (bytes_read < bytes_received) {
		if (bytes_read + (int)sizeof(ssize_t) > bytes_received) {
			fprintf(stderr, "Received message in incorrect format.\n");
			return 1;
		}

		const ssize_t* file_size = (const ssize_t*)&buffer[bytes_read];
		printf("File size = %ld bytes.\n", *file_size);
		bytes_read += sizeof(file_size_t);
	}

    close(fd);
    return 0;
}
