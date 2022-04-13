#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <sys/types.h>
#include <signal.h>

// Including common header file defined for string handling operations
#include "../common.h"

#define MAX_CLIENTS 2
#define BUFFER_SZ 2048

static unsigned int cli_count = 0;
static int uid = 0;

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

/* Send message to all clients */
void send_message(char *s, client_t *cli)
{
  pthread_mutex_lock(&clients_mutex);

  int search_id = cli->uid ^ 1;

  if (write(clients[search_id]->sockfd, s, strlen(s)) < 0)
    perror("ERROR: Descriptor write failed.");

  printf("MESSAGE SENT TO ");
  print_client_addr(clients[search_id]->address);
  printf(":%d\n\n", clients[search_id]->address.sin_port);

  pthread_mutex_unlock(&clients_mutex);
}

/* Handle communication with the client */
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
        printf("\n%s\nIP and Port - ", buff_out);

        if (strcmp(buff_out, "EXIT") != 0)
        {
          time_t seconds;
          seconds = time(NULL);

          FILE *fptr;

          char filename[100];
          sprintf(filename, "%d_%ld.txt", cli->uid, seconds);
          fptr = fopen(filename, "w");

          if (fptr == NULL)
          {
            printf("Error!");
            exit(EXIT_FAILURE);
          }

          fputs(buff_out, fptr);
          fclose(fptr);
        }

        send_message(buff_out, cli);
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

  /* Detach thread */
  pthread_detach(pthread_self());

  return NULL;
}

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    printf("Usage: %s <port>\n", argv[0]);
    return EXIT_FAILURE;
  }

  char *ip = "10.4.19.169";
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

      shutdown(connfd, SHUT_RDWR);
      close(connfd);
      continue;
    }

    /* Client settings */
    client_t *cli = (client_t *)malloc(sizeof(client_t));
    cli->address = cli_addr;
    cli->sockfd = connfd;
    cli->uid = uid;
    uid ^= 1;

    /* Add client to the queue and fork thread */
    queue_add(cli);
    pthread_create(&tid, NULL, &handle_client, (void *)cli);

    /* Reduce CPU usage */
    sleep(1);
  }
}
