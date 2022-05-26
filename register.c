#include "socket-utils.h"

int main(int argc, char* argv[]) {
  SocketDetails sock;
  sock = initSocket("register_socket");
  buildSocket(sock, 5);
  runSocket(sock);
}

