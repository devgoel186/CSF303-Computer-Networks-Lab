#include <stdio.h>
#include <string.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/err.h>

// reverse string
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

RSA *createRSAWithFilename(char *filename, int public)
{
  FILE *fp = fopen(filename, "rb");
  if (fp == NULL)
  {
    printf("Unable to open file %s \n", filename);
    return NULL;
  }
  RSA *rsa = RSA_new();

  if (public)
  {
    rsa = PEM_read_RSA_PUBKEY(fp, &rsa, NULL, NULL);
  }
  else
  {
    rsa = PEM_read_RSAPrivateKey(fp, &rsa, NULL, NULL);
  }
  fclose(fp);
  return rsa;
}

int padding = RSA_PKCS1_PADDING;

int public_encrypt(char *data, int data_len, char *key_filename, char *encrypted)
{
  RSA *rsa = createRSAWithFilename(key_filename, 1);
  int result = RSA_public_encrypt(data_len, data, encrypted, rsa, padding);
  return result;
}

int private_decrypt(char *enc_data, int data_len, char *key_filename, char *decrypted)
{
  RSA *rsa = createRSAWithFilename(key_filename, 0);
  int result = RSA_private_decrypt(data_len, enc_data, decrypted, rsa, padding);
  return result;
}