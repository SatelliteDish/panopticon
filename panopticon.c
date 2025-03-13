#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "themeSwitcher.h"
#include "logger.h"

#define SOCKET_PATH "/tmp/panopticon.sock"
#define BUFFER_SIZE 256

void handle_command(char *args) {
  char* command = strtok(args, " ");

  if (strcmp(command, "status") == 0) {
    Logger_log("Daemon status: Running\n");
  } else if (strcmp(command, "stop") == 0) {
    Logger_log("Stopping daemon...\n");
    unlink(SOCKET_PATH);
    exit(0);
  } else if(strcmp(command, "theme") == 0) {
    char* name = strtok(NULL, " ");

    if( name == NULL ) {
      return;
    } else if ( strcmp(name, "next") == 0 ) {
      printf("next theme\n");
      next_theme();
    } else if ( strcmp(name, "prev") == 0 ) {
      printf("prev theme\n");
      previous_theme();
    } else if ( strcmp(name, "mode") == 0 ) {
      char* direction = strtok(NULL, " ");

      if( direction == NULL ) {
        return;
      }

      if ( strcmp(direction, "next") == 0 ) {
        printf("next mode\n");
        next_mode();
      } else if ( strcmp(direction, "prev") == 0 ) {
        printf("prev mode\n");
        previous_mode();
      }
    }
    changeTheme(name);

  } else {
    char buff[64];
    sprintf(buff, "Unknown command: %s\n", command);
    Logger_log( buff );
  }
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_un address;
    char buffer[BUFFER_SIZE];

    // Create socket
    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        return 1;
    }

    // Configure the socket address
    memset(&address, 0, sizeof(address));
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, SOCKET_PATH, sizeof(address.sun_path) - 1);

    // Remove existing socket file
    unlink(SOCKET_PATH);

    // Bind socket
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
        perror("Socket bind failed");
        return 1;
    }

    // Listen for incoming connections
    if (listen(server_fd, 5) == -1) {
        perror("Socket listen failed");
        return 1;
    }

    printf("Daemon running, waiting for commands...\n");

    // Accept and handle commands
    while (1) {
        if ((client_fd = accept(server_fd, NULL, NULL)) == -1) {
            perror("Socket accept failed");
            continue;
        }

        // Read command from the client
        int read_bytes = read(client_fd, buffer, BUFFER_SIZE - 1);
        if (read_bytes > 0) {
            buffer[read_bytes] = '\0';
            handle_command(buffer);
        }

        close(client_fd);
    }

    close(server_fd);
    unlink(SOCKET_PATH);
    return 0;
}

