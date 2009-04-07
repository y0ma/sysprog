#include "common.h"

void run_child(void);

int main(void) {
  key_t key;
  int shm_id, sem_id;
  int retval;
  mystruct *nums;
  struct sembuf sops[1];
  
  key = ftok(".", PROJ_ID);
  if(key < 0) {
    perror("ftok");
    exit(1);
  }
  
  shm_id = shmget(key, sizeof(mystruct), IPC_CREAT | 0600);
  if(shm_id < 0) {
    perror("shmget");
    exit(1);
  }
  
  nums = (mystruct*) shmat(shm_id, NULL, 0);
  if(nums == (void*) -1) {
    perror("shmat");
    exit(1);
  }
  
  printf("input a: ");
  scanf("%d", &(nums->a));
  printf("input b: ");
  scanf("%d", &(nums->b));  
  printf("input c: ");
  scanf("%d", &(nums->c));
  
  sem_id = semget(key, 1, IPC_CREAT | 0600);
  if(sem_id < 0) {
    perror("semget");
    exit(1);
  }
  
  //увеличить значение семафора на 1
  sops[0].sem_num = 0;
  sops[0].sem_op = 1;
  sops[0].sem_flg = 0;
  retval = semop(sem_id, sops, 1);
  if(retval < 0) {
    perror("semop");
    exit(1);
  }
  
  run_child();
  
  waitpid(-1, NULL, 0);
  waitpid(-1, NULL, 0);
  
  retval = semctl(sem_id, 0, IPC_RMID);
  if(retval < 0) {
    exit(1);
  }
  
  printf("(a+b)*c = %d\n", nums->a);
  
  retval = shmdt(nums);
  if(retval < 0) {
    exit(1);
  }
  
  retval = shmctl(shm_id, IPC_RMID, 0);
  if(retval < 0) {
    exit(1);
  }
  
  return 0;
}

void run_child() {
  int s_pid;
  int m_pid;
  
  s_pid = fork();
  
  if(ISCHILD(s_pid)) {
    execl("./s", "sum", NULL);
    exit(1);
  } else if(s_pid < 0) {
    exit(1);
  }
  
  m_pid = fork(); 
  
  if(ISCHILD(m_pid)){
    execl("./m", "mul", NULL); 
    exit(1);
  } else if(m_pid < 0) {
    exit(1);
  }
  
  return;
}
