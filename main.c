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
    SDL_Rect dest;
    struct Entity *next; // For next Entity in linked list
} Entity;

typedef struct
{ // Linked lists for players & bullets
    Entity playerHead, *playerTail;
    Entity bulletHead, *bulletTail;
    Entity fairyHead, *fairyTail;
} Stage;

// Function Declarations
int load(void);
void doInput(void);
void prepareScene(void);
int end(void);
void initStage(void);
static void initPlayer(void);
static void fireBullet(void);
static void manipulateBullets(void);
static void drawBullets(void);
static void spawnFairies(void);
static void manipulateFairy(void);
static void drawFairy(void);
static int bulletHit(Entity *);
static int playerCollide();

// Variables
SDL_Window *win;
SDL_Renderer *rend;
SDL_Surface *surface;

Entity *player;
Action action;
Stage stage;

int fairySpawnTimer;

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

int main(int argc, char **argv)
{

    load();
    initStage();

    memset(&action, 0, sizeof(Action)); // Set action variables to 0

    while (true)
    {
        prepareScene(); // Prepare Scene (Background & Clear)

        doInput();

        // Collision detected with bounds (detect if sprite is going out of  window)
        if (player->x_pos <= 0)
            player->x_pos = 0; // Reset positions to keep in window
        if (player->y_pos <= 0)
            player->y_pos = 0;
        if (player->x_pos >= WINDOW_WIDTH - player->dest.w)
            player->x_pos = WINDOW_WIDTH - player->dest.w;
        if (player->y_pos >= WINDOW_HEIGHT - player->dest.h)
            player->y_pos = WINDOW_HEIGHT - player->dest.h;

        // Set the positions in the struct
        player->dest.y = player->y_pos;
        player->dest.x = player->x_pos;

        if (action.fire && player->reload == 0)
        {
            fireBullet();
        }

        manipulateBullets();
        manipulateFairy();
        spawnFairies();

        // Present Scene: draw the image to the window
        SDL_RenderCopy(rend, player->tex, NULL, &player->dest);

        drawBullets();
        drawFairy();

        SDL_RenderPresent(rend);

        SDL_Delay(1000 / 60); // Wait 1/60th of a second
    }

    end();
}

void doInput()
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

void prepareScene()
{
    //SDL_SetRenderDrawColor(rend, 96, 128, 255, 255);
    SDL_SetRenderCopy(rend, "background.png",NULL,NULL);
    SDL_RenderPresent(rend);
    //SDL_RenderClear(rend);
}

int end()
{
    // Clean up all initialised subsystems:
    SDL_DestroyTexture(player->tex);
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
    exit(0);
}

void initStage()
{

    memset(&stage, 0, sizeof(Stage)); // Set stage variables to 0
    stage.playerTail = &stage.playerHead;
    stage.bulletTail = &stage.bulletHead;
    stage.fairyTail = &stage.fairyHead;

    initPlayer();

    fairySpawnTimer = 0;
}

static void initPlayer()
{
    player = malloc(sizeof(Entity));
    memset(player, 0, sizeof(Entity)); // Set player variables to 0

    // Player object added to player linked list
    stage.playerTail->next = player;
    stage.playerTail = player;

    player->tex = IMG_LoadTexture(rend, "sprite.png"); // Create texture for player

    // Get & Scale dimensions of texture:
    SDL_QueryTexture(player->tex, NULL, NULL, &player->dest.w, &player->dest.h);
    player->dest.w += 15; // scales image up
    player->dest.h += 15;

    // Sprite in centre of screen at start
    player->x_pos = (WINDOW_WIDTH - player->dest.w) / 2;
    player->y_pos = (WINDOW_HEIGHT - player->dest.h) / 2;

    // Initial sprite velocity 0 (because keyboard controls it)
    player->x_vel = 0;
    player->y_vel = 0;
}

static void fireBullet()
{
    Entity *bullet;

    bullet = malloc(sizeof(Entity));
    memset(bullet, 0, sizeof(Entity));

    bullet->tex = IMG_LoadTexture(rend, "bullet.png");
    SDL_QueryTexture(bullet->tex, NULL, NULL, &bullet->dest.w, &bullet->dest.h);
    bullet->dest.w += 15; // Scales image up
    bullet->dest.h += 15;

    stage.bulletTail->next = bullet;
    stage.bulletTail = bullet;

    bullet->x_pos = player->x_pos;
    bullet->y_pos = player->y_pos;
    bullet->x_vel = 0;
    bullet->y_vel = -SPEED - 500;
    bullet->life = 1;

    player->reload = 8;
}

static void manipulateBullets()
{
    Entity *b, *prev;
    prev = &stage.bulletHead;

    for (b = stage.bulletHead.next; b != NULL; b = b->next)
    {
        b->x_pos += b->x_vel / 60;
        b->y_pos += b->y_vel / 60;

        // Set the positions in the struct
        b->dest.y = b->y_pos;
        b->dest.x = b->x_pos;

        // If bullet hits enemy OR goes beyond the top of the screen
        if (bulletHit(b) || b->y_pos < -10)
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
}

static void drawBullets()
{
    for (Entity *b = stage.bulletHead.next; b != NULL; b = b->next)
    {
        SDL_RenderCopy(rend, b->tex, NULL, &b->dest);
    }
}

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
        SDL_QueryTexture(fairy->tex, NULL, NULL, &fairy->dest.w, &fairy->dest.h);
        fairy->dest.w += 15; // Scales image up
        fairy->dest.h += 15;

        stage.fairyTail->next = fairy;
        stage.fairyTail = fairy;

        fairySpawnTimer = 30 + (rand() % 60); // ttimer for random enemy creation
    }
}

static void manipulateFairy()
{
    Entity *f, *prev;

    prev = &stage.fairyHead;

    for (f = stage.fairyHead.next; f != NULL; f = f->next)
    {
        f->x_pos += f->x_vel / 60;
        f->y_pos += f->y_vel / 60;

        // Set the positions in the struct
        f->dest.y = f->y_pos;
        f->dest.x = f->x_pos;

        // printf("PlayerCollide value: %d\n",playerCollide(player));
        // if(f->x_pos <= -30 || f->life ==0 || playerCollide()==true ) //If fairy x_pos is 0, == far left of screen. (remove it from list)
        if (f->x_pos <= -30 || f->life == 0) // If fairy x_pos is 0, == far left of screen. (remove it from list)
                                             // if fairy health is 0 delete it
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

static void drawFairy()
{
    Entity *f;

    for (f = stage.fairyHead.next; f != NULL; f = f->next)
    {
        SDL_RenderCopy(rend, f->tex, NULL, &f->dest);
    }
}

static int bulletHit(Entity *b)
{
    Entity *f;
    SDL_Rect *result;

    for (f = stage.fairyHead.next; f != NULL; f = f->next)
    {
        if (SDL_IntersectRect(&b->dest, &f->dest, result) == SDL_TRUE)
        { // Check if bullet Rect & fairy Rect intersect
            // set bullet & fairy life to 0 so it despawns
            b->life = 0;
            f->life = 0;

            return true;
        }
    }
    return false;
}

// static int playerCollide()
// { // If player walks into enemy:
//     Entity *f;
//     SDL_Rect *result;

//     for (f = stage.fairyHead.next; f != NULL; f = f->next)
//     {
//         if (SDL_IntersectRect(&player->dest, &f->dest, result) == SDL_TRUE)
//         { // Check if player Rect & fairy Rect intersect
//             // reset player position, delete fairy
//             player->x_pos = (WINDOW_WIDTH - player->dest.w) / 2;
//             player->y_pos = (WINDOW_HEIGHT - player->dest.h) / 2;

//             // Initial sprite velocity 0 (because keyboard controls it)
//             player->x_vel = 0;
//             player->y_vel = 0;
//             f->life = 0;
//             return 1;
//         }
//     }
//     return 0;
// }