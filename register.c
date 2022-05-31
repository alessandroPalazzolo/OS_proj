#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>

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
    perror("error loading map");
    exit(EXIT_FAILURE);
  }

  initSocket(&sock, "register_socket");
  buildSocket(&sock, 5);
  runSocket(&sock, &runSocketHandler);
}

void runSocketHandler(int clientFd, void* payload) {
  Map* map = (Map*) payload;
  char trainName[5];
  int readResult, writeResult, trainIndex;

  readResult = read(clientFd, trainName, 5);

  if (readResult == -1){
    perror("runSocketHandler");
    exit(EXIT_FAILURE);
  }

  trainIndex = atoi(trainName + 1) - 1;

  for (int i = 0; i < MAX_ROUTE_SEGMENTS; i++){
    writeResult = write(clientFd, (*map)[trainIndex][i], strlen((*map)[trainIndex][i]) + 1);

    if (writeResult == -1) {
      perror("runSocketHandler");
      exit(EXIT_FAILURE);
    }

    if ((*map)[trainIndex][i][0] == 'S' && i) break;
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