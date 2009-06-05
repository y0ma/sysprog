#ifndef __RSA_HEADERS__
#define __RSA_HEADERS__

#define FALSE (0)
#define TRUE  (1)

struct rsa_key {
  unsigned long key;
  unsigned long n;
};

extern void rsa_generate_keys(struct rsa_key **public_key, struct rsa_key **private_key);

extern long rsa_ext_gcd(long, long, long*, long*, long*);
extern unsigned long rsa_exp(unsigned long, unsigned long, unsigned long);
  
#endif /* __RSA_HEADERS__ */
