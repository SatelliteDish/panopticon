#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define SOCKET_PATH "/tmp/panopticon.sock"

int main(int argc, char *argv[]) {
  int client_fd;
  struct sockaddr_un address;

  if (argc < 2) {
    fprintf(stderr, "Usage: %s [command]\n", argv[0]);
    return 1;
  }

  // Create socket
  if ((client_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
    perror("Socket creation failed");
    return 1;
  }

  // Configure the socket address
  memset(&address, 0, sizeof(address));
  address.sun_family = AF_UNIX;
  strncpy(address.sun_path, SOCKET_PATH, sizeof(address.sun_path) - 1);

  // Connect to the daemon
  if (connect(client_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
    perror("Socket connect failed");
    return 1;
  }

  // Send the command
  // Concatenate all arguments except argv[0]
  char command[256] = {0};
  for (int i = 1; i < argc; i++) {
    strcat(command, argv[i]);
    if (i < argc - 1) {
      strcat(command, " "); // Add a space between arguments
    }
  }
  if (write(client_fd, command, strlen(command)) == -1) {
    perror("Socket write failed");
    return 1;
  }

  close(client_fd);
  return 0;
}

