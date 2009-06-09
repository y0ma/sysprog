#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "rsa.h"

//Commands
#define CODE_CMD ('c')
#define DECODE_CMD ('d')

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

int get_nbytes(unsigned long n) {
  int cnt;

  cnt = 0;
  while(n > 0) {
    cnt++;
    n >>= 8;
  }

  return cnt;
}

void code(int srcfd, int destfd, struct rsa_key *public_key) {
  unsigned long buf;
  int portion, r;
  
  portion = get_nbytes(public_key->n);
  
  for(buf = 0; (r = read(srcfd, &buf, portion - 1)) > 0; buf = 0) {
    buf = rsa_convert(buf, public_key);
    write(destfd, &buf, portion);

    if(r < (portion - 1)) {
      r = portion - 1 - r;
      write(destfd, &r, 1);
    }
  }
}

void decode(int srcfd, int destfd, struct rsa_key *private_key) {
  unsigned long buf;
  int portion, r;

  portion = get_nbytes(private_key->n);
  
  for(buf = 0; (r = read(srcfd, &buf, portion)) > 0; buf = 0) {
    if(r < portion) {
      ftruncate(destfd, lseek(destfd, 0, SEEK_END) - buf);
    } else {
      buf = rsa_convert(buf, private_key);
      write(destfd, &buf, portion - 1);
    }
  }
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
  
  destfd = open(argv[3], O_CREAT | O_WRONLY | O_TRUNC, 0644);
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
  
  if(*cmd == CODE_CMD) {
    code(srcfd, destfd, public_key);
  } else if(*cmd == DECODE_CMD) {
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
