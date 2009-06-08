#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "rsa.h"


#define CODE ('c')
#define DECODE ('d')

#define RSA_KEYS_FILE ("rsa.keys")

void usage(char*, int);
void read_keys(FILE*, struct rsa_key*, struct rsa_key*);
void code(int, int, struct rsa_key*);


void usage(char *name, int exitcode) {
  printf("use: ");

  printf("%s CMD src dest\n\n", name);
  printf("CMD\n" );
  printf("    c - code message\n");
  printf("    d - decode message\n");
  
  exit(exitcode);
}

void read_keys(FILE *keysfd, struct rsa_key *public_key, struct rsa_key *private_key) {
  unsigned long s, e, n;

  fscanf(keysfd, "s:%lu\n", &s);
  fscanf(keysfd, "e:%lu\n", &e);
  fscanf(keysfd, "n:%lu\n", &n);

  public_key->key = e;
  public_key->n = n;

  private_key->key = s;
  private_key->n = n;
}

void code(int srcfd, int destfd, struct rsa_key *public_key) {
  printf("public_key: %lu\n", public_key->key);
}

void decode(int srcfd, int destfd, struct rsa_key *private_key) {
  printf("private_key: %lu\n", private_key->key);
}

int main(int argc, char **argv) {
  FILE *keysfd;
  int srcfd, destfd;
  char *cmd;
  struct rsa_key *public_key, *private_key;
  
  if(argc <= 3) {
    usage(argv[0], 1);
  }
  
  srcfd = open(argv[2], O_RDONLY);
  if(srcfd < 0) {
    perror(argv[2]);
    exit(1);
  }
  
  destfd = open(argv[3], O_CREAT | O_WRONLY, 0644);
  if(destfd < 0) {
    perror(argv[3]);
    exit(1);
  }

  keysfd = fopen(RSA_KEYS_FILE, "r");
  if(keysfd == NULL) {
    perror(RSA_KEYS_FILE);
    exit(1);
  }

  public_key = (struct rsa_key*) malloc(sizeof(struct rsa_key));
  private_key = (struct rsa_key*) malloc(sizeof(struct rsa_key));

  read_keys(keysfd, public_key, private_key);
  
  fclose(keysfd);

  cmd = argv[1];
  
  if(*cmd == CODE) {
    code(srcfd, destfd, public_key);
  } else if(*cmd == DECODE) {
    decode(srcfd, destfd, private_key);
  } else {
    fprintf(stderr, "unknow command '%s'\n", cmd);
    exit(1);
  }
  
  free(public_key);
  free(private_key);
  close(srcfd);
  close(destfd);
  
  return 0;
}
