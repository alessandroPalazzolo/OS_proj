#include "globals.h"

void getMap(Map*);
void runSocketHandlerClient(int, void*);
void runSocketHandlerServer(int, void*);
void logRbc(int, char*, MASegment, MASegment, bool);
typedef struct {
    Map map;
    int segmentsOccupation[SEGMENTS_COUNT];
    int stationsOccupation[STATIONS_COUNT];
    int logFileFd;
} RBC;
