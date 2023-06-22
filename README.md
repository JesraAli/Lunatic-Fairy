# SDL2
Starting development of a game using SDL2 in C

## Building and Running

To compile:

```sh
gcc main.c -o main `sdl2-config --cflags --libs` -lSDL2_image -lm
```

To run:

```sh
./main
```
       if(action.fire && bullet.health == 0){
            bullet.x_pos = player.x_pos + 16;
            bullet.y_pos = player.y_pos + 16;
        }
        if(bullet.x_pos > WINDOW_WIDTH){
            bullet.health = 0;
        }

        if(bullet.health > 0){
            blit(bullet.tex, bullet.x_pos, bullet.y_pos);
        }