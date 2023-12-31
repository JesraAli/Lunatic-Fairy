#ifndef gui
#define gui
#include "libs.h"
#include "structs.h"
#include "highscoreInfo.h"
#include <enet/enet.h>

void signalSecondClientJoined(void);
void resetSecondClientStatus(void);

int load(void);
void userInput(void);
void initTitle(void);
void titleLoop(void);
bool returnMultiplayerStatus(void);
void loadingScreen(void);

void initModes(void);
Background *initSeperateBackground(char *);

void presentModes(void);
int returnMode(void);

void initStage(void);

void resetStage(void);
// void restartGame(void);

void initPlayers(void);
void initPlayer(void);
Entity initPlayer2(void);

int playerCollideFairy(void);
int playerCollidePowerUp(void);

void fireBullet(void);
void fireDiagonalBullet(int x_vel, int y_vel, int distance);
void manipulateAllBullets(void);
void manipulateDBullet(void);
void drawBullets(void);
void drawDBullets(void);
int bulletHit(Entity *b);

void manipulateExplosion();
void spawnExplosion(int x, int y, int w, int h);
void drawEnemyExplosion(void);

void manipulatePowerUp();
void spawnPowerUp(int x, int y, int w, int h);
void drawPowerUp();

void spawnFairies(char);
void manipulateFairy(void);
void drawFairy(void);
void calcAtkSlope(int x_pos, int y_pos, int x_rect, int y_rect, float *dx, float *dy);
void fireEnemyBulletCall(void);
void fireEnemyBullet(Entity *f);
void drawEnemyBullets(void);

void prepareScene(void);
void presentScene(void);
int returnPlayerScore(void);
int returnPlayerLife(void);
int returnFairySpawnTimer(void);
void rendCopyBackground(void);
void rendCopyPlayer(void);

bool playerNullCheck(void);
void collisionDetection(void);

void drawText(int x, int y, int r, int g, int b, char *format, ...);

int end(void);

ENetPacket *bulletPackets(void);
void processBulletPacket(ENetPacket *packet);

ENetPacket *DBulletPackets(void);
void processDBulletPacket(ENetPacket *packet);

ENetPacket *playerPackets();
void processPlayerPacket(ENetPacket *packet);

ENetPacket *fairyPackets(void);
void processFairyPacket(ENetPacket *packet);

ENetPacket *powerUpPackets(void);
void processPowerUpPacket(ENetPacket *packet);


void rendCopyPlayer2(void);
void drawOpponentBullets(void);
void drawOpponentDBullets(void);

void resetPlayer(void);

void updateMode(ENetPacket *packet);

void setInvincible(void);
void freeCurrentPlayer(void);
void freeOpponentPlayer(void);

void resetFairyBullet(void);

#endif
