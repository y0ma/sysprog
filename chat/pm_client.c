#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

#include "x_syscalls.h"
#include "pm.h"

#define PS ("> ")

#define IS_CHILD(pid) ((pid) == 0)

int msqid;

void run_pm_client();
void stop_pm_client();
void ps();
void process_cmd();
void pm_client_terminal();


void send(int to, int cmd_type, const char *message) {
  struct pm_msg msg;

  msg.mtype = SERVER_MTYPE;
  msg.to = to;
  msg.from = x_getpid();
  msg.cmd_type = cmd_type;
  strncpy(msg.message, message, MSG_MAX_SIZE);

  x_msgsnd(msqid, &msg, sizeof(struct pm_msg) - sizeof(long), 0);
}

void run_pm_client() {
  struct pm_msg msg;
  size_t msg_size;
  int pid;

  msg_size = sizeof(struct pm_msg) - sizeof(long);
  pid = x_getppid();
  
  while(13) {
    x_msgrcv(msqid, &msg, msg_size, pid, 0);
    switch(msg.cmd_type) {
    case NICK:
      printf("%s\n", msg.message);
      ps();
      break;
    case WRITE:
      printf("[%d]: ", msg.from);
      printf("%s\n", msg.message);
      break;
    case LS:
      printf("%s\n", msg.message);
      break;
    case EOLS:
      ps();
      break;
    default:
      printf("**unknown command\n");
      ps();
    }
  }
}

void stop_pm_client() {
  signal(SIGCHLD, SIG_IGN);
  signal(SIGINT, SIG_IGN);
  
  kill(0, SIGINT);
  //sleep(2);
  wait(NULL);

  //send QUIT
  send(SERVER_MTYPE, QUIT, "");
}

void pm_client_nick() {
  char message[MSG_MAX_SIZE];

  printf("Введите ник: ");
  fflush(stdout);

  fgets(message, MSG_MAX_SIZE, stdin);
  pm_chop(message);

  send(SERVER_MTYPE, NICK, message);
}

void pm_client_write() {
  char message[MSG_MAX_SIZE];
  int to;

  printf("Кому: ");
  fflush(stdout);

  scanf("%d\n", &to);
  
  puts("нажмите Ctrl-D для завершения");
  while(fgets(message, MSG_MAX_SIZE, stdin)) {
    pm_chop(message);
    send(to, WRITE, message);
  }
  ps();
}

void pm_client_help() {
  puts("Доступные команды:");
  puts("\thelp - помощь");
  puts("\tquit - выход");
  puts("\twrite - послать сообщение");
  puts("\tls - получить список пользователей");
  puts("\tnick - сменить имя");
  ps();
}

void process_cmd(const char *cmd) {
  if(strcmp(cmd, QUIT_CMD) == 0) {
    stop_pm_client();
    exit(0);
  } else if (strcmp(cmd, LS_CMD) == 0) {
    send(SERVER_MTYPE, LS, "");
  } else if (strcmp(cmd, WRITE_CMD) == 0) {
    pm_client_write();
  } else if (strcmp(cmd, NICK_CMD) == 0) {
    pm_client_nick();
  } else if(strcmp(cmd, HELP_CMD) == 0) {
    pm_client_help();
  } else {
    printf("Unknow command <%s>\n", cmd);
    ps();
  }
}

//show prompt string
void ps() {
  printf(PS);
  fflush(stdout);
}

void pm_client_terminal() {
  char cmd[CMD_SIZE];
  
  ps();
  pm_client_nick();
  while(13) {
    if(fgets(cmd, CMD_SIZE, stdin) != NULL) {
      pm_chop(cmd);
      process_cmd(cmd);
    }
  }
}

void sigchld_handler(int sig) {
  pid_t pid;
  key_t key;
  int status;

  waitpid(-1, &status, WNOHANG |WUNTRACED | WCONTINUED);
  
  if(!WIFSTOPPED(status) && !WIFCONTINUED(status)) {
    key = x_ftok(".", MSG_PROJ_ID);
    msqid = x_msgget(key, 0);
    
    pid = x_fork();

    if(IS_CHILD(pid)) {
      printf("***restarting client\n");
      ps();

      run_pm_client();
    }
  }
}

int main() {
  pid_t pid;
  key_t key;
  
  key = x_ftok(".", MSG_PROJ_ID);
  msqid = x_msgget(key, 0);

  pid = x_fork();

  if(IS_CHILD(pid)) {
    run_pm_client();
  } else {
    signal(SIGCHLD, sigchld_handler);
    pm_client_terminal();
  }
  /*delete me
  struct pm_msg msg = {1, 0, 0, QUIT, "test message"};
  x_msgsnd(msqid, &msg, sizeof(struct pm_msg) - sizeof(long), 0);
  */
  
  return 0;
}
