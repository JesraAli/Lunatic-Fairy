# SDL2
An in development 2D shooter game, written in C using SDL2. It is based off the bullet-hell video game 'Touhou Project'.

## Building and Running

To compile:

```sh
gcc src/gui.c src/main.c src/highscore.c -o main `sdl2-config --cflags --libs` -lSDL2_image -lm
```

To run:

```sh
./main
```

## Controls:

To control your character, use the ```WASD``` keys to move ```UP```, ```DOWN```, ```LEFT```, ```RIGHT``` respectively, or use the corresponding up/down/left/right keys.

To fire a bullet, press ```Z```.
