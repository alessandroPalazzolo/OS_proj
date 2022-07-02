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
void runSocketHandlerClient(int, void*);
void runSocketHandlerServer(int, void*);

typedef struct {
    Map map;
    int segmentsOccupation[SEGMENTS_COUNT];
    int stationsOccupation[STATIONS_COUNT];
} RBC;


int main(int argc, char* argv[]) {

    RBC statusRBC;

    getMap(&statusRBC.map);

    for (int i = 0; i < SEGMENTS_COUNT; i++) { 
        statusRBC.segmentsOccupation[i] = 0;
    }

    for (int i = 0; i < TRAINS_COUNT; i++) {
        int stationIndex = atoi(statusRBC.map[i][0] + 1) - 1;
        statusRBC.stationsOccupation[stationIndex]++;
    }


    SocketDetails sock;
    sock.type = SERVER;
    sock.payload = &statusRBC;

    initSocket(&sock, "RBC_socket");
    buildSocket(&sock, 5);
    runSocket(&sock, &runSocketHandlerServer);
}

void runSocketHandlerServer(int clientFd, void* payload) {
    
    RBC* statusRBC = (RBC*) payload;
    char train[4], action[7], segment[4];
    int result;

    if (read(clientFd, train, 4) < 0)
        perror("runSocketHandlerServer");

    if (read(clientFd, action, 7) < 0) 
        perror("runSocketHandlerServer");
    
    if (read(clientFd, segment, 4) < 0) 
        perror("runSocketHandlerServer");

    int MAIndex = atoi(segment+2);

    if (strncmp(action, "enter", 5)) {
        
        if(statusRBC->segmentsOccupation[MAIndex] == 0){
            statusRBC->segmentsOccupation[MAIndex] = 1;

            if(write(clientFd, "granted", 7) < 0) 
                perror("runSocketHandlerServer");

            //logRbc(train, segment);

        } else {

            if(write(clientFd, "denied", 7) < 0)
                perror("runSocketHandlerServer");
                
            //logRbc(train, segment);
        }
    }
    
    if (strncmp(action, "exit", 4)) {
        if(write(clientFd, "granted", 7) < 0)
            perror("runSocketHandlerServer");
    }
}

 
void getMap(Map* map) {
    SocketDetails sock;
    sock.type = CLIENT;
    sock.payload = &map;
    
    initSocket(&sock, "register_socket");
    runSocket(&sock, &runSocketHandlerClient);
}



// RBC pretend to be each train, maybe better if register can handle an rbc request sending map directly (?)
void runSocketHandlerClient(int clientFd, void* payload) {
    Map* map = (Map*) payload;
    int writeResult;
    char train[4];

    writeResult = write(clientFd, "RBC", 4);

    if (writeResult == -1) {
        perror("runSocketHandlerClient");
    }
    
    for(int i = 0; i < TRAINS_COUNT; i++) {
       	char buffer[5], currentChar;
    	int hasRead, k = 0, j = 0;

    	do {
        	hasRead = read(clientFd, &currentChar, 1);
        	buffer[k] = currentChar;
        	if (currentChar == '\0') {
        		strcpy(*map[i][j], buffer);
        		j++;
        		k = 0;
        		memset(buffer, '\0', 4);
        	} else {
        		k++;
        	}
        } while(hasRead);
    }
}