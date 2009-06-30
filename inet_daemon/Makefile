CC = gcc -Wall

INETDAEMON_NAME = "inetd"

MAINOBJ = daemon.o
LIBOBJ = x_syscalls.o inetd.o

default: compile

compile: compile_inetd

compile_inetd: $(LIBOBJ) $(MAINOBJ)
	$(CC) $(CFLAGS) $(LIBOBJ) $(MAINOBJ) -o $(INETDAEMON_NAME)

.c.o:
	$(CC) -c $< -o $@

clean:
	rm -rf *~ *.o > /dev/null

git_clean: clean
	rm $(INETDAEMON_NAME) > /dev/null

