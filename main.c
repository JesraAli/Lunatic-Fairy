/*gcc main.c -o main `sdl2-config --cflags --libs` -lSDL2_image -lm*/

//Scancodes: Suited for layout-depended WASD/ARROW keys
//Keycodes: Suited for character-dependent controlls (e.g: I for inventory)

#include "SDL2/SDL.h"
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define WINDOW_WIDTH (640)
#define WINDOW_HEIGHT (480)
#define SCROLL_SPEED (300) //Speed in pixels per second
#define SPEED (300)

//Variables
SDL_Window *win;
SDL_Renderer *rend;
SDL_Surface *surface;
SDL_Texture *tex;

int up, down, left, right;
float x_vel, y_vel, x_pos, y_pos;

void load(){

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

    //Load image into memory
    surface = IMG_Load("sprite.png");
    if(!surface){
        printf("Error creating surface\n");
        SDL_DestroyRenderer(rend);
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }
    

    //Load image data into graphics hardware memory
    tex = SDL_CreateTextureFromSurface(rend, surface);
    SDL_FreeSurface(surface);
    if(!tex){
        printf("Error creating texture: %s\n",SDL_GetError());
        SDL_DestroyRenderer(rend);
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }
}


int main(int argc, char **argv){
    load();

    SDL_Rect dest;  //Struct: holds position, sprite size & its destination on screen
    up=down=left=right=0;

    // //Get & Scale dimensions of texture:
    SDL_QueryTexture(tex, NULL, NULL, &dest.w, &dest.h);
    dest.w /= 6; //scales image down by 4
    dest.h /= 6;

    //Sprite in centre of screen at start
     x_pos = (WINDOW_WIDTH - dest.w) / 2;
     y_pos = (WINDOW_HEIGHT - dest.h) / 2;
     
    //Initial sprite velocity 0 (because keyboard controls it)
     x_vel = 0; 
     y_vel = 0;

    while (1){

        prepareScene(); //Prepare Scene (Background & Clear)

        doInput();
        
        //collision detected with bounds (detect if sprite is going out of  window)
        if (x_pos <= 0) x_pos = 0; //reset positions to keep in window
        if (y_pos <= 0) y_pos = 0;
        if (x_pos >= WINDOW_WIDTH - dest.w) x_pos = WINDOW_WIDTH - dest.w;
        if (y_pos >= WINDOW_HEIGHT - dest.h) y_pos = WINDOW_HEIGHT - dest.h;


        // set the positions in the struct
        dest.y = (int) y_pos;
        dest.x = (int) x_pos;

        
        // Present Scene: draw the image to the window
        SDL_RenderCopy(rend, tex, NULL, &dest);
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
                up = 1;
                break;
            case SDL_SCANCODE_A:
            case SDL_SCANCODE_LEFT:
                left = 1;
                break;
            case SDL_SCANCODE_S:
            case SDL_SCANCODE_DOWN:
                down = 1;
                break;
            case SDL_SCANCODE_D:
            case SDL_SCANCODE_RIGHT:
                right = 1;
                break;
            }
            break;
        case SDL_KEYUP:
            switch (event.key.keysym.scancode)
            {
            case SDL_SCANCODE_W:
            case SDL_SCANCODE_UP:
                up = 0;
                break;
            case SDL_SCANCODE_A:
            case SDL_SCANCODE_LEFT:
                left = 0;
                break;
            case SDL_SCANCODE_S:
            case SDL_SCANCODE_DOWN:
                down = 0;
                break;
            case SDL_SCANCODE_D:
            case SDL_SCANCODE_RIGHT:
                right = 0;
                break;
            }
            break;
        }
    }
    //determine velocity of keyboard input
    x_vel = y_vel = 0;
    if(up && !down) y_vel = -SPEED; //if up pressed & NOT down, == negative (==up)
    if (down && !up) y_vel = SPEED; //(positive == down)
    if(left && !right) x_vel = -SPEED;
    if (right && !left) x_vel = SPEED;

    //update positions (divide by 60 as only  calculating 1/60th of a second)
    x_pos += x_vel / 60;
    y_pos += y_vel / 60;

}

void prepareScene(){
        SDL_SetRenderDrawColor(rend, 96, 128, 255, 255);
        SDL_RenderClear(rend);
}

void end(){
    //Clean up all initialised subsystems:
    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
