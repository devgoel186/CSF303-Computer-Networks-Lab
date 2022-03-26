/*
Name - Dev Goel
ID - 2019A7PS0236G
*/

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>

#define MAX 10000

char *initial_logic(int new_socket)
{
  char buff[MAX]; // Declaring a buffer for reading and writing file contents and filenames
  bzero(buff, sizeof(buff));

  char *filename = (char *)malloc(100);

  read(new_socket, buff, sizeof(buff));
  strcpy(filename, buff);

  return (char *)filename;
}

void readFile(int new_socket)
{
  char buff[MAX];
  bzero(buff, sizeof(buff));
  char ch;
  char filename[100];

  strcpy(filename, initial_logic(new_socket));
  printf("File requested by client -> %s\n", filename);

  FILE *fptr;

  // Opening requested file for reading contents
  if ((fptr = fopen(filename, "r")) == NULL)
  {
    printf("No such file exists.\n");
    write(new_socket, buff, sizeof(buff));
    exit(EXIT_FAILURE);
  }

  printf("Sending first 10 bytes...\n");

  int count = 0;
  ch = fgetc(fptr);

  while ((ch != EOF) && (count < 10))
  {
    buff[count] = ch;
    ch = fgetc(fptr);
    count++;
  }

  write(new_socket, buff, sizeof(buff));
  bzero(buff, sizeof(buff));

  fclose(fptr);
}

// Driver function
int main(int argc, char **argv)
{
  int server_fd, new_socket, len;
  struct sockaddr_in servaddr, cli;

  int p_server_port = 8000;

  if (argc == 2)
  {
    p_server_port = atoi(argv[1]);
  }

  else
  {
    if (argc < 2)
      printf("Insufficient input parameters\n");
    else
      printf("Too many input parameters\n");
    exit(EXIT_FAILURE);
  }

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
  servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  servaddr.sin_port = htons(p_server_port);

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
  else
    printf("Client connected at PORT -> %d\n", p_server_port);

  // Function for reading the file requested by client
  readFile(new_socket);

  // Closing the socket after successful communication
  close(server_fd);
}