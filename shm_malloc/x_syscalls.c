#include "x_syscalls.h"

// declaration

void x_error(const char*);

void *x_malloc(size_t);

pid_t x_fork();

key_t x_ftok(const char*, int);

int x_semget(key_t, int, int);
void x_semctl(int, int, int);
void x_semop(int, struct sembuf*, int);

int x_shmget(key_t, size_t, int);
void x_shmctl(int, int, struct shmid_ds*);
void *x_shmat(int, const void*, int);
void x_shmdt(const void*);

// implementation

void *x_malloc(size_t size) {
  void *ptr;

  ptr = malloc(size);
  if(ptr == NULL) {
    x_error("can't allocate memory");
  }

  return ptr;
}

pid_t x_fork() {
  pid_t pid;

  pid = fork();
  if(pid < 0) {
    x_error("can't fork");
  }

  return pid;
}

key_t x_ftok(const char *pathname, int proj_id) {
  key_t key;

  key = ftok(pathname, proj_id);
  if(key < 0) {
    x_error("can't get a System V IPC key");
  }
  
  return key;
}

int x_semget(key_t key, int nsems, int semflg) {
  int semid;

  semid = semget(key, nsems, semflg);
  if(semid < 0) {
    if(errno == EEXIST) {
      semid = x_semget(key, nsems, 0);
      x_semctl(semid, nsems, IPC_RMID);
      semid = x_semget(key, nsems, semflg);
    } else {
      x_error("can't get a semaphore set identifier");
    }
  }
  
  return semid;
}

void x_semctl(int semid, int semnum, int cmd) {
  int retval;

  retval = semctl(semid, semnum, cmd);
  if(retval < 0) {
    x_error("can't perform semaphore control operations");
  }
}

void x_semop(int semid, struct sembuf *sops, int nsops) {
  int retval;

  retval = semop(semid, sops, nsops);
  if(retval < 0) {
    x_error("can't perform semaphore operations");
  }
}

int x_shmget(key_t key, size_t size, int shmflg) {
  int shmid;

  shmid = shmget(key, size, shmflg);
  if(shmid < 0) {
    if(errno == EEXIST) {
      shmid = x_shmget(key, 0, 0);
      x_shmctl(shmid, IPC_RMID, NULL);
      shmid = x_shmget(key, size, shmflg);
    } else {
      x_error("can't allocate a shred memory segment");
    }
  }
  
  return shmid;
}

void x_shmctl(int shmid, int cmd, struct shmid_ds *buf) {
  int retval;

  retval = shmctl(shmid, cmd, buf);
  if(retval < 0) {
    x_error("can't control shared memory");
  }
}

void *x_shmat(int shmid, const void *shmaddr, int shmflg) {
  void *ptr;

  ptr = shmat(shmid, shmaddr, shmflg);
  if(ptr == NULL) {
    x_error("can't attach the shared memory segment");
  }
  
  return ptr;
}

void x_shmdt(const void *shmaddr) {
  int retval;

  retval = shmdt(shmaddr);
  if(retval < 0) {
    x_error("can't detach the shared memory segment");
  }
}

void x_error(const char *message) {
  perror(message);
  exit(X_ERROR_EXIT);
}
