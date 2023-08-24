#include "libs.h"
#include "structs.h"
#include "gui.h"
#include "highscoreInfo.h"
#include <enet/enet.h>
#include "client.h"
#include <pthread.h>
#include "server.h" // Include the modified header

void multiplayerCheck();

pthread_t serverThread;
pthread_t clientThread;
int serverPort;

// Define the structure to pass arguments to the server thread
struct ServerThreadArgs
{
    ENetHost *server;
    int serverPort;
};

void *serverThreadFunction(void *arg)
{
    int serverPort = *((int *)arg); // Get the server port from the argument

    runServer(serverPort);
    return NULL;
}

void *clientThreadFunction(void *arg)
{
    int serverPort = *((int *)arg); // Get the server port from the argument

    runClient(serverPort);
    return NULL;
}

/*Main Function*/
int main(int argc, char **argv)
{

    char *serverAddress = NULL;
    serverPort = 0;

    // Parse command-line arguments
    if (argc < 4)
    {
        printf("Usage: %s -server <server_ip> -port <server_port>\n", argv[0]);
        return 1;
    }
    for (int i = 1; i < argc - 1; i += 2)
    {
        if (strcmp(argv[i], "-server") == 0)
        {
            serverAddress = argv[i + 1];
        }
        else if (strcmp(argv[i], "-port") == 0)
        {
            serverPort = atoi(argv[i + 1]);
        }
        else
        {
            printf("Unknown option: %s\n", argv[i]);
            return 1;
        }
    }
    if (serverAddress == NULL || serverPort == 0)
    {
        printf("Missing or invalid arguments. Usage: %s -server <server_ip> -port <server_port>\n", argv[0]);
        return 1;
    }

    load();
    initStage();
    initHighScoreTable();
    initTitle();

    titleLoop();
    SDL_ShowCursor(0); // Hide cursor
    initPlayers();

    while (true)
    {
        if (returnPlayerLife() == 0)
        {
            addHighscore(returnPlayerScore(), returnHighscoreList());
            drawHighscores(returnHighscoreList());
            presentScene();
            restartGame(); // Restart the game
            multiplayerCheck();
        }

        prepareScene(); // Prepare Scene (Background & Clear)

        userInput();

        rendCopyBackground();

        // Collision detected with bounds (detect if sprite is going out of  window)
        collisionDetection();

        manipulateAllBullets();

        // Check if player is NULL, then repeat the while loop from beginning (avoids repetition of playerCollide() call)
        if (playerNullCheck())
        {
            if (returnMultiplayerStatus() == true)
            {
                resetPlayer();
                initPlayers();
            }
            else
            {
                resetStage();
                initPlayers();
            }
            continue;
        }

        manipulateDBullet();

        manipulateFairy();
        fireEnemyBulletCall();

        spawnFairies('L');
        spawnFairies('R');

        manipulateExplosion();
        manipulatePowerUp();
        playerCollidePowerUp();
        playerCollideFairy();

        if (playerNullCheck())
        {
            if (returnMultiplayerStatus() == true)
            {
                resetPlayer();
                initPlayers();
            }
            else
            {
                resetStage();
                initPlayers();
            }
            continue;
        }
        // // Create & send packets for all entities
        if (returnMultiplayerStatus() == true)
        {
            playerPackets();
        }

        // Present Scene: draw the image to the window
        // if (player->rect.x == 0 && player->rect.y == 0) // check if its 0 (in top left corner) //NEED TO CHANGE AT LATER DATE
        // {
        //     continue;
        // }

        rendCopyPlayer();

        if (returnMultiplayerStatus() == true)
        {
            rendCopyPlayer2();
        }

        // Drawing:

        drawDBullets();
        drawBullets();
        drawOpponentBullets();

        drawEnemyBullets();
        drawFairy();
        drawEnemyExplosion();
        drawPowerUp();
        drawStats(returnHighscoreList());
        presentScene();
        SDL_Delay(1000 / 60); // Wait 1/60th of a second
    }

    if (returnMultiplayerStatus() == true)
    {
        pthread_join(serverThread, NULL); // Join server thread
        pthread_join(clientThread, NULL);
        enet_deinitialize();
    }
    end();
}

void multiplayerCheck()
{
    bool multiplayer = returnMultiplayerStatus();

    if (multiplayer == true)
    {
        printf("Multiplayer Mode: true\n");
        // Create Server Thread
        if (pthread_create(&serverThread, NULL, serverThreadFunction, &serverPort) != 0)
        {
            printf("Failed to create server thread.\n");
            return;
        }

        // Create Client Thread
        if (pthread_create(&clientThread, NULL, clientThreadFunction, &serverPort) != 0)
        {
            printf("Failed to create client thread.\n");
            return;
        }

        SDL_Delay(10); // Delay so the server and client can initialise
    }
    else
    {
        printf("Multiplayer Mode: false\n");
    }
}