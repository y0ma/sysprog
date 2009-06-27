#include <stdio.h>
#include <stdlib.h>

#include <sys/wait.h>
#include <sys/sem.h>
#include <unistd.h>

#include "x_syscalls.h"

#define ISCHILD(pid) ((pid) == 0)

#define SEM_PROJ_ID (20)
#define SHM_PROJ_ID (30)

#define MAX(a, b) ((a) > (b) ? (a) : (b))

int shmid, semid;
void *shmaddr;
int *array, *schedule;
int a_size, cascades;

struct sembuf swait[2] = {{0, 0, 0}, {1, 0, 0}};
struct sembuf dec[2] = {{0, -1, 0}, {1, -1, 0}};

void sn_init();
int sn_log2(int);

void sn(int, int);
void mn(int, int);
void bs(int, int, int);
void hcr(int, int, int);
void hc(int, int, int);

void sn_init() {
  key_t key;
  int k;
  int shm_size;

  k = sn_log2(a_size);
  cascades = k*(k + 1)/2;

  key = x_ftok(".", SEM_PROJ_ID);
  semid = x_semget(key, 2, IPC_CREAT | IPC_EXCL | 0600);

  shm_size = a_size + cascades*a_size;
  key = x_ftok(".", SHM_PROJ_ID);
  shmid = x_shmget(key, shm_size*sizeof(int), IPC_CREAT | IPC_EXCL | 0600);
  shmaddr = x_shmat(shmid, NULL, 0);

  array = (int*) shmaddr;
  schedule = (int*) shmaddr + a_size;
}

void clean(int sig) {
  x_shmdt(shmaddr);
  x_shmctl(shmid, IPC_RMID, NULL);
  x_semctl(semid, 0, IPC_RMID);
}

int sn_log2(int n) {
  int k;

  if(n <= 0) {
    return -1;
  }
  
  k = 0;
  n += (n - 1);
  while(n > 1) {
    k++;
    n >>= 1;
  }

  return k;
}


int round2(int n) {
  int k;

  k = sn_log2(n);

  return (1 << k);
}

void print_schedule() {
  int i, j, *row;

  for(i = 0; i < cascades; i++) {
    row = schedule + i*a_size;
    for(j = 0; j < a_size; j++) {
      printf("%d ", row[j]);
    }
    printf("\n");
  }
}

void sn(int start, int n) {
  if(n > 2) {
    sn(start, n/2);
    sn(start + n/2, n/2);
  }
  
  mn(start, n);
}

void mn(int start, int n) {
  int cascade, k;

  k = sn_log2(n);
  cascade = k*(k - 1)/2;
  
  hcr(cascade, start, n);
  
  if(n > 2) {
    cascade++;
    bs(cascade, start, n/2);
    bs(cascade, start + n/2, n/2);
  }
}

void bs(int cascade, int start, int n) {
  hc(cascade, start, n);

  if(n > 2) {
    cascade++;
    bs(cascade, start, n/2);
    bs(cascade, start + n/2, n/2);
  }
}

void hcr(int cascade, int start, int n) {
  int *row;
  int i, nn;

  row = schedule + cascade*a_size;
  nn = n/2;

  for(i = 0; i < nn; i++) {
    *(row + start + 2*i) = start + i;
    *(row + start + 2*i + 1) = start + n - 1 - i;
  }
}

void hc(int cascade, int start, int n) {
  int *row;
  int i, nn;

  row = schedule + cascade*a_size;
  nn = n/2;

  for(i = 0; i < nn; i++) {
    *(row + start + 2*i) = start + i;
    *(row + start + 2*i + 1) = start + nn + i;
  }
}

void build_schedule() {
  sn(0, a_size);
}

void sigchld_handler(int sig) {
  while(waitpid(-1, NULL, WNOHANG) > 0);
}

void run_comparator(int i, int cascades) {
  int j, tmp;
  int *wires;

  wires = schedule + 2*i;

  for(j = 0; j < cascades; j++) {
    x_semop(semid, &swait[(j + 1)%2], 1);

    if(array[wires[0]] > array[wires[1]]) {
      tmp = array[wires[0]];
      array[wires[0]] = array[wires[1]];
      array[wires[1]] = tmp;
    }

    x_semop(semid, &dec[j%2], 1);

    wires += a_size;
  }
}

void run() {
  pid_t pid;
  int i;
  struct sembuf inc[2] = {{0, a_size/2, 0}, {1, a_size/2, 0}};
  
  x_semop(semid, &inc[0], 1);

  for(i = 0; i < a_size/2; i++) {
    pid = x_fork();

    if(ISCHILD(pid)) {
      run_comparator(i, cascades);
      exit(0);
    }
  }

  i = 0;
  while(13) {
    x_semop(semid, &swait[i%2], 1);
    x_semop(semid, &inc[(i + 1)%2], 1);
    
    i++;
    if(cascades == i) {
      break;
    }
  }
}

int main(int argc, char **argv) {
  int a_origin_size;
  int i, max;

  signal(SIGCHLD, sigchld_handler);
  signal(SIGTERM, clean);
  signal(SIGQUIT, clean);
  
  if(argc == 1) {
    printf("введите количество элементов в массиве: ");
    fflush(stdout);
  }
  
  scanf("%d", &a_origin_size);

  if(a_origin_size <= 0) {
    exit(0);
  }
  
  a_size = round2(a_origin_size);

  sn_init();

  for(i = max = 0; i < a_origin_size; i++) {
    scanf("%d", array + i);
    max = MAX(array[i], max);
  }
  for(; i < a_size; array[i++] = max);
  
  if(a_origin_size > 1) {
    build_schedule();
    //print_schedule();

    run();
  }

  for(i = 0; i < a_origin_size; printf("%d ", array[i++]));
  printf("\n");

  clean(0);
  
  return 0;
}
