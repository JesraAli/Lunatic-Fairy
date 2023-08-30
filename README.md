# Lunatic Fairy
Lunatic Fairy is an in development 2D shooter game, written in C using Simple DirectMedia Layer 2 (SDL2) and ENet libraries. 

It features a single-player mode and a two-player mode using a server-client connection. 

The game is inspired by a popular japanese bullet-hell video game.


<img src="https://github.com/JesraAli/Lunatic-Fairy/blob/main/gif/trailer.gif" width="630" height="480"/>


## Objective
The objective of the game is for the player to defeat as many fairies as possible to obtain points towards their overall score.

The player has 3 lives, and must dodge the fairies bullets to prevent losing a life.

## Building and Running

### Linux:

To download:



To run:

```sh
./main -server <serverAddress> -port <portNumber>
```

Example of running the program:
```sh
./main -server 192.124.1.332 -port 2000
```

## Controls:

To control your character, use the ```WASD``` keys to move ```UP```, ```DOWN```, ```LEFT```, ```RIGHT``` respectively, or use the corresponding up/down/left/right keys.

To fire a bullet, press ```Z```.
