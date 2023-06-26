# SDL2
A 2D shooter game, using SDL2 in C

## Building and Running

To compile:

```sh
gcc main.c -o main `sdl2-config --cflags --libs` -lSDL2_image -lm
```

To run:

```sh
./main
```