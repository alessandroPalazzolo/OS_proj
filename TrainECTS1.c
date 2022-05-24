#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <error.h>
#include <stdbool.h>

typedef char MASegment[4];

typedef struct {
  char name [3];
  MASegment route[6];
  MASegment currentPosition;  
} train; 

void getMap();

int main(int argc, char* argv[]){
  
  train Train;
  strcpy(Train.name, argv[2]);
  
  bool arrived = false;
  
  getMap(Train.name, Train.route);

  while(!arrived){
    

  } 


}


void getMap(char* name, MASegment* route) {
        
}
