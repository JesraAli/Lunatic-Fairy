#ifndef gui
#define gui
#include "libs.h"
#include "structs.h"
#include "highscore.h"

int load(void);
void userInput(void);
void prepareScene(void);
void presentScene(void);
void initTitle(void);
void titleLoop(void);
void initBackground(void);
void initStage(void);

void resetStage(void);
void restartGame(void);

void initPlayer(void);
int playerCollide(void); // collide with fairy

void fireBullet(void);
void manipulateAllBullets(void);
void drawBullets(void);
int bulletHit(Entity *b);

void manipulateExplosion();
void fireExplosion(int x, int y, int w, int h);
void drawEnemyExplosion(void);

void spawnFairies(void);
void manipulateFairy(void);
void drawFairy(void);
void calcAtkSlope(int x_pos, int y_pos, int x_rect, int y_rect, float *dx, float *dy);
void fireEnemyBulletCall(void);
void fireEnemyBullet(Entity *f);
void drawEnemyBullets(void);

int returnPlayerScore(void);
int returnPlayerLife(void);
int returnFairySpawnTimer(void);
void rendCopyBackground(void);
void rendCopyPlayer(void);

bool playerNullCheck(void);
void collisionDetection(void);

void drawText(int x, int y, int r, int g, int b, char *format, ...);

int end(void);


#endif
