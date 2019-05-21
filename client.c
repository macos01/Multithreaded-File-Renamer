#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>

int main(int argc, char **argv) {
  
  size_t len;
  
  if (argc < 3){
      fprintf(stdout,"Usage: <ip> <port>\n");
      exit(1);
  }
  
  //struct addrinfo to setup socket getting info of the server
  struct addrinfo hints, *res;
  memset(&hints,0, sizeof(hints));
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_family = AF_INET;
 
  
  int s = getaddrinfo(argv[1],argv[2],&hints,&res);
  if (s != 0) {
    perror("ERROR getaddrinfo");
    exit(1);
  }
  
  //creating socket
  int socket_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  if (socket_fd == -1){
    perror("ERROR creating socket");
    exit(1);
  }
   
  freeaddrinfo(res);
   
  //conecting to server  
  if (connect(socket_fd, res->ai_addr, res->ai_addrlen) != 0){
    perror("ERROR connect");
    exit(1);
  }
  
    char *buffer = "Esto es una prueba";
    
    len = strlen(buffer) + 1;
    
 
  
  while(1){
    
  
    if (send(socket_fd,buffer,len,0) == -1){
	 perror("ERROR writing socket");
	 exit(1);
    }
    
    printf("Enviado\n");
   
//     char resp[1000];
//     int len = read(socket_fd, resp, sizeof(resp) - 1);
//     
//     if (len == -1){
//       perror("ERROR reading from server");
//       exit(1);
//     }
//     
//     resp[len] = '\0';
//     printf("Answer: %s\n", resp);
    
    sleep(2);
    
  }
  
  return 0;
  
 
}
