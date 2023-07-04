#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define SCROLL_SPEED 300 // Speed in pixels per second
#define SPEED 300

#define MAX(a, b) (a > b ? a : b)
#define MIN(a, b) (a < b ? a : b)

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
    SDL_Texture *tex;
    SDL_Rect rect;
    SDL_Rect hitbox;
    struct Entity *next; // For next Entity in linked list
} Entity;

typedef struct
{ // Linked lists for players & bullets
    Entity playerHead, *playerTail;
    Entity bulletHead, *bulletTail;
    Entity fairyHead, *fairyTail;
    Entity enemyBulletHead, *enemyBulletTail;
} Stage;

// Function Declarations
int load(void);
void userInput(void);
void prepareScene(void);
int end(void);
void initStage(void);
static void initPlayer(void);
static void fireBullet(void);
static void manipulateAllBullets(void);
static void drawBullets(void);
static void spawnFairies(void);
static void manipulateFairy(void);
static void drawFairy(void);
static int bulletHit(Entity *);
static int playerCollide(void);
static void resetStage(void);
static void fireEnemyBulletCall(void);
static void fireEnemyBullet(Entity *);
static void drawEnemyBullets(void);

// Variables
SDL_Window *win;
SDL_Renderer *rend;
SDL_Surface *surface;

Entity *player;
Action action;
Stage stage;

int fairySpawnTimer;

/*Main Function*/
int main(int argc, char **argv)
{

    load();
    initStage();

    memset(&action, 0, sizeof(Action)); // Set action variables to 0

    while (true)
    {
        prepareScene(); // Prepare Scene (Background & Clear)

        userInput();

        // Collision detected with bounds (detect if sprite is going out of  window)
        if (player != NULL)
        {
            if (player->x_pos <= 0)
                player->x_pos = 0; // Reset positions to keep in window
            if (player->y_pos <= 0)
                player->y_pos = 0;
            if (player->x_pos >= WINDOW_WIDTH - player->rect.w)
                player->x_pos = WINDOW_WIDTH - player->rect.w;
            if (player->y_pos >= WINDOW_HEIGHT - player->rect.h)
                player->y_pos = WINDOW_HEIGHT - player->rect.h;

            // Set the positions in the struct
            player->rect.y = player->y_pos;
            player->rect.x = player->x_pos;

            player->hitbox.y = player->y_pos + 26; //+ is down, - is up
            // printf("y position is: %d\n", player->hitbox.y);
            player->hitbox.x = player->x_pos;
        }

        if (action.fire && player->reload == 0)
        {
            fireBullet();
        }

        manipulateAllBullets();
        manipulateFairy();
        fireEnemyBulletCall();
        spawnFairies();
        playerCollide(); // Check if player collided with fairy

        // Check if player killed
        if (player == NULL)
        {
            resetStage();
        }

        // Present Scene: draw the image to the window
        if (player->rect.x == 0 && player->rect.y == 0) //check if its 0 (in top left corner) //NEED TO CHANGE AT LATER DATE
        {
            continue;
        }
        SDL_RenderCopy(rend, player->tex, NULL, &player->rect);

        drawBullets();
        drawEnemyBullets();
        drawFairy();

        SDL_RenderPresent(rend);

        SDL_Delay(1000 / 60); // Wait 1/60th of a second
    }

    end();
}

