CC = gcc -Wall

SERVER_NAME = "pm_server"
CLIENT_NAME = "pm_client"

SERVEROBJ = pm_server.o
CLIENTOBJ = pm_client.o
LIBOBJ = x_syscalls.o pm.o

default: compile clean

compile: compile_pm_server compile_pm_client

compile_pm_server: $(LIBOBJ) $(SERVEROBJ)
	$(CC) $(CFLAGS) $(LIBOBJ) $(SERVEROBJ) -o $(SERVER_NAME)

compile_pm_client: $(LIBOBJ) $(CLIENTOBJ)
	$(CC) $(CFLAGS) $(LIBOBJ) $(CLIENTOBJ) -o $(CLIENT_NAME)

.c.o:
	$(CC) -c $< -o $@

clean:
	rm -rf *~ *.o

git_clean: clean
	rm -f $(SERVER_NAME) $(CLIENT_NAME)
