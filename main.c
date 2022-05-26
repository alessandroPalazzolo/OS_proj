#include "main.h"

int main(int argc, char* argv[]) {
  pid_t registerPid;

  parseArgs(argc, argv);
  if (env.isRBC) {
    // exec RBC without fork
  }

  if(!initMASegments()){
    exit(EXIT_FAILURE);
  }

  initTrains();
  spawnRegister();

  exit(EXIT_SUCCESS);
}
