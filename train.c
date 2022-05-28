#include "train.h";

int main(int argc, char* argv[]){
  Train* train;
  fillTrain(train, argv);
  runTrain(train);
}


