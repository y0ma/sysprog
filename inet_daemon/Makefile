CC = gcc -Wall

INETDAEMON_NAME = "inetd"

MAINOBJ = inet_daemon.o
LIBOBJ = x_syscalls.o

default: compile

compile: compile_inet_daemon

compile_inet_daemon: $(LIBOBJ) $(MAINOBJ)
	$(CC) $(CFLAGS) $(LIBOBJ) $(MAINOBJ) -o $(INETDAEMON_NAME)

.c.o:
	$(CC) -c $< -o $@

clean:
	rm -rf *~ *.o > /dev/null

git_clean: clean
	rm $(INETDAEMON_NAME) > /dev/null

