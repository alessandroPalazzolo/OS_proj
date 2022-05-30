#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <error.h>
#include <stdbool.h>
#include <semaphore.h>

#include "globals.h"
#include "main.h"

int main(int argc, char* argv[]) {
  pid_t registerPid;

  parseArgs(argc, argv);
  if (env.isRBC) {
    // exec RBC without fork
  }

  if(!initMASegments()){
    exit(EXIT_FAILURE);
  }

  initTrains();
  spawnRegister();

  exit(EXIT_SUCCESS); 
}


void parseArgs(int length, char** args) { 
    switch(length){
        case 3:
            strcpy(env.MODE, args[1]);
            strcpy(env.MAP, args[2]);
            env.isRBC = true;
            break;
        case 4:
            strcpy(env.MODE, args[1]);
            strcpy(env.MAP, args[3]);
            env.isRBC = false;
            break;
        default:
            // usage()
            break;
    }

    if (strcmp(env.MODE, "ETCS1") && strcmp(env.MODE, "ETCS2")){
        // usage()
    }

    if (strcmp(env.MAP, "MAPPA1") && strcmp(env.MAP, "MAPPA2")){
        // usage()
    }
}

bool initMASegments(){
    char MAFilePath[FILE_PATH_SIZE];
    char MASemName[3];

    for (int i = 0; i < SEGMENTS_COUNT; i++){
        sprintf(MAFilePath, "./assets/MA%d", i + 1);
                
        int fd = open(MAFilePath, O_CREAT | O_RDWR, 0666);
        if (fd < 0) {
            perror("initMASegments error: ");
            return false;
        }

        if (write(fd, "0", 1) != 1){
            perror("initMASegments error: ");
            return false;
        }

        close(fd); 
    }

    return true;
}

void initTrains() {
    pid_t pid;
    for (int i = 0 ; i < TRAINS_COUNT ; i++) {
        pid = fork();
        if (pid == 0) {
            char trainName[5];
            sprintf(trainName, "T%d", i + 1);
            execlp("train", "train", trainName, env.MODE, NULL);
            perror("initTrains");
        } else if (pid < 0){
            perror("initTrains");
            exit(EXIT_FAILURE);
        }
    }
}

void spawnRegister() {
  if (fork()==0) {
    execlp("Register", "Register", NULL);
    perror("spawnRegister"); // should never be executed
  }
}

