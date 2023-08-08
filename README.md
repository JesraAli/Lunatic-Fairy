# Lunatic Fairy
Lunatic Fairy is an in development single player 2D shooter game, written in C using Simple DirectMedia Layer 2 (SDL2). 

The game is based off a popular japanese bullet-hell video game.


<img src="https://github.com/JesraAli/Lunatic-Fairy/blob/main/img/trailer.gif" width="630" height="480"/>


## Objective
The objective of the game is for the player to defeat as many fairies as possible to obtain points towards their overall score.

The player has 3 lives, and must dodge the fairies bullets to prevent losing a life.

## Building and Running

To compile:

```sh
gcc src/gui.c src/main.c src/highscoreInfo.c -o main `sdl2-config --cflags --libs` -lSDL2_image -lm
```

To run:

```sh
./main
```

## Controls:

To control your character, use the ```WASD``` keys to move ```UP```, ```DOWN```, ```LEFT```, ```RIGHT``` respectively, or use the corresponding up/down/left/right keys.

To fire a bullet, press ```Z```.
