#include <sys/socket.h>
#include <sys/un.h>

#define DEFAULT_PROTOCOL 0
#define SERVER 0
#define CLIENT 1

typedef struct {
    int type;
    int serverFd;
    int clientFd;
    socklen_t serverLen;
    socklen_t clientLen;
    struct sockaddr_un serverUNIXaddr;
    struct sockaddr_un clientUNIXaddr;
    void* payload;
} SocketDetails;

void initSocket(SocketDetails*, char*);
void buildSocket(SocketDetails*, int);
void runSocket(SocketDetails*, void(*func_pt)(int, void*));

