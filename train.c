#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>

#include "globals.h"

void getMap();

int main(int argc, char* argv[]){
  
  Train train;
  strcpy(train.name, argv[1]);
  
  bool arrived = false;
  
  getMap(train.name, train.route);

  while(!arrived){
    

  } 
}


void getMap(char* name, MASegment* route) {
        
}
