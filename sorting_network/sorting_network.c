#include <stdio.h>
#include <stdlib.h>

#include <sys/wait.h>
#include <sys/sem.h>
#include <unistd.h>

#include "x_syscalls.h"

#define ISCHILD(pid) ((pid) == 0)

#define PROJ_ID (100)

#define MAX(a, b) ((a) > (b) ? (a) : (b))

int array_shmid, schedule_shmid, semid;
int *array, *schedule;
int a_size;

struct sembuf swait = {0, 0, 0};
struct sembuf dec = {0, -1, 0};

void sn_init();
int sn_log2(int);

void sn(int, int);
void mn(int, int);
void bs(int, int, int);
void hcr(int, int, int);
void hc(int, int, int);

void sn_init() {
  key_t key;
  int proj_id;
  int k;

  k = sn_log2(a_size);
  proj_id = PROJ_ID;

  key = x_ftok(".", proj_id++);
  array_shmid = x_shmget(key, a_size*sizeof(int), IPC_CREAT | IPC_EXCL | 0600);
  array = (int*) x_shmat(array_shmid, NULL, 0);

  key = x_ftok(".", proj_id++);
  schedule_shmid = x_shmget(key, (k*(k + 1)/2)*a_size*sizeof(int), IPC_CREAT | IPC_EXCL | 0600);
  schedule = (int*) x_shmat(schedule_shmid, NULL, 0);

  key = x_ftok(".", proj_id);
  semid = x_semget(key, 1, IPC_CREAT | IPC_EXCL | 0600);
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
  int i, j, k, *row;
  
  k = sn_log2(a_size);
  k = k*(k + 1)/2;

  for(i = 0; i < k; i++) {
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

void build_schedule(int *schedule, int a_size) {
  sn(0, a_size);
}

void sigchld_handler(int sig) {
  while(waitpid(-1, NULL, WNOHANG) > 0) {
    printf("sigchld\n");
  };
}

void run() {
  pid_t pid;
  int i;
  struct sembuf inc = {0, a_size/2, 0};
  
  semop(semid, &inc, 1);
  for(i = 0; i < a_size/2; i++) {
    pid = x_fork();
    if(ISCHILD(pid)) {
      sleep(4*i);
      semop(semid, &dec, 1);
      printf("ready\n");
      exit(0);
    }
  }
  while(semop(semid, &swait, 1) < 0);
}

int main() {
  int a_origin_size;
  int i, max;

  signal(SIGCHLD, sigchld_handler);
  
  printf("введите количество элементов в массиве: ");
  scanf("%d", &a_origin_size);

  if(a_origin_size <= 0) {
    exit(0);
  }
  
  a_size = round2(a_origin_size);
  
  //printf("a_size: %d\n", a_size);

  sn_init();

  for(i = max = 0; i < a_origin_size; i++) {
    scanf("%d", array + i);
    max = MAX(array[i], max);
  }
  for(; i < a_size; array[i++] = max);
  
  build_schedule(schedule, a_size);
  //print_schedule(schedule, a_size);

  run();

  for(i = 0; i < a_origin_size; printf("%d ", array[i++]));
  printf("\n");
  
  return 0;
}
