#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <error.h>

#include "globals.h"

void parseArgs(int, char**);
int initMASegments();
void parseMap(char*);
int initTrains(pid_t*);
void spawnRegister();
void usage();

void parseArgs(int length, char** args) { 
    switch(length){
        case 3:
            strcpy(env.MODE, args[1]);
            strcpy(env.MAP, args[2]);
            env.isRBC = 0;
            break;
        case 4:
            strcpy(env.MODE, args[1]);
            strcpy(env.MAP, args[3]);
            env.isRBC = 1;
            break;
        default:
            // usage()
            break;
    }

    if (strcmp(env.MODE, "ETCS1") && strcmp(env.MODE, "ETCS2")){
        // usage()
    }

    if (strcmp(env.MAP, "MAP1") && strcmp(env.MAP, "MAP2")){
        // usage()
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
    perror("spawnRegister: (duce)"); // should never be executed
  }
}
