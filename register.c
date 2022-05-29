#include "register.h"

int main(int argc, char* argv[]) {
  SocketDetails sock;
  sock.type = SERVER;
  int mapFd, mapIsLoaded;
  Map map;

  mapIsLoaded = loadMapFromFile("MAPPA1", &map); // replace with argv[1]
  if(!mapIsLoaded){
    perror("error loading map");
    exit(EXIT_FAILURE);
  }

  initSocket(&sock, "register_socket");
  buildSocket(&sock, 5);
  runSocket(&sock, &runSocketHandler, &map);
}