#include <stdio.h>
#include <stdlib.h>

#include "x_syscalls.h"

#define PROJ_ID (100)

void sn_init(int **a, int a_size) {
  key_t key;
  int a_shmid;
  
  key = x_ftok(".", PROJ_ID);
  a_shmid = x_shmget(key, a_size, IPC_CREAT | IPC_EXCL | 0600);
  *a = (int*) x_shmat(a_shmid, NULL, 0);
}

int round2(int n) {
  int shift, step;

  if(!n) {
    return 1;
  }

  if(n < 3) {
    return n;
  }

  shift = step = sizeof(int) * 4;
  
  while(step) {
    if((n >> shift) == 1) {
      break;
    }

    step >>= 1;
    shift += (n >> shift) ? step : -step;
  }
  
  if(~(-1 << shift) & n) {
    shift++;
  }
  
  return (1 << shift);
}

int main() {
  int a_origin_size, a_size, *a;
  
  printf("введите количество элементов в массиве: ");
  scanf("%d", &a_origin_size);

  if(a_origin_size <= 0) {
    exit(0);
  }
  
  a_size = round2(a_origin_size);

  sn_init(&a, a_size);
  
  return 0;
}
