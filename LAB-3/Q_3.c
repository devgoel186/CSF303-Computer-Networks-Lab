#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#define PORT 4444

int main(int argc, char const *argv[])
{
  int server_fd, new_sock;
  long valread;
  struct sockaddr_in address;
  int addrlen = sizeof(address);

  char *hello = "Hello World";

  // Creating socket file descriptor
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
  {
    perror("Socket Failed");
    exit(EXIT_FAILURE);
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  memset(address.sin_zero, '\0', sizeof(address.sin_zero));

  // Attaching socket to the port 4444
  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
  {
    perror("Bind Failed");
    exit(EXIT_FAILURE);
  }

  // Listening to socket
  if (listen(server_fd, 10) < 0)
  {
    perror("Listen");
    exit(EXIT_FAILURE);
  }

  while (1)
  {
    printf("\nWaiting for Connection\n");

    if ((new_sock = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
    {
      perror("Accept");
      exit(EXIT_FAILURE);
    }

    char buffer[30000] = {0};
    valread = read(new_sock, buffer, 30000);
    printf("%s\n", buffer);

    write(new_sock, hello, strlen(hello));
    printf("Hello message sent to browser");

    close(new_sock);
  }

  return 0;
}