#ifndef CLIENT_H
#define CLIENT_H

#include <enet/enet.h>
void runClient(int serverPort);
ENetHost *returnClientServer(void);
void sendUpdateToServerAndBroadcast(ENetPacket *packet);
#endif