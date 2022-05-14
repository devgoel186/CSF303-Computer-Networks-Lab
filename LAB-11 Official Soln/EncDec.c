#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <string.h>
#include <openssl/err.h>
#include <stdio.h>
#include <stdlib.h>

 
int padding = RSA_PKCS1_PADDING;
 
RSA * createRSA(unsigned char * key,int public)
{
    RSA *rsa= NULL;
    BIO *keybio ;
    keybio = BIO_new_mem_buf(key, -1);
    if (keybio==NULL)
    {
        printf( "Failed to create key BIO");
        return 0;
    }
    if(public)
    {
        rsa = PEM_read_bio_RSA_PUBKEY(keybio, &rsa,NULL, NULL);
    }
    else
    {
        rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa,NULL, NULL);
    }
    if(rsa == NULL)
    {
        printf( "Failed to create RSA");
    }
 
    return rsa;
}
 
int public_encrypt(unsigned char * data,int data_len,unsigned char * key, unsigned char *encrypted)
{
    RSA * rsa = createRSA(key,1);
    int result = RSA_public_encrypt(data_len,data,encrypted,rsa,padding);
    return result;
}
int private_decrypt(unsigned char * enc_data,int data_len,unsigned char * key, unsigned char *decrypted)
{
    RSA * rsa = createRSA(key,0);
    int  result = RSA_private_decrypt(data_len,enc_data,decrypted,rsa,padding);
    return result;
}
 
 
int private_encrypt(unsigned char * data,int data_len,unsigned char * key, unsigned char *encrypted)
{
    RSA * rsa = createRSA(key,0);
    int result = RSA_private_encrypt(data_len,data,encrypted,rsa,padding);
    return result;
}
int public_decrypt(unsigned char * enc_data,int data_len,unsigned char * key, unsigned char *decrypted)
{
    RSA * rsa = createRSA(key,1);
    int  result = RSA_public_decrypt(data_len,enc_data,decrypted,rsa,padding);
    return result;
}
 
void printLastError(char *msg)
{
    char * err = malloc(130);;
    ERR_load_crypto_strings();
    ERR_error_string(ERR_get_error(), err);
    printf("%s ERROR: %s\n",msg, err);
    free(err);
}

long getFileSize(char *filename)
{
  FILE* ptr;
  long lSize;
  ptr = fopen(filename, "r");
  fseek( ptr , 0L , SEEK_END);
  lSize = ftell( ptr );
  rewind( ptr );
  return lSize;
}

char * readFile(char *filename)
{
    FILE* ptr;
  long lSize = getFileSize(filename);
  char *buffer;
   ptr = fopen(filename, "r");
  
  /* allocate memory for entire content */
buffer = calloc( 1, lSize+1 );
if( !buffer ) fclose(ptr),fputs("memory alloc fails",stderr),exit(1);

/* copy the file into the buffer */
if( 1!=fread( buffer , lSize, 1 , ptr) )
  fclose(ptr),free(buffer),fputs("entire read fails",stderr),exit(1);

fclose(ptr);
fflush(ptr);
return buffer;

}
 
int main(){
 
char pubKey[] = "./public_key2.pem";
char priKey[] = "./private_key2.pem";
char fileToEncrypt[]  = "./testEncDec.txt";
char encryptedFile[] = "./enc.txt";
char decryptedFile[] = "./dec.txt";

//get length of files
long len = getFileSize(pubKey);
long len2 = getFileSize(priKey);


//public key
char publicK[len]; 
strcpy(publicK,readFile(pubKey));
printf("Size of public key is %ld, \n Public Key:\n%s",sizeof(publicK),publicK);

//private key
char privateK[len2];
strcpy(privateK,readFile(priKey));
printf("\nPrivate Key:%s(Length:%ld)",privateK,sizeof(privateK));
printf("PK done");


//file to encrypt
char textOfFile[2048/8];
strcpy(textOfFile,readFile(fileToEncrypt)); 
printf("\nCont:%s",textOfFile);
printf("\nStrlen: %ld",strlen(textOfFile));


//buffer for stirung enc/dec vals
unsigned char  encrypted[4098]={};
unsigned char decrypted[4098]={};



//encryptor
int encrypted_length= public_encrypt(textOfFile,strlen(textOfFile),publicK,encrypted);

if(encrypted_length == -1)
{
    printLastError("Public Encrypt failed ");
    exit(0);
}

printf("\nEncrypted Text =%s\n",encrypted);
printf("\nEncrypted length =%d\n",encrypted_length);

//write to file
FILE *file = fopen(encryptedFile, "w");
fwrite(encrypted,encrypted_length,1,file); 
fclose(file);

//start with the decryption
unsigned char buffer[getFileSize(encryptedFile)];
FILE *ptr;

ptr = fopen(encryptedFile,"r");  // r for read, b for binary

fread(buffer,sizeof(buffer),1,ptr); 
printf("%ld is the length",getFileSize(encryptedFile));
printf("\nEncrypted file data:%s\n",buffer);

//decrypt the file
int decrypted_length = private_decrypt(buffer,sizeof(buffer),privateK, decrypted);
if(decrypted_length == -1)
{
    printLastError("Private Decrypt failed ");
    exit(0);
}
printf("Decrypted Text =%s\n",decrypted);
printf("Decrypted Length =%d\n",decrypted_length);

FILE *write_ptr;
write_ptr = fopen(decryptedFile,"w");  // w for write, b for binary
fwrite(decrypted,decrypted_length,1,write_ptr); // write 10 bytes from our buffer 
 
 
 
 
}