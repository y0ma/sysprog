#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "x_syscalls.h"

#define SEM_PROJ_ID (10)
#define SHM_PROJ_ID (20)

#define SIZE (1024*1024 + 3*sizeof(block))

#define NUM_PROC (10)
#define NUM_ALLOC (10)
#define MAX_ALLOC_SIZE (1024)

#define IS_CHILD(pid) ((pid) == 0)

typedef struct block_t {
  unsigned long size;
  int next_index;
} block;

struct sembuf lock = {0, -1, 0};
struct sembuf unlock = {0, 1, 0};

int semid, shmid;
void *shmaddr;

block *buf;
int *first; // 'first use' flag

int out_of_memory_index;

void *shm_malloc(size_t);
void shm_free(void*);
void try_merge(block*, block*);

void *shm_malloc(size_t size) {
  block *p, *pn;
  int index;

  x_semop(semid, &lock, 1);
  
  if(*first) {
    buf[0].size = 0;
    buf[0].next_index = 2;
    
    buf[1].size = 0;
    buf[1].next_index = out_of_memory_index;
    
    buf[2].size = SIZE - 3*sizeof(block);
    buf[2].next_index = out_of_memory_index;

    *first = 0;
  }

  if(size == 0) {
    x_semop(semid, &unlock, 1);
    return NULL;
  }

  size = (size + sizeof(block) - 1) & ~(sizeof(block) - 1);
  
  for(p = buf; (p->next_index != out_of_memory_index) && 
	((pn = buf + p->next_index)->size < size); p = pn);
 
  if(p->next_index == out_of_memory_index) {
    x_semop(semid, &unlock, 1);
    return NULL;
  }
  
  index = p->next_index;
  if(pn->size > size) {
    pn->size -= (size + sizeof(block));
    index += pn->size/sizeof(block) + 1;
    pn = buf + index;
    pn->size = size;
  } else {
    p->next_index = pn->next_index;
  }

  for(p = (buf + 1); p->next_index < index; p = buf + p->next_index);

  pn->next_index = p->next_index;
  p->next_index = index;
  
  x_semop(semid, &unlock, 1);

  return (pn + 1);
}

void shm_free(void *ptr) {
  block *p1, *p2, *p3, *pp;
  int index;

  if(ptr == NULL) {
    return;
  }

  pp = (block*) ptr - 1;

  x_semop(semid, &lock, 1);
  
  for(p1 = buf + 1; (p1->next_index != out_of_memory_index) &&
	((p2 = buf + p1->next_index) != pp); p1 = p2);

  if(p1->next_index == out_of_memory_index) {
    x_semop(semid, &unlock, 1);
    return;
  }

  index = p1->next_index;
  p1->next_index = p2->next_index;

  for(p1 = buf; p1->next_index < index; p1 = buf + p1->next_index);
  
  p3 = NULL;
  if(p1->next_index != out_of_memory_index) {
    p3 = buf + p1->next_index;
  }
  p2->next_index = p1->next_index;
  p1->next_index = index;

  try_merge(p2, p3);
  try_merge(p1, p2);
  
  x_semop(semid, &unlock, 1);
}

void try_merge(block *p1, block *p2) {
  if(( (char*) (p1 + 1) + p1->size) == (char*) p2) {
    p1->size += (p2->size + sizeof(block));
    p1->next_index = p2->next_index;
  }
}

void print_memory() {
  block *p;
  int index;

  printf("\n\nfree memory:\n");
  for(index = 0; index < out_of_memory_index; index = (&buf[index])->next_index) {
    p = buf + index;
    printf("[index = %d; size = %ld; next_index = %d] -> ", index, p->size, p->next_index);
  }
  printf("END OF MEMORY\n");

  printf("\n\nallocated memory:\n");
  for(index = 1; index < out_of_memory_index; index = (&buf[index])->next_index) {
    p = buf + index;
    printf("[index = %d; size = %ld; owner = %d; next = %d] -> ", index, p->size, ((index != 1) ? *(pid_t*)(p+1) : 0) , p->next_index);
  }
  printf("END OF MEMORY\n");
}

void test_routine() {
  pid_t *ptrs[NUM_ALLOC], pid;
  int i;

  pid = getpid();
  printf("my_pid is %d\n", pid);
  srand(pid);

  for(i = 0; i < NUM_ALLOC; i++) {
    ptrs[i] = shm_malloc(rand() % MAX_ALLOC_SIZE);
    if(ptrs[i]) {
      *ptrs[i] = pid;
    }
    sleep(rand() % 2);
  }

  print_memory();

  for(i = 0; i < NUM_ALLOC; i++) {
    if(ptrs[i]) {
      printf("#%d == %d: ", pid, *ptrs[i]);
      printf("%s\n", *ptrs[i] == pid ? "true" : "false");
      shm_free(ptrs[i]);
      sleep(rand() % 2);
    }
  }
}

void run_test() {
  int n;
  pid_t pid;

  n = NUM_PROC;
  while(n-- > 0) {
    pid = x_fork();
    
    if(IS_CHILD(pid)) {
      // do something;
      test_routine();

      exit(0);
    }
  }

  n = NUM_PROC;
  while(n-- > 0) {
    pid = x_wait(NULL);
    //printf("Done: %d\n", pid);
  }
}

void shm_malloc_init() {
  key_t key;

  key = x_ftok(".", SEM_PROJ_ID);
  semid = x_semget(key, 1, IPC_CREAT | IPC_EXCL | 0600);
  x_semop(semid, &unlock, 1);

  key = x_ftok(".", SHM_PROJ_ID);
  shmid = x_shmget(key, SIZE + sizeof(int), IPC_CREAT | IPC_EXCL | 0600);
  shmaddr = x_shmat(shmid, NULL, 0);
  
  first = (int*) shmaddr;
  *first = 1;
  buf = (block*) (shmaddr + sizeof(int));

  out_of_memory_index = SIZE/sizeof(block);

  shm_malloc(0);
}

void shm_malloc_clean() {
  x_shmdt(shmaddr);
  x_shmctl(shmid, IPC_RMID, NULL);

  x_semctl(semid, 0, IPC_RMID);
}

int main() {
  shm_malloc_init();
  
  run_test();
  print_memory();

  shm_malloc_clean();

  return 0;
}
