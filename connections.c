/*
Functions to run a forking socket server_setup
chose to use forking over choose because we will be using exceptvp to call adder.run and player.run
logic is that why use choose if we are going to fork anyways for exceptvp
*/
/*=========================
  server_setup
  args:

  creates the WKP (upstream) and opens it, waiting for a
  connection.

  removes the WKP once a connection has been made

  returns the file descriptor for the upstream pipe.
  =========================*/
#include "connections.h"
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
    close(client_socket);
    return 0;
  }else{
    return (client_socket);
  }
}



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
