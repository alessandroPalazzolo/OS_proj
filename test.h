#define TRAINS_COUNT 2
#define SEGMENTS_COUNT 16

void parseModeArg(char**);
int forkTrains(pid_t*);
int initMASegments(int*);

void parseModeArg(char** args){
    if (!strcmp(args[1], "ETCS1")){
        printf("ETCS1 detected\n");
        // set flag etcs1
    } else if (!strcmp(args[1], "ETCS2")){

        if (!strcmp(args[2], "RBC")){
            printf("ETCS2 RBC detected\n");
            // set flag RBC
        } else {
            printf("ETCS2 detected\n");
            // set flag ETCS2
        }

    } else {
        printf("wrong arguments\n");
        //vaffanculo
    }
}

int forkTrains(pid_t* pids){
    for (int i = 0; i < TRAINS_COUNT; i++){
        pids[i] = fork(); // PROCESSI TRENO
        if (pids[i] < 0){
        perror("forkTrains error: ");            
            return 0;
            // error
        }
        printf("%d --> %d\n",pids[i], getpid());
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
    }

    return 1;
}