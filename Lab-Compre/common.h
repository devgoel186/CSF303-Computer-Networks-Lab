#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <string.h>
#include <openssl/err.h>
#include <stdio.h>
#include <stdlib.h>

void splitString(char *str)
{
    char *token = strtok(str, "-");
    while (token != NULL)
    {
        printf("%s\n", token);
        token = strtok(NULL, "-");
    }
    return;
}

char *strrev(char *str)
{
  char *p1, *p2;

  if (!str || !*str)
    return str;
  for (p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2)
  {
    *p1 ^= *p2;
    *p2 ^= *p1;
    *p1 ^= *p2;
  }
  return str;
}

void str_overwrite_stdout()
{
  printf("\r%s", "> ");
  fflush(stdout);
}

void str_trim_lf(char *arr, int length)
{
  for (int i = 0; i < length; i++)
  {
    if (arr[i] == '\n')
    {
      arr[i] = '\0';
      break;
    }
  }
}