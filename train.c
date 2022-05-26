#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h> 
#include <errno.h>
#include <stdbool.h>

#include "globals.h"

void getMap();

int main(int argc, char* argv[]){
  int clientFd, serverLen, result;
  struct sockaddr_un serverUNIXAddress;
  struct sockaddr* serverSockAddrPtr;
  serverSockAddrPtr = (struct sockaddr*) &serverUNIXAddress;
  serverLen = sizeof (serverUNIXAddress);
  clientFd = socket (AF_UNIX, SOCK_STREAM, 0);
  serverUNIXAddress.sun_family = AF_UNIX; /* Server domain */
  strcpy (serverUNIXAddress.sun_path, "register_socket");/*Server name*/
  do { 
  result = connect (clientFd, serverSockAddrPtr, serverLen);
  if (result == -1) sleep (1); /* Wait and then try again */
  } while (result == -1);
  char socketOutput[10];
  read(clientFd, socketOutput, 8);
  printf("%s\n", socketOutput);
  close (clientFd); /* Close the socket */
  exit (/* EXIT_SUCCESS */ 0); /* Done */
  
  // Train train;
  // strcpy(train.name, argv[1]);

  // bool arrived = false;
  
  // getMap(train.name, train.route);

  // while(!arrived){
    

  // } 
}


void getMap(char* name, MASegment* route) {
        
}
