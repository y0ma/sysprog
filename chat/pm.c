#include <string.h>
#include <stdlib.h>

#include "pm.h"

void pm_chop(char*);
struct contact *add_contact(struct contact**, int, const char*);
struct contact *find_contact(struct contact*, int);
struct contact *drop_contact(struct contact**, int);

void pm_chop(char *cmd) {
  while(*cmd != '\n' && *cmd != '\0') {
    cmd++;
  }

  *cmd = '\0';
}

struct contact *add_contact(struct contact **head, int pid, const char *nick) {
  struct contact *p;

  p = find_contact(*head, pid);

  if(p) {
    strncpy(p->nick, nick, MSG_MAX_SIZE);
    return p;
  }

  p = (struct contact*) malloc(sizeof(struct contact));
  
  p->pid = pid;
  strncpy(p->nick, nick, MSG_MAX_SIZE);
  p->prev = NULL;
  p->next = *head;

  if(*head != NULL) {
    (*head)->prev = p;
  }

  *head = p;

  return p;
}

struct contact *find_contact(struct contact *head, int pid) {
  while(head != NULL) {
    if(head->pid == pid) {
      return head;
    }
    
    head = head->next;
  }

  return NULL;
}

struct contact *drop_contact(struct contact **head, int pid) {
  struct contact *target;

  target = find_contact(*head, pid);
  
  if(target) {
    if(target == *head) {
      *head = (*head)->next;
    }

    if(target->prev) {
      target->prev->next = target->next;
    }

    if(target->next) {
      target->next->prev = target->prev;
    }

    return target;
  }

  return NULL;
}
