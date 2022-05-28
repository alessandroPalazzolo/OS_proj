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
#define NEXT_SEG_STATION 2


void getRoute(char*, Route*);
int checkNextMASegmentECTS1(MASegment);
int checkNextMASegmentECTS2(MASegment);
void enterMASegment(MASegment, int*);
void exitMASegment(MASegment, int*);
void fillTrain(Train*,char**);
void runTrain(Train*);
void logTrainStatus(int, MASegment, MASegment);

void fillTrain(Train* train, char* argv[]) {
  char mode [6];
  strcpy(train->name, "T1"/* argv[1]*/);
  strcpy(mode, "ECTS1" /*argv[2]*/);
  Route testRoute;
  strcpy(testRoute[0], "S0");
  strcpy(testRoute[1], "MA1");
  strcpy(testRoute[2], "MA2");
  strcpy(testRoute[3], "S4");
  //train->route = testRoute;
  //  getRoute(name, train->route);
  memmove(&train->route, testRoute, sizeof(testRoute));
  if (!strcmp(mode, "ECTS1")){
    train->checkNextMAFuncPtr = (*checkNextMASegmentECTS1);
  } else if (!strcmp(mode, "ECTS2")){
    train->checkNextMAFuncPtr = (*checkNextMASegmentECTS2);
  }

  char logFilePath[20];
  sprintf(logFilePath, "log/%s.log", train->name);
  int fd = open(logFilePath, O_WRONLY|O_TRUNC|O_CREAT, 0666);

  if (fd < 0) {
    perror("runTrain");
  }

  train->logFileFd = fd;
}

void runTrain(Train* train) { 
  int MAFileFd;
  MASegment* currentMA = train->route; 
  MASegment* nextMA = currentMA + 1;  
  bool arrived = false;

  while(!arrived) {

    logTrainStatus(train->logFileFd, *currentMA, *nextMA);
    
    switch (train->checkNextMAFuncPtr(*nextMA)) {
      case NEXT_SEG_FREE:
        exitMASegment(*currentMA, &MAFileFd);
        currentMA++;
        nextMA++;
        enterMASegment(*currentMA, &MAFileFd);
        break;
      case NEXT_SEG_OCCUPIED:
        break;
      case NEXT_SEG_STATION:
        exitMASegment(*currentMA, &MAFileFd);
        arrived = true;
        break;
      default:
        exit(EXIT_FAILURE);
        break;
    }
    sleep(2);
  }
  close(train->logFileFd);
  exit(EXIT_SUCCESS); //return 0
}

void getRoute(char* name, Route* route) {
  // retrieve route from socket
}

int checkNextMASegmentECTS1(MASegment nextMA) {
  char MAStatus;
  char MASegmentPath[20];

  if (nextMA[0] == 'S')
    return NEXT_SEG_STATION;

  sprintf(MASegmentPath, "assets/%s", nextMA);

  int MAFileFd = open(MASegmentPath, O_RDONLY);
  if (MAFileFd < 0){
    perror(MASegmentPath);
    close(MAFileFd);
    return -1;
  }
  if (read(MAFileFd, &MAStatus, 1) < 0 ) {
    perror("checkNextMASegment");
    close(MAFileFd);
    return -1;
  }
    close(MAFileFd);

  if (MAStatus == '0')
    return NEXT_SEG_FREE;

  if (MAStatus == '1') 
    return NEXT_SEG_OCCUPIED;
  
  return -1;
}

int checkNextMASegmentECTS2(MASegment nextMA) {
  int resECTS2 = 0; // ask RBC for permission
                    
  int resECTS1 = checkNextMASegmentECTS1(nextMA);
  if (resECTS2 == resECTS1){
    return resECTS2;
  } else {
    return NEXT_SEG_OCCUPIED;
  }
}

void logTrainStatus(int fd, MASegment currentMA, MASegment nextMA) {
  char logString[100]; 
  char timeString[40];
  time_t timeSec = time(NULL);
  struct tm* timeInfo = localtime(&timeSec);
  strftime(timeString, sizeof(timeString), "%d %B %Y %X", timeInfo);
  sprintf(logString, "[actual:%s] [next:%s] %s\n", currentMA, nextMA, timeString);
  if (write(fd, logString, strlen(logString)) != strlen(logString)) {
    perror("logTrainStatus");
    //some error
  }
}

void enterMASegment(MASegment segment, int* MAFileFd) {
  char pathMASegment[20];
  sprintf(pathMASegment, "assets/%s", segment);
  fprintf(stderr, "entering: %s\n", pathMASegment);
  *MAFileFd = open(pathMASegment, O_WRONLY);
  if (MAFileFd < 0) {
    perror("enterMASegment");
  }
  if (write(*MAFileFd, "1", 1) != 1){
    perror("enterMASegment");
  }
}

void exitMASegment(MASegment segment, int* MAFileFd) {
  if (segment[0] != 'S') {  
    if (write(*MAFileFd, "0", 1) != 1){
      perror("exitMASegment");
    }
    
    close(*MAFileFd);
  }
}
