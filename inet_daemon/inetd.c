#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "inetd.h"
#include "x_syscalls.h"

#define PID_BUF_SIZE 16

int become_daemon(const char*, int*, int*);

int become_daemon(const char* lock_file, int *lock_fd, int *d_pid) {
  int num_files, std_fds;
  pid_t pid;
  char pid_buf[PID_BUF_SIZE];

  //x_chdir("/");
  
  *lock_fd = open(lock_file, O_RDWR | O_CREAT | O_EXCL, 0644);
  if(*lock_fd < 0) {
    x_error("can't open lock file");
  }
  
  pid = x_fork();
  
  if(!IS_CHILD(pid)) {
    exit(0);
  }

  if(setsid() < 0) {
    exit(1);
  }

  pid = x_fork();
  if(!IS_CHILD(pid)) {
    exit(0);
  }

  *d_pid = x_getpid();
  sprintf(pid_buf, "%d\n", *d_pid);
  write(*lock_fd, pid_buf, PID_BUF_SIZE);

  num_files = sysconf(_SC_OPEN_MAX);
  while(num_files-- >= 0) {
    if(num_files != *lock_fd) {
      close(num_files);
    }
  }

  std_fds = open("/dev/null", O_RDWR);
  dup(std_fds);
  dup(std_fds);

  x_setpgrp();

  return 0;
}

int config_signal_handlers() {
  signal(SIGUSR2, SIGIGN);
  signal(SIGPIPE, SIGIGN);
  signal(SIGALRM, SIGIGN);
  signal(SIGTSTP, SIGIGN);
  signal(SIGPROF, SIGIGN);

  signal(SIGQUIT, fatal_signal_handler);
  signal(SIGILL, fatal_signal_handler);
  signal(SIGTRAP, fatal_signal_handler);
  signal(SIGIOT, fatal_signal_handler);
  signal(SIGBUS, fatal_signal_handler);
  signal(SIGFPE, fatal_signal_handler);
  signal(SIGSEGV, fatal_signal_handler);
  signal(SIGTKFLT, fatal_signal_handler);
  signal(SIGCONT, fatal_signal_handler);
  signal(SIGPWR, fatal_signal_handler);
  signal(SIGSYS, fatal_signal_handler);
}

void fatal_signal_handler(int signum) {
  shoutdown();
  _exit(0);
}
