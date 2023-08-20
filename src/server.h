#ifndef SERVER_H
#define SERVER_H

#include <enet/enet.h>
void runServer(int serverPort);
ENetHost* returnServerVar(void);
void sendSecondClientStatus(bool status);
// void sendServerPointer(ENetPeer *clientPeer, const ENetAddress *serverAddress)
#endif