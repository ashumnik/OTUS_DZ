#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "Utils.h"
#include "HTTP.h"

void PrintHelp(FILE* output) {
    fprintf(output, "Program requires at least 3 input argument in the following order:\n"
                    "1. Listening 'address:port';\n"
                    "2. -f option to receive a single file or -l option to receive a list of files;\n"
                    "3. Single file name or file path (if -l specified the corresponding file must have a list of files names);\n"
                    "4 (optional). Output directory path;");
}

void ValidateInput(const char* mode, const char* file_path) {
    if (strcmp(mode, "-f") != 0 && strcmp(mode, "-l") != 0) {
        fprintf(stderr, "Invalid file mode.\n");
        PrintHelp(stderr);
        exit(EXIT_FAILURE);
    }
    if (strcmp(mode, "-f") == 0 && file_path[0] != '/') {
        fprintf(stderr, "Wrong format of file path: it must start with '/'\n");
        exit(EXIT_FAILURE);
    }
}

char* GenerateClientID() {
    char buffer[10];
    snprintf(buffer, 10, "%d", getpid());
    size_t len = strlen(buffer);

    char* result = (char*)malloc(len + 1);
    strncpy(result, buffer, len + 1);
    return result;
}

void CreateOutputDirectory(const char* output_directory_path) {
    if (mkdir(output_directory_path, S_IRWXU) < 0) {
        if (errno == EEXIST) {
            struct stat stats;
            if (stat(output_directory_path, &stats) < 0) {
                perror("Failed to get output directory stats");
                exit(errno);
            }
            if (!(stats.st_mode & S_IFDIR)) {
                fprintf(stderr, "Output directory path already exists and isn't a directory.\n");
                exit(EXIT_FAILURE);
            } else {
                return;
            }
        }
        perror("Failed to create output directory");
        exit(errno);
    }
}

