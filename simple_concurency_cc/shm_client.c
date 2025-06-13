#include "shared.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(void) {
  int shmid;
  char *shmaddr;
  int semid;
  struct sembuf sembuf;

  semid = semget(SEM_KEY, SEM_CAP, 0666);
  if (-1 == semid) {
    perror("semget");
    return 1;
  }

  shmid = shmget(SHM_KEY, SHM_SIZE, 0666);
  if (shmid == -1) {
    perror("shmget");
    return 1;
  }

  shmaddr = (char *)shmat(shmid, NULL, 0);
  if (shmaddr == (char *)-1) {
    perror("shmat");
    return 1;
  }

  for (;;) {
    SEM_OP(semid, 1, -1); // забираем чтение

    printf("Client recieve: %s\n", shmaddr);
    fflush(stdout);

    char BUF[SHM_SIZE] = "UwU meow UwU";
    SEM_OP(semid, 0, -1); // забираем запись

    const char *tmp = strncpy(shmaddr, BUF, SHM_SIZE - 1);
    if (tmp == NULL) {
      perror("strncpy");
      return 1;
    }
    if (BUF[sizeof(BUF) - 1] != 0) {
      fprintf(stdout, "%s was cutted to %s", BUF, shmaddr);
    }

    SEM_OP(semid, 0, 1); // разрешаем запись
    SEM_OP(semid, 1, 1); // разрешаем чтение
  }

  if (shmdt(shmaddr) == -1) {
    perror("shmdt");
    return 1;
  }

  return 0;
}
