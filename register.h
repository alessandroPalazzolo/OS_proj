#include <fcntl.h>

#include "socket-utils.h"
#include "globals.h"

Map map;

void runSocketHandler(int);
int loadMapFromFile(char* chosenMap, Map*);

void runSocketHandler(int clientFd) {
  char buffer[5];
  read(clientFd, buffer, 4);
  printf("train requesting: %s\n", buffer);

  for (int i = 0; i < 10; i++){
    write(clientFd, map[0][i], strlen(map[0][i]) + 1); // replace 0 with train index
    printf("wrote: %s\n", map[0][i]); 
    if (map[0][i][0] == 'S' && i) {
      break;
    }
  }
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