#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "common.h"

#define LENGTH 2048

// Global variables
volatile sig_atomic_t flag = 0;
int sockfd = 0;

void catch_ctrl_c_and_exit(int sig)
{
  flag = 1;
}

void send_msg_handler()
{
  char message[LENGTH] = {};
  char buffer[LENGTH + 32] = {};

  while (1)
  {
    str_overwrite_stdout();
    fgets(message, LENGTH, stdin);
    str_trim_lf(message, LENGTH);

    if (strcmp(message, "EXIT") == 0)
    {
      send(sockfd, message, strlen(message), 0);
      break;
    }

    else
    {
      sprintf(buffer, "%s\n", message);
      send(sockfd, buffer, strlen(buffer), 0);
    }

    bzero(&message, LENGTH);
    bzero(&buffer, LENGTH + 32);
  }

  catch_ctrl_c_and_exit(2);
}

void receive_msg_handler()
{
  char message[LENGTH] = {};
  char private_key[100] = "privateKey_client1.txt";
  char decrypted[8192] = {};

  while (1)
  {
    int receive = recv(sockfd, message, LENGTH, 0);
    if (receive > 0)
    {

      int decrypted_length = private_decrypt(message, strlen(message), private_key, decrypted);
      // printf("%s\n", message);

      if (decrypted_length == -1)
      {
        printf("Private Decrypt failed ");
        exit(EXIT_FAILURE);
      }

      printf("\n%s\n", decrypted);

      if (strcmp(decrypted, "EXIT") == 0)
      {
        catch_ctrl_c_and_exit(2);
        break;
      }

      str_overwrite_stdout();
    }

    else if (receive == 0)
    {
      printf("Max clients reached. Terminating...\n");
      exit(EXIT_FAILURE);
    }

    // memset(message, 0, sizeof(message));
    bzero(&message, strlen(message));
    bzero(&decrypted, strlen(decrypted));
  }
}

int main(int argc, char **argv)
{
  char *ip;
  int port;

  if (argc == 3)
  {
    ip = argv[1];
    port = atoi(argv[2]);
  }

  else
  {
    printf("Usage: %s <address> <port>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  signal(SIGINT, catch_ctrl_c_and_exit);

  struct sockaddr_in server_addr;

  /* Socket settings */
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(ip);
  server_addr.sin_port = htons(port);

  /* Connect to Server */
  if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
  {
    printf("ERROR: Connection failed\n");
    exit(EXIT_FAILURE);
  }

  pthread_t send_msg_thread;
  if (pthread_create(&send_msg_thread, NULL, (void *)send_msg_handler, NULL) != 0)
  {
    printf("ERROR: pthread failed\n");
    exit(EXIT_FAILURE);
  }

  pthread_t receive_msg_thread;
  if (pthread_create(&receive_msg_thread, NULL, (void *)receive_msg_handler, NULL) != 0)
  {
    printf("ERROR: pthread failed\n");
    exit(EXIT_FAILURE);
  }

  while (1)
  {
    if (flag)
    {
      printf("\nTerminating...\n");
      break;
    }
  }

  close(sockfd);
}
