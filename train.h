#define NEXT_SEG_FREE 0
#define NEXT_SEG_OCCUPIED 1
#define NEXT_SEG_STATION 2

void fillTrainData(Train*,char**);
void getRoute(Train* train);
void runSocketHandler(int, void*);
int checkNextMASegmentETCS1(MASegment);
int checkNextMASegmentETCS2(MASegment);
void enterMASegment(MASegment, int*);
void exitMASegment(MASegment, int*);
void runTrain(Train*);
void logTrainStatus(int, MASegment, MASegment);

