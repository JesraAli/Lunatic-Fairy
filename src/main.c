#include "libs.h"
#include "structs.h"
#include "gui.h"
#include "highscoreInfo.h"
#include <enet/enet.h>
#include "client.h"
#include "server.h"
#include "main.h"

#ifdef _WIN32
#include <windows.h>
#include <winsock2.h>
#pragma comment (lib,"ws2_32.lib")
#else
#include <pthread.h>
#endif

#define PLAYER_CHANNEL 1

void multiplayerCheck();

#ifdef _WIN32
DWORD WINAPI WserverThreadFunction(LPVOID lpParam);
DWORD WINAPI WclientThreadFunction(LPVOID lpParam);
#endif

#ifdef _WIN32
HANDLE WserverThread; // HANDLE: represents an object (e.g threads), like a pointer to the object
HANDLE WclientThread;
#else
pthread_t serverThread;
pthread_t clientThread;
#endif
int serverPort;
bool secondPlayerDead;
bool hostPlayerDead;
bool noLivesFlagCalled;

struct ServerThreadArgs
{
    ENetHost *server;
    int serverPort;
};

#ifdef _WIN32
// DWORD: Data type representing 32-bit unsigned integer
// WINAPI: Specifies how function parameters are passed & how function return value is handled
// LPVOID: (Long Pointer to VOID), represents pointer to memory of ANY data type
DWORD WINAPI WserverThreadFunction(LPVOID lpParam)
{
    int serverPort = *((int *)lpParam); // Retrieve server port from the argument

    runServer(serverPort);
    return 0;
}

DWORD WINAPI WclientThreadFunction(LPVOID lpParam)
{
    int serverPort = *((int *)lpParam); // Retrieve server port from the argument

    runClient(serverPort);
    return 0;
}
#else

void *serverThreadFunction(void *arg)
{
    int serverPort = *((int *)arg); // Retrieve server port from the argument

    runServer(serverPort);
    return NULL;
}

void *clientThreadFunction(void *arg)
{
    int serverPort = *((int *)arg); // Retrieve server port from the argument

    runClient(serverPort);
    return NULL;
}

#endif

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

    noLivesFlagCalled = false;

    load();
    initStage();
    initHighScoreTable();
    initTitle();

    titleLoop();
    SDL_ShowCursor(0); // Hide cursor
    initPlayers();

    while (true)
    {
        updateInvincible(); // Update player invincibility timer if player is invincible

        if (returnMultiplayerStatus() == true)
        {

            if (returnPlayerLife() == 0 && noLivesFlagCalled == false)
            {

                playerNoLivesFunction();
            }

            if (secondPlayerDead == true && hostPlayerDead == true)
            {
                resetStage();
                prepareScene();       // Reset screen
                rendCopyBackground(); // Render background again
                addHighscore(returnPlayerScore(), returnHighscoreList());
                drawHighscores(returnHighscoreList());
                presentScene();

                // Reset variables
                secondPlayerDead = false;
                hostPlayerDead = false;
                noLivesFlagCalled = true;
                // resetStage();
                restartGame(); // Restart the game
                initPlayers();
            }
        }
        else
        {
            if (returnPlayerLife() == 0)
            {
                addHighscore(returnPlayerScore(), returnHighscoreList());
                drawHighscores(returnHighscoreList());
                presentScene();
                resetStage();
                restartGame(); // Restart the game
                initPlayers();
            }
        }

        // prepareScene(); // Prepare Scene (Background & Clear)

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
                // Only re-initialise one player if the other player is dead
                if (hostPlayerDead == true || secondPlayerDead == true)
                {
                    initPlayer();
                }
                else
                {
                    initPlayers();
                }
                setInvincible();
            }
            else
            {
                // resetStage();
                resetFairyBullet(); // Only reset fairy bullet when player dies? Prevent screen overload?
                initPlayers();
                setInvincible();
            }
            continue;
        }

        manipulateDBullet();

        manipulateFairy();
        fireEnemyBulletCall();

        // Fairies: Want host player to create all the fairies, and send them to second player.
        // DONT want second player to create ANY fairies.
        if (returnMultiplayerStatus() == true)
        { // Check multiplayer and it is HOST calling this
            if (returnServerVar() != NULL)
            {
                spawnFairies('L');
                spawnFairies('R');
            }
        }
        else
        { // Single Player
            spawnFairies('L');
            spawnFairies('R');
        }

        manipulateExplosion();
        manipulatePowerUp();
        playerCollidePowerUp();
        playerCollideFairy();

        if (playerNullCheck())
        {
            if (returnMultiplayerStatus() == true)
            {
                resetPlayer();
                if (hostPlayerDead == true || secondPlayerDead == true)
                {
                    initPlayer();
                }
                else
                {
                    initPlayers();
                }
                setInvincible();
            }
            else
            {
                // resetStage();
                resetFairyBullet();
                initPlayers();
                setInvincible();
            }
            continue;
        }
        // // Create & send player packets
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
        drawOpponentDBullets();
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
#ifdef _WIN32
        CloseHandle(WserverThread);
        CloseHandle(WclientThread);
#else
        pthread_join(serverThread, NULL); // Join server thread
        pthread_join(clientThread, NULL);
#endif
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

#ifdef _WIN32
        // Create Windows Server Thread
        WserverThread = CreateThread(NULL, 0, WserverThreadFunction, &serverPort, 0, NULL);

        if (WserverThread == NULL)
        {
            printf("Failed to create windows server thread.\n");
            return 1;
        }

        // Create Windows Client Thread
        WclientThread = CreateThread(NULL, 0, WclientThreadFunction, &serverPort, 0, NULL);

        if (WclientThread == NULL)
        {
            printf("Failed to create windows client thread.\n");
            return 1;
        }
#else
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

#endif
        SDL_Delay(10); // Delay so the server and client can initialise
    }
    else
    {
        printf("Multiplayer Mode: false\n");
    }
}

void processPlayerNoLives(ENetPacket *packet)
{
    int *receivedValue = (int *)packet->data;

    // 2 = second player died
    // 1 = host died
    if (*receivedValue == 2)
    {
        secondPlayerDead = true;
        if (returnServerVar() != NULL)
        {
            freeOpponentPlayer();
        }
    }
    else if (*receivedValue == 1)
    {
        hostPlayerDead = true;

        if (returnServerVar() == NULL)
        {                         // Second player calling code
            freeOpponentPlayer(); // Free opponent on second players screen
        }
    }
}

void playerNoLivesFunction()
{

    int playerNoLives = 0; // 1 is host, 2 is second player

    if (returnServerVar() == NULL)
    {
        playerNoLives = 2; // 2nd player
    }
    else
    {
        playerNoLives = 1; // Host
    }
    ENetPacket *playerNoLivesPacket = enet_packet_create(&playerNoLives, sizeof(int), ENET_PACKET_FLAG_RELIABLE);
    sendUpdateToServerAndBroadcast(playerNoLivesPacket, PLAYER_CHANNEL);
    noLivesFlagCalled = true;
    freeCurrentPlayer();
}