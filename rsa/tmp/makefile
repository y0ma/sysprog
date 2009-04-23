OBJS = a.o b.o

CC = gcc -Wall -O2 -pipe

.c.o:
	$(CC) -c $< -o $@

default: a

a: $(OBJS)
	$(CC) $? -o $@
