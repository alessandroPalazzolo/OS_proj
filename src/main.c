#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <error.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <signal.h>

#include "globals.h"
#include "main.h"

int main(int argc, char* argv[]) {
    pid_t registerPid;
    int sigCounter;

    parseArgs(argc, argv);

    if (env.isRBC) {
        execl("rbc", "./rbc", NULL);
    }

    if(!initMASegments()){
        exit(EXIT_FAILURE);
    }

    execRegister();  
    execTrains();

    while(sigCounter < 5 );
    exit(EXIT_SUCCESS);
}

void parseArgs(int length, char** args) { 
    switch(length) {
        case 3:
            strcpy(env.MODE, args[1]);
            strcpy(env.MAP, args[2]); 
            env.isRBC = false;
            break;
        case 4:
            strcpy(env.MODE, args[1]);
            strcpy(env.MAP, args[3]);
            env.isRBC = true;
            break;
        default:
            perror("parseArgs");
            exit(EXIT_FAILURE);// usage()
            break;
    }

    if (strncmp(env.MODE, "ETCS", 4)) {
        perror("parseArgs");
        exit(EXIT_FAILURE);// usage()
    }

    if (strncmp(env.MAP, "MAPPA", 5)) {
        perror("parseArgs");
        exit(EXIT_FAILURE);// usage()
    }

    if (env.isRBC && strcmp(args[2], "RBC")) {
        perror("parseArgs");
        exit(EXIT_FAILURE);// usage()
    }
}

bool initMASegments() {
    char MAFilePath[FILE_PATH_SIZE];
    char MASemName[3];
    umask(0);

    for (int i = 0; i < SEGMENTS_COUNT; i++) {
        sprintf(MAFilePath, "./assets/MA%d", i + 1);
        sprintf(MASemName, "MA", i + 1);
        sem_unlink(MASemName); 
        int fd = open(MAFilePath, O_CREAT | O_WRONLY | O_TRUNC, 0666);
        if (fd < 0) {
            perror("initMASegments");
            return false;
        }

        if (write(fd, "0", 1) != 1) {
            perror("initMASegments");
            return false;
        }

        close(fd); 
    }

    return true;
}

void execTrains() {
    pid_t pid;

    for (int i = 0 ; i < TRAINS_COUNT ; i++) {
        pid = fork();

        if (pid == 0) {
            char trainName[5];
            sprintf(trainName, "T%d", i + 1);
            execl("train", "./train", trainName, env.MODE, NULL);
            perror("execTrains");
        } else if (pid < 0){
            perror("execTrains");
            exit(EXIT_FAILURE);
        }
    }
}

void execRegister() {
    pid_t pid = fork();  

    if (pid == 0) {
        execl("register", "./register", env.MAP, NULL);
        perror("execRegister");
    } else if (pid < 0){
        perror("execRegister");
        exit(EXIT_FAILURE);
    }
}
