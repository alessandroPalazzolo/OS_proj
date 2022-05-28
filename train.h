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
int checkNextMASegmentECTS1(Route*);
int checkNextMASegmentECTS2(Route*);
void enterMASegment(MASegment, int*);
void exitMASegment(MASegment, int*);
void fillTrain(Train*,char**);
void runTrain(Train*);
void logTrainStatus(int, Route*);

void fillTrain(Train* train, char* argv[]) {
  char name [4];
  char mode [6];
  strcpy(name, "T1"/* argv[1]*/);
  strcpy(mode, "ECTS1" /*argv[2]*/);
  Route testRoute;
  strcpy(testRoute[0], "S0");
  strcpy(testRoute[1], "MA1");
  strcpy(testRoute[2], "MA2");
  strcpy(testRoute[3], "S4");
  //train->route = testRoute;
  //  getRoute(name, train->route);
  train->route = &testRoute;
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
  Route* currentMA = train->route;
  bool arrived = false;

  while(!arrived){
    logTrainStatus(train->logFileFd, currentMA);
    sleep(2);
    switch (train->checkNextMAFuncPtr(currentMA+1)) {
      case NEXT_SEG_FREE:
        enterMASegment(**currentMA, &MAFileFd);
        currentMA = currentMA + 1;
        exitMASegment(**currentMA, &MAFileFd);
        break;
      case NEXT_SEG_OCCUPIED:
        break;
      case NEXT_SEG_STATION:
        exitMASegment(**currentMA, &MAFileFd);
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
int checkNextMASegmentECTS1(Route* nextMA) {
  MASegment segment;
  char MAStatus;
  char MASegmentPath[10];
  strcpy(segment, **nextMA);  // <- non troppo elegante?
  
  if (segment[0] = 'S')
    return NEXT_SEG_STATION;

  sprintf(MASegmentPath, "asset/%s", segment);
  int MAFileFd = open(MASegmentPath, O_RDONLY);
  if (MAFileFd < 0){
    perror("checkNextMASegment");
    return -1;
  }
  if (read(MAFileFd, &MAStatus, 1) != 1 ) {
    perror("checkNextMASegment");
    return -1;
  }
  if (MAStatus == '0')
    return NEXT_SEG_FREE;

  if (MAStatus == '1') 
    return NEXT_SEG_OCCUPIED;
  
  close(MAFileFd);
  return -1;
}

int checkNextMASegmentECTS2(Route* nextMA) {
  int resECTS2 = 0; // ask RBC for permission
                    
  int resECTS1 = checkNextMASegmentECTS1(nextMA);
  if (resECTS2 == resECTS1){
    return resECTS2;
  } else {
    return NEXT_SEG_OCCUPIED;
  }
}

void logTrainStatus(int fd, Route* currentMA){
  char logString[100]; 
  char timeString[40];
  MASegment* actualSegment = *currentMA;
  MASegment* nextSegment = *currentMA + 1;
  time_t timeSec = time(NULL);
  struct tm* timeInfo = localtime(&timeSec);
  strftime(timeString, sizeof(timeString), "%d %B %Y %X", timeInfo);
  sprintf(logString, "[actual:%s] [next:%s] %s\n", actualSegment, nextSegment, timeString);
  if (write(fd, logString, strlen(logString) != strlen(logString))) {
    perror("logTrainStatus");
    //some error
  }
}

void enterMASegment(MASegment segment, int* MAFileFd) {
  char pathMASegment[12];
  sprintf(pathMASegment, "assets/%s", *segment);
  printf("%s\n", pathMASegment);
  *MAFileFd = open(pathMASegment, O_WRONLY);
  if (MAFileFd < 0) {
    perror("writeToMASegment");
  }
}

void exitMASegment(MASegment segment, int* MAFileFd) {
  if (segment[0] != 'S') {  
    if (write(*MAFileFd, "1", 1) != 1){
      perror("exitToMASegment");
    }
    
    close(*MAFileFd);
  }
}
