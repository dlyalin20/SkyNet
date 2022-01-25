#include "pipe_networking.h"
#include "setup.h"

int main() {

  int sd = server_setup();
  create_sema();
  create_shmem();
  // delete to clean file
  remove("queue.txt");
  // open file
  close(open("queue.txt", O_RDWR | O_APPEND | O_CREAT, 0644));
  // fclose(fopen("data.txt", "w"));

  printf("Socket file descripter: %d\n", sd);
  listen(sd,10);
  server_connect(sd);
}
