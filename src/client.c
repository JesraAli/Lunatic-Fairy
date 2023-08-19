#include "enet/enet.h"
#include "enet/list.h"
#include "enet/protocol.h"
#include "enet/time.h"
#include "enet/types.h"
#include "enet/utility.h"
#include "enet/unix.h"
#include <stdio.h>
#include <stdlib.h>
#include "gui.h"

// Different channels for packet passing
#define BULLET_CHANNEL 0
#define PLAYER_CHANNEL 1

void runClient(int serverPort)
{
    // Initialize ENet for Client Host & Connection
    if (enet_initialize() != 0)
    {
        printf("Client: An error occurred while initializing ENet for client.\n");
        return 1;
    }
    atexit(enet_deinitialize);
    ENetEvent event;

    ENetHost *client = enet_host_create(NULL /* create a client host */,
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

    printf("Client: Connecting to server at  port %d...\n", address.port);
    ENetPeer *peer = enet_host_connect(client, &address, 2, 0);

    if (peer == NULL)
    {
        printf("Client: No available peers for initiating an ENet connection.\n");
        return;
    }

    if (enet_host_service(client, &event, 100) > 0)
    {
        if (event.type == ENET_EVENT_TYPE_CONNECT)
        {
            printf("Client Connection succeeded.\n");
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
                if (event.channelID == BULLET_CHANNEL)
                {
                    printf("Client: BULLET Packet data recieved\n");
                    // Process bullet packet and update your local game state
                    processBulletPacket(event.packet);
                }

                // Handle received packets based on packet type
                if (event.channelID == PLAYER_CHANNEL)
                {
                    printf("Client: PLAYER Packet data recieved\n");
                    // Process Player packet and update your local game state
                    processPlayerPacket(event.packet);
                }
                enet_packet_destroy(event.packet);
                break;
            case ENET_EVENT_TYPE_DISCONNECT:
                printf("Disconnected.\n");
                break;
                event.peer->data = NULL;
                break;
            default:
                break;
            }
        }
    }
}

// int main()
// {
//     if (enet_initialize())
//     {
//         printf("An error occurred while initializing ENet.\n");
//         return 1;
//     }
//     atexit(enet_deinitialize);
//     ENetEvent event;

//     ENetHost *client = enet_host_create(NULL /* create a client host */,
//                                         1 /* only allow 1 outgoing connection */,
//                                         2 /* allow up 2 channels to be used, 0 and 1 */,
//                                         0 /* assume any amount of incoming bandwidth */,
//                                         0 /* assume any amount of outgoing bandwidth */);
//     if (client == NULL)
//     {
//         printf("An error occurred while trying to create an ENet client host.\n");
//         return 1;
//     }

//     connectToServer(client, &event);

//     printf("Starting while loop:\n");
//     int running = 1;
//     while (running)
//     {
//         while (enet_host_service(client, &event, 10) > 0)
//         {
//             switch (event.type)
//             {
//             case ENET_EVENT_TYPE_RECEIVE:
//                 printf("Received packet data\n");
//                 enet_packet_destroy(event.packet);
//                 break;
//             case ENET_EVENT_TYPE_DISCONNECT:
//                 printf("Disconnected.\n");
//                 running = 0; // Break the loop and exit gracefully
//                 event.peer->data = NULL;
//                 break;
//             default:
//                 break;
//             }
//         }
//     }

//     enet_host_destroy(client);
//     return 0;
// }