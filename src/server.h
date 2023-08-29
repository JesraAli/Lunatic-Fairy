#ifndef SERVER_H
#define SERVER_H

#include <enet/enet.h>
#include <stdbool.h>

void runServer(int serverPort);
ENetHost* returnServerVar(void);
void sendSecondClientStatus(bool status);
#endif