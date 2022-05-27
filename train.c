#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <error.h>
#include <stdbool.h>
#include <time.h>

#include "globals.h"

#define NEXT_SEG_FREE 0
#define NEXT_SEG_OCCUPIED 1
#define NEXT_SEG_STATION 3


void getRoute(char*, Route*);
int checkNextMASegmentECTS1(Route*, int);
int checkNextMASegmentECTS2(Route*, int);
void writeToMASegment(MASegment*, char);
void startTrain(char*, Route*, int (*checkNextMASegment)(Route*, int));
void logTrainStatus(int, Route*, int);

int main(int argc, char* argv[]){
  char name [3];
  char mode [6];
  Route routeTest = {"S0","MA1","MA3","S5"} /*= NULL*/ ;
  Route route = routeTest;
  strcpy(name, argv[1]);
  strcpy(mode, argv[2]);
//  getRoute(name, route);
  if (!strcmp(mode, "ECTS1")){
    startTrain(name, route, checkNextMASegmentECTS1);
  } else if (!strcmp(mode, "ECTS2")){
    startTrain(name, route, checkNextMASegmentECTS2);
  }
}

void startTrain(char* name, Route* route, int (*checkNextMASegment)(Route*, int)){ 
  bool arrived = false;
  int currentPosition = 0;
  char logFilePath[20];
  sprintf(logFilePath, "log/%s.log", name);
  int logFileFd = open(logFilePath, O_WRONLY|O_TRUNC|O_CREAT, 0666);
  while(!arrived){
    logTrainStatus(logFileFd, route, currentPosition);
    sleep(2);
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
  }
  exit(EXIT_SUCCESS); //return 0
}

void getRoute(char* name, Route* route) {
  // retrieve route from socket
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
  if (MAStatus == '0') {
    return NEXT_SEG_FREE;
  }
  if (MAStatus == '1') {
    close(MAFileFd);
    return NEXT_SEG_OCCUPIED;
  }
  return -1;
}

int checkNextMASegmentECTS2(Route* route, int currentPosition) {
  int resECTS2 = 0; // ask RBC for permission
                    //
  int resECTS1 = checkNextMASegmentECTS1(route, currentPosition);
  if (resECTS2 == resECTS1){
    return resECTS2;
  } else {
    return NEXT_SEG_OCCUPIED;
  }
}

void logTrainStatus(int fd, Route* route, int currentPosition){
  char logString[100]; 
  char timeString[40];
  MASegment* actualSegment = route[currentPosition];
  MASegment* nextSegment = route[currentPosition+1];
  time_t timeSec = time(NULL);
  struct tm* timeInfo = localtime(&timeSec);
  strftime(timeString, sizeof(timeString), "%d %B %Y %X", timeInfo);
  sprintf(logString, "[actual:%s] [next:%s] %s\n", actualSegment, nextSegment, timeString);
  if (write(fd, logString, strlen(logString) != strlen(logString))) {
    perror("logTrainStatus");
    //some error
  }
}

void writeToMASegment(MASegment* segment, char status) {
  char pathMASegment[12];
  sprintf(pathMASegment, "assets/%s", *segment);
  int MAFileFd = open(pathMASegment, O_WRONLY);
  if (MAFileFd < 0) {
    perror("writeToMASegment");
  }
  if (write(MAFileFd, &status, 1)){
    perror("writeToMASegment");
  }
  close(MAFileFd);
}

