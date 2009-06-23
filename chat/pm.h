#ifndef __PM_H__
#define __PM_H__

#define CMD_SIZE 80
#define QUIT_CMD "quit"
#define HELP_CMD "help"
#define LS_CMD "ls"
#define WRITE_CMD "write"
#define NICK_CMD "nick"

#define MSG_PROJ_ID 100
#define MSG_MAX_SIZE 1024

#define SERVER_MTYPE 1

enum cmd_types {NICK, LS, WRITE, QUIT, EOLS};

struct pm_msg {
  long mtype;
  int to;
  int from;
  int cmd_type;
  char message[MSG_MAX_SIZE];
};

struct contact {
  int pid;
  char nick[MSG_MAX_SIZE];
  struct contact *next;
  struct contact *prev;
};

extern void pm_chop(char*);

extern struct contact *add_contact(struct contact**, int, const char*);
extern struct contact *find_contact(struct contact*, int);
extern struct contact *drop_contact(struct contact**, int);

#endif // __PM_H__
