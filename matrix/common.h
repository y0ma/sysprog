#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <errno.h>

#define PROJ_ID (97)
#define NUM_MATRIX (3)

#define A (0)
#define B (1)
#define C (2)

typedef struct matrix_tag {
  int rows;
  int cols;
  int *body;
} matrix;
