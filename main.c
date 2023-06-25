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
    int health;
    SDL_Texture *tex;
    SDL_Rect dest;
} Entity;

//Function Declarations
int load(void);
void doInput(void);
void prepareScene(void);
int end(void);
 

//Variables
SDL_Window *win;
SDL_Renderer *rend;
SDL_Surface *surface;

Entity player,bullet;
Action action;


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

    //initSprite(player, "sprite.png", 15);

    action.up=action.down=action.left=action.right=0;


    //player
    player.tex = IMG_LoadTexture(rend,"sprite.png"); //Create texture for player
    // //Get & Scale dimensions of texture:
    SDL_QueryTexture(player.tex, NULL, NULL, &player.dest.w, &player.dest.h);
    player.dest.w += 15; //scales image up
    player.dest.h += 15;

    //Sprite in centre of screen at start
     player.x_pos = (WINDOW_WIDTH - player.dest.w) / 2;
     player.y_pos = (WINDOW_HEIGHT - player.dest.h) / 2;

    //Initial sprite velocity 0 (because keyboard controls it)
     player.x_vel = 0; 
     player.y_vel = 0;
     

    //bullet 
    bullet.tex = IMG_LoadTexture(rend,"bullet.png");
    SDL_QueryTexture(bullet.tex, NULL, NULL, &bullet.dest.w, &bullet.dest.h);
    bullet.dest.w += 15; //scales image up
    bullet.dest.h += 15;

    //Sprite in centre of screen at start
     bullet.x_pos = (WINDOW_WIDTH - bullet.dest.w) / 2;
     bullet.y_pos = (WINDOW_HEIGHT - bullet.dest.h) / 2;

    //Initial sprite velocity 0 (because keyboard controls it)
     bullet.x_vel = 0; 
     bullet.y_vel = 0;


    while (1){

        prepareScene(); //Prepare Scene (Background & Clear)

        doInput();
        
        //collision detected with bounds (detect if sprite is going out of  window)
        if (player.x_pos <= 0) player.x_pos = 0; //reset positions to keep in window
        if (player.y_pos <= 0) player.y_pos = 0;
        if (player.x_pos >= WINDOW_WIDTH - player.dest.w) player.x_pos = WINDOW_WIDTH - player.dest.w;
        if (player.y_pos >= WINDOW_HEIGHT - player.dest.h) player.y_pos = WINDOW_HEIGHT - player.dest.h;


        // set the positions in the struct
        player.dest.y = (int) player.y_pos;
        player.dest.x = (int) player.x_pos;


        if(action.fire && bullet.health == 0){ //when bullet health is 0, decrease SPEED (because we want it going UP The screen)
            bullet.x_pos = player.x_pos;
            bullet.y_pos = player.y_pos;
            bullet.x_vel = 0; 
            bullet.y_vel = -SPEED;
   
            bullet.health = 1;
        }

        bullet.x_pos += bullet.x_vel / 60;
        bullet.y_pos += bullet.y_vel / 60;

        // set the positions in the struct
        bullet.dest.y = (int) bullet.y_pos;
        bullet.dest.x = (int) bullet.x_pos;

        //printf("bullet position is: %d\n", bullet.y_pos);

        if(bullet.y_pos < -10){ //If bullet goes beyond the top of the screen!
            bullet.health = 0; //reset bullet health to 0
        }

        
        // Present Scene: draw the image to the window
        SDL_RenderCopy(rend, player.tex, NULL, &player.dest);

        if(bullet.health > 0){
            SDL_RenderCopy(rend, bullet.tex, NULL, &bullet.dest);
        }

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
    player.x_vel = player.y_vel = 0;
    if(action.up && !action.down) player.y_vel = -SPEED; //if up pressed & NOT down, == negative (==up)
    if (action.down && !action.up) player.y_vel = SPEED; //(positive == down)
    if(action.left && !action.right) player.x_vel = -SPEED;
    if (action.right && !action.left) player.x_vel = SPEED;

    //update positions (divide by 60 as only  calculating 1/60th of a second)
    player.x_pos += player.x_vel / 60;
    player.y_pos += player.y_vel / 60;
}

void prepareScene(){
        SDL_SetRenderDrawColor(rend, 96, 128, 255, 255);
        SDL_RenderClear(rend);
}

int end(){
    //Clean up all initialised subsystems:
    SDL_DestroyTexture(player.tex);
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}


// void initSprite(Entity sprite, char *filename, int scale){

//     sprite.tex = IMG_LoadTexture(rend,*filename); //Create texture for player

//     // //Get & Scale dimensions of texture:
//     SDL_QueryTexture(sprite.tex, NULL, NULL, &sprite.dest.w, &sprite.dest.h);
//     sprite.dest.w += scale; //scales image up
//     sprite.dest.h += scale;

//     //Sprite in centre of screen at start
//      sprite.x_pos = (WINDOW_WIDTH - sprite.dest.w) / 2;
//      sprite.y_pos = (WINDOW_HEIGHT - sprite.dest.h) / 2;

//     //Initial sprite velocity 0 (because keyboard controls it)
//      sprite.x_vel = 0; 
//      sprite.y_vel = 0;
// }