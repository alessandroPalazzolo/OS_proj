#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <error.h>
#include <stdbool.h>
#include <time.h>
#include <semaphore.h>

#include "socket-utils.h"
#include "globals.h"
#include "train.h"

int main(int argc, char* argv[]){
  Train train;
  fillTrain(&train, argv);
  getRoute(&train);
  runTrain(&train);
}

void fillTrain(Train* train, char* argv[]) {
  char mode [6];
  strcpy(train->name,  argv[1]);
  strcpy(mode, argv[2]);
  if (!strcmp(mode, "ETCS1")) {
    train->checkNextMAFuncPtr = (*checkNextMASegmentETCS1);
  } else if (!strcmp(mode, "ETCS2")) {
    train->checkNextMAFuncPtr = (*checkNextMASegmentETCS2);
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
  int MAFd, MAFdNext;
  MASegment* currentMA = train->route; 
  MASegment* nextMA = currentMA + 1;  
  bool arrived = false;

  while(!arrived) {

    logTrainStatus(train->logFileFd, *currentMA, *nextMA);
    
    switch (train->checkNextMAFuncPtr(*nextMA)) {
      case NEXT_SEG_FREE:
        fprintf(stderr, "%s entering: %s\n", train->name, nextMA);// debug purpose
        enterMASegment(*nextMA, &MAFdNext);
        fprintf(stderr, "%s exiting: %s\n", train->name, currentMA); // debug purpose
        exitMASegment(*currentMA, &MAFd);
        currentMA++;
        nextMA++;
        MAFd = MAFdNext;
        break;
      case NEXT_SEG_OCCUPIED:
        break;
      case NEXT_SEG_STATION:
        fprintf(stderr, "%s exiting: %s\n", train->name, currentMA); // debug purpose
        exitMASegment(*currentMA, &MAFd);
        fprintf(stderr, "%s arrived: %s\n", train->name, nextMA); // debug purpose
        arrived = true;
        break;
      default:
        printf("%s failure", train->name); //debug purpose
        exit(EXIT_FAILURE);
        break;
    }
    sleep(2);
  }
  close(train->logFileFd);
  exit(EXIT_SUCCESS); //return 0
}


int checkNextMASegmentETCS1(MASegment nextMA) {
  char MAStatus;
  char MASegmentPath[20];

  if (nextMA[0] == 'S')
    return NEXT_SEG_STATION;

  sprintf(MASegmentPath, "./assets/%s", nextMA);
  
  sem_t* MASem = sem_open(nextMA, O_CREAT, 0666, 1);
  sem_wait(MASem);

  int MAFileFd = open(MASegmentPath, O_RDONLY);
  if (MAFileFd < 0){

    perror("checkNextMASegmentETCS1");
    sem_post(MASem);
    sem_close(MASem);
    close(MAFileFd);
    return -1;
  }
  if (read(MAFileFd, &MAStatus, 1) < 0 ) {

    perror("checkNextMASegmentETCS1");
    sem_post(MASem);
    sem_close(MASem);
    close(MAFileFd);
    return -1;
  }
    close(MAFileFd);

  sem_post(MASem);
  sem_close(MASem);
  return atoi(&MAStatus);

}

int checkNextMASegmentETCS2(MASegment nextMA) {
  int resETCS2 = 0; // ask RBC for permission
                    
  int resETCS1 = checkNextMASegmentETCS1(nextMA);
  if (resETCS2 == resETCS1){
    return resETCS2;
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
  char MASegmentPath[20];
  sprintf(MASegmentPath, "./assets/%s", segment);
  *MAFileFd = open(MASegmentPath, O_WRONLY);
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

void runSocketHandler(int clientFd, void* payload) {
  Train* train = (Train*) payload; 
  char buffer[5], currentChar;
  int hasRead, i = 0, j = 0;
  write(clientFd, train->name, 4);
  
  do {
    hasRead = read(clientFd, &currentChar, 1);
    buffer[i] = currentChar;

    if (currentChar == '\0') {
      strcpy(train->route[j], buffer);
      j++;
      i = 0;
      memset(buffer, '\0', 4);
    } else {
      i++;
    }
  } while(hasRead);
  
}

void getRoute(Train* train) {
  SocketDetails sock;
  sock.type = CLIENT;
  sock.payload = train;

  initSocket(&sock, "register_socket");
  runSocket(&sock, &runSocketHandler);
}