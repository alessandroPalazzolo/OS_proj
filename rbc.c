#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <stdbool.h>

#include "globals.h"
#include "socket-utils.h"

void getMap(Map*);
void runSocketHandlerClient(int, void*);
void runSocketHandlerServer(int, void*);
void logRbc(int, char*, MASegment, MASegment, bool);
typedef struct {
    Map map;
    int segmentsOccupation[SEGMENTS_COUNT];
    int stationsOccupation[STATIONS_COUNT];
    int logFileFd;
} RBC;

int main(int argc, char* argv[]) {
    RBC rbc;

    getMap(&rbc.map);

    for (int i = 0; i < SEGMENTS_COUNT; i++) { 
        rbc.segmentsOccupation[i] = 0;
    }

    for (int i = 0; i < STATIONS_COUNT; i++) {
        rbc.stationsOccupation[i] = 1;
    }

    char logFilePath[20];
    sprintf(logFilePath, "log/rbc.log");
    rbc.logFileFd = open(logFilePath, O_WRONLY | O_TRUNC | O_CREAT, 0666);
    if (rbc.logFileFd < 0){
        perror("rbc_main");
    }

    SocketDetails sock;
    sock.type = SERVER;
    sock.payload = &rbc;

    initSocket(&sock, "RBC_socket");
    buildSocket(&sock, 5);
    runSocket(&sock, &runSocketHandlerServer);
}

void runSocketHandlerServer(int clientFd, void* payload) {
    RBC* rbc = (RBC*) payload;
    char train[4], nextSegStatus[2];
    MASegment exitSegment, enterSegment;
    ssize_t readTrain, readEnterSegment, readExitSegment;
    int enterMAindex, exitMAindex;
    bool accessGranted; 

    readTrain = read(clientFd, train, 4);
    readExitSegment = read(clientFd, exitSegment, 5);
    readEnterSegment = read(clientFd, enterSegment, 5);

    if (readTrain < 0 || readExitSegment < 0 || readEnterSegment < 0);
        printf("runSocketHandlerServer %s", train);

    enterMAindex = atoi(enterSegment+2) - 1;
    exitMAindex = atoi(exitSegment+2) - 1;

    if (enterSegment[0] == 'S'){
        sprintf(nextSegStatus, "%d", NEXT_SEG_STATION);
        if (write(clientFd, nextSegStatus, 1) < 0){
            perror("runSocketHandlerServer");
        }

        enterMAindex = atoi(enterSegment+1) - 1;
        rbc->stationsOccupation[enterMAindex]++;
        rbc->segmentsOccupation[exitMAindex] = 0;
        return;
    }

    if (!(rbc->segmentsOccupation[enterMAindex])){
        sprintf(nextSegStatus, "%d", NEXT_SEG_FREE);
        if (write(clientFd, nextSegStatus, 1) < 0){
            perror("runSocketHandlerServer");
        }
        rbc->segmentsOccupation[enterMAindex] = 1;
        if (exitSegment[0] == 'S'){ 
            exitMAindex = atoi(exitSegment+1) - 1;
            rbc->stationsOccupation[exitMAindex] = 0;
        } else {
            rbc->segmentsOccupation[exitMAindex] = 0;
        }
    } else {
        sprintf(nextSegStatus, "%d", NEXT_SEG_OCCUPIED);
        if (write(clientFd, nextSegStatus, 1) < 0){
            perror("runSocketHandlerServer");
        }
    }

    accessGranted = atoi(nextSegStatus);
    logRbc(rbc->logFileFd, train, exitSegment, enterSegment, accessGranted);

}
 
void getMap(Map* map) {
    SocketDetails sock;
    sock.type = CLIENT;
    sock.payload = &map;
    
    initSocket(&sock, "register_socket");
    runSocket(&sock, &runSocketHandlerClient);
}

void runSocketHandlerClient(int clientFd, void* payload) {
    Map* map = (Map*) payload;

    if (write(clientFd, "rbc", 3) < 0){
        perror("runSocketHandlerClient");
        exit(EXIT_FAILURE);
    }

    for(int i = 0; i < TRAINS_COUNT; i++) {
       	char buffer[5], currentChar;
    	int hasRead, k = 0, j = 0;

    	do {
        	hasRead = read(clientFd, &currentChar, 1);
        	buffer[k] = currentChar;
        	if (currentChar == '\0') {
        		strcpy((*map)[i][j], buffer);
        		j++;
        		k = 0;
        		memset(buffer, '\0', 4);
        	} else {
        		k++;
        	}
        } while(hasRead);
    }
}

void logRbc(int fd, char* trainName, MASegment exitSegment, MASegment enterSegment, bool accessGranted){
    char logString[100], timeString[40], accessStr[8];
    time_t timeSec = time(NULL);
    struct tm* timeInfo = localtime(&timeSec);
    strftime(timeString, sizeof(timeString), "%d %B %Y %X", timeInfo);

    if (accessGranted){
        strcpy(accessStr, "true");
    } else {
        strcpy(accessStr, "false");
    }
 
    sprintf(logString, "[train: %s] [actual: %s] [next: %s] [access granted: %s] %s", trainName, exitSegment, enterSegment, accessStr, timeString);
    if (write(fd, logString, strlen(logString)) != strlen(logString)) {
        perror("logTrainStatus");
    }

}