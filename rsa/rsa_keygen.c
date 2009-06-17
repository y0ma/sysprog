#include <stdio.h>
#include <stdlib.h>

#include "rsa.h"

#define RSA_KEYS_FILE ("rsa.keys")

int main(int argc, char **argv) {
  struct rsa_key *private_key, *public_key;
  FILE *fd;
  
  if((fd = fopen(RSA_KEYS_FILE, "w+")) == NULL) {
  	fprintf(stderr, "Error: Can't open file\n");
    exit(1);
  }
  
  rsa_generate_keys(&public_key, &private_key);

  fprintf(fd, "d:%lu\n", private_key->key);
  fprintf(fd, "e:%lu\n", public_key->key);
  fprintf(fd, "n:%lu\n", public_key->n);
   
  free(public_key);
  free(private_key);
  fclose(fd);
  return 0;
}
