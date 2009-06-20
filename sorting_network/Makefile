CC = gcc -Wall

SN_NAME = "sn"

MAINOBJ = sorting_network.o
LIBOBJ = x_syscalls.o

default: compile clean

compile: compile_sorting_network

compile_sorting_network: $(LIBOBJ) $(MAINOBJ)
	$(CC) $(CFLAGS) $(LIBOBJ) $(MAINOBJ) -o $(SN_NAME)

.c.o:
	$(CC) -c $< -o $@

clean:
	rm -rf *~ *.o

git_clean: clean
	rm $(SN_NAME)
