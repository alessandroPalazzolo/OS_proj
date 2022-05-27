#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h> 
#include <errno.h>

#define DEFAULT_PROTOCOL 0

typedef struct {
    int serverFd;
    socklen_t serverLen;
    socklen_t clientLen;
    struct sockaddr_un serverUNIXaddr;
    struct sockaddr_un clientUNIXaddr;
} SocketDetails;

void initSocket(SocketDetails*, char*);
void buildSocket(SocketDetails*, int);
void runSocket(SocketDetails*, void(*func_pt)(int));

void initSocket(SocketDetails* sock, char* path){
    signal(SIGCHLD, SIG_IGN);

    sock->serverUNIXaddr.sun_family = AF_UNIX;
    strcpy(sock->serverUNIXaddr.sun_path, path);
    sock->serverLen = sizeof(sock->serverUNIXaddr);
    sock->clientLen = sizeof(sock->clientUNIXaddr);

    sock->serverFd = socket(AF_UNIX, SOCK_STREAM, DEFAULT_PROTOCOL);
    if (sock->serverFd < 0) {
        perror("initSocket: ");
        exit(EXIT_FAILURE);
    }

    return sock;
}

void buildSocket(SocketDetails* sock, int bufferSize) {
    struct sockaddr* serverSockAddrPtr = (struct sockaddr*) &sock->serverUNIXaddr;
    int isBound;

    unlink(sock->serverUNIXaddr.sun_path);
    isBound = bind(sock->serverFd, serverSockAddrPtr, sock->serverLen);

    if (isBound < 0) {
        perror("buildSocket");
        exit(EXIT_FAILURE);
    }

    listen(sock->serverFd, bufferSize);
}

void runSocket(SocketDetails* sock, void (*func_pt)(int)) {
    int clientFd;
    pid_t pid;
    struct sockaddr* clientSockAddrPtr = (struct sockaddr*) &sock->clientUNIXaddr;

    while(1){
        clientFd = accept(sock->serverFd, clientSockAddrPtr, &sock->clientLen);
        pid = fork();

        if (pid == 0){
            (*func_pt)(clientFd);
            close(clientFd);
            exit(EXIT_SUCCESS);
        } else if (pid > 0) {
            close(clientFd);
        } else {
            perror("runSocket");
        }
    }
}