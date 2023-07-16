#ifndef structs
#define structs
#include "libs.h"

// Structs
typedef struct
{
    int up;
    int down;
    int left;
    int right;
    int fire;
} Action;

typedef struct
{
    int x_pos;
    int y_pos;
    float x_vel;
    float y_vel;
    int life;
    int reload;
    int expVisibility;
    bool leftDir;
    bool rightDir;
    SDL_Texture *tex;
    SDL_Rect rect;
    SDL_Rect hitbox;
    struct Entity *next; // For next Entity in linked list
} Entity;

typedef struct
{ // Linked lists for players, bullets, fairies, explosions
    Entity playerHead, *playerTail;
    Entity bulletHead, *bulletTail;
    Entity fairyHead, *fairyTail;
    Entity enemyBulletHead, *enemyBulletTail;
    Entity explosionHead, *explosionTail;
} Stage;

typedef struct
{
    int active;
    SDL_Texture *tex;
    SDL_Rect rect;
} Background;

typedef struct
{
    int recent;
    int score;
} Highscore;

typedef struct
{
    Highscore highscore[NUM_HIGHSCORES];

} HighscoreList;

#endif
