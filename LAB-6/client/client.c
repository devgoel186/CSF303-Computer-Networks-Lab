/*
Name - Dev Goel
ID - 2019A7PS0236G
*/

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define MAX 10000

void receiveFile(int new_socket, char *filename)
{
  char buff[MAX]; // Declaring a buffer for reading and writing file contents and filenames
  bzero(buff, sizeof(buff));

  strcpy(buff, filename);

  write(new_socket, buff, sizeof(buff));
  bzero(buff, sizeof(buff));

  read(new_socket, buff, sizeof(buff));
  printf("File contents received -> %s\n", buff);

  FILE *fptr;

  fptr = fopen(filename, "w+"); // Opening file for storing data
  fputs(buff, fptr);

  fclose(fptr);
}

int main(int argc, char **argv)
{
  int sockfd, connfd;
  struct sockaddr_in servaddr, cli;

  char *p_server_address = "", *p_filename = "";
  int p_server_port = 8000;

  if (argc == 4)
  {
    p_server_address = argv[1];
    p_server_port = atoi(argv[2]);
    p_filename = argv[3];
  }

  // In case of insufficient / excess arguments
  else
  {
    printf("Invalid input parameters\n");
    exit(EXIT_FAILURE);
  }

  // Socket creation and verification
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1)
  {
    printf("Socket creation failed...\n");
    exit(EXIT_FAILURE);
  }

  bzero(&servaddr, sizeof(servaddr));

  // Assign IP and PORT
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr(p_server_address);
  servaddr.sin_port = htons(p_server_port);

  // Connect the client socket to server socket
  if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0)
  {
    printf("Server is unreachable. Unable to connect.\n");
    exit(EXIT_FAILURE);
  }
  else
    printf("Connected to Server -> %s, at Port -> %d\n", p_server_address, p_server_port);

  receiveFile(sockfd, p_filename);

  // Close the socket after successful communication
  close(sockfd);
}
