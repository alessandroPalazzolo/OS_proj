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
#include <sys/wait.h>

#include "globals.h"
// #include "socket-utils.h"
#include "main.h"

pid_t trainPids[TRAINS_COUNT];
int sigCounter = 0;

int main(int argc, char* argv[]) {
    signal(SIGUSR1, signalHandler);

    parseArgs(argc, argv);

    if (env.isRBC) {
        execl("bin/rbc", "./rbc", NULL);
    }

    if(!initMASegments()){
        exit(EXIT_FAILURE);
    }

    execRegister();
    execTrains();

    // if (env.MODE == "ETCS2") {
    //     killRbc();
    // }

    exit(EXIT_SUCCESS);
}

void runSocketHandlerKillRbc(int clientFd, void* payload){
    pid_t rbcPid;
    char rbcPidToString[5];

    if (read(clientFd, rbcPidToString, 5) < 0){
        perror("killRbc");
        exit(EXIT_FAILURE);
    }

    rbcPid = atoi(rbcPidToString);
    kill(rbcPid, SIGTERM);
}

// void killRbc(){
//     SocketDetails sock;
//     sock.type = SERVER;

//     initSocket(&sock, "pid_socket");
//     buildSocket(&sock, 5);
//     runSocket(&sock, &runSocketHandlerKillRbc);
// }

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
            exit(EXIT_FAILURE);
            break;
    }

    if (strncmp(env.MODE, "ETCS", 4)) {
        perror("parseArgs");
        exit(EXIT_FAILURE);
    }

    if (strncmp(env.MAP, "MAPPA", 5)) {
        perror("parseArgs");
        exit(EXIT_FAILURE);
    }

    if (env.isRBC && strcmp(args[2], "RBC")) {
        perror("parseArgs");
        exit(EXIT_FAILURE);
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

void signalHandler(int sig){
    sigCounter++;

    if (sigCounter == TRAINS_COUNT){
        for (int i = 0; i < TRAINS_COUNT; i++){
            kill(trainPids[i], SIGTERM);
        }
    }
}

void execTrains() {
    pid_t pid;

    for (int i = 0 ; i < TRAINS_COUNT ; i++) {
        pid = fork();

        if (pid == 0) {
            char trainName[5];
            sprintf(trainName, "T%d", i + 1);
            execl("bin/train", "./train", trainName, env.MODE, NULL);
            perror("execTrains");
        } else if (pid < 0){
            perror("execTrains");
            exit(EXIT_FAILURE);
        } else {
            trainPids[i] = pid;
        }
    }

    for (int i = 0; i < TRAINS_COUNT; i++){
        pause();
    }
}

void execRegister() {
    pid_t pid = fork();  

    if (pid == 0) {
        execl("bin/register", "./register", env.MAP, NULL);
        perror("execRegister");
    } else if (pid < 0){
        perror("execRegister");
        exit(EXIT_FAILURE);
    }
}
