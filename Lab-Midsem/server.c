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

void initial_logic(int new_socket)
{
  char buff[MAX]; // Declaring a buffer for reading and writing file contents and filenames
  bzero(buff, sizeof(buff));

  char *name = (char *)malloc(100);

  read(new_socket, buff, sizeof(buff));
  strcpy(name, buff);
  printf("C: %s", buff);
  bzero(buff, sizeof(buff));

  long id = strtol(name, NULL, 10);
  long y = (id % ((id % 599) + (id % 599)) / 3) + 98;
  printf("S: %ld %ld", id, y);

  FILE *fptr = fopen("math.txt", "r");
  char *lineptr = NULL;
  size_t len, read;

  int count = 0;

  // Opening requested file for reading contents
  if (fptr == NULL)
  {
    printf("No such file exists.\n");
    write(new_socket, buff, sizeof(buff));
    exit(EXIT_FAILURE);
  }

  else
  {
    while((read = getdelim(&lineptr, &len, ';', fptr)) != -1 && count < y) {
      count++;
      printf("%s\n", lineptr);
    }

    printf("Found - %s\n", lineptr);
  }

  long x = 0, x2 = 0;
  char a1[10];
  char a2[10];
  int i;

  for(i = 0; i < sizeof(lineptr); i++)
  {
    if(!(lineptr[i] >= '0' && lineptr[i] <= '9'))
      break;
  }

  int temp = i;

  char *token = strtok(lineptr, ";");
  strcpy(a1, token);
  token = strtok(NULL, ";");
  token = strtok(lineptr, ";");
  strcpy(a2, token);

  x = strtol(a1, NULL, 10);
  x2 = strtol(a2, NULL, 10);

  long final_ans = 0;

  if(lineptr[temp] == '+')
    final_ans = x + x2;
  if(lineptr[temp] == '-')
    final_ans = x - x2;
  if(lineptr[temp] == '*')
    final_ans = x * x2;
  if(lineptr[temp] == '/')
    final_ans = x / x2;

  printf("Answer = %ld", final_ans);

  bzero(buff, sizeof(buff));
  char answer_string[20];
  sprintf(answer_string, "%ld", final_ans);
  strcpy(buff, answer_string);
  write(new_socket, buff, sizeof(buff));
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
  initial_logic(new_socket);

  // Closing the socket after successful communication
  close(server_fd);
}