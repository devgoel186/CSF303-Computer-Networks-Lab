/*
Name - Dev Goel
ID - 2019A7PS0236G
*/

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <signal.h>

#include "common.h"

#define MAX_CLIENTS 4
#define BUFFER_SZ 2048

static unsigned int cli_count = 0;
static int uid = 10;

/* Client structure */
typedef struct
{
  struct sockaddr_in address;
  int sockfd;
  int uid;
} client_t;

client_t *clients[MAX_CLIENTS];

pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void print_client_addr(struct sockaddr_in addr)
{
  printf("%d.%d.%d.%d",
         addr.sin_addr.s_addr & 0xff,
         (addr.sin_addr.s_addr & 0xff00) >> 8,
         (addr.sin_addr.s_addr & 0xff0000) >> 16,
         (addr.sin_addr.s_addr & 0xff000000) >> 24);
}

/* Add clients to the client queue */
void queue_add(client_t *cl)
{
  pthread_mutex_lock(&clients_mutex);

  for (int i = 0; i < MAX_CLIENTS; ++i)
  {
    if (!clients[i])
    {
      clients[i] = cl;
      break;
    }
  }

  pthread_mutex_unlock(&clients_mutex);
}

/* Remove clients from queue */
void queue_remove(int uid)
{
  pthread_mutex_lock(&clients_mutex);

  for (int i = 0; i < MAX_CLIENTS; ++i)
  {
    if (clients[i])
    {
      if (clients[i]->uid == uid)
      {
        clients[i] = NULL;
        break;
      }
    }
  }

  pthread_mutex_unlock(&clients_mutex);
}

void send_message(char *s)
{
  pthread_mutex_lock(&clients_mutex);

  for (int i = 0; i < MAX_CLIENTS; ++i)
  {
    if (clients[i])
    {
      if (write(clients[i]->sockfd, s, strlen(s)) < 0)
      {
        perror("ERROR: Descriptor write failed.");
        break;
      }
    }
  }

  pthread_mutex_unlock(&clients_mutex);
}

void poll()
{
  for(int i = 0; i < 3; i++)
  {
    if(cli_count < 2)
      break;

    for(int i = 0; i < cli_count; i++)
    {
      if(clients[i])
      {
        if (write(clients[i]->sockfd, "POLL", strlen("POLL")) < 0)
        {
          perror("ERROR: Descriptor write failed.");
          break;
        }

        char message[BUFFER_SZ] = {};
        int receive = recv(clients[i]->sockfd, message, BUFFER_SZ, 0);
        if (receive > 0)
        {
          if(i == 0)
          {
            if(strcmp(message, "NONE") != 0)
              break;
          }
          if(i == 1)
          {
            if(strcmp(message, "LIST") != 0)
              break;
          }
          if(i == 2)
          {
            if(strcmp(message, "EXIT") != 0)
              break;
          }
        }
      }
    }
  }
}

void *handle_client(void *arg)
{
  /* Defining buffer array */
  char buff_out[BUFFER_SZ];

  int leave_flag = 0;

  cli_count++;
  client_t *cli = (client_t *)arg;

  while (1)
  {
    if (leave_flag)
      break;

    int receive = recv(cli->sockfd, buff_out, BUFFER_SZ, 0);
    if (receive > 0)
    {
      if (strlen(buff_out) > 0)
      {
        str_trim_lf(buff_out, strlen(buff_out));
        printf("\n%s\nIP and Port - ", strrev(buff_out));

        print_client_addr(cli->address);
        printf(":%d\n\n", cli->address.sin_port);

        str_overwrite_stdout();
      }
    }
    else if (receive == 0 || strcmp(buff_out, "exit") == 0)
      leave_flag = 1;
    else
    {
      printf("ERROR\n");
      leave_flag = 1;
    }

    bzero(buff_out, BUFFER_SZ);
  }

  /* Remove client from queue, detach thread */
  close(cli->sockfd);
  queue_remove(cli->uid);

  /* Free cli memory */
  free(cli);
  cli_count--;

  pthread_detach(pthread_self());

  return NULL;
}

void broadcast_msg_handler()
{
  char message[BUFFER_SZ] = {};
  char buffer[BUFFER_SZ + 32] = {};

  while (1)
  {
    str_overwrite_stdout();
    fgets(message, BUFFER_SZ, stdin);
    str_trim_lf(message, BUFFER_SZ);

    sprintf(buffer, "%s\n", message);
    send_message(buffer);

    bzero(message, BUFFER_SZ);
    bzero(buffer, BUFFER_SZ + 32);
  }
}

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    printf("Usage: %s <port>\n", argv[0]);
    return EXIT_FAILURE;
  }

  char *ip = "127.0.0.1";
  int port = atoi(argv[1]);
  int option = 1;
  int listenfd = 0, connfd = 0;
  struct sockaddr_in serv_addr;
  struct sockaddr_in cli_addr;
  pthread_t tid;

  /* Socket settings */
  listenfd = socket(AF_INET, SOCK_STREAM, 0);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr(ip);
  serv_addr.sin_port = htons(port);

  /* Ignore pipe signals */
  signal(SIGPIPE, SIG_IGN);

  if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (char *)&option, sizeof(option)) < 0)
  {
    perror("ERROR: setsockopt failed");
    exit(EXIT_FAILURE);
  }

  /* Bind */
  if (bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
  {
    perror("ERROR: Socket binding failed");
    exit(EXIT_FAILURE);
  }

  /* Listen */
  if (listen(listenfd, 10) < 0)
  {
    perror("ERROR: Socket listening failed");
    exit(EXIT_FAILURE);
  }

  pthread_t broadcast_msg_thread;
  if (pthread_create(&broadcast_msg_thread, NULL, (void *)broadcast_msg_handler, NULL) != 0)
  {
    printf("ERROR: pthread failed\n");
    exit(EXIT_FAILURE);
  }

  while (1)
  {
    socklen_t clilen = sizeof(cli_addr);
    connfd = accept(listenfd, (struct sockaddr *)&cli_addr, &clilen);

    /* Check if max clients is reached */
    if (cli_count == MAX_CLIENTS)
    {
      printf("\nMax clients reached. Rejected: ");
      print_client_addr(cli_addr);
      printf(":%d\n\n", cli_addr.sin_port);
      str_overwrite_stdout();

      send(connfd, "terminate", strlen("terminate"), 0);

      shutdown(connfd, SHUT_RDWR);
      close(connfd);
      continue;
    }

    /* Client settings */
    client_t *cli = (client_t *)malloc(sizeof(client_t));
    cli->address = cli_addr;
    cli->sockfd = connfd;
    cli->uid = uid++;

    /* Add client to the queue and fork thread */
    queue_add(cli);
    pthread_create(&tid, NULL, &handle_client, (void *)cli);

    poll();

    /* Reduce CPU usage */
    sleep(1);
  }
}