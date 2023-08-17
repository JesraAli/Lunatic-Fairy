#include "libs.h"
#include "structs.h"
#include "gui.h"
#include "highscoreInfo.h"
#include <enet/enet.h>
#include "client.h"
// #include "server.h"

/*Main Function*/
int main(int argc, char **argv)
{

    // // Initialise ENet
    // if (enet_initialize() != 0)
    // {
    //     fprintf(stderr, "Error initializing ENet.\n");
    //     return 1;
    // }

    // // Initialize server
    // ENetHost *server = initialiseServer();
    // if (server == NULL)
    // {
    //     // Handle the error here, for example, print an error message and exit.
    //     fprintf(stderr, "Failed to initialize the server.\n");
    //     return EXIT_FAILURE;
    // }

    // // Initialize client
    // ENetHost *client;
    // client = enet_host_create(NULL /* create a client host */,
    //                                     10 /* allow 10 outgoing connections */,
    //                                     2 /* allow up 2 channels to be used, 0 and 1 */,
    //                                     0 /* assume any amount of incoming bandwidth */,
    //                                     0 /* assume any amount of outgoing bandwidth */);
    // if (client == NULL)
    // {
    //     fprintf(stderr, "Error occurred while trying to create an ENet client host.\n");
    //     return EXIT_FAILURE;
    // }

    // // Add a short delay (for example, 1 second) before connecting
    // sleep(1);

    // connectToServer(client, "127.0.0.1", 1234);

    if (enet_initialize() != 0)
    {
        fprintf(stderr, "Error initializing ENet.\n");
        return 1;
    }

    // ENetHost *server = initialiseServer();
    //     sleep(10);


    ENetHost *client;
    client = enet_host_create(NULL /* create a client host */,
                              1 /* only allow 1 outgoing connection */,
                              2 /* allow up 2 channels to be used, 0 and 1 */,
                              0 /* assume any amount of incoming bandwidth */,
                              0 /* assume any amount of outgoing bandwidth */);
    if (client == NULL)
    {
        fprintf(stderr, "An error occurred while trying to create an ENet client host.\n");
        return EXIT_FAILURE;
    }

    connectToServer(client, "127.0.0.1", 1234);

    load();
    initStage();
    initHighScoreTable();
    initTitle();

    titleLoop();
    SDL_ShowCursor(0); // Hide cursor

    while (true)
    {

        // ENetEvent event;

        // // Handle events for both server and client
        // while (enet_host_service(server, &event, 0) > 0 || enet_host_service(client, &event, 0) > 0)
        // {
        //     if (event.type == ENET_EVENT_TYPE_CONNECT)
        //     {
        //         // Handle the new client connection
        //         printf("A new client connected from %x:%u.\n",
        //                event.peer->address.host,
        //                event.peer->address.port);

        //         event.peer->data = "Client information"; // Store any relevant client information here.
        //     }
        // }

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

    enet_host_destroy(client); // Cleanup
    enet_deinitialize();
    end();
}