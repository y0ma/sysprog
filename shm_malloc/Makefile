CC = gcc -Wall

SHM_MALLOC_NAME = "shm_malloc"

MAINOBJ = shm_malloc.o
LIBOBJ = x_syscalls.o

default: compile clean

compile: compile_shm_malloc

compile_shm_malloc: $(LIBOBJ) $(MAINOBJ)
	$(CC) $(CFLAGS) $(LIBOBJ) $(MAINOBJ) -o $(SHM_MALLOC_NAME)

.c.o:
	$(CC) -c $< -o $@

clean:
	rm -rf *~ *.o

git_clean: clean
	rm $(SHM_MALLOC_NAME)

