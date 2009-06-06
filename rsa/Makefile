CC = gcc -Wall -std=c99

RSA_KEYGEN_NAME = "keygen"
RSA_KEYS_NAME = "rsa.keys"

LIBOBJ = rsa.o
MAINOBJ = rsa_keygen.o

default: compile clean

compile: $(LIBOBJ) $(MAINOBJ)
	$(CC) $(LIBOBJ) $(MAINOBJ) -o $(RSA_KEYGEN_NAME)

.c.o:
	$(CC) -c $< -o $@

clean:
	rm -rf *~
	rm -rf *.o

git_clean: clean
	rm -rf $(RSA_KEYGEN_NAME)
	rm -rf $(RSA_KEYS_NAME)

