#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <error.h>
#include <stdbool.h>

#include "globals.h"

void getTrainInfo(char*, Train);

  int main(int argc, char* argv[]){
  
  char trainName[3];
  bool arrived = false;
  Train train;
  strcpy(trainName, argv[1]);
  getTrainInfo(trainName, train);

    while(!arrived){
        

    } 


}


void getTrainInfo(char* trainName, Train train) {
            
}
