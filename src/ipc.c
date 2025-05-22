#include "ipc.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <errno.h>

// Wrapper for shared memory creation
int create_shared_memory(key_t key, size_t size){
  int shmid = shmget(key, size, IPC_CREAT|0666);
  if(shmid == -1){
    perror("shmget");
    exit(EXIT_FAILURE);
  }
  return shmid;
}

void *attach_shared_memory(int shmid){
  void *addr = shmat(shmid, NULL, 0);
  if(addr == (void *)-1){
    perror("shmat");
    exit(EXIT_FAILURE);
  }
  return addr;
}

void detach_shared_memory(void *shmaddr){
  if(shmdt(shmaddr) == -1){
    perror("shmdt");
  }
}

void destroy_shared_memory(int shmid){
  if(shmctl(shmid, IPC_RMID, NULL) == -1){
    perror("shmctl");
  }
}

// Semaphore helpers
int create_semaphore_set(key_t base_key, int count){
  int semid = semget(base_key, count, IPC_CREAT|0666);
  if(semid == -1){
    perror("semget");
    exit(EXIT_FAILURE);
  }
  return semid;
}

void init_semaphore(int semid, int index, int value){
  if(semctl(semid, index, SETVAL, value) == -1){
    perror("semctl SETVAL");
    exit(EXIT_FAILURE);
  }
}

void semaphore_wait(int semid, int index){
  struct sembuf op = {index, -1, 0};
  if(semop(semid, &op, 1) == -1){
    perror("semop wait");
    exit(EXIT_FAILURE);
  }
}

void semaphore_signal(int semid, int index){
  struct sembuf op = {index, 1, 0};
  if(semop(semid, &op, 1) == -1){
    perror("semop signal");
    exit(EXIT_FAILURE);
  }
}

void destroy_semaphore_set(int semid){
  if(semctl(semid, 0, IPC_RMID) == -1){
    perror("semctl IPC_RMID");
  }
}
