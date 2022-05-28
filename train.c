#include "train.h"

int main(int argc, char* argv[]){
  Train* train = (Train*) malloc(sizeof(Train));
  fillTrain(train, argv);
  runTrain(train);
}


