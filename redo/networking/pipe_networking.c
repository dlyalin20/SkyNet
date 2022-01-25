#include "pipe_networking.h"
#include "setup.h"
#include "datatypes.h"
void append_to_queue(char * str, int client_socket){
  // NOT DoNEASK USER OR SEARCH IF I ALREADY HAVE FILE
  int semd = semget(KEY, 1, 0);
  struct sembuf sb;
  sb.sem_num = 0;
  sb.sem_flg = SEM_UNDO;
  sb.sem_op = -1; //setting the operation to down
  semop(semd, &sb, 1); //preform operation
  int shmd = shmget(SHMKEY, 0, 0);
  // int* size = shmat(shmd, 0, 0); //get access to shared memory

  int fd = open("data.txt", O_WRONLY | O_APPEND);
  // *size = strlen(str);
  write(fd, str, strlen(str));
  // shmdt(size); //lose access to shared memory

  sb.sem_op = 1; //set the operation to up
  semop(semd, &sb, 1); //preform operation
  write(client_socket, "success", 7);

  // NOT DONE: SEND NEW QUEUE TO ALL CLIENTS
}

void sendQueue(int client_socket){
  // read and send queue to client
}

void sendSong(char * song_name, int client_socket){
  // send file from music tempdirectory
}

void * rot13(int client_socket){
    // READ SEVER COMMANDS BY THE CLIENT
    printf("NEW REQUEST\n");
    struct request req;
    read(client_socket,&req, sizeof(struct request));
    printf("%s: %s\n",req.request_type, req.song_name);
    if (!strcmp(req.request_type, "add")){
      append_to_queue(req.song_name,client_socket);
    }
    if(!strcmp(req.request_type, "get_queue")){
      sendQueue(client_socket);
    }
    if(!strcmp(req.request_type, "get_song")){
      sendSong(req.song_name, client_socket);
    }
    // append_to_queue(array);
    // write(client_socket, array, 50);

}
/*=========================
  server_setup
  args:

  creates the WKP (upstream) and opens it, waiting for a
  connection.

  removes the WKP once a connection has been made

  returns the file descriptor for the upstream pipe.
  =========================*/
int server_setup() {
  //use getaddrinfo
  struct addrinfo * hints, * results;
  hints = calloc(1,sizeof(struct addrinfo));
  hints->ai_family = AF_INET;
  hints->ai_socktype = SOCK_STREAM; //TCP socket
  // hints->ai_flags = AI_PASSIVE; //only needed on server
  getaddrinfo("127.0.0.1", "9845", hints, &results);  //Server sets node to NULL

  //create socket
  int sd = socket(results->ai_family, results->ai_socktype, results->ai_protocol);

  bind(sd, results->ai_addr, results->ai_addrlen);

  //DO STUFF

  free(hints);
  freeaddrinfo(results);
  return sd;
}

/*=========================
  server_connect
  args: int from_client

  handles the subserver portion of the 3 way handshake

  returns the file descriptor for the downstream pipe.
  =========================*/
int server_connect(int sd) {
  int client_socket;
  socklen_t sock_size;
  struct sockaddr_storage client_address;
  sock_size = sizeof(client_address);
  client_socket = accept(sd,(struct sockaddr *)&client_address, &sock_size);
  // forking server part
  int f = fork();
  if (f){
    server_connect(sd);
  }else{
    rot13(client_socket);
  }
}


// /*=========================
//   server_handshake
//   args: int * to_client
//
//   Performs the server side pipe 3 way handshake.
//   Sets *to_client to the file descriptor to the downstream pipe.
//
//   returns the file descriptor for the upstream pipe.
//   =========================*/
// int server_handshake(int *to_client) {
//
//   int b, from_client;
//   char buffer[HANDSHAKE_BUFFER_SIZE];
//
//   printf("[server] handshake: making wkp\n");
//   b = mkfifo(WKP, 0600);
//   if ( b == -1 ) {
//     printf("mkfifo error %d: %s\n", errno, strerror(errno));
//     exit(-1);
//   }
//   //open & block
//   from_client = open(WKP, O_RDONLY, 0);
//   //remove WKP
//   remove(WKP);
//
//   printf("[server] handshake: removed wkp\n");
//
//
//   *to_client = open(buffer, O_WRONLY, 0);
//   //create SYN_ACK message
//   srand(time(NULL));
//   int r = rand() % HANDSHAKE_BUFFER_SIZE;
//   sprintf(buffer, "%d", r);
//
//   write(*to_client, buffer, sizeof(buffer));
//   //rad and check ACK
//   read(from_client, buffer, sizeof(buffer));
//
//   return from_client;
// }


/*=========================
  client_handshake
  args: int * to_server

  Performs the client side pipe 3 way handshake.
  Sets *to_server to the file descriptor for the upstream pipe.

  returns the file descriptor for the downstream pipe.
  =========================*/
int client_handshake() {

  struct addrinfo * hints, * results;
  hints = calloc(1,sizeof(struct addrinfo));
  hints->ai_family = AF_INET;
  hints->ai_socktype = SOCK_STREAM; //TCP socket
  getaddrinfo("127.0.0.1", "9845", hints, &results);  //Server sets node to NULL

  int sd = socket(results->ai_family, results->ai_socktype, results->ai_protocol);

  bind(sd, results->ai_addr, results->ai_addrlen);


  free(hints);
  freeaddrinfo(results);

  socket(results->ai_family, results->ai_socktype, results->ai_protocol);
  connect(sd, results->ai_addr, results->ai_addrlen);

  return sd;

}
