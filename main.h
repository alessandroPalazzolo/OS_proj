#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <error.h>

#include "globals.h"

void parseArgs(int, char**);
bool initMASegments();
void parseMap(char*);
void initTrains();
void spawnRegister();
void usage();

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

    if (strcmp(env.MAP, "MAP1") && strcmp(env.MAP, "MAP2")){
        // usage()
    }

    if (env.isRBC && strcmp(args[2], "RBC")){
        // usage()
    }
}

bool initMASegments(){
    char paths[SEGMENTS_COUNT][FILE_PATH_SIZE];
    umask(0);
    mkdir("assets", 0766);

    for (int i = 0; i < SEGMENTS_COUNT; i++){
        sprintf(paths[i], "./assets/MA%d", i + 1);
        int fd = open(paths[i], O_CREAT | O_RDWR, 0666);
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
            execl("train.o", "train", trainName, NULL);
            perror("initTrains: ");
        } else if (pid < 0){
            perror("initTrains: ");
            exit(EXIT_FAILURE);
        }
    }
}

void spawnRegister() {
  if (fork() == 0) {
    execlp("Register", "Register");
    perror("spawnRegister: (duce)"); // should never be executed
  }
}
