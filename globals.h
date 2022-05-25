#define GLOBAL_FLAG 

#define TRAINS_COUNT 5
#define SEGMENTS_COUNT 16
#define MA_FILE_PATH 12

typedef char MASegment[4];

typedef struct gegio {
  char name [3];
  MASegment route[6];
  MASegment currentPosition;  
} Train; 

struct Config {
    char MODE[10];
    char MAP[10];
    int isRBC;
} env;
