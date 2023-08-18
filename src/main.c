#include "libs.h"
#include "structs.h"
#include "gui.h"
#include "highscoreInfo.h"
#include <enet/enet.h>
#include "client.h"
#include <pthread.h>
#include "server.h" // Include the modified header


// #include "server.h"

// Define the structure to pass arguments to the server thread
struct ServerThreadArgs {
    ENetHost *server;
    // ENetEvent *eventVar;
};


// Function to run the server in a separate thread
// void *serverThreadFunction(void *arg) {
//     struct ServerThreadArgs *args = (struct ServerThreadArgs *)arg;
//     runServer(args->server);
//     return NULL;
// }


void *serverThreadFunction(void *arg) {
    runServer(); // Call the refactored server code
    return NULL;
}


/*Main Function*/
int main(int argc, char **argv)
{

    // // Initialize ENet for server
    // if (enet_initialize() != 0)
    // {
    //     printf("An error occurred while initializing ENet for server.\n");
    //     return 1;
    // }
    // atexit(enet_deinitialize);

    // ENetHost *server = initServer();

    // // Create a thread for the server
    // pthread_t serverThread;
    // // ENetEvent eventS; // Initialise ENetEvent variable
    // struct ServerThreadArgs args;
    // args.server = server;
    // // args.eventVar = &eventS; // Pass a pointer to the event

    // if (pthread_create(&serverThread, NULL, serverThreadFunction, &args) != 0) {
    //     printf("Failed to create server thread.\n");
    //     return 1;
    // }

    pthread_t serverThread;
    if (pthread_create(&serverThread, NULL, serverThreadFunction, NULL) != 0) {
        printf("Failed to create server thread.\n");
        return 1;
    }


    // Initialize ENet for Client Host & Connection
    if (enet_initialize() != 0)
    {
        printf("An error occurred while initializing ENet for client.\n");
        return 1;
    }
    atexit(enet_deinitialize);
    ENetEvent eventC;

    ENetHost *client = enet_host_create(NULL /* create a client host */,
                                        1 /* only allow 1 outgoing connection */,
                                        2 /* allow up 2 channels to be used, 0 and 1 */,
                                        0 /* assume any amount of incoming bandwidth */,
                                        0 /* assume any amount of outgoing bandwidth */);

    if (client == NULL)
    {
        printf("An error occurred while trying to create an ENet client host.\n");
        return 1;
    }
    connectToServer(client, &eventC);

    load();
    initStage();
    initHighScoreTable();
    initTitle();

    titleLoop();
    SDL_ShowCursor(0); // Hide cursor

    while (true)
    {

        if (returnPlayerLife() == 0)
        {
            addHighscore(returnPlayerScore(), returnHighscoreList());
            drawHighscores(returnHighscoreList());
            presentScene();
            restartGame(); // Restart the game
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
            resetStage();
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

        // Create & send packets for all entities
        // bulletPackets(server);

        if (playerNullCheck())
        {
            resetStage();
            continue;
        }
        // Present Scene: draw the image to the window
        // if (player->rect.x == 0 && player->rect.y == 0) // check if its 0 (in top left corner) //NEED TO CHANGE AT LATER DATE
        // {
        //     continue;
        // }

        rendCopyPlayer();

        // Drawing:
        drawDBullets();

        drawBullets();
        drawEnemyBullets();
        drawFairy();
        drawEnemyExplosion();
        drawPowerUp();
        drawStats(returnHighscoreList());
        presentScene();
        SDL_Delay(1000 / 60); // Wait 1/60th of a second
    }

    // Join the server thread after the client loop is done
    // pthread_join(serverThread, NULL);
        pthread_join(serverThread, NULL);

    enet_host_destroy(client); // Cleanup
    // cleanupServer(server);
    enet_deinitialize();
    end();
}