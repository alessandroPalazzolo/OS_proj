#include <fcntl.h>

#include "socket-utils.h"
#include "globals.h"

void runSocketHandler(int);
int loadMapFromFile(char* chosenMap, Map*);

int main(int argc, char* argv[]) {
  SocketDetails sock;
  int mapFd, mapIsLoaded;
  Map map;

  mapIsLoaded = loadMapFromFile(argv[1], &map);
  if(!mapIsLoaded){
    perror("error loading map");
    exit(EXIT_FAILURE);
  }

  // initSocket(&sock, "register_socket");
  // buildSocket(&sock, 5);
  // runSocket(&sock, &runSocketHandler);
}

void runSocketHandler(int clientFd) {
  char buffer[5];
  read(clientFd, buffer, 2);
  // get data from stored map
  write(clientFd, "robe", 4);
}

int loadMapFromFile(char* chosenMap, Map* map) {
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
        strcpy((*map)[z][j], buffer);
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