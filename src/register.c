#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <stdbool.h>

#include "globals.h"
#include "socket-utils.h"
#include "register.h"

int main(int argc, char* argv[]) {
  Map map;
  SocketDetails sock;
  char mapName[10];
  int mapFd, mapIsLoaded;

  strcpy(mapName, argv[1]);
  sock.type = SERVER;
  sock.payload = &map;

  mapIsLoaded = loadMapFromFile(mapName, &map); 
  if(!mapIsLoaded){
    perror("Register error loading map");
    exit(EXIT_FAILURE);
  }

  initSocket(&sock, "register_socket");
  buildSocket(&sock, 5);
  runSocket(&sock, &runSocketHandler);
}

void sendRoute(int trainId, int clientFd, Map* map){
  int writeResult;
  char routeSegment[5]; 
  bool isLastSegment = false;

  for (int i = 0; i < MAX_ROUTE_SEGMENTS; i++){
    strcpy(routeSegment, (*map)[trainId][i]);
    writeResult = write(clientFd, routeSegment, strlen(routeSegment) + 1);

    if (writeResult == -1) {
      perror("runSocketHandler");
      exit(EXIT_FAILURE);
    }

    isLastSegment = routeSegment[0] == 'S' && i;
    if (isLastSegment) break;
  }
}

void runSocketHandler(int clientFd, void* payload) {
  Map* map = (Map*) payload;
  char requestSender[5];
  int readResult;

  readResult = read(clientFd, requestSender, 5);

  if (readResult == -1){
    perror("runSocketHandler");
    exit(EXIT_FAILURE);
  }

  if (!strcmp(requestSender, "rbc")){
    for (int i = 0; i<TRAINS_COUNT; i++)
      sendRoute(i, clientFd, map);
  } else {
    int trainIndex = atoi(requestSender + 1) - 1;
    sendRoute(trainIndex, clientFd, map);
  }
}

int loadMapFromFile(char* chosenMap, Map* destMap) {
  int mapFd, readResult;
  char path[20], buffer[20], currentChar;
  sprintf(path, "./assets/maps/%s", chosenMap);

  mapFd = open(path, O_RDONLY);
  if (mapFd < 0){
    return 0;
  }

  int i, j, z;
  bool isEOF = false;

  for (z = 0; z < TRAINS_COUNT; z++){
    i = 0;
    j = 0;

    do {
      readResult = read(mapFd, &currentChar, 1);

      if (readResult == -1){
        return 0; 
      }

      if (!readResult){
        isEOF = true;
      }
      
      buffer[i] = currentChar;

      if (currentChar == 32 || currentChar == '\n') {
        buffer[i] = '\0';
        strcpy((*destMap)[z][j], buffer);
        memset(buffer, '\0', 20);
        j++;
        i = 0;
      } else {
        i++;
      }
    } while (currentChar != '\n');

    if (isEOF) break;
  }
  
  return 1;
}