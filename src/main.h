struct Config {
    char MODE[10];
    char MAP[10];
    bool isRBC;
} env;

void parseArgs(int, char**);
bool initMASegments();
void execTrains();
void execRegister();
void usage();

