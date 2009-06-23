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

int msqid;
struct contact *contact_list = NULL;

void send(int to, int from, int cmd_type, const char *message) {
  struct pm_msg msg;

  msg.mtype = to;
  msg.to = to;
  msg.from = from;
  msg.cmd_type = cmd_type;
  strncpy(msg.message, message, MSG_MAX_SIZE);

  x_msgsnd(msqid, &msg, sizeof(struct pm_msg) - sizeof(long), 0);
}

void pm_server_nick(const struct pm_msg *msg) {
  char message[MSG_MAX_SIZE];
  
  add_contact(&contact_list, msg->from, msg->message);

  snprintf(message, MSG_MAX_SIZE, "Ваш ник: %s", msg->message);
  send(msg->from, SERVER_MTYPE, NICK, message);
}

void pm_server_write(const struct pm_msg *msg) {
  char message[MSG_MAX_SIZE];

  snprintf(message, MSG_MAX_SIZE, "%s", msg->message);
  send(msg->to, msg->from, WRITE, message);
}

void pm_server_list(const struct pm_msg *msg) {
  char message[MSG_MAX_SIZE];
  struct contact *p;

  p = contact_list;
  while(p) {
    snprintf(message, MSG_MAX_SIZE, "%-5d --- %s", p->pid, p->nick);
    send(msg->from, SERVER_MTYPE, LS, message);
    p = p->next;
  }
  send(msg->from, SERVER_MTYPE, EOLS, "");
}

void run_pm_server() {
  struct pm_msg msg;
  size_t msg_size;

  msg_size = sizeof(struct pm_msg) - sizeof(long);
  
  add_contact(&contact_list, SERVER_MTYPE, "#SERVER#");

  while(13) {
    x_msgrcv(msqid, &msg, msg_size, SERVER_MTYPE, 0);
    switch(msg.cmd_type) {
    case NICK:
      printf("**nick request from %d\n", msg.from);
      pm_server_nick(&msg);
      break;
    case LS:
      printf("**user_list request from %d\n", msg.from);
      pm_server_list(&msg);
      break;
    case WRITE:
      printf("**chat request from %d to %d\n", msg.from, msg.to);
      if(msg.to != SERVER_MTYPE) {
	pm_server_write(&msg);
      }
      break;
    case QUIT:
      printf("**quit request\n");
      drop_contact(&contact_list, msg.from);
      break;
    default:
      printf("**unknown command\n");
    }
    ps();
  }
}

void stop_pm_server() {
  signal(SIGCHLD, SIG_IGN);
  signal(SIGINT, SIG_IGN);
  kill(0, SIGINT);
  //sleep(2);
  wait(NULL);
  x_msgctl(msqid, IPC_RMID, NULL);
}

void pm_server_help() {
  puts("Доступные команды:");
  puts("\thelp - помощь");
  puts("\tquit - остановить сервер и выйти");
}

void process_cmd(const char *cmd) {
  if(strcmp(cmd, QUIT_CMD) == 0) {
    stop_pm_server();
    exit(0);
  } else if(strcmp(cmd, HELP_CMD) == 0) {
    pm_server_help();
  } else {
    printf("Unknow command <%s>\n", cmd);
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
      pm_chop(cmd);
      process_cmd(cmd);
    }
  }
}

void sigchld_handler(int sig) {
  pid_t pid;
  int status;
  
  waitpid(-1, &status, WNOHANG |WUNTRACED | WCONTINUED);
  
  if(!WIFSTOPPED(status) && !WIFCONTINUED(status)) {
    pid = x_fork();

    if(IS_CHILD(pid)) {
      printf("***restarting server\n");
      ps();

      run_pm_server();
    }
  }
}

int main() {
  pid_t pid;
  key_t key;

  key = x_ftok(".", MSG_PROJ_ID);
  msqid = x_msgget(key, IPC_CREAT | IPC_EXCL | 0600);

  pid = x_fork();
  
  if(IS_CHILD(pid)) {
    run_pm_server();
  } else {
    signal(SIGCHLD, sigchld_handler);
    pm_server_terminal();
  }
  
  return 0;
}
