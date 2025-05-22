#ifndef IPC_H
#define IPC_H

#include "common.h"

// Shared memory operations
int create_shared_memory(key_t key, size_t size);
void *attach_shared_memory(int shmid);
void detach_shared_memory(void *shmaddr);
void destroy_shared_memory(int shmid);

// Semaphore operations
int create_semaphore_set(key_t base_key, int count);
void init_semaphore(int semid, int index, int value);
void semaphore_wait(int semid, int index);
void semaphore_signal(int semid, int index);
void destroy_semaphore_set(int semid);

#endif // IPC_H
