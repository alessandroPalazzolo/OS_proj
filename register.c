#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h> 
#include "globals.h"


void readTrainName(int, char*);
void writeTrainRoute(int, MASegment*);
void loadMapFromFile(int, Map*);

int main(int argc, char* argv[]) {
  
  Map* map;
  char* mapName = argv[1];
  char mapPath[15];
  sprintf(mapPath, "maps/%s", mapName);
  int mapFd = open(mapPath, O_RDONLY);
  if (mapFd < 0) {
    perror("main :");
  }

  loadMapFromFile(mapFd, map);

  signal(SIGCHLD, SIG_IGN); // ignore SGNCHLD per far morire i figli (?)
  int serverFd, clientFd;
  socklen_t serverLen, clientLen;
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
    char trainName[3];
    MASegment route[6];
    clientFd = accept (serverFd, clientSockAddrPtr, &clientLen);
    readTrainName(clientFd, trainName);    
    retrieveTrainRoute(trainName, route); 
    writeTrainRoute(clientFd, route);
  }
}

void readTrainName(int fd, char* trainName) { // change to int (?)
  if (read(fd, trainName, 3) != 3) {
    perror("readTrainName: ");
  }
}

void loadMapFromFile(int fd, Map* map) {
  lseek(fd, 0, SEEK_SET);
  for (int i = 0; i < 5; i++) {
    int segmentIndex = 0;
    bool lastSegment = false;
    char c = map[i][0][0];
    while (!lastSegment) {
      char tmp;
      if (read(fd, tmp, 1) < 0){ //EOF
        lastSegment = true;
        i = 5;
      }
      if (tmp = ' ') {
        segmentIndex++;
        c = map[i][segmentIndex];
      } else if (!strcmp(tmp, "\n")){
        lastSegment = true;
      } else {
      c++ = tmp;
      }
    } 
  }
}

/*
    while (read(fd, tmp, 1) > 0) {
      char* tmp;
      if (strcmp(tmp, " ")) {
        char* MAs = map[i][segmentIndex];
        strcpy(MAs, tmp);
      } 
      segmentIndex++;
    }
  }
}

*/

void writeTrainRoute(int fd, MASegment* route) {
  for (int i = 0 ; i < 6 ; i++) {
    if (write(fd, route[i], strlen(route[i]) + 1)) {
      perror("writeTrainRoute: ");  
    }
  }
}
