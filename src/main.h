struct Config {
    char MODE[10];
    char MAP[10];
    bool isRBC;
} env;

void parseArgs(int, char**);
// void runSocketHandlerKillRbc(int, void*);
// void killRbc();
bool initMASegments();
void signalHandler(int);
void execTrains();
void execRegister();
void usage();

