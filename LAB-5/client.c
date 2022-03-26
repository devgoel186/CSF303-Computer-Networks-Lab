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
#define PORT 4444

int initial_logic(int sockfd)
{
  char buff[MAX];
  int flag = 0;
  int n = 0;

  bzero(buff, MAX);

  read(sockfd, buff, sizeof(buff));
  printf("S: %sC: ", buff);
  bzero(buff, MAX);

  while ((buff[n++] = getchar()) != '\n')
    ;

  write(sockfd, buff, sizeof(buff));
  bzero(buff, sizeof(buff));

  read(sockfd, buff, sizeof(buff));
  printf("S: %s", buff);

  if (strncmp(buff, "500 ERROR", 9) == 0)
  {
    printf("C: Error Received\n");

    bzero(buff, sizeof(buff));
    strcpy(buff, ".\n");
    write(sockfd, buff, sizeof(buff));
    printf("C: %s", buff);

    bzero(buff, sizeof(buff));
    read(sockfd, buff, sizeof(buff));
    printf("S: %s", buff);

    flag = 1;
  }

  return flag;
}

void func(int sockfd)
{
  int flag = initial_logic(sockfd);
  if (flag)
    return;

  char buff[MAX];
  int n;

  while (1)
  {
    bzero(buff, sizeof(buff));
    n = 0;

    printf("C: ");
    while ((buff[n++] = getchar()) != '\n')
      ;
    write(sockfd, buff, sizeof(buff));

    if (strncmp(buff, ".", 1) == 0)
    {
      bzero(buff, sizeof(buff));
      read(sockfd, buff, sizeof(buff));
      printf("S: %s", buff);

      break;
    }
  }
}

int main()
{
  int sockfd, connfd;
  struct sockaddr_in servaddr, cli;

  // Socket creation and verification
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1)
  {
    printf("Socket creation failed...\n");
    exit(0);
  }

  bzero(&servaddr, sizeof(servaddr));

  // Assign IP and PORT
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  servaddr.sin_port = htons(PORT);

  // Connect the client socket to server socket
  if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0)
  {
    printf("Connection with the server failed...\n");
    exit(0);
  }

  // Function for chatting
  func(sockfd);

  // Close the socket
  close(sockfd);
}
