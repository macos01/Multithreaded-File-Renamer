#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>

#define MAX_DATA 1000
#define PORT "6767"
#define BACKLOG 10 // how many pending connections queue will hold

int main (int argc, char *argv[]){

  //structs to create socket and response client getaddrinfo request
  struct addrinfo hints, *res;
  memset(&hints,0, sizeof(hints));
  hints.ai_flags = AI_PASSIVE;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_family = AF_INET;
  
  
  /*
	 AI_PASSIVE with NULL -> 0.0.0.0
*/
  int s = getaddrinfo(NULL, PORT,&hints,&res);
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
    
  fprintf(stdout,"Ready \n");
  
  
   struct sockaddr_storage client; //IPv4 and IPv6
    socklen_t client_size = sizeof(client);
    
    //accept the conection and retrieve the client info and store in client struct
    int client_fd = accept(sock_fd,(struct sockaddr *) &client, &client_size);
    if (client_fd == -1){
      perror("ERROR accept");
      exit(1);
    }
    
    char buffer[MAX_DATA];
  
  while(1) {
      
   
    
    
    int len = recv(client_fd,buffer,sizeof(buffer) - 1,0);
    
    if (len == -1){
      perror("ERROR reading from client");
      exit(1);
    }
    
    
//     buffer[len] = '\0';
//       
//     char host[NI_MAXHOST], service[NI_MAXSERV];
// 
//     if (getnameinfo((struct sockaddr *) &client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICSERV) != 0) {
// 	perror("ERROR getnameinfo");
// 	exit(1);
//     }
//     
//     printf("Received %i bytes from %s:%s\n", len, host, service);
    printf("Received: %s\n",buffer);
    
    memset(&buffer,0,sizeof(buffer));
    
    
//     if (send(client_fd,buffer,len,0) != len){
//       perror("ERROR sending data to client");
//       exit(1);
//     }
    
//     printf("Enviado\n");
    
      
  }
  
    
//     if(shutdown(client_fd , SHUT_RDWR)){
//       perror("ERROR shutdown");
//       exit(1);
//     } 
  
  
  return 0;
} 
 