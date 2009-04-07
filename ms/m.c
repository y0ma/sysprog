#include "common.h"

int main(void) {
  key_t key;
  int shm_id, sem_id;
  int retval;
  mystruct *nums;
  struct sembuf sops[1];
  
  key = ftok(".", PROJ_ID);
  if(key < 0) {
    perror("m:ftok");
    exit(1);
  }
  
  sem_id = semget(key, 1, 0);
  
  shm_id = shmget(key, 0, 0);
  if(shm_id < 0) {
    perror("m:shmget");
    exit(1);
  }
  
  nums = (mystruct*) shmat(shm_id, NULL, 0);
  if(nums == (void*) -1) {
    perror("m:shmat");
    exit(1);
  }
  
  //ждать обнуления семафора
  sops[0].sem_num = 0;
  sops[0].sem_op = 0;
  sops[0].sem_flg = 0;
  retval = semop(sem_id, sops, 1);
  if(retval < 0) {
    perror("semop");
    exit(1);
  }
 
  
  nums->a = nums->a * nums->c;
  
  retval = shmdt(nums);
  if(retval < 0) {
    exit(1);
  }
  
  return 0;
}