int ConnectToServer(const char* address, uint32_t port) {
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd < 0) {
        perror("Failed to create a socket");
        exit(errno);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(address);

    if (connect(sfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Failed to connect to server");
        close(sfd);
        exit(errno);
    }

    printf("Client has successfully connected to server.\n");

    return sfd;
}

void SendRequest(int sfd, const char* request) {
    printf("Sending HTTP request to server: '%s'\n", request);

    ssize_t status = send(sfd, request, strlen(request), 0);
    if (status < 0) {
        perror("Failed to send a request to server");
        close(sfd);
        exit(EXIT_FAILURE);
    }

    if (status == 0) {
        fprintf(stderr, "Failed to send a request. Connection to server was closed.\n");
        exit(EXIT_FAILURE);
    }

    printf("Request was sent successfully! (%ld bytes sent)\n", status);
}

int Receive(int sfd, char* buffer, size_t buffer_len) {
    ssize_t recv_len = recv(sfd, buffer, buffer_len, 0);

    if (recv_len < 0) {
        perror("Failed to receive a response from server");
        close(sfd);
        exit(EXIT_FAILURE);
    }

    return (int)recv_len;
}

int ReceiveResponse(int sfd, FILE* output_file) {
    int recv_len;
    char buffer[MAX_RESPONSE_LEN + 1];

    printf("Receiving server response...\n");

    Response* response = NULL;
    int last_line_start = 0;
    int header_len = 0;
    int lines_read = 0;
    while (Receive(sfd, buffer + header_len, 1)) {
        if (header_len && buffer[header_len - 1] == '\r' && buffer[header_len] == '\n') {
            if (last_line_start == header_len - 1) {
                break;
            }

            if (lines_read == 0) {
                response = ParseResponse(buffer + last_line_start);
                if (response == NULL) {
                    fprintf(stderr, "Failed to parse response status: `%s`\n", buffer);
                    return -1;
                }
            } else {
                ParseHeader(buffer + last_line_start, response);
            }
            last_line_start = header_len + 1;
            ++lines_read;
        }
        ++header_len;
    }

    printf("Successfully received a response with status code = `%s`, status message = `%s`\n"
           "Content-Length: %ld\n",
           response->status_code, response->status_message, response->content_length);
    if (strcmp(response->status_code, "200") != 0) {
        free(response);
        return -1;
    }

    printf("Receiving file from server...\n");

    ssize_t total_bytes_received = 0;
    while ((recv_len = Receive(sfd, buffer, MAX_RESPONSE_LEN))) {
        fwrite(buffer, 1, recv_len, output_file);
        total_bytes_received += recv_len;
    }

    if (total_bytes_received != response->content_length) {
        fprintf(stderr, "File wasn't received successfully from server\n");
        free(response);
        return -1;
    }

    free(response);
    return 0;
}

void GetFile(int sfd, const char* file_path, const char* output_dir_path) {
    static char request[MAX_REQUEST_LEN];
    snprintf(request, MAX_REQUEST_LEN, "GET %s HTTP/1.0\n", file_path);

    SendRequest(sfd, request);

    char* output_file_path = ConcatenateStrings(output_dir_path, file_path);
    FILE* output_file = fopen(output_file_path, "w");
    if (output_file == NULL) {
        perror("Failed to open output file");
        exit(errno);
    }

    int status = ReceiveResponse(sfd, output_file);
    if (status < 0) {
        fprintf(stderr, "Failed to receive the response from server\n");
    } else {
        printf("Successfully received file from server\n");
    }

    fclose(output_file);
    free(output_file_path);
}

void RunClient(const char* directory_path, const char* address, uint32_t port, const char* file_path) {
    printf("Running client...\n");

    int sfd = ConnectToServer(address, port);

    GetFile(sfd, file_path, directory_path);

    close(sfd);

    sleep(1);
}

int main(int argc, char** argv) {
    if (argc != 4 && argc != 5) {
        fprintf(stderr, "Wrong number of input arguments.\n");
        PrintHelp(stderr);
        return 1;
    }

    char* address = NULL;
    uint32_t port = -1;
    ParseAddressAndPort(argv[1], &address, &port);

    const char* mode = argv[2];
    const char* file = argv[3];
    ValidateInput(mode, file);

    printf("Server address = %s, port = %u\n", address, port);
    if (strcmp(mode, "-f") == 0) {
        printf("Mode: receive single file (-f)\n");
        printf("File name: %s\n", file);
    } else {
        printf("Mode: receive list of files (-l)\n");
        printf("File with file names: %s\n", file);
    }

    char* output_directory_path;
    if (argc == 5) {
        size_t len = strlen(argv[4]) + 1;
        output_directory_path = (char*)malloc(len);
        strncpy(output_directory_path, argv[4], len);
    } else {
        char* client_id = GenerateClientID();
        printf("Generated client id (the same as PID): %s\n", client_id);
        output_directory_path = ConcatenateStrings("./", client_id);
        free(client_id);
    }
    CreateOutputDirectory(output_directory_path);
    printf("Output directory path: %s\n", output_directory_path);

    if (strcmp(mode, "-f") == 0) {
        RunClient(output_directory_path, address, port, file);
    } else {
        FILE* file_with_file_names = fopen(file, "r");
        if (file_with_file_names == NULL) {
            fprintf(stderr, "Failed to open input file.\n");
            exit(EXIT_FAILURE);
        }

        char* file_name = NULL;
        size_t file_name_len = 0;
        ssize_t len;
        while ((len = getline(&file_name, &file_name_len, file_with_file_names)) > 0) {
            --len;
            file_name[len] = '\0';
            if (len > 0) {
                printf("File name: %s\n", file_name);
                RunClient(output_directory_path, address, port, file_name);
            }
        }

        free(file_name);
        fclose(file_with_file_names);
    }

    free(output_directory_path);
    free(address);
    return 0;
}
