#include "main.h"

int main(int argc, char* argv[]) {
  pid_t trainPids[TRAINS_COUNT]; 
  pid_t registerPid;

  parseArgs(argc, argv);

  if(!initMASegments()){
    exit(EXIT_FAILURE);
  }

  if (!initTrains(trainPids)){
    exit(EXIT_FAILURE);
  }

  spawnRegister();

  exit(EXIT_SUCCESS);
}
