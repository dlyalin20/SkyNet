#ifndef setup_h
#define setup_h

#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>

void delete_shmem();
void create_shmem();
int create_sema();

#define KEY 24602
#define SHMKEY 24601
#endif
