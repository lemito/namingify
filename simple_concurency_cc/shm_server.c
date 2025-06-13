#include "shared.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(void) {
  int shmid;
  char *shmaddr;
  int semid;
  union semun semun_arg;
  struct sembuf sembuf;

  semid = semget(SEM_KEY, SEM_CAP, IPC_CREAT | 0666);
  if (-1 == semid) {
    perror("semget");
    return 1;
  }

  // [0] на запись, серв сразу пишет, поэтому изначально он 1
  semun_arg.val = 1;
  if (semctl(semid, 0, SETVAL, semun_arg)) {
    perror("semctrl init[0]");
    return 1;
  }

  // [1] для чтения, изначально он 0, ведь и читать надо ПОСЛЕ записи
  semun_arg.val = 0;
  if (semctl(semid, 1, SETVAL, semun_arg)) {
    perror("semctrl init[1]");
    return 1;
  }

  shmid = shmget(SHM_KEY, SHM_SIZE, IPC_CREAT | 0666);
  if (-1 == shmid) {
    perror("shmget");
    return 1;
  }

  shmaddr = (char *)shmat(shmid, NULL, 0);
  if ((char *)-1 == shmaddr) {
    perror("shmget");
    return 1;
  }

  for (;;) {
    printf("Please input message or word '!exit' to exit (this combinations of "
           "symbols cant be message): ");
    char BUF[SHM_SIZE] = 0;
    const char *inpt = fgets(BUF, SHM_SIZE, stdin);
    if (NULL == inpt) {
      perror("fgets");
      return 1;
    }

    const size_t ix = strcspn(BUF, "\n");
    if (ix >= SHM_SIZE) {
      perror("strcspn err");
      return 1;
    }
    BUF[ix] = '\0';

    if (strcmp(BUF, "!exit") == 0) {
      break;
    }

    SEM_OP(semid, 0, -1); // запись данных

    const char *tmp = strncpy(shmaddr, BUF, SHM_SIZE - 1);
    if (tmp == NULL) {
      perror("strncpy");
      return (1);
    }
    if (BUF[sizeof(BUF) - 1] != 0) {
      fprintf(LOG_FD, "%s was cutted to %s", BUF, shmaddr);
    }

    SEM_OP(semid, 1, 1); // разрешаем чтение
    SEM_OP(semid, 0, 1); // разрешаем запись

    SEM_OP(semid, 1, -1); // забираем чтение

    printf("from client: %s\n", shmaddr);
  }

  if (shmctl(shmid, IPC_RMID, NULL) == -1) {
    perror("shmctl");
    return (1);
  }

  if (semctl(semid, 0, IPC_RMID) == -1) {
    perror("semctl");
    return (1);
  }

  return 0;
}
int main(void) {
  int shmid;
  char *shmaddr;
  int semid;
  union semun semun_arg;
  struct sembuf sembuf;

  semid = semget(SEM_KEY, SEM_CAP, IPC_CREAT | 0666);
  if (-1 == semid) {
    perror("semget");
    return 1;
  }

  // [0] на запись, серв сразу пишет, поэтому изначально он 1
  semun_arg.val = 1;
  if (semctl(semid, 0, SETVAL, semun_arg)) {
    perror("semctrl init[0]");
    return 1;
  }

  // [1] для чтения, изначально он 0, ведь и читать надо ПОСЛЕ записи
  semun_arg.val = 0;
  if (semctl(semid, 1, SETVAL, semun_arg)) {
    perror("semctrl init[1]");
    return 1;
  }

  shmid = shmget(SHM_KEY, SHM_SIZE, IPC_CREAT | 0666);
  if (-1 == shmid) {
    perror("shmget");
    return 1;
  }

  shmaddr = (char *)shmat(shmid, NULL, 0);
  if ((char *)-1 == shmaddr) {
    perror("shmget");
    return 1;
  }

  for (;;) {
    printf("Please input message or word '!exit' to exit (this combinations of "
           "symbols cant be message): ");
    char BUF[SHM_SIZE] = {0};
    const char *inpt = fgets(BUF, SHM_SIZE, stdin);
    if (NULL == inpt) {
      perror("fgets");
      return 1;
    }
    const size_t ix = strcspn(BUF, "\n");
    if (ix >= SHM_SIZE) {
      perror("strcspn err");
      return 1;
    }
    BUF[ix] = '\0';

    if (strcmp(BUF, "!exit") == 0) {
      break;
    }

    SEM_OP(semid, 0, -1); // запись данных

    const char *tmp = strncpy(shmaddr, BUF, SHM_SIZE - 1);
    if (NULL == tmp) {
      perror("strncpy");
      return 1;
    }
    if (BUF[sizeof(BUF) - 1] != 0) {
      fprintf(stdout, "%s was cutted to %s", BUF, shmaddr);
    }

    SEM_OP(semid, 1, 1); // разрешаем чтение
    SEM_OP(semid, 0, 1); // разрешаем запись

    SEM_OP(semid, 1, -1); // забираем чтение

    printf("from client: %s\n", shmaddr);
  }

  if (shmctl(shmid, IPC_RMID, NULL) == -1) {
    perror("shmctl");
    return 1;
  }

  if (semctl(semid, 0, IPC_RMID) == -1) {
    perror("semctl");
    return 1;
  }

  return 0;
}
