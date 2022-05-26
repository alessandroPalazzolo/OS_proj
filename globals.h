#include <stdbool.h>

#define TRAINS_COUNT 5
#define SEGMENTS_COUNT 16
#define FILE_PATH_SIZE 12

typedef char MASegment[5];
typedef MASegment Route[10];
typedef Route Map[5];

typedef struct {
  char name [3];
  MASegment route[10];
  MASegment currentPosition; 
} Train; 

struct Config {
    char MODE[10];
    char MAP[10];
    bool isRBC;
} env;

