#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>

#define BUFSIZE (1)

#define READ (0)
#define WRITE (1)

#define STDIN_FD (0)
#define STDOUT_FD (1)

#define IS_CHILD(pid) ((pid) == 0)
#define IS_PARENT(pid) ((pid) > 0)

int x_pipe(int*);
pid_t x_fork(void);
int x_dup2(int, int);
int x_close(int);
pid_t x_waitpid(pid_t, int*, int);
int x_select(int, fd_set*, fd_set*, fd_set*, struct timeval*);
int x_write(int, const void*, size_t);
int x_read(int, void*, size_t);

int main(void) {
  int in[2];
  int out[2];
  pid_t pid;

  printf("type 'quit' for exit\n\n");

  x_pipe(in);
  x_pipe(out);
  
  pid = x_fork();
  
  if(IS_CHILD(pid)) {
    x_dup2(in[READ], STDIN_FD);
    x_dup2(out[WRITE], STDOUT_FD);
    
    x_close(in[READ]);
    x_close(in[WRITE]);
    x_close(out[READ]);
    x_close(out[WRITE]);
    
    execl("/usr/bin/bc", "runbc", NULL);

    perror("execl");
    exit(1);
  } else if(IS_PARENT(pid)) {
    char buf[BUFSIZE + 1];
    fd_set rfds;
    int count;
    int flag = 1;

    x_close(in[READ]);
    x_close(out[WRITE]);
    
    while(13) {
      FD_ZERO(&rfds);
      FD_SET(STDIN_FD, &rfds);
      FD_SET(out[READ], &rfds);
      
      count = 0;

      x_select(out[READ] + 1, &rfds, NULL, NULL, NULL);

      if(FD_ISSET(out[READ], &rfds)) {
        // read messages from BC
        count = x_read(out[READ], buf, BUFSIZE);
        if(count) {
          x_write(STDOUT_FD, buf, count);
        } else {
          break;
        }
      }

      if(FD_ISSET(STDIN_FD, &rfds) && flag) {
        // send commands to BC
        count = x_read(STDIN_FD, buf, BUFSIZE);
        if(count) {
          x_write(in[WRITE], buf, count);
        } else {
          x_write(in[WRITE], "quit\n", 6);
          flag = 0;
        }
      }
    }

    x_waitpid(pid, NULL, 0);

    x_close(in[WRITE]);
    x_close(out[READ]);
  }

  return 0;
}

int x_pipe(int fds[2]) {
  int ret;
  
  ret = pipe(fds);
  if(ret == -1) {
			perror("pipe");
			exit(1);
  }
	
  return ret;
}

pid_t x_fork() {
  pid_t pid;

  pid = fork();
	if(pid == -1) {
		perror("fork");
		exit(1);
	}

	return pid;
}

int x_dup2(int oldfd, int newfd) {
  int ret;

  ret = dup2(oldfd, newfd);
  if(ret == -1) {
    perror("dup2");
    exit(1);
  }

  return ret;
}

int x_close(int fd) {
  int ret;

  ret = close(fd);
  if(ret == -1) {
    perror("close");
    exit(1);  
  }

  return ret;
}

pid_t x_waitpid(pid_t pid, int *status, int options) {
  int ret;

  ret = waitpid(pid, status, options);
  if(ret == -1) {
    perror("waitpid");
    exit(1);
  }

  return ret;
}

int x_select(int maxfd, fd_set *readfds, fd_set *writefds, fd_set *errorfds, struct timeval *timeout) {
  int ret;

  ret = select(maxfd, readfds, writefds, errorfds, timeout);
  if(ret == -1) {
    perror("select");
    exit(1);
  }
  
  return ret;
}

int x_write(int fd, const void *buf, size_t count) {
  int ret;
  
  ret = write(fd, buf, count);
  if(ret == -1) {
    perror("write");
    exit(1);
  }
  
  return ret;
}

int x_read(int fd, void *buf, size_t count) {
  int ret;
  
  ret = read(fd, buf, count);
  if(ret == -1) {
    perror("read");
    exit(1);
  }
  
  return ret;
}

