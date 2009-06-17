#include "x_syscalls.h"

#define SEM_PROJ_ID (10)
#define SHM_PROJ_ID (20)

#define SIZE (1024)

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

  if(size == 0) {
    return NULL;
  }

  size = (size + sizeof(block) - 1) & ~(sizeof(block) - 1);

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

void print_buf() {
  block *p;
  int index;

  printf("free memory:\n");
  for(index = 0; index < out_of_memory_index; index = (&buf[index])->next_index) {
    p = buf + index;
    printf("(index = %d; size = %ld; next_index = %d) -> ", index, p->size, p->next_index);
  }
  printf("END OF MEMORY\n");

  printf("allocated memory:\n");
  for(index = 1; index < out_of_memory_index; index = (&buf[index])->next_index) {
    p = buf + index;
    printf("(index = %d; size = %ld; next_index = %d) -> ", index, p->size, p->next_index);
  }
  printf("END OF MEMORY\n");
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
}

void shm_malloc_clean() {
  x_shmdt(shmaddr);
  x_shmctl(shmid, IPC_RMID, NULL);

  x_semctl(semid, 0, IPC_RMID);
}

int main() {
  shm_malloc_init();
  
  void *a, *b, *c;
  c = shm_malloc(10);
  b = shm_malloc(20);
  a = shm_malloc(32);
  print_buf();
  shm_free(b);
  shm_free(c);
  shm_free(a);
  print_buf();

  shm_malloc_clean();

  return 0;
}
