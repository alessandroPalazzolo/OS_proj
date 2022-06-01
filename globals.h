#include <stdbool.h>

#define TRAINS_COUNT 5
#define SEGMENTS_COUNT 16
#define FILE_PATH_SIZE 20
#define MAX_ROUTE_SEGMENTS 10

typedef char MASegment[5];
typedef MASegment Route[10];
typedef Route Map[TRAINS_COUNT];

typedef struct {
  char name[4];
  Route route;
  int logFileFd;
  int (*checkNextMAFuncPtr)(MASegment);
} Train;