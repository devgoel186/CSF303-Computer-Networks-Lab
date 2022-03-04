/*
Name - Dev Goel
ID - 2019A7PS0236G
*/

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#define MAX 10000
#define PORT 4444

char *initial_logic(int new_socket)
{
  char buff[MAX];
  bzero(buff, sizeof(buff));

  char *name = (char *)malloc(100);

  strcpy(buff, "Hello, what is your name?\n");
  write(new_socket, buff, sizeof(buff));
  printf("S: %s", buff);
  bzero(buff, sizeof(buff));

  read(new_socket, buff, sizeof(buff));
  strcpy(name, buff);
  printf("C: %s", buff);

  if (buff[0] >= 'A' && buff[0] <= 'Z')
  {
    bzero(buff, sizeof(buff));
    strcpy(buff, "200 OK\n");
  }
  else
  {
    bzero(buff, sizeof(buff));
    strcpy(buff, "500 ERROR\n");
  }

  write(new_socket, buff, sizeof(buff));
  printf("S: %s", buff);

  return (char *)name;
}

// Function designed for chat between client and server.
void func(int new_socket)
{
  char name[100];
  char buff[MAX];
  int n;

  // Calling the initial_logic function for getting name and evaluating it
  strcpy(name, initial_logic(new_socket));

  // Infinite loop for chatting b/w client and server
  while (1)
  {
    bzero(buff, sizeof(buff));

    read(new_socket, buff, sizeof(buff));
    printf("C: %s", buff);

    if (strncmp(".", buff, 1) == 0)
    {
      bzero(buff, sizeof(buff));

      char x[100] = "Thank you";
      if (name[0] >= 'A' && name[0] <= 'Z')
      {
        strcat(x, ", ");
        strcat(x, name);
      }
      else
        strcat(x, "\n");

      strcpy(buff, x);
      write(new_socket, buff, sizeof(buff));

      printf("S: %s", buff);

      break;
    }
  }
}

// Driver function
int main()
{
  int server_fd, new_socket, len;
  struct sockaddr_in servaddr, cli;

  // Socket file descriptor
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == -1)
  {
    printf("Socket creation failed...\n");
    exit(EXIT_FAILURE);
  }

  bzero(&servaddr, sizeof(servaddr));

  // Assign IP and PORT
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(PORT);

  // Binding newly created socket to given IP
  if ((bind(server_fd, (struct sockaddr *)&servaddr, sizeof(servaddr))) != 0)
  {
    printf("Socket bind failed...\n");
    exit(EXIT_FAILURE);
  }

  // Now server is ready to listen and verification
  if ((listen(server_fd, 5)) != 0)
  {
    printf("Listen failed...\n");
    exit(EXIT_FAILURE);
  }

  len = sizeof(cli);

  // Accept the data packet from client and verification
  new_socket = accept(server_fd, (struct sockaddr *)&cli, &len);
  if (new_socket < 0)
  {
    printf("Server accept failed...\n");
    exit(EXIT_FAILURE);
  }

  // Function for chatting between client and server
  func(new_socket);

  // Closing the socket after successful communication
  close(server_fd);
}
