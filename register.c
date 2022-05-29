#include "register.h"

int main(int argc, char* argv[]) {
  SocketDetails sock;
  char mapName[10];
  int mapFd, mapIsLoaded;

  strcpy(mapName, argv[1]);
  sock.type = SERVER;

  mapIsLoaded = loadMapFromFile(mapName); 
  if(!mapIsLoaded){
    perror("error loading map");
    exit(EXIT_FAILURE);
  }

  initSocket(&sock, "register_socket");
  buildSocket(&sock, 5);
  runSocket(&sock, &runSocketHandler);
}