#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <error.h>

#include "globals.h"

enum modes {ECTS1, ECTS2} MODE;
enum maps {MAPPA1, MAPPA2} MAP;

int parseArgs(char**);
int initMASegments();
void parseMap(char*);
int initTrains(pid_t*);
void spawnRegister();
void usage();

void parseArgs(char** args) { 
    char* mode = NULL;
    char* variant = NULL;
    strcpy(mode, args[1]);
    strcpy(variant, args[2]);

    if (!strcmp(mode, "ETCS1")){
        MODE = ECTS1;
    } else if (!strcmp(mode, "ETCS2")){
        MODE = ECTS2;

        if (!strcmp(variant, "RBC")){
            parseMap(args[3]); // rename
            if (fork() == 0) {
                // exec RBC  
            }    
        }

    } else {
        // usage()
    }

    parseMap(variant);
}

void parseMap(char* map) {
  if (!strcmp(map, "MAPPA1")) {
    MAP = MAPPA1;
  } else if (!strcmp(map, "MAPPA2")){
    MAP = MAPPA2;  
  } else {
    //usage();
  }
}

int initMASegments(){
    char paths[SEGMENTS_COUNT][MA_FILE_PATH];

    for (int i = 0; i < SEGMENTS_COUNT; i++){
        sprintf(paths[i], "./assets/MA%d", i + 1);
        int fd = open(paths[i], O_CREAT | O_RDWR, 0666);
        if (fd < 0) {
            perror("initMASegments error: ");
            return 0;
        }

        if (write(fd, "0", 1) != 1){
            perror("initMASegments error: ");
            return 0;
        }

        close(fd); 
    }

    return 1;
}

void spawnTrain(pid_t* trains) {
  for (int i = 0 ; i < 5 ; i++) {
    if(fork()==0) {
        char trainName[5];
        sprintf(trainName, "T%i", i);
        execlp("train", "train", trainName);
        perror("spawnTrain (prof finochio)");
      
    } 
  }
}

void spawnRegister() {
  if (fork()==0) {
    execlp("Register", "Register");
    perror("spawnRegister: (duce)"); // should never been executed
  }
}
