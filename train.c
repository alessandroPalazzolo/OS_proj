#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <error.h>
#include <stdbool.h>

#include "globals.h"
#include "socket-utils.h"

#define NEXT_SEG_FREE 0
#define NEXT_SEG_OCCUPIED 1
#define NEXT_SEG_STATION 3
struct TrainDetails {
  char name[4];
  char mode[10];
  Route route;
} train;

void runSocketHandler(int);
void getRoute();
int checkNextMASegmentECTS1(Route*, int);
int checkNextMASegmentECTS2(Route*, int);
void writeToMASegment(MASegment*, char);
void startTrain(Route*, int (*checkNextMASegment)(Route*, int));

int main(int argc, char* argv[]){
  strcpy(train.name, argv[1]);
  getRoute();
}

void startTrain(Route* route, int (*checkNextMASegment)(Route*, int)){
  bool arrived = false;
  int currentPosition = 0;
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

void runSocketHandler(int clientFd){
  char buffer[5], currentChar;
  int hasRead, i = 0, j = 0;
  write(clientFd, train.name, 4);

  do {
    hasRead = read(clientFd, &currentChar, 1);

    buffer[i] = currentChar;

    if (currentChar == '\0') {
      strcpy(train.route[j], buffer);
      j++;
      i = 0;
      memset(buffer, '\0', 4);
    } else {
      i++;
    }
  } while(hasRead);
}

void getRoute() {
  SocketDetails sock;
  sock.type = CLIENT;
  initSocket(&sock, "register_socket");
  runSocket(&sock, &runSocketHandler);
}

/*
 // passare il MAFileFd ed utilizzarlo per le funzioni successive (?)
 // eliminare current Position e iterare direttamente sul puntatore a route (?)
 */
int checkNextMASegmentECTS1(Route* route, int currentPosition) {
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
