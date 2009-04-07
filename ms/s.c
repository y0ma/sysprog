#include "common.h"

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
  
  sem_id = semget(key, 1, 0);
  
  shm_id = shmget(key, 0, 0);
  if(shm_id < 0) {
    perror("shmget");
    exit(1);
  }
  
  nums = (mystruct*) shmat(shm_id, NULL, 0);
  if(nums == (void*) -1) {
    perror("shmat");
    exit(1);
  }
  
  printf("s> i'm sleep!\n");
  sleep(5);
  
  nums->a = nums->a + nums->b;
  
  //уменьшить значение семафора на 1
  sops[0].sem_num = 0;
  sops[0].sem_op = -1;
  sops[0].sem_flg = 0;
  retval = semop(sem_id, sops, 1);
  if(retval < 0) {
    perror("semop");
    exit(1);
  }
  
  retval = shmdt(nums);
  if(retval < 0) {
    exit(1);
  }
  
  return 0;
}
