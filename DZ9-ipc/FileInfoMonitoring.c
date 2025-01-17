#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <syslog.h>
#include <errno.h>
#include <string.h>

#include "FileInfoMonitoring.h"
#include "CommonData.h"

ssize_t GetFileSize(const char* file_path) {
    struct stat file_stat;
    if (stat(file_path, &file_stat) != 0) {
        syslog(LOG_CRIT, "Failed to get file stats for file `%s`. Errno: %d", file_path, errno);
        exit(EXIT_FAILURE);
    }
    return (ssize_t)file_stat.st_size;
}

void SendMessageToClient(int fd, file_size_t current_file_size) {
    static char buffer[MAX_MSG_LENGTH];
    memcpy(buffer, (void*)&current_file_size, sizeof(current_file_size));

    int length = sizeof(current_file_size);
    int bytes_sent = send(fd, buffer, length, 0);

    if (bytes_sent < 0) {
        close(fd);
        syslog(LOG_CRIT, "Failed to send message to server.");
        exit(EXIT_FAILURE);
    }

    if (bytes_sent != length) {
        close(fd);
        syslog(LOG_CRIT, "Last message wasn't sent completely. Disconnecting...");
        exit(EXIT_FAILURE);
    }

    syslog(LOG_INFO, "Successfully sent %d bytes to server.\n", length);
}

int StartMonitoring(const char* file_path, int server_fd) {
    int fd = inotify_init();
    if (fd < 0) {
        close(server_fd);
        syslog(LOG_CRIT, "Failed to initialize inotify.");
        exit(EXIT_FAILURE);
    }

    int wd = inotify_add_watch(fd, file_path, IN_DELETE | IN_MODIFY);
    if (wd == -1) {
        close(server_fd);
        syslog(LOG_CRIT, "Failed to add watch to inotify.");
        exit(EXIT_FAILURE);
    }

    syslog(LOG_INFO, "Started monitoring the file: `%s`", file_path);

    ssize_t last_file_size = GetFileSize(file_path);
    int len;
	char buffer[MAX_MSG_LENGTH];
	struct pollfd fds[2];
	
	fds[0].fd = server_fd;
	fds[0].events = POLLIN;
	fds[1].fd = fd;
	fds[1].events = POLLIN;
	
	while(1) {
		int ret = poll( (struct pollfd *)&fds, 2, 100 );

		if ( ret == -1 )
			syslog(LOG_CRIT, "Failed to poll.");
		// else if ( ret == 0 )
			// syslog(LOG_CRIT, "Nothing happend.");
		else
		{
			if ( fds[0].revents & POLLIN ) {
				fds[0].revents = 0;
				int server_wd = accept(server_fd, NULL, NULL);
				if (server_wd < 0) {
					close(server_fd);
					syslog(LOG_ERR, "Failed to accept the client connection");
					return errno;
				}
				syslog(LOG_INFO, "Client was successfully connected with file descriptor = %d.\n", server_wd);
				SendMessageToClient(server_wd, last_file_size);

				close(server_wd);
			}

			if ( fds[1].revents & POLLIN ) {
				fds[1].revents = 0;
				len = read(fd, buffer, MAX_MSG_LENGTH);
				if (len < 0) {
					close(server_fd);
					syslog(LOG_CRIT, "Failed to read data from inotify file descriptor. Errno: %d", errno);
					exit(EXIT_FAILURE);
				}

				int pos = 0;
				while (pos < len) {
					struct inotify_event* event = (struct inotify_event*)(&buffer[pos]);
					if (event->mask & (IN_MODIFY | IN_DELETE )) {
						ssize_t new_file_size = GetFileSize(file_path);
						if (new_file_size != last_file_size) {
							syslog(LOG_INFO, "File '%s' was changed, new file size is = %ld", file_path, new_file_size);
							last_file_size = new_file_size;
						}
					} else {
						syslog(LOG_WARNING, "Undefined event from inotify, mask = %d", event->mask);
					}
					if (event->len) {
						pos += event->len;
					}
					pos += sizeof(struct inotify_event);
				}
			}
		}
	}

    inotify_rm_watch(fd, wd);
    close(fd);
}
