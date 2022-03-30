/*
Name - Dev Goel
ID - 2019A7PS0236G
*/

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <string.h>

int ReadHttpStatus(int sock)
{
  char c;
  char buff[1024] = "", *ptr = buff + 1;

  int bytes_received, status;

  while (bytes_received = recv(sock, ptr, 1, 0))
  {
    if (bytes_received == -1)
    {
      perror("ReadHttpStatus");
      exit(1);
    }

    if ((ptr[-1] == '\r') && (*ptr == '\n'))
      break;
    ptr++;
  }

  *ptr = 0;
  ptr = buff + 1;

  sscanf(ptr, "%*s %d ", &status);

  printf("%s\n", ptr);
  printf("status = %d\n\n", status);

  return (bytes_received > 0) ? status : 0;
}

int ParseHeader(int sock)
{
  char c;
  char buff[1024] = "", *ptr = buff + 4;
  int bytes_received, status;

  while (bytes_received = recv(sock, ptr, 1, 0))
  {
    if (bytes_received == -1)
    {
      perror("Parse Header");
      exit(1);
    }

    if (
        (ptr[-3] == '\r') && (ptr[-2] == '\n') &&
        (ptr[-1] == '\r') && (*ptr == '\n'))
      break;

    ptr++;
  }

  *ptr = 0;
  ptr = buff + 4;

  if (bytes_received)
  {
    ptr = strstr(ptr, "Content-Length:");
    if (ptr)
    {
      sscanf(ptr, "%*s %d", &bytes_received);
    }
    else
      bytes_received = -1;

    printf("Content-Length: %d\n", bytes_received);
  }

  return bytes_received;
}

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    printf("Incorrect number of input arguments\n");
    printf("Syntax : ./client <url>\n");
    exit(EXIT_FAILURE);
  }

  char protocol[25];
  char domain[100];
  char path[100];

  sscanf(argv[1], "%[^:]://%99[^/]/%99[^\n]", protocol, domain, path);
  printf("Domain: %s\n", domain);
  printf("Path: %s\n\n", path);

  int sock, bytes_received;
  char send_data[1024], recv_data[1024], *p;

  struct sockaddr_in server_addr;
  struct hostent *hostip;

  hostip = gethostbyname(domain);

  if (hostip == NULL)
  {
    herror("ERROR -> gethostbyname");
    exit(EXIT_FAILURE);
  }

  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    perror("ERROR -> Socket");
    exit(EXIT_FAILURE);
  }

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(80);
  server_addr.sin_addr = *((struct in_addr *)hostip->h_addr_list[0]);
  bzero(&(server_addr.sin_zero), 8);

  if (connect(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
  {
    perror("ERROR -> Connect");
    exit(EXIT_FAILURE);
  }

  snprintf(send_data, sizeof(send_data), "GET /%s HTTP/1.1\r\nHost: %s\r\n\r\n", path, domain);

  if (send(sock, send_data, strlen(send_data), 0) == -1)
  {
    perror("ERROR -> send");
    exit(EXIT_FAILURE);
  }

  int contentlength;
  if (ReadHttpStatus(sock) && (contentlength = ParseHeader(sock)))
  {
    int bytes = 0;

    char *filename = basename(path);

    FILE *fd = fopen(filename, "wb");
    printf("Saving data...\n\n");

    while (bytes_received = recv(sock, recv_data, 1024, 0))
    {
      if (bytes_received == -1)
      {
        perror("Recieve");
        exit(EXIT_FAILURE);
      }

      fwrite(recv_data, 1, bytes_received, fd);
      bytes += bytes_received;
      printf("No. of bytes received: %d from %d\n", bytes, contentlength);
      if (bytes == contentlength)
        break;
    }

    fclose(fd);
  }

  close(sock);
  printf("\nDone.\n");
  return 0;
}