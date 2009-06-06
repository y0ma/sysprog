#include <stdio.h>
#include <stdlib.h>

#include "rsa.h"

int main(int argc, char **argv) {
  struct rsa_key *private_key, *public_key;

  rsa_generate_keys(&public_key, &private_key);

  printf("private: %lu\n", private_key->key);
  printf("public: %lu\n", public_key->key);
  printf("n: %lu\n", public_key->n);
  
  unsigned long a;
  
  a = rsa_exp(rsa_exp(123456, private_key->key, private_key->n), public_key->key, public_key->n);
  printf("message: %lu\n", a);
  return 0;
}
