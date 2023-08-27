#ifndef CLIENT_H
#define CLIENT_H

#include <enet/enet.h>
void runClient(int serverPort);
void sendUpdateToServerAndBroadcast(ENetPacket *packet, int channel);
#endif