#define TRAINS_COUNT 2
#define SEGMENTS_COUNT 16

enum modes {ECTS1, ECTS2} MODE;
enum maps {MAPPA1, MAPPA2} MAP;

int parseMode(char**);
int initMASegments(int*);
void parseMap(char*);
void spawnTrain(pid_t*);
void spawnRegister();
void usage();

int parseMode(char** args) { 
    char* mode = NULL;
    char* variant = NULL;
    strcpy(mode, args[1]);

    if (!strcmp(mode, "ETCS1")){
        MODE = ECTS1;
    } else if (!strcmp(mode, "ETCS2")){

        strcpy(variant, args[2]);
        if (!strcmp(variant, "RBC")){
            int status = 0;
            if (fork() == 0) {
                // exec RBC  
            }    
        } else {
            MODE = ECTS2;
        }

    } else {
        // usage()
        return 0;
    }

    return 1;
}

int initMASegments(int* fds){
    char paths[SEGMENTS_COUNT][12];

    for (int i = 0; i < SEGMENTS_COUNT; i++){
        sprintf(paths[i], "./assets/MA%d", i + 1);
        fds[i] = open(paths[i], O_CREAT | O_RDWR, 0666);
        if (fds[i] < 0) {
            perror("initMASegments error: ");
            return 0;
        }

        if (write(fds[i], "0", 1) != 1){
            perror("initMASegments error: ");
            return 0;
        } 
    }

    return 1;
}

void spawnTrain(pid_t* trains) {
  for (int i = 0 ; i < 5 ; i++) {
    if(fork()==0) {
      char trainName[5];
      sprintf(trainName, "T%i", i);
      switch(MODE) { //same code
        case ECTS1 : 
          execlp("TrainECTS1", "TrainECTS1", trainName);
          perror(main); // should never been executed
          break;
        case ECTS2 :
          execlp("TrainECTS2", "TrainECTS2", trainName);
          perror(main);
      }
    } 
  }
}

void parseMap(char* map) {
  if (strcmp(map, "MAPPA1")) {
    MAP = MAPPA1;
  } else if (strcmp(map, "MAPPA2")){
    MAP = MAPPA2;  
  } else {
    //usage();
  }
}

void spawnRegister() {
  if (fork()==0) {
    execlp("Register", "Register");
    perror(main); // should never been executed
  }
}
