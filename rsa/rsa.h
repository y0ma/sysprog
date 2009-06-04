#ifndef __RSA_HEADERS__
#define __RSA_HEADERS__

#define FALSE (0)
#define TRUE  (1)

struct rsa_key {
  unsigned long key;
  unsigned long n;
};

extern void gen_key(struct rsa_key *public_key, struct rsa_key *private_key);

#endif /* __RSA_HEADERS__ */
