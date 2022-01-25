#include <stdio.h>
#include <stdlib.h>

#include <sys/ipc.h>
#include <sys/types.h>

#include <string.h>
#include <unistd.h>

#include <sys/sem.h>
#include <sys/shm.h>

#include <errno.h>

#include <fcntl.h>

#define KEY 24602
#define SHMKEY 24601


union semun {
   int              val;    // Value for SETVAL
   struct semid_ds *buf;    // Buffer for IPC_STAT, IPC_SET *\/
   unsigned short  *array;  // Array for GETALL, SETALL *\/ */
   struct seminfo  *__buf;  // Buffer for IPC_INFO */
                               // (Linux-specific) */
};



void delete_shmem();
void create_shmem();
int create_sema();
// int main(int argc, char* argv[]) {
//   if (argc == 2){
//     if (!strcmp("create", argv[1])){
//       printf("%s\n", "creating");
//       create_sema();
//       create_shmem();
//       // open file
//       open("data.txt", O_RDWR | O_APPEND | O_CREAT, 0644);
//     }else if (!strcmp("delete", argv[1])){
//       printf("%s\n", "deleting");
//       delete_shmem();
//       int semd = create_sema();
//       semd = semctl(semd, IPC_RMID, 0);
//     }else{
//       printf("%s\n", "arguement can be \"create\" or \"delete\"");
//     }
//   }else{
//     printf("%s\n", "provide exactly one arguement");
//   }
// }
void delete_shmem(){
  int shmd;
  int *data;
  /* Create the segment (happens once) - shmget */
  /* Access the segment (happens once per process) - shmget */
  shmd = shmget(SHMKEY, 0, 0);
  // printf("shmd: %d\n", shmd);
  /* Remove the segment (happens once) - shmctl */
  shmctl(shmd, IPC_RMID, 0);
}

void create_shmem(){
  int shmd;
  int *data;
  /* Create the segment (happens once) - shmget */
  /* Access the segment (happens once per process) - shmget */
  // ULONG_MAX - (1 << 24)
  shmd = shmget(SHMKEY, sizeof(int), IPC_CREAT | 0600);
  /* Attach the segment to a variable (once per process) */

  data = shmat(shmd, 0, 0);
  *data = 0;
  // printf("data: %d\n", *(data));

  /* Detach the segment from a variable (once per process) - shmdt */
  shmdt(data);
}
int create_sema(){
  // create semaphore
  int v, r;
  int semd = semget(KEY, 1, IPC_CREAT | IPC_EXCL | 0644);
  if (semd == -1) {
    // printf("error %d: %s\n", errno, strerror(errno));
    semd = semget(KEY, 1, 0);
    v = semctl(semd, 0, GETVAL, 0);
    // printf("semctl returned: %d\n", v);
  }
  else {
    union semun us;
    us.val = 1;
    r = semctl(semd, 0, SETVAL, us);
    // printf("semctl returned: %d\n", r);
  }
  return semd;
}
