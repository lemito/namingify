#ifndef SHARED_H
#define SHARED_H

#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>

#define SHM_SIZE 1024
#define SEM_KEY 0x1234
#define SHM_KEY 0xABCD
#define SEM_CAP 2

#define LOG_FD stdout

union semun {
  int val;
  struct semid_ds *buf;
  unsigned short *array;
};

// require sembuf
#define SEM_OP(sem, ix, op)                                                    \
  ({                                                                           \
    sembuf.sem_num = ix;                                                       \
    sembuf.sem_op = op;                                                        \
    sembuf.sem_flg = 0;                                                        \
    if (semop(sem, &sembuf, 1) == -1) {                                        \
      perror("semop");                                                         \
      exit(1);                                                                 \
    }                                                                          \
  })

#endif // SHARED_H
