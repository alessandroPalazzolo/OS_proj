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
    RBC rbc;

    getMap(&rbc.map);

    for (int i = 0; i < SEGMENTS_COUNT; i++) { 
        rbc.segmentsOccupation[i] = 0;
    }

    for (int i = 0; i < TRAINS_COUNT; i++) {
        int stationIndex = atoi(rbc.map[i][0] + 1) - 1;
        rbc.stationsOccupation[stationIndex]++;
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
    char train[4], action[5], segment[5];
    ssize_t readTrain, readAction, readSegment;
    int MAindex, currentAction = -1;
    char nextSegFree[2], nextSegOccupied[2], nextSegStation[2];

    readTrain = read(clientFd, train, 4);
    readAction = read(clientFd, action, 10);
    readSegment = read(clientFd, segment, 5);

    if (readTrain < 0 || readAction < 0 || readSegment < 0)
        perror("runSocketHandlerServer");

    MAindex = atoi(segment+2) - 1;
    currentAction = atoi(action);
    sprintf(nextSegFree, "%d", NEXT_SEG_FREE);
    sprintf(nextSegOccupied, "%d", NEXT_SEG_OCCUPIED);
    sprintf(nextSegStation, "%d", NEXT_SEG_STATION);

    switch(currentAction){
        case ACTION_ENTER_SEGMENT:
            if (segment[0] == "S"){
                if (write(clientFd, nextSegStation, 1) < 0){
                    perror("runSocketHandlerServer");
                }
                rbc->stationsOccupation[MAindex]++;
            } else if (!rbc->segmentsOccupation[MAindex]){
                if (write(clientFd, nextSegFree, 1) < 0){
                    perror("runSocketHandlerServer");
                }
                rbc->segmentsOccupation[MAindex] = 1;
                rbc->segmentsOccupation[MAindex-1] = 0; //remove
            } else {
                if (write(clientFd, nextSegOccupied, 1) < 0){
                    perror("runSocketHandlerServer");
                }
            }
            break;
        case ACTION_EXIT_SEGMENT:
            rbc->segmentsOccupation[MAindex] = 0;
            break;
        default:
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
    char train[4];

    for(int i = 0; i < TRAINS_COUNT; i++) {
        sprintf(train, "T%d", i+1);
        write(clientFd, train, 4);

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