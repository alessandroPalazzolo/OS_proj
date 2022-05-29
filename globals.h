#include <stdbool.h>

#define TRAINS_COUNT 5
#define SEGMENTS_COUNT 16
#define FILE_PATH_SIZE 15

typedef char MASegment[5];
typedef MASegment Route[10];
typedef Route Map[TRAINS_COUNT];

typedef struct {
  char name[4];
  Route route;
  int (*checkNextMAFuncPtr)(MASegment);
  int logFileFd;
} Train;

struct Config {
    char MODE[10];
    char MAP[10];
    bool isRBC;
} env;

