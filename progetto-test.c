#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <error.h>


void createMASegment();
void parseArg(char**);
void parseMode(char*);
void parseMap(char*);
void spawnTrain(int**);
void spawnRegister(int**);
void usage();


typedef enum {ECTS1, ECTS2} mode;
typedef enum {MAPPA1, MAPPA2} map;
mode MODE;
map MAP;

int main(int argc, char* argv[]) {
  int* trainPid[5];
  parseMode(argv[1]);
  if (MODE == ECTS2 && strcmp(argv[2], "RBC")) {
    int status = 0;
    if (fork() == 0) {
    //exec RBC  
    }    
  //wait for RBC termination and then exit
  }
  parseMap(argv[2]);
  spawnTrain(trainPid);
  spawnRegister()
}

void parseMode(char* mode) {
  if (strcmp(mode, "ECTS1")){
    MODE = ECTS1;
  } else if (strcmp(mode, "ECTS2")){
    MODE = ECTS2;
  } else {
    //usage();
  }
}

void parseMap(char* map) {
  if (strcmp(map, "MAPPA1")) {
    MAP = MAPPA1;
  } else if (strcmp(map, "MAPPA2")){
    MAP = MAPPA2;  
  } else {
    //usage();
  }
}

void spawnRegister() {
  if (fork()==0) {
    execlp("Register", "Register");
    perror(main); // should never been executed
  }
}

void spawnTrain() {
  for (int i = 0 ; i < 5 ; i++) {
    if(fork()==0) {
      char trainName[5];
      sprintf(trainName, "T%i", i);
      switch(MODE) { //same code
        case ECTS1 : 
          execlp("TrainECTS1", "TrainECTS1", trainName);
          perror(main); // should never been executed
          break;
        case ECTS2 :
          execlp("TrainECTS2", "TrainECTS2", trainName);
          perror(main);
      }
    } 
  }
}

void createMaSegment(){
  char path[20];
  for (int i = 0; i < 16; i++) {
    sprintf(path, "asset/MA%d", i);
    int fd = open(path, O_CREAT|O_WRONLY|O_TRUNC, 0666);
    if (fd == -1){
      // gestire l'errore
    }
    if (write(fd, "0", 1) != 1){
      // gestire l'errore
    } 
    close(fd); 
  }

}
