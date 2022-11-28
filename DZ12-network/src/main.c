#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <string.h>
#include <stdbool.h>

#include "telnet.h"

const char* kFigletStr = "figlet";
const char* kTelehackName = "telehack.com";
const size_t kTelnetPort = 23;
char buffer[MAX_BUFFER_LEN];

char* combine_input_words(char** words, int n) 
{
    if (n < 0) 
	{
        return NULL;
    }

    size_t total_length = 0;
    total_length += n ? n - 1 : 0;
    for (int i = 0; i < n; ++i) 
	{
        total_length += strlen(words[i]);
    }

    char* result = (char*)malloc(total_length + 1);
    size_t last_pos = 0;
    for (int i = 0; i < n; ++i) 
	{
        if (i) 
		{
            result[last_pos++] = ' ';
        }
        snprintf(result + last_pos, total_length + 1 - last_pos, "%s", words[i]);
        last_pos += strlen(words[i]);
    }
    result[last_pos] = '\0';

    return result;
}

bool server_waits_for_new_command(const char* str, ssize_t len) 
{
    if (len <= 0) 
	{
        return false;
    }
    if (len == 1) 
	{
        return str[len - 1] == '.';
    }
    return str[len - 2] == '\n' && str[len - 1] == '.';
}

void read_start_message(int fd) 
{
    do 
	{
        ssize_t len = recv(fd, buffer, MAX_BUFFER_LEN, 0);
        if (len < 0) 
		{
            perror("Failed to receive data from server");
            exit(errno);
        }
        if (len == 0) 
		{
            fprintf(stderr, "Connection was closed");
            exit(EXIT_FAILURE);
        }
        if (server_waits_for_new_command(buffer, len)) 
		{
            break;
        }
    } while (1);

    printf("Successfully read start message from server\n\n");
}

void send_figlet_command(int fd, const char* font, const char* text) 
{
    const size_t command_str_len = strlen(kFigletStr) + 2 + strlen(font) + 1 + strlen(text) + 1;
    char* command_str = (char*)(malloc(command_str_len));
    snprintf(command_str, command_str_len, "%s %c%s %s", kFigletStr, '/', font, text);
    printf("Command: \"%s\"\n", command_str);

    const unsigned char command_prefix[] = 
	{
		IAC, DONT, SUPPRESS, IAC, WILL, SUPPRESS,
        IAC, DONT, ECHO_C, IAC, WILL, ECHO_C,
        13, 10, '\0'
	};
    const unsigned char command_suffix[] = 
	{
		13, 10, '\0'
	};

    snprintf(buffer, MAX_BUFFER_LEN, "%s%s%s", command_prefix, command_str, command_suffix);

    const size_t cmd_len = strlen(buffer);

    ssize_t len = send(fd, buffer, cmd_len + 1, 0);
    if (len < 0) 
	{
        perror("Failed to send data to server");
        exit(errno);
    }
    if (len == 0) 
	{
        fprintf(stderr, "Connection was closed");
        exit(EXIT_FAILURE);
    }

    printf("Successfully sent the command to the server\n");

    free(command_str);
}

void read_result_response(int fd) 
{
    do 
	{
        ssize_t len = recv(fd, buffer, MAX_BUFFER_LEN, 0);
        if (len < 0) 
		{
            perror("Failed to receive data from server");
            exit(errno);
        }
        if (len == 0) 
		{
            fprintf(stderr, "Connection was closed");
            exit(EXIT_FAILURE);
        }
        if (server_waits_for_new_command(buffer, len)) 
		{
            buffer[len - 1] = '\0';
            printf("%s", buffer);
            break;
        }
        buffer[len] = '\0';
        printf("%s", buffer);
    } while (1);
}

int usage()
{
	fprintf(stderr, "Wrong number of input arguments.\n"
                    "Program requires 2 input arguments in the following order:\n"
                    "- text font;\n"
                    "- input text (it might be separated in individual words);\n");

	return 1;
}

int main(int argc, char** argv) 
{
    
	if (argc < 3)
		return usage(argc);

    const char* font = argv[1];
    char* input_text = combine_input_words(argv + 2, argc - 2);

    printf("Font: \"%s\"\n", font);
    printf("Input text: \"%s\"\n", input_text);
	
	struct hostent *hostnm;
	if ((hostnm = gethostbyname(kTelehackName)) == NULL) {
		perror("Gethostbyname failed");
		exit(errno);
	}
	
	struct sockaddr_in server = {
        .sin_family = AF_INET,
        .sin_port  = htons(kTelnetPort),
        .sin_addr.s_addr = *((unsigned long *) hostnm->h_addr_list[0]),
	};

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
	{
        perror("Failed to create a socket");
        exit(errno);
    }
	
    if (connect(fd, (struct sockaddr *) &server, sizeof(server)) < 0) {
        perror("Connect error");
        exit(errno);
    }
    printf("Connecting to server %s\n", inet_ntoa(server.sin_addr));

    read_start_message(fd);
    send_figlet_command(fd, font, input_text);
    read_result_response(fd);
	
    free(input_text);
    return 0;
}
