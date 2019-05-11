#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>

#define MAX_DATA 1000
#define PORT 6767
#define BACKLOG 10 // how many pending connections queue will hold

int main (int argc, char *argv[]){

  //structs to create socket and response client getaddrinfo request
  struct addrinfo hints, *res;
  memset(&hints,0, sizeof(hints);
  hints.ai_flags = AI_PASSIVE;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_family = AF_INET;
  
  int s = getaddrinfo(NULL,PORT,&hints,&res);
  if (s != 0) {
      perror("ERROR getaddrinfo");
      exit(1);
  }
  
  //creating socket
  int sock_fd = socket(res->ai_family,res->ai_socktype,res->ai_protocol);
  if (sock_fd == -1){
   perror("ERROR creating socket");
   exit(1);
  }
  
  //assigning a name to a socket
  if (bind(sock_fd,(struct sockaddr *) res->ai_addr,res->ai_addrlen) != 0) {
    perror("ERROR bind socket");
    exit(1);
  }
  
  //listening
  if (listen(sock_fd,BACKLOG) != 0) {
    perror("ERROR listen");
    exit(1);
  }
  
  struct sockaddr_storage client; //IPv4 and IPv6
  
  //accept the conection and retrieve the client info and store in clie struct
  int client_fd = accept(sock_fd,(struct sockaddr *) &client, sizeof(client));
  if (client_fd == -1){
    perror("ERROR accept");
    exit(1);
  }
  
  char buffer[MAX_DATA];
  int len = read(client_fd,buffer,sizeof(buffer) - 1);
  
  buffer[len] ='\0';
  send(client_fd,buffer,len,0);
    
  //close comunication
  if(shutdown(client_fd , SHUT_RDWR)){
    perror("ERROR shutdown");
    exit(1);
  }
  
  return 0;
} 
 