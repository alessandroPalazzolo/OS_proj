#include "train.h"

int main(int argc, char* argv[]){
  Train* train = NULL;
  fillTrain(train, argv);
  getRoute(train);
  runTrain(train);
}


