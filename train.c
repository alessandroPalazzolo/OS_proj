#include "train.h"

int main(int argc, char* argv[]){
  Train train;
  // fillTrain(&train, argv);
  strcpy(train.name, argv[1]); //replace with ln 4
  getRoute(&train);
  printf("%s\n", train.route[2]);
  // runTrain(&train);
}