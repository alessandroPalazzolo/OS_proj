#include "register.h"

int main(int argc, char* argv[]) {
  Map map;
  SocketDetails sock;
  char mapName[10];
  int mapFd, mapIsLoaded;

  strcpy(mapName, "MAPPA1"); //replace arv[1]
  sock.type = SERVER;
  sock.payload = &map;

  mapIsLoaded = loadMapFromFile(mapName, &map); 
  if(!mapIsLoaded){
    perror("error loading map");
    exit(EXIT_FAILURE);
  }

  initSocket(&sock, "register_socket");
  buildSocket(&sock, 5);
  runSocket(&sock, &runSocketHandler);
}