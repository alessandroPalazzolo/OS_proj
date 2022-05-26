#include <stdbool.h>

#define TRAINS_COUNT 5
#define SEGMENTS_COUNT 16
#define FILE_PATH_SIZE 12

typedef char MASegment[5];
typedef MASegment Route[10];

typedef struct {
  Route T1;
  Route T2;
  Route T3;
  Route T4;
  Route T5;
} Map;

struct Config {
    char MODE[10];
    char MAP[10];
    bool isRBC;
} env;

