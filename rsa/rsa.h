#ifndef __RSA_HEADERS__
#define __RSA_HEADERS__

struct rsa_public_key {
  long long key;
  long long n;
};

void gen_key(struct rsa_key *public_key, struct rsa_key *private_key);

#endif /* __RSA_HEADERS__ */
