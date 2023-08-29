// #include "enet/enet.h"
// #include "enet/list.h"
// #include "enet/protocol.h"
// #include "enet/time.h"
// #include "enet/types.h"
// #include "enet/utility.h"
// #include "enet/unix.h"
#include <enet/enet.h>
#include <stdio.h>
#include <stdlib.h>
#include "gui.h"

#ifdef _WIN32
#include <windows.h>
#include <winsock2.h>
#pragma comment (lib,"ws2_32.lib")
#pragma comment (lib,"Winmm.lib")
#endif
// #include <pthread.h>

// Different channels for packet passing
#define BULLETANDSTATUS_CHANNEL 0
#define PLAYER_CHANNEL 1
#define STATUS_CHANNEL 2

extern bool secondClientJoined;
ENetPeer *peer;
ENetHost *client;

void runClient(int serverPort)
{
    // Initialize ENet for Client Host & Connection
    if (enet_initialize() != 0)
    {
        printf("Client: An error occurred while initializing ENet for client.\n");
        return;
    }
    atexit(enet_deinitialize);
    ENetEvent event;

    client = enet_host_create(NULL /* create a client host */,
                              1 /* only allow 1 outgoing connection */,
                              2 /* allow up 2 channels to be used, 0 and 1 */,
                              0 /* assume any amount of incoming bandwidth */,
                              0 /* assume any amount of outgoing bandwidth */);

    if (client == NULL)
    {
        printf("Client: An error occurred while trying to create an ENet client host.\n");
        return;
    }

    ENetAddress address;

    enet_address_set_host(&address, "localhost");
    address.port = serverPort;

    // printf("Client: Connecting to server at  port %d...\n", address.port);
    peer = enet_host_connect(client, &address, 2, 0);

    if (peer == NULL)
    {
        printf("Client: No available peers for initiating an ENet connection.\n");
        return;
    }

    if (enet_host_service(client, &event, 100) > 0)
    {
        if (event.type == ENET_EVENT_TYPE_CONNECT)
        {
            printf("Client Connection succeeded to port %d.\n", address.port);
            enet_host_flush(client); // Need to include this so in server.c it can print the client connected
        }
        else
        {
            printf("Client: Event type: %d\n", event.type); // Print the event type to diagnose
        }
    }
    else
    {
        enet_peer_reset(peer);
        printf("Client: Connection to some.server.net:1234 failed.");
        return;
    }

    // While loop:
    while (1)
    {
        while (enet_host_service(client, &event, 10) > 0)
        {
            switch (event.type)
            {
            case ENET_EVENT_TYPE_RECEIVE:

                // Handle received packets based on packet type
                if (event.channelID == BULLETANDSTATUS_CHANNEL)
                {
                    if (event.packet->dataLength == sizeof(bool))
                    {
                        signalSecondClientJoined(); // Signal the gui.c function
                    }
                    else if (event.packet->dataLength == sizeof(Entity))
                    {
                        // Process bullet packet and update local game state

                        Entity *receivedBullet = (Entity *)event.packet->data;

                        // Check bullet type and broadcast accordingly
                        if (receivedBullet->bulletType == 2) // Diagonal Bullet
                        {
                            processDBulletPacket(event.packet);
                        }
                        else
                        {
                            processBulletPacket(event.packet);
                        }
                    }
                }

                // Handle received packets based on packet type
                if (event.channelID == PLAYER_CHANNEL)
                {
                    if (event.packet->dataLength == sizeof(Mode))
                    {
                        updateMode(event.packet);
                    }
                    else if (event.packet->dataLength == sizeof(int))
                    { // If player ran out of lives
                        processPlayerNoLives(event.packet);
                    }
                    else
                    {
                        Entity *receivedPacket = (Entity *)event.packet->data;

                        if (receivedPacket->fairyID == 1) // Process fairy packet
                        {
                            processFairyPacket(event.packet);
                        }
                        else if (receivedPacket->powerupID == 1 || receivedPacket->powerupID == 2)
                        {
                            processPowerUpPacket(event.packet);
                        }
                        else
                        {
                            // Process Player packet and update local game state
                            processPlayerPacket(event.packet);
                        }
                    }
                }
                enet_packet_destroy(event.packet);
                break;

            case ENET_EVENT_TYPE_DISCONNECT:
                printf("Client: Disconnected.\n");
                break;
                event.peer->data = NULL;
                break;
            default:
                break;
            }
        }
    }
}

void sendUpdateToServerAndBroadcast(ENetPacket *packet, int channel)
{
    if (peer != NULL)
    {
        // Create an update packet and send it to the server
        enet_peer_send(peer, channel, packet);
    }
}
