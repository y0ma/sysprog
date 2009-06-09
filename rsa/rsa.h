#ifndef __RSA_HEADERS__
#define __RSA_HEADERS__

#define FALSE (0)
#define TRUE  (1)

struct rsa_key {
  unsigned long key;
  unsigned long n;
};

extern void rsa_generate_keys(struct rsa_key **public_key, struct rsa_key **private_key);
extern unsigned long rsa_convert(unsigned long msg, struct rsa_key* key);
  
#endif /* __RSA_HEADERS__ */
