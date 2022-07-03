#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>

#include "globals.h"
#include "socket-utils.h"

void getMap(Map*);
void sendPid();
void runSocketHandlerSendPid(int, void*);
void runSocketHandlerClient(int, void*);
void runSocketHandlerServer(int, void*);
typedef struct {
    Map map;
    int segmentsOccupation[SEGMENTS_COUNT];
    int stationsOccupation[STATIONS_COUNT];
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

    SocketDetails sock;
    sock.type = SERVER;
    sock.payload = &rbc;

    initSocket(&sock, "RBC_socket");
    buildSocket(&sock, 5);
    runSocket(&sock, &runSocketHandlerServer);

    // sendPid();
}

void runSocketHandlerSendPid(int clientFd, void* payload){
    pid_t myPid = *((pid_t*) payload);
    char myPidToString[5];

    sprintf(myPidToString, "%d", myPid);

    if (write(clientFd, myPidToString, strlen(myPidToString)) < 0){
        perror("runSocketHandlerSendPid");
        exit(EXIT_FAILURE);
    }
}

void sendPid(){
    pid_t myPid = getpid();
    SocketDetails sock;
    sock.type = CLIENT;
    sock.payload = (void*) &myPid;

    initSocket(&sock, "pid_socket");
    runSocket(&sock, &runSocketHandlerSendPid);
}

void runSocketHandlerServer(int clientFd, void* payload) {
    RBC* rbc = (RBC*) payload;
    char train[4], exitSegment[5], enterSegment[5];
    ssize_t readTrain, readEnterSegment, readExitSegment;
    int enterMAindex, exitMAindex;
    char nextSegFree[2], nextSegOccupied[2], nextSegStation[2];

    readTrain = read(clientFd, train, 4);
    readExitSegment = read(clientFd, exitSegment, 5);
    readEnterSegment = read(clientFd, enterSegment, 5);

    if (readTrain < 0 || readExitSegment < 0 || readEnterSegment < 0){
        perror("runSocketHandlerServer");
        exit(EXIT_FAILURE);
    }

    enterMAindex = atoi(enterSegment+2) - 1;
    exitMAindex = atoi(exitSegment+2) - 1;

    sprintf(nextSegFree, "%d", NEXT_SEG_FREE);
    sprintf(nextSegOccupied, "%d", NEXT_SEG_OCCUPIED);
    sprintf(nextSegStation, "%d", NEXT_SEG_STATION);

    if (enterSegment[0] == 'S'){
        if (write(clientFd, nextSegStation, 1) < 0){
            perror("runSocketHandlerServer");
        }

        enterMAindex = atoi(enterSegment+1) - 1;
        rbc->stationsOccupation[enterMAindex]++;
        rbc->segmentsOccupation[exitMAindex] = 0;
        return;
    }

    if (!(rbc->segmentsOccupation[enterMAindex])){
        if (write(clientFd, nextSegFree, 1) < 0){
            perror("runSocketHandlerServer");
        }
        rbc->segmentsOccupation[enterMAindex] = 1;
        if (exitSegment[0] == 'S'){ 
            exitMAindex = atoi(exitSegment+1) - 1;
            rbc->stationsOccupation[exitMAindex]--;
        } else {
            rbc->segmentsOccupation[exitMAindex] = 0;
        }
    } else {
        if (write(clientFd, nextSegOccupied, 1) < 0){
            perror("runSocketHandlerServer");
        }
    }
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