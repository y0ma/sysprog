#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "x_syscalls.h"
#include "pm.h"

#define PS ("> ")

#define IS_CHILD(pid) ((pid) == 0)

void run_pm_server();
void stop_pm_server();
void ps();
void process_cmd();
void pm_server_terminal();

void run_pm_server() {
  while(13) {
    //do something
    sleep(10);
  }
}

void stop_pm_server() {
  signal(SIGCHLD, SIG_IGN);
  signal(SIGINT, SIG_IGN);
  kill(0, SIGINT);
  //sleep(2);
  wait(NULL);
}

void process_cmd(const char *cmd) {
  if(strcmp(cmd, QUIT_CMD) == 0) {
    stop_pm_server();
    exit(0);
  }
}

//show prompt string
void ps() {
  printf(PS);
  fflush(stdout);
}

void pm_server_terminal() {
  char cmd[CMD_SIZE];

  while(13) {
    ps();
    if(fgets(cmd, CMD_SIZE, stdin) != NULL) {
      process_cmd(cmd);
    }
  }
}

void sigchld_handler(int sig) {
  pid_t pid;

  pid = x_fork();

  if(IS_CHILD(pid)) {
    printf("***restarting server\n");
    ps();

    run_pm_server();
  }
}

int main() {
  pid_t pid;

  signal(SIGCHLD, sigchld_handler);

  pid = x_fork();
  
  if(IS_CHILD(pid)) {
    run_pm_server();
  } else {
    pm_server_terminal();
  }
  
  return 0;
}
