#ifndef __X_SYSCALLS__
#define __X_SYSCALLS__

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

#define X_ERROR_EXIT (-1)

extern void *x_malloc(size_t);

extern pid_t x_fork();

extern key_t x_ftok(const char*, int);

extern int x_semget(key_t, int, int);
extern void x_semctl(int, int, int);
extern void x_semop(int, struct sembuf*, int);

extern int x_shmget(key_t, size_t, int);
extern void x_shmctl(int, int, struct shmid_ds*);
extern void *x_shmat(int, const void*, int);
extern void x_shmdt(const void*);

#endif // __X_SYSCALLS__
