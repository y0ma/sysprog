#include "common.h"
#include "signal.h"

#define MAXCHILD (2)
#define STRSIZE (12)

#define IS_CHILD(pid) ((pid) == 0)

static int cnt = 0;
char arg[STRSIZE];

int mmult(int);
void run_child(int);
void sigchld_handler(int);

int main(void) {
  key_t key;
  int shm_id;
  int *addr, *addr_origin;
  matrix m[NUM_MATRIX];
  
  int i, j, retval;
  int n, p, q;

  signal(SIGCHLD, sigchld_handler);
  
  key = ftok(".", PROJ_ID);
  if(key < 0) {
    perror("ftok");
    exit(1);
  }
  
  printf("A[n x p]; B[p x q]\n\n");
  printf("input [n], [p] and [q]:\n");
  scanf("%d %d %d", &n, &p, &q);
  
  shm_id = shmget(key, (n*p + p*q + n*q + 6) * sizeof(int), IPC_CREAT | 0600);
  if(shm_id < 0) {
    perror("shmget");
    exit(1);
  }
  
  addr_origin = shmat(shm_id, NULL, 0);
  if(addr_origin == (void*) -1) {
    perror("shmat");
    exit(1);
  }
  
  m[A].cols = m[C].cols = n;
  m[A].rows = m[B].cols = p;
  m[B].rows = m[C].rows = q;
  
  addr = (int*) addr_origin;
  for(i = 0; i < NUM_MATRIX; i++) {
    *addr = m[i].cols; addr++;
    *addr = m[i].rows; addr++;
  }
  
  printf("input matrix A[%d x %d]:\n", m[A].cols, m[A].rows);
  for(i = 0; i < (m[A].cols * m[A].rows); i++) {
    scanf("%d", addr); addr++;
  }
  
  printf("input matrix B[%d x %d]:\n", m[B].cols, m[B].rows);
  for(i = 0; i < (m[B].cols * m[B].rows); i++) {
    scanf("%d", addr); addr++;
  }
  
  mmult(m[C].cols * m[C].rows);
  
  printf("\n");
  printf("Matrix C = A*B:\n");
  for(i = 0; i < m[C].rows; i++) {
    for(j = 0; j < m[C].cols; j++) {
      printf("%4d ", *addr); addr++;
    }
    printf("\n");
  }
  
  retval = shmdt(addr_origin);
  if(retval < 0) {
    perror("shmdt");
    exit(1);
  }
  
  retval = shmctl(shm_id, 0, IPC_RMID);
  if(retval < 0) {
    perror("shmctl:RMID");
    exit(1);
  }
  
  return 0;
}

int mmult(int n) {
  int i, k;
  
  k = (MAXCHILD < n) ?  MAXCHILD : n;
  
  for(i = 0; i < k; i++) {
    run_child(n - i - 1);
  }

  cnt = n - k;
  
  for(i = 0; i < n; i++) {
    waitpid(-1, NULL, 0);
  }
  
  return 0;
}

void sigchld_handler(int sig) {
  if(cnt > 0) {
    cnt--;
    run_child(cnt);
  }
}

void run_child(int cell_id) {
  int w;
  pid_t pid;

  w = sprintf(arg, "%d", cell_id);
  arg[w] = '\0';
  pid = fork();

  if(IS_CHILD(pid)) {
    execl("calc_cell", "calc_cell", arg, NULL);
    perror("fork");
    exit(1);
  } else if(pid < 0) {
    perror("fork");
    exit(1);
  }
}
