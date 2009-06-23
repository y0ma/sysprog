#ifndef __X_SYSCALLS__
#define __X_SYSCALLS__

#include <sys/types.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>

#define X_ERROR_EXIT (-1)

extern void *x_malloc(size_t);

extern pid_t x_fork(void);
extern pid_t x_wait(int*);
extern pid_t x_getpid(void);
extern pid_t x_getppid(void);

extern key_t x_ftok(const char*, int);

extern int x_semget(key_t, int, int);
extern void x_semctl(int, int, int);
extern void x_semop(int, struct sembuf*, int);

extern int x_shmget(key_t, size_t, int);
extern void x_shmctl(int, int, struct shmid_ds*);
extern void *x_shmat(int, const void*, int);
extern void x_shmdt(const void*);

extern int x_msgget(key_t, int);
extern void x_msgctl(int, int, struct msqid_ds*);
extern void x_msgsnd(int, const void*, size_t, int);
extern void x_msgrcv(int, void*, size_t, long, int);

#endif // __X_SYSCALLS__
