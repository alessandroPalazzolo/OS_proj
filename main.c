#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <error.h>

#include "main.h"

int main(int argc, char* argv[]) {
  pid_t trainPids[TRAINS_COUNT]; 
  pid_t registerPid;
  int segmentFds[SEGMENTS_COUNT];

  if(!parseMode(argv)){
    exit(EXIT_FAILURE);
  }

  if(!initMASegments(segmentFds)){
    exit(EXIT_FAILURE);
  }

  if (!forkTrains(trainPids)){
    exit(EXIT_FAILURE);
  }

  parseMap(argv[2]);
  spawnTrain(trainPid);
  spawnRegister();

  exit(EXIT_SUCCESS);
}
