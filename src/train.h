void fillTrainData(Train*,char**);
void getRoute(Train* train);
void runSocketHandler(int, void*);
int checkNextMASegmentETCS1(Train*);
int checkNextMASegmentETCS2(Train*);
void enterMASegment(MASegment, int*);
void exitMASegment(MASegment, int*);
void runTrain(Train*);
void logTrainStatus(int, MASegment, MASegment);

