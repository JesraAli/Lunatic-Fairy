#ifndef CLIENT_H
#define CLIENT_H

#include <enet/enet.h>
void connectToServer(ENetHost *client, const char *serverAddress, int serverPort);
#endif