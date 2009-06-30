#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <error.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include "x_syscalls.h"
#include "inetd.h"

#define STOP_CMD "stop"
#define RESTART_CMD "restart"

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

int LockFileDesc = -1;
//const char *LockFilePath = "/var/run/xaa10_inetd.pid";
const char *LockFilePath = "./xaa10_inetd.pid";

int main(int argc, char **argv) {
  int fd, len;
  pid_t pid, daemon_pid;
  char pid_buf[16];

  if(argc > 1) {
    if((fd = open(LockFilePath, O_RDONLY)) < 0) {
      perror("Lock file not found. May be the server is not running?");
      exit(fd);
    }
    
    len = read(fd, pid_buf, 16);
    pid_buf[len] = '\0';
    pid = atoi(pid_buf);

    if(strcmp(argv[1], STOP_CMD) == 0) {
      kill(pid, SIGUSR1);
      exit(EXIT_SUCCESS);
    }
    
    if(strcmp(argv[1], RESTART_CMD) == 0) {
      kill(pid, SIGHUP);
      exit(EXIT_SUCCESS);
    }
    
    printf("usage %s [STOP|RESTART]\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  become_daemon(LockFilePath, &LockFileDesc, &daemon_pid);
  config_signal_handlers();

  return 0;
}
