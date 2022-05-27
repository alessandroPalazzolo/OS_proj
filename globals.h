#include <stdbool.h>

#define TRAINS_COUNT 4
#define SEGMENTS_COUNT 16
#define FILE_PATH_SIZE 12

typedef char MASegment[4];
typedef MASegment Route[10];
typedef char Map[TRAINS_COUNT][10][4];

struct Config {
    char MODE[10];
    char MAP[10];
    bool isRBC;
} env;
