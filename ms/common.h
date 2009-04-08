#ifndef COMMON_FILE

#define COMMON_FILE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>

#define PROJ_ID (97)

#define ISCHILD(pid) ((pid) == 0)

typedef struct mystruct_tag {
  int a;
  int b;
  int c;
} mystruct;

#endif
