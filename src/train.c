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
#include <sys/stat.h>


#include "socket-utils.h"
#include "globals.h"
#include "train.h"

int main(int argc, char* argv[]){
  Train train;
  fillTrainData(&train, argv);
  runTrain(&train);
}

void fillTrainData(Train* train, char* argv[]) {
  char mode[10];

  strcpy(train->name,  argv[1]);
  strcpy(mode, argv[2]);

  if (!strcmp(mode, "ETCS1")) {
    train->checkNextMAFuncPtr = &checkNextMASegmentETCS1;
  } else if (!strcmp(mode, "ETCS2")) {
    train->checkNextMAFuncPtr = &checkNextMASegmentETCS2;
  }

  getRoute(train); 

  char logFilePath[20];
  sprintf(logFilePath, "log/%s.log", train->name);
  umask(0);
  int fd = open(logFilePath, O_WRONLY | O_TRUNC | O_CREAT, 0666);

  if (fd < 0) {
    perror("runTrain");
    exit(EXIT_FAILURE);
  }

  train->logFileFd = fd;
}

void runTrain(Train* train) { 
  int MAfd, MAfdNext;
  MASegment* currentMApt = train->route;
  MASegment* nextMApt = train->route + 1;  
  bool arrived = false; 

  while(!arrived) {
    train->nextMApt = nextMApt;
    sem_t* MASem = sem_open(*nextMApt, O_CREAT, 0666, 1);
    sem_wait(MASem);

    logTrainStatus(train->logFileFd, *currentMApt, *nextMApt);

    switch (train->checkNextMAFuncPtr(train)) {
      case NEXT_SEG_FREE:
        fprintf(stderr, "%s entering: %s\n", train->name, *nextMApt);// debug 
        enterMASegment(*nextMApt, &MAfdNext);
        fprintf(stderr, "%s exiting: %s\n", train->name, *currentMApt); // debug 
        exitMASegment(*currentMApt, &MAfd);
        sem_post(MASem);
        currentMApt = nextMApt;
        nextMApt++;
        MAfd = MAfdNext;
        break;
      case NEXT_SEG_OCCUPIED:
        sem_post(MASem);
        break;
      case NEXT_SEG_STATION:
        fprintf(stderr, "%s exiting: %s\n", train->name, *currentMApt); // debug 
        exitMASegment(*currentMApt, &MAfd);
        sem_post(MASem);
        fprintf(stderr, "%s arrived: %s\n", train->name, *nextMApt); // debug 
        arrived = true;
        break;
      default:
        sem_post(MASem);
        sem_close(MASem);
        printf("%s failure", train->name); //debug 
        exit(EXIT_FAILURE);
        break;
    }
    
    sem_close(MASem);
    sleep(2);
  }

  close(train->logFileFd);
  exit(EXIT_SUCCESS); //return 0
}

int checkNextMASegmentETCS1(Train* train) {
  char MAStatus;
  char MASegmentPath[20];
  MASegment nextMA;

  strncpy(nextMA, *(train->nextMApt), sizeof(MASegment));

  if (nextMA[0] == 'S')
    return NEXT_SEG_STATION;

  sprintf(MASegmentPath, "./assets/%s", nextMA);

  int MAFileFd = open(MASegmentPath, O_RDONLY);

  if (MAFileFd < 0){
    perror("checkNextMASegmentETCS1");
    close(MAFileFd);
    return -1;
  }

  if (read(MAFileFd, &MAStatus, 1) < 0 ) {
    perror("checkNextMASegmentETCS1");
    close(MAFileFd);
    return -1;
  }
  
  close(MAFileFd);

  return atoi(&MAStatus);
}

void runSocketHandlerRbc(int clientFd, void* payload){
  Train* train = (Train*) payload;
  MASegment nextSegment, currentSegment;
  ssize_t writeTrain, writeCurrentSegment, writeNextSegment;
  char nextSegmentStatus[5];

  strncpy(nextSegment, *(train->nextMApt), sizeof(*(train->nextMApt)));
  strncpy(currentSegment, *(train->nextMApt - 1), sizeof(*(train->nextMApt - 1)));


  writeTrain = write(clientFd, train->name, sizeof(train->name));
  writeCurrentSegment = write(clientFd, currentSegment, sizeof(currentSegment));
  writeNextSegment = write(clientFd, nextSegment, sizeof(nextSegment));

  if (writeTrain <= 0 || writeCurrentSegment <= 0 || writeNextSegment <= 0)
    perror("runSocketHandlerRbc");

  if (read(clientFd, nextSegmentStatus, 1) != 1){
    perror("runSocketHandlerRbc");
  }

  train->RBCresponse = atoi(nextSegmentStatus);
}

int checkNextMASegmentETCS2(Train* train) {    
  SocketDetails sock;
  sock.type = CLIENT;
  sock.payload = train;
  int nextSegmentFileStatus;

  initSocket(&sock, "RBC_socket");
  runSocket(&sock, &runSocketHandlerRbc);

  nextSegmentFileStatus = checkNextMASegmentETCS1(train);

  if (nextSegmentFileStatus == train->RBCresponse) {
    return train->RBCresponse;
  } else {
    return NEXT_SEG_OCCUPIED;
  }
}

void logTrainStatus(int fd, MASegment currentMA, MASegment nextMA) {
  char logString[100], timeString[40];
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
  *MAFileFd = open(MASegmentPath, O_WRONLY | O_TRUNC);

  if (*MAFileFd < 0) {
    perror("enterMASegment");
  }

  if (write(*MAFileFd, "1", 1) != 1){
    perror("enterMASegment");
  }
}

void exitMASegment(MASegment segment, int* MAFileFd) {
  if (segment[0] != 'S') {  
    lseek(*MAFileFd, 0, SEEK_SET);

    if (write(*MAFileFd, "0", 1) != 1){
      perror("exitMASegment");
    }
    
    close(*MAFileFd);
  }
}

void runSocketHandlerRegister(int clientFd, void* payload) {
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
  runSocket(&sock, &runSocketHandlerRegister);
}