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

SocketDetails initSocket(char*);
void buildSocket(SocketDetails, int);
void runSocket(SocketDetails);

SocketDetails initSocket(char* path){
    SocketDetails sock;
    signal(SIGCHLD, SIG_IGN);

    sock.serverUNIXaddr.sun_family = AF_UNIX;
    strcpy(sock.serverUNIXaddr.sun_path, path);
    sock.serverLen = sizeof(sock.serverUNIXaddr);
    sock.clientLen = sizeof(sock.clientUNIXaddr);
    sock.serverFd = socket(AF_UNIX, SOCK_STREAM, DEFAULT_PROTOCOL);
    if (sock.serverFd < 0) {
        perror("initSocket: ");
        exit(EXIT_FAILURE);
    }
    return sock;
}

void buildSocket(SocketDetails sock, int bufferSize) {
    unlink(sock.serverUNIXaddr.sun_path);
    struct sockaddr* serverSockAddrPtr = (struct sockaddr*) &sock.serverUNIXaddr;
    if (bind(sock.serverFd, serverSockAddrPtr, sock.serverLen) < 0) {
        perror("buildSocket");
        exit(EXIT_FAILURE);
    }
    listen(sock.serverFd, bufferSize);
}

void runSocket(SocketDetails sock) {
    int clientFd;
    pid_t pid;
    struct sockaddr* clientSockAddrPtr = (struct sockaddr*) &sock.clientUNIXaddr;

    while(1){
        clientFd = accept(sock.serverFd, clientSockAddrPtr, &sock.clientLen);
        pid = fork();
        if (pid == 0){
            write(clientFd, "diomerda", 8);
            close(clientFd);
            exit(EXIT_SUCCESS);
        } else if (pid > 0) {
            printf("nre connection\n");
            close(clientFd);
        } else {
            perror("runSocket");
        }
    }
}