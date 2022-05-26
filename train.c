#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <error.h>
#include <stdbool.h>

#include "globals.h"

#define NEXT_SEG_FREE 0
#define NEXT_SEG_OCCUPIED 1
#define NEXT_SEG_STATION 3


void getRoute(char*, Route*);
int checkNextMASegment(Route*, int);
void writeToMASegment(MASegment*, char);

int main(int argc, char* argv[]){
  char name [3];
  Route* route = NULL;
  int currentPosition = 0;
  bool arrived = false;
  strcpy(name, argv[1]);
  getRoute(name, route);
  
    while(!arrived){
      switch (checkNextMASegment(route, currentPosition)) {
        case NEXT_SEG_FREE:
          writeToMASegment(route[currentPosition], '0');
          currentPosition++;
          writeToMASegment(route[currentPosition], '1');
          break;
        case NEXT_SEG_OCCUPIED:
          break;
        case NEXT_SEG_STATION:
          writeToMASegment(route[currentPosition], '0');
          arrived = true;
          break;
        default:
          // some error
          break;
      }
      sleep(2);
    }
}


void getRoute(char* name, Route* route) {
  // retrieve route from socket
}

int checkNextMASegment(Route* route, int currentPosition) { // passare il MAFileFd ed utilizzarlo per le funzioni successive (?)
  MASegment segment;
  char MAStatus;
  char MASegmentPath[10];

  strcpy(segment, &route[currentPosition]);
  if (strcmp(&segment[0], "S"))
    return NEXT_SEG_STATION;

  sprintf(MASegmentPath, "asset/%s", segment);
  int MAFileFd = open(MASegmentPath, O_RDONLY);
  if (MAFileFd < 0){
    perror("checkNextMASegment");
    exit(EXIT_FAILURE);
  }
  if (read(MAFileFd, &MAStatus, 1) != 1 ){
    perror("checkNextMASegment");
    exit(EXIT_FAILURE);
  }
  if (!strcmp(&MAStatus, "0")) {
    return NEXT_SEG_FREE;
  }
  if (!strcmp(&MAStatus, "1")){
    close(MAFileFd);
    return NEXT_SEG_OCCUPIED;
  }
  return -1;
}

void writeToMASegment(MASegment* segment, char status) {
  char pathMASegment[12];
  sprintf(pathMASegment, "asset/%s", segment);
  int MAFileFd = open(pathMASegment, O_WRONLY);
  if (MAFileFd < 0) {
    perror("releaseMASegment");
  }
  if (write(MAFileFd, &status, 1)){
    perror("releaseMASegment");
  }
  close(MAFileFd);
}

