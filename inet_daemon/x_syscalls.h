#ifndef __X_SYSCALLS__
#define __X_SYSCALLS__

#include <sys/types.h>
#include <sys/sem.h>
#include <sys/shm.h>

#define X_ERROR_EXIT (-1)

#define IS_CHILD(pid) ((pid) == 0)

extern void x_error(const char*);

extern void *x_malloc(size_t);

extern void x_chdir(const char*);

extern pid_t x_fork();
extern pid_t x_wait(int*);
extern pid_t x_getpid();
extern pid_t x_getppid();
extern void x_setpgrp();

extern key_t x_ftok(const char*, int);

extern int x_semget(key_t, int, int);
extern void x_semctl(int, int, int);
extern void x_semop(int, struct sembuf*, int);

extern int x_shmget(key_t, size_t, int);
extern void x_shmctl(int, int, struct shmid_ds*);
extern void *x_shmat(int, const void*, int);
extern void x_shmdt(const void*);

#endif // __X_SYSCALLS__