/*Load Initialisations Function*/
int load()
{
    // Initialise SDL Subsystems
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
    {
        printf("Error in intialising SDL: %s\n", SDL_GetError());
        return 1;
    }

    // Create Window
    win = SDL_CreateWindow(
        "Jesra's Game",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT, 0);

    if (!win)
    { // Check if error when creating window:
        printf("Error creating window: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Creating renderer
    Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    rend = SDL_CreateRenderer(win, -1, render_flags);
    if (!rend)
    {
        printf("Error creating renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(win);
        SDL_Quit;
        return 1;
    }
}

/*User Key Input Function*/
void userInput()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {

        switch (event.type)
        {
        case SDL_QUIT:
            end();
            break;

        case SDL_KEYDOWN: // Meaning we've pressed the button on a keyboard
            // Gets key event structure, retrieves the key pressed
            switch (event.key.keysym.scancode)
            {
            case SDL_SCANCODE_W:
            case SDL_SCANCODE_UP:
                action.up = 1;
                break;
            case SDL_SCANCODE_A:
            case SDL_SCANCODE_LEFT:
                action.left = 1;
                break;
            case SDL_SCANCODE_S:
            case SDL_SCANCODE_DOWN:
                action.down = 1;
                break;
            case SDL_SCANCODE_D:
            case SDL_SCANCODE_RIGHT:
                action.right = 1;
                break;
            case SDL_SCANCODE_Z:
                action.fire = 1;
                break;
            }
            break;
        case SDL_KEYUP:
            switch (event.key.keysym.scancode)
            {
            case SDL_SCANCODE_W:
            case SDL_SCANCODE_UP:
                action.up = 0;
                break;
            case SDL_SCANCODE_A:
            case SDL_SCANCODE_LEFT:
                action.left = 0;
                break;
            case SDL_SCANCODE_S:
            case SDL_SCANCODE_DOWN:
                action.down = 0;
                break;
            case SDL_SCANCODE_D:
            case SDL_SCANCODE_RIGHT:
                action.right = 0;
                break;
            case SDL_SCANCODE_Z:
                action.fire = 0;
                break;
            }
            break;
        }
    }

    if (player != NULL) // Check if player has been killed or not
    {
        // Determine velocity of keyboard input
        if (player->reload > 0)
            player->reload--;
        player->x_vel = player->y_vel = 0;
        if (action.up && !action.down)
            player->y_vel = -SPEED; // If up pressed & NOT down, == negative (==up)
        if (action.down && !action.up)
            player->y_vel = SPEED; //(positive == down)
        if (action.left && !action.right)
            player->x_vel = -SPEED;
        if (action.right && !action.left)
            player->x_vel = SPEED;

        // Update positions (divide by 60 as only calculating 1/60th of a second)
        player->x_pos += player->x_vel / 60;
        player->y_pos += player->y_vel / 60;
    }
}

/*Prepare Scene Function*/
void prepareScene()
{
    SDL_SetRenderDrawColor(rend, 96, 128, 255, 255);
    SDL_RenderClear(rend);
}

/**End Function*/
int end()
{
    // Clean up all initialised subsystems:
    SDL_DestroyTexture(player->tex);
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
    exit(0);
}

/**Initialise Stage Function*/
void initStage()
{
    memset(&stage, 0, sizeof(Stage)); // Set stage variables to 0
    stage.playerTail = &stage.playerHead;
    stage.bulletTail = &stage.bulletHead;
    stage.fairyTail = &stage.fairyHead;
    stage.enemyBulletTail = &stage.enemyBulletHead;

    initPlayer();

    fairySpawnTimer = 0;
}

/**Reset Stage Function*/
static void resetStage()
{
    Entity *e;

    while (stage.playerHead.next)
    {
        e = stage.playerHead.next;
        stage.playerHead.next = e->next;
        free(e);
    }

    while (stage.bulletHead.next)
    {
        e = stage.bulletHead.next;
        stage.bulletHead.next = e->next;
        free(e);
    }

    while (stage.fairyHead.next)
    {
        e = stage.fairyHead.next;
        stage.fairyHead.next = e->next;
        free(e);
    }

    while (stage.enemyBulletHead.next)
    {
        e = stage.enemyBulletHead.next;
        stage.enemyBulletHead.next = e->next;
        free(e);
    }

    initStage();
}

/**Iniitialise Player Function*/
static void initPlayer()
{
    player = malloc(sizeof(Entity));
    memset(player, 0, sizeof(Entity)); // Set player variables to 0

    // Player object added to player linked list
    stage.playerTail->next = player;
    stage.playerTail = player;

    player->tex = IMG_LoadTexture(rend, "sprite.png"); // Create texture for player

    // Get & Scale dimensions of texture:
    SDL_QueryTexture(player->tex, NULL, NULL, &player->rect.w, &player->rect.h);
    player->rect.w += 15; // scales image up
    player->rect.h += 15;

    // Hitbox Scaling
    player->hitbox.w = player->rect.w / 3.5;
    player->hitbox.h = player->rect.h / 5;

    // Sprite in centre of screen at start
    player->x_pos = (WINDOW_WIDTH - player->rect.w) / 2;
    player->y_pos = (WINDOW_HEIGHT - player->rect.h) / 2;

    // Initial sprite velocity 0 (because keyboard controls it)
    player->x_vel = 0;
    player->y_vel = 0;
}

/**Initialise / Fire Player Bullet Function*/
static void fireBullet()
{
    Entity *bullet;

    bullet = malloc(sizeof(Entity));
    memset(bullet, 0, sizeof(Entity));

    bullet->tex = IMG_LoadTexture(rend, "bullet.png");
    SDL_QueryTexture(bullet->tex, NULL, NULL, &bullet->rect.w, &bullet->rect.h);
    bullet->rect.w += 15; // Scales image up
    bullet->rect.h += 15;

    stage.bulletTail->next = bullet;
    stage.bulletTail = bullet;

    bullet->x_pos = player->x_pos;
    bullet->y_pos = player->y_pos;
    bullet->x_vel = 0;
    bullet->y_vel = -SPEED - 500;
    bullet->life = 1;

    player->reload = 8;
}

/**Player & Enemy: Bullet Update & Collision Check*/
static void manipulateAllBullets()
{
    Entity *b, *prev;
    prev = &stage.bulletHead;

    Entity *eB, *eBPrev;
    eBPrev = &stage.enemyBulletHead;

    // Player Bullet Manipulation
    for (b = stage.bulletHead.next; b != NULL; b = b->next)
    {
        b->x_pos += b->x_vel / 60;
        b->y_pos += b->y_vel / 60;

        // Set the positions in the struct
        b->rect.y = b->y_pos;
        b->rect.x = b->x_pos;

        // If bullet hits enemy OR goes beyond the top of the screen OR BulletLife = 0
        if (bulletHit(b) || b->y_pos < -10 || b->life == 0)
        {
            if (b == stage.bulletTail)
            {
                stage.bulletTail = prev;
            }
            prev->next = b->next;
            free(b);
            b = prev;
        }
    }

    // Enemy Bullet Manipulation
    for (eB = stage.enemyBulletHead.next; eB != NULL; eB = eB->next)
    {
        eB->x_pos += eB->x_vel / 60;
        eB->y_pos += eB->y_vel / 60;

        // Set the positions in the struct
        eB->rect.y = eB->y_pos;
        eB->rect.x = eB->x_pos;

        // If bullet goes beyond the top of the screen / BulletLife = 0
        if (eB->y_pos < -10 || eB->life == 0)
        {
            if (eB == stage.enemyBulletTail)
            {
                stage.enemyBulletTail = eBPrev;
            }
            eBPrev->next = eB->next;
            free(eB);
            eB = eBPrev;
        }

        // If Enemy bullet HITS Player
        if (SDL_HasIntersection(&eB->rect, &player->hitbox) == SDL_TRUE) // Check if Player Rect & fairy Rect intersect
        {
            // set player to NULL & fairy life to 0 so it despawns
            eB->life = 0;
            player = NULL;
            break;
        }
    }

    // Check if player is NULL
    if (player == NULL)
    {
        resetStage();
    }
}

/**Draw Player Bullets Function*/
static void drawBullets()
{
    for (Entity *b = stage.bulletHead.next; b != NULL; b = b->next)
    {
        SDL_RenderCopy(rend, b->tex, NULL, &b->rect);
    }
}

/**Initialise Fairies Function*/
static void spawnFairies()
{
    Entity *fairy;

    if (--fairySpawnTimer <= 0)
    { // Adds new enemy if timer falls below 1
        fairy = malloc(sizeof(Entity));
        memset(fairy, 0, sizeof(Entity));

        fairy->x_pos = WINDOW_WIDTH;
        fairy->y_pos = rand() % WINDOW_HEIGHT / 2; // Fairies appear top half of window
        fairy->life = 1;
        fairy->x_vel = -140; // Allows fairy to move left and right of screen

        fairy->tex = IMG_LoadTexture(rend, "fairy.png");
        SDL_QueryTexture(fairy->tex, NULL, NULL, &fairy->rect.w, &fairy->rect.h);
        fairy->rect.w += 15; // Scales image up
        fairy->rect.h += 15;

        // Hitbox Scaling
        fairy->hitbox.w = player->rect.w /1.5;
        fairy->hitbox.h = player->rect.h /1.5;

        stage.fairyTail->next = fairy;
        stage.fairyTail = fairy;

        fairySpawnTimer = 20 + (rand() % 20); // timer for random enemy creation

        fairy->reload = 60 * (1 + (rand())); // make sure fairies dont fire instantly when created
    }
}

/**Fairy Update & Collision Check*/
static void manipulateFairy()
{
    Entity *f, *prev;

    prev = &stage.fairyHead;

    for (f = stage.fairyHead.next; f != NULL; f = f->next)
    {
        f->x_pos += f->x_vel / 60;
        f->y_pos += f->y_vel / 60;

        // Set the positions in the struct
        f->rect.y = f->y_pos;
        f->rect.x = f->x_pos;
        f->hitbox.y = f->y_pos;
        f->hitbox.x = f->x_pos;

        if (f->x_pos <= -30 || f->life == 0) // If fairy x_pos is 0, == far left of screen. (remove it from list) && if fairy health is 0 delete it
        {
            if (f == stage.fairyTail)
            {
                stage.fairyTail = prev;
            }
            prev->next = f->next;
            free(f);
            f = prev;
        }
        prev = f;
    }
}

/**Draw Fairy Function*/
static void drawFairy()
{
    Entity *f;

    for (f = stage.fairyHead.next; f != NULL; f = f->next)
    {
        // If its 0 and 0, it will appear in top left corner (perhaps make it so if x&y position is
        // 0 at beginning, make it to -1 and check for -1 OR have a visiblity flag and check if its visible)
        if (f->rect.x == 0 && f->rect.y == 0)
        {
            continue;
        }
        SDL_RenderCopy(rend, f->tex, NULL, &f->rect);
    }
}

/**Check if Bullet intersect / hit Fairy*/
static int bulletHit(Entity *b)
{
    Entity *f;

    for (f = stage.fairyHead.next; f != NULL; f = f->next)
    {
        if (SDL_HasIntersection(&b->rect, &f->hitbox) == SDL_TRUE) // Check if bullet Rect & fairy Rect intersect
        {
            // set bullet & fairy life to 0 so it despawns
            b->life = 0;
            f->life = 0;
            return true;
        }
    }
    return false;
}

/**Check if player collide with fairy*/
static int playerCollide()
{
    Entity *f;
    SDL_Rect *result;

    for (f = stage.fairyHead.next; f != NULL; f = f->next)
    {
        if (SDL_HasIntersection(&player->hitbox, &f->hitbox) == SDL_TRUE) // Check if Player Rect & fairy Rect intersect
        {
            // set player & fairy life to 0 so it despawns
            player = NULL;
            f->life = 0;
            return true;
        }
    }
    return false;
}

/**Calculate fairy attack slope to target player*/
void calcAtkSlope(int x_pos, int y_pos, int x_rect, int y_rect, float *dx, float *dy)
{
    int steps = MAX(abs(x_pos - x_rect), abs(y_pos - y_rect));
    if (steps == 0)
    {
        *dx = *dy = 0;
        return;
    }
    *dx = (x_pos - x_rect);
    *dx /= steps;

    *dy = (y_pos - y_rect);
    *dy /= steps;
}

/**Allows each fairy to fire bullets*/
static void fireEnemyBulletCall()
{
    Entity *f;
    for (f = stage.fairyHead.next; f != NULL; f = f->next)
    {
        if (player != NULL && --f->reload <= 0) // if reload is <=0 and player is alive:
        {
            fireEnemyBullet(f);
        }
    }
}

/**Initialise Enemy Bullet Function*/
static void fireEnemyBullet(Entity *f)
{
    Entity *enemyBullet, *eBPrev;
    eBPrev = &stage.enemyBulletHead;
    // SDL_Rect *result;

    enemyBullet = malloc(sizeof(Entity));
    memset(enemyBullet, 0, sizeof(Entity));

    enemyBullet->tex = IMG_LoadTexture(rend, "enemyBullet.png");
    SDL_QueryTexture(enemyBullet->tex, NULL, NULL, &enemyBullet->rect.w, &enemyBullet->rect.h);
    enemyBullet->rect.w += 5; // Scales image up
    enemyBullet->rect.h += 5;

    stage.enemyBulletTail->next = enemyBullet;
    stage.enemyBulletTail = enemyBullet;

    enemyBullet->x_pos = f->x_pos;
    enemyBullet->y_pos = f->y_pos;
    enemyBullet->life = 1;

    calcAtkSlope(player->x_pos + (player->hitbox.w / 2), player->y_pos + (player->hitbox.h / 2), f->x_pos, f->y_pos, &enemyBullet->x_vel, &enemyBullet->y_vel);

    enemyBullet->x_vel *= SPEED;
    enemyBullet->y_vel *= SPEED;
    f->reload = (rand() % 60 * 2);

    // Set the positions in the struct
    enemyBullet->rect.y = enemyBullet->y_pos;
    enemyBullet->rect.x = enemyBullet->x_pos;
}

static void drawEnemyBullets()
{
    for (Entity *b = stage.enemyBulletHead.next; b != NULL; b = b->next)
    {
        SDL_RenderCopy(rend, b->tex, NULL, &b->rect);
    }
}