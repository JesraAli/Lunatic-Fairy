#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define WINDOW_WIDTH (640)
#define WINDOW_HEIGHT (480)
#define SCROLL_SPEED (300) //Speed in pixels per second
#define SPEED (300)

//Structs
typedef struct{
    int up;
    int down;
    int left;
    int right;
    int fire;
    
} Action;

typedef struct{
    int x_pos;
    int y_pos;
    float x_vel;
    float y_vel;
    int life;
    int reload;
    SDL_Texture *tex;
    SDL_Rect dest;
    struct Entity *next; //For next Entity in linked list
} Entity;


typedef struct {  //Linked lists for players & bullets
    Entity playerHead, *playerTail;
    Entity bulletHead, *bulletTail;
}Stage;

//Function Declarations
int load(void);
void doInput(void);
void prepareScene(void);
int end(void);
void initStage(void);
static void initPlayer(void);
static void fireBullet(void);
static void manipulateBullets(void);
static void drawBullets(void);


//Variables
SDL_Window *win;
SDL_Renderer *rend;
SDL_Surface *surface;

Entity *player;
// *bullet;
Action action;
Stage stage;



int load(){

    //Initialise SDL Subsystems
    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0){

        printf("Error in intialising SDL: %s\n", SDL_GetError());
        return 1;
    }

    //Create Window
    win = SDL_CreateWindow(
        "Game",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,WINDOW_HEIGHT,0
    );       
    if (!win){  //Check if error when creating window:
        printf("Error creating window: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    } 

    //Creating renderer
    Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    rend = SDL_CreateRenderer(win, -1, render_flags);
    if (!rend){
        printf("Error creating renderer: %s\n",SDL_GetError());
        SDL_DestroyWindow(win);
        SDL_Quit;
        return 1;
    }        
}


int main(int argc, char **argv){
    
    load();
    initStage();

    action.up=action.down=action.left=action.right=0;
     
    //bullet 
    // bullet->tex = IMG_LoadTexture(rend,"bullet->png");
    // SDL_QueryTexture(bullet->tex, NULL, NULL, &bullet->dest.w, &bullet->dest.h);
    // bullet->dest.w += 15; //scales image up
    // bullet->dest.h += 15;

    // //Sprite in centre of screen at start
    //  bullet->x_pos = (WINDOW_WIDTH - bullet->dest.w) / 2;
    //  bullet->y_pos = (WINDOW_HEIGHT - bullet->dest.h) / 2;

    // //Initial sprite velocity 0 (because keyboard controls it)
    //  bullet->x_vel = 0; 
    //  bullet->y_vel = 0;


    while (1){


        prepareScene(); //Prepare Scene (Background & Clear)

        doInput();
        
        //collision detected with bounds (detect if sprite is going out of  window)
        if (player->x_pos <= 0) player->x_pos = 0; //reset positions to keep in window
        if (player->y_pos <= 0) player->y_pos = 0;
        if (player->x_pos >= WINDOW_WIDTH - player->dest.w) player->x_pos = WINDOW_WIDTH - player->dest.w;
        if (player->y_pos >= WINDOW_HEIGHT - player->dest.h) player->y_pos = WINDOW_HEIGHT - player->dest.h;


        // set the positions in the struct
        player->dest.y = (int) player->y_pos;
        player->dest.x = (int) player->x_pos;

        //printf("player->reload is: %d\n", player->reload);

        if(action.fire && player->reload == 0){ //when bullet life is 0, decrease SPEED (because we want it going UP The screen)
            fireBullet();
        }

        manipulateBullets();

        // Present Scene: draw the image to the window
        SDL_RenderCopy(rend, player->tex, NULL, &player->dest);

        drawBullets();

        SDL_RenderPresent(rend);

        SDL_Delay(1000/60); // wait 1/60th of a second

    }

    end();
}


void doInput(){
    SDL_Event event; 
    while(SDL_PollEvent(&event)){

        switch(event.type)
        {
        case SDL_QUIT:
            end();
            break;
        
        case SDL_KEYDOWN: //meaning we've PRESSED The button on a keyboard
        //gets key event structure, goes to its field 'keysym', then to 'scancode', retrieves the key pressed
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

    //determine velocity of keyboard input

    if(player->reload > 0) player->reload--;
    player->x_vel = player->y_vel = 0;
    if(action.up && !action.down) player->y_vel = -SPEED; //if up pressed & NOT down, == negative (==up)
    if (action.down && !action.up) player->y_vel = SPEED; //(positive == down)
    if(action.left && !action.right) player->x_vel = -SPEED;
    if (action.right && !action.left) player->x_vel = SPEED;

    //update positions (divide by 60 as only  calculating 1/60th of a second)
    player->x_pos += player->x_vel / 60;
    player->y_pos += player->y_vel / 60;
}

void prepareScene(){
        SDL_SetRenderDrawColor(rend, 96, 128, 255, 255);
        SDL_RenderClear(rend);
}

int end(){
    //Clean up all initialised subsystems:
    SDL_DestroyTexture(player->tex);
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}


void initStage(){

    memset(&stage, 0, sizeof(Stage)); //Set stage variables to 0
    stage.playerTail = &stage.playerHead;
    stage.bulletTail = &stage.bulletHead;

    initPlayer();
}


static void initPlayer(){

    player = malloc(sizeof(Entity));
    memset(player, 0, sizeof(Entity)); //set player variables to 0

    //Player object added to player linked list
    stage.playerTail->next = player;
    stage.playerTail = player;

    player->tex = IMG_LoadTexture(rend,"sprite.png"); //Create texture for player
    // //Get & Scale dimensions of texture:
    SDL_QueryTexture(player->tex, NULL, NULL, &player->dest.w, &player->dest.h);
    player->dest.w += 15; //scales image up
    player->dest.h += 15;

    //Sprite in centre of screen at start
    player->x_pos = (WINDOW_WIDTH - player->dest.w) / 2;
    player->y_pos = (WINDOW_HEIGHT - player->dest.h) / 2;

    //Initial sprite velocity 0 (because keyboard controls it)
     player->x_vel = 0; 
     player->y_vel = 0;

}


static void fireBullet(){


    Entity *bullet;

    bullet = malloc(sizeof(Entity));
    memset(bullet, 0, sizeof(Entity));

    //bullet 
    bullet->tex = IMG_LoadTexture(rend,"bullet.png");
    SDL_QueryTexture(bullet->tex, NULL, NULL, &bullet->dest.w, &bullet->dest.h);
    bullet->dest.w += 15; //scales image up
    bullet->dest.h += 15;

    // //Sprite in centre of screen at start
    //  bullet->x_pos = (WINDOW_WIDTH - bullet->dest.w) / 2;
    //  bullet->y_pos = (WINDOW_HEIGHT - bullet->dest.h) / 2;

    // //Initial sprite velocity 0 (because keyboard controls it)
    //  bullet->x_vel = 0; 
    //  bullet->y_vel = 0;

    stage.bulletTail->next = bullet;
    stage.bulletTail = bullet;


    bullet->x_pos = player->x_pos;
    bullet->y_pos = player->y_pos;
    bullet->x_vel = 0; 
    bullet->y_vel = -SPEED;

    bullet->life = 1;
    
    player->reload = 8;

}


static void manipulateBullets(){

    Entity *b, *prev;
    prev = &stage.bulletHead;
    

   for (b = stage.bulletHead.next ; b!=NULL ; b = b->next) //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    {

        b->x_pos += b->x_vel / 60;
        b->y_pos += b->y_vel / 60;

        // set the positions in the struct
        b->dest.y = (int) b->y_pos;
        b->dest.x = (int) b->x_pos;

        //printf("bullet position is: %d\n", bullet->y_pos);

        if(b->y_pos < -10){ //If bullet goes beyond the top of the screen

            if(b == stage.bulletTail){
                stage.bulletTail = prev;
            }
            prev->next = b->next;
            free(b);
            b = prev;        
        }
    }

}


static void drawBullets(){

    Entity *b;

    for(b = stage.bulletHead.next ; b!=NULL; b=b->next){
        SDL_RenderCopy(rend, b->tex, NULL, &b->dest);
    }
}
