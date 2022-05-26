#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <error.h>
#include <stdbool.h>

#include "globals.h"

void getRoute(char*, Route*);
int checkNextMASegment(Route*, int, int*);
void moveToNextMASegment(int*);
  int main(int argc, char* argv[]){

  char name [3];
  Route* route = NULL;
  int* currentPosition = 0;
  int* fd;
  bool arrived = false;
  strcpy(name, argv[1]);
  getRoute(name, route);
  
    while(!arrived){
      switch (checkNextMASegment(route, currentPosition, fd)) {
        case 0:
          moveToNextMaSegment(fd);
          break;
        case 1:
          break;
        case 2:
          moveToStation();
          arrived = true;
          break;
        default:
          // something
          break;
      }
      sleep(2);
    }
}


void getRoute(char* trainName, Route* route) {
  // retrieve route from socket
}

int checkNextMASegment(Route* route, int currentPosition, int* fd) {
  char* MASegment = NULL;
  char* MAStatus = NULL;
  char MASegmentPath[10];

  strcpy(MASegment, route[currentPosition]);
  if (strcmp(&MASegment[1], "S"))
    return 2;

  sprintf(MASegmentPath, "asset/%s", MASegment);
  fd = open(MASegmentPath, O_RDONLY);
  if (fd < 0){
    perror("checkNextMASegment");
    exit(EXIT_FAILURE);
  }
  if (read(fd, MAStatus, 1) != 1 ){
    perror("checkNextMASegment");
    exit(EXIT_FAILURE);
  }
  if (!strcmp(MAStatus, "0")) {
    return 0;
  }
  if (!strcmp(MAStatus, "1")){
    close(fd);
    return 1;
  }
  return -1;
}
