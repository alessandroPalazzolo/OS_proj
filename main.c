#include "main.h"

int main(int argc, char* argv[]) {
  pid_t trainPids[TRAINS_COUNT]; 
  pid_t registerPid;

  parseArgs(argv);

  if(!initMASegments()){
    exit(EXIT_FAILURE);
  }

  if (!initTrains(trainPids)){
    exit(EXIT_FAILURE);
  }

  parseMap(argv[2]);
  spawnRegister();

  exit(EXIT_SUCCESS);
}
