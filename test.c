#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

#include "test.h"

int main(int argc, char* argv[]){
    pid_t trainPids[TRAINS_COUNT]; 
    pid_t registerPid;
    int segmentFds[SEGMENTS_COUNT];

    parseModeArg(argv);

    if(!initMASegments(segmentFds)){
        exit(EXIT_FAILURE);
    }

    if (!forkTrains(trainPids)){
        // error
    }
}  