#define TRAINS_COUNT 5
#define SEGMENTS_COUNT 16
#define STATIONS_COUNT 8
#define FILE_PATH_SIZE 20
#define MAX_ROUTE_SEGMENTS 10
#define ACTION_ENTER_SEGMENT 1
#define ACTION_EXIT_SEGMENT 0
#define NEXT_SEG_FREE 0
#define NEXT_SEG_OCCUPIED 1
#define NEXT_SEG_STATION 2

typedef char MASegment[5];
typedef MASegment Route[10];
typedef Route Map[TRAINS_COUNT];

typedef struct trainData {
  char name[4];
  Route route;
  int logFileFd;
  MASegment* nextMApt;
  int RBCresponse;
  int (*checkNextMAFuncPtr)(struct trainData*);
} Train;