#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h> 

int main(int argc, char* argv[]) {
  signal(SIGCHLD, SIG_IGN);
  int serverFd, clientFd, serverLen, clientLen;
  struct sockaddr_un serverUNIXAddress;
  struct sockaddr_un clientUNIXAddress;
  struct sockaddr* serverSockAddrPtr;
  struct sockaddr* clientSockAddrPtr;
  
  serverSockAddrPtr = (struct sockaddr*) &serverUNIXAddress;
  clientSockAddrPtr = (struct sockaddr*) &clientUNIXAddress;
  serverLen = sizeof(serverUNIXAddress);
  clientLen = sizeof(clientUNIXAddress);
  serverFd = socket(AF_UNIX, SOCK_STREAM, 0);
  serverUNIXAddress.sun_family = AF_UNIX;
  strcpy(serverUNIXAddress.sun_path, "register_socket");
  unlink("register_socket");
  bind(serverFd, serverSockAddrPtr, serverLen);
  listen(serverFd, 5); // numero massimo di connessioni pendenti
  while(1){
    clientFd = accept (serverFd, clientSockAddrPtr, &clientLen)
    
  }
}

