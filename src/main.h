#ifndef MAIN_H
#define MAIN_H

void multiplayerCheck(void);
void *serverThreadFunction(void *arg);
void *clientThreadFunction(void *arg);
void multiplayerCheck(void);

void processPlayerNoLives(ENetPacket *packet);
void playerNoLivesFunction(void);

#endif MAIN_H
