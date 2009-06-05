#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>

#include "rsa.h"

#define RSA_S (100)

void rsa_generate_keys(struct rsa_key**, struct rsa_key**);
unsigned short rsa_get_prime();
void rsa_get_keys(unsigned long*, unsigned long*, unsigned long, unsigned long);
unsigned long rsa_exp(unsigned long, unsigned long, unsigned long);
int rsa_witness(unsigned short, unsigned short);
int rsa_miller_rabin(unsigned short, int);
unsigned long rsa_phi(unsigned short, unsigned short);
long rsa_ext_gcd(long, long, long*, long*, long*);

void rsa_generate_keys(struct rsa_key **public_key, struct rsa_key **private_key) {
  unsigned short p, q;
  unsigned long n, e, d;
  
  *public_key = (struct rsa_key*) calloc(1, sizeof(struct rsa_key));
  *private_key = (struct rsa_key*) calloc(1, sizeof(struct rsa_key));

  //printf("here 1\n");

  srand(time(NULL));
  
  p = rsa_get_prime();
  q = rsa_get_prime();

  printf("prime 1: %d\n", p);
  printf("prime 2: %d\n", q); 

  n = (unsigned long) p * q;

  //printf("n: %lu\n", n);

  rsa_get_keys(&e, &d, n, rsa_phi(p, q));

  (*private_key)->key = d;
  (*private_key)->n = n;
  
  (*public_key)->key = e;
  (*public_key)->n = n;
}

unsigned short rsa_get_prime() {
  unsigned short prime;

  // srand(time(NULL));
  
  while(13) {
    prime = (rand() % (1 << 8*sizeof(unsigned short)) - 1) + 1;

    if(rsa_miller_rabin(prime, RSA_S)) {
      break;
    }
  }

  return prime;
}

void rsa_get_keys(unsigned long *e, unsigned long *d, unsigned long n, unsigned long phi) {
  long gcd, x, y;

  //srand(time(NULL));

  while(13) {
    *e = (rand() % (1 << 8*sizeof(unsigned short)) - 1) + 1;

    if(rsa_ext_gcd(*e, phi, &x, &y, &gcd) == 1) {
      break;
    }
  }

  *d = x;
  if(*d <= 0) {
    *d += phi;
  }
}

unsigned long rsa_exp(unsigned long a, unsigned long b, unsigned long n) {
  unsigned long d;
  
  unsigned long long _d, _a;
   
  _d = 1;
  _a = a;
  
  while(b) {
    if(b & 1) {
      _d = (_a*_d) % n;
    }
    
    _a = (_a*_a) % n;
    b >>= 1;
  }

  d = _d;
  
  return d;
}

int rsa_witness(unsigned short a, unsigned short n) {
  unsigned short b, c;
  unsigned long  _d, _a;

  _d = 1;
  b = n - 1;
  
  while(b) {
    if(b & 1) {
      _d = (_a*_d) % n;
    }
    
    c = _a;
    
    _a = (_a*_a) % n;
    b >>= 1;

    if(_a == 1 && c != 1 && c != (n - 1)) {
      return TRUE;
    }
  }

  if(_d == 1) {
    return FALSE;
  }

  return TRUE;
}

int rsa_miller_rabin(unsigned short n, int s) {
  unsigned short a;
  
  //srand(time(NULL));
  
  while(s-- > 0) {
    a = (rand() % (n - 1)) + 1;

    if(rsa_witness(a, n) == TRUE) {
      return FALSE;
    }
  }

  return TRUE;
}

unsigned long rsa_phi(unsigned short p, unsigned short q) {
  return (unsigned long) (p - 1) * (q - 1);
}

long rsa_ext_gcd(long a, long b, long *x, long *y, long *d) {
  long tmp;
  
  if(!b) {
    *d = a; *x = 1; *y = 0;
    
    return *d;
  }

  rsa_ext_gcd(b, a%b, x, y, d);

  tmp = *x - (a/b)*(*y);
  *x = *y;
  *y = tmp;
  
  return *d;
}
