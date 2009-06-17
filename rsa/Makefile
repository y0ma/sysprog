CC = gcc -Wall

RSA_KEYGEN_NAME = "keygen"
RSA_CRYPT_NAME = "crypt"

RSA_KEYS_NAME = "rsa.keys"

LIBOBJ = rsa.o
KEYGENOBJ = rsa_keygen.o
CRYPTOBJ = rsa_crypt.o

default: compile clean

compile: compile_keygen compile_crypt

compile_keygen: $(LIBOBJ) $(KEYGENOBJ)
	$(CC) $(LIBOBJ) $(KEYGENOBJ) -o $(RSA_KEYGEN_NAME)

compile_crypt: $(LIBOBJ) $(CRYPTOBJ)
	$(CC) $(LIBOBJ) $(CRYPTOBJ) -o $(RSA_CRYPT_NAME)

.c.o:
	$(CC) -c $< -o $@

clean:
	rm -rf *~ *.o

git_clean: clean
	rm -rf $(RSA_KEYGEN_NAME) $(RSA_KEYS_NAME) $(RSA_CRYPT_NAME)

