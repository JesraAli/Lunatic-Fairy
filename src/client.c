// #include <stdio.h>
// #include <stdlib.h>
// #include <enet/enet.h>
// #include "client.h"

// // ENetPeer *peer;

// void connectToServer(ENetHost *client, const char *serverAddress, int serverPort)
// {
//     ENetAddress address;
//     ENetEvent event;
//     ENetPeer *peer;

//     enet_address_set_host(&address, serverAddress); // Connect to server IP
//     address.port = serverPort;                      // Connect to server Port

//     printf("Connecting to server at %s:%d...\n", serverAddress, serverPort);

//     // Initiate connection, allocating the two channels 0 and 1.
//     peer = enet_host_connect(client, &address, 2, 0);
//     if (peer == NULL)
//     {
//         fprintf(stderr, "No available peers for initiating an ENet connection.\n");
//         return; // Return without attempting to reconnect
//     }

//     // Wait up to 5 seconds for the connection attempt to succeed.
//     if (enet_host_service(client, &event, 5000) > 0 &&
//         event.type == ENET_EVENT_TYPE_CONNECT)
//     {
//         puts("Client Connection succeeded.");
//         while (1)
//         {
//             // (Client logic and communication code)
//         }
//     }
//     else
//     {
//         // 5 seconds are up OR disconnect event received.
//         // Reset the peer if the 5 seconds ran out without any significant event.
//         enet_peer_reset(peer);
//         printf("Connection to %s:%d failed.\n", serverAddress, serverPort);
//     }
// }

// #include <stdio.h>
// #include <stdlib.h>
// #include <enet/enet.h>

// void connectToServer(ENetHost *client, const char *serverAddress, int serverPort)
// {
//     ENetAddress address;
//     ENetEvent event;
//     ENetPeer *peer;

//     enet_address_set_host(&address, serverAddress); // Connect to server IP
//     address.port = serverPort;                      // Connect to server Port

//     // Initiate connection, allocating the two channels 0 and 1.
//     peer = enet_host_connect(client, &address, 2, 0);
//     if (peer == NULL)
//     {
//         fprintf(stderr, "No available peers for initiating an ENet connection.\n");
//         return; // Return without attempting to reconnect
//     }

//     // Wait up to 5 seconds for the connection attempt to succeed.
//     if (enet_host_service(client, &event, 5000) > 0 &&
//         event.type == ENET_EVENT_TYPE_CONNECT)
//     {
//         puts("Connection succeeded.");
//         enet_host_flush;
//         while (1)
//         {
//             // (Client logic and communication code)
//         }
//     }
//     else
//     {
//         // 5 seconds are up OR disconnect event received.
//         // Reset the peer if the 5 seconds ran out without any significant event.
//         enet_peer_reset(peer);
//         puts("Connection to 127.0.0.1 failed.");
//     }
// }

// int main()
// {
//     if (enet_initialize() != 0)
//     {
//         fprintf(stderr, "Error initializing ENet.\n");
//         return 1;
//     }

//     ENetHost *client;
//     client = enet_host_create(NULL /* create a client host */,
//                               1 /* only allow 1 outgoing connection */,
//                               2 /* allow up 2 channels to be used, 0 and 1 */,
//                               0 /* assume any amount of incoming bandwidth */,
//                               0 /* assume any amount of outgoing bandwidth */);
//     if (client == NULL)
//     {
//         fprintf(stderr, "An error occurred while trying to create an ENet client host.\n");
//         return EXIT_FAILURE;
//     }

//     connectToServer(client, "127.0.0.1", 1234);

//     enet_host_destroy(client); // Cleanup

//     // Implement delay before the next reconnection attempt
//     usleep(1000000); // Sleep for 1 second

//     enet_deinitialize();
//     return 0;
// }

#include "enet/enet.h"
#include "enet/list.h"
#include "enet/protocol.h"
#include "enet/time.h"
#include "enet/types.h"
#include "enet/utility.h"
#include "enet/unix.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{
    if (enet_initialize())
    {
        printf("An error occurred while initializing ENet.\n");
        return 1;
    }
    atexit(enet_deinitialize);
    ENetAddress address;
    ENetEvent event;

    // ENetHost *client = enet_host_create(NULL, 1, 57600 / 8, 14400 / 8);

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
    enet_address_set_host(&address, "localhost");
    address.port = 5000;

    printf("Connecting to server at  port %d...\n", 5000);
    ENetPeer *peer = enet_host_connect(client, &address, 2, 0);

    if (peer == NULL)
    {
        printf("No available peers for initiating an ENet connection.\n");
        return 1;
    }

    if (enet_host_service(client, &event, 100) > 0)
    {
        if (event.type == ENET_EVENT_TYPE_CONNECT)
        {
            printf("Connection succeeded.\n");
            enet_host_flush(client); //Need to include this so in server.c it can print the client connected
        }
        else
        {
            printf("Event type: %d\n", event.type); // Print the event type to diagnose
        }
    }
    else
    {
        enet_peer_reset(peer);
        printf("Connection to some.server.net:1234 failed.");
    }




    printf("Starting while loop:\n");
    int running = 1;
    while (running) {
        while (enet_host_service(client, &event, 10) > 0) {
            switch (event.type) {
                case ENET_EVENT_TYPE_RECEIVE:
                    printf("Received packet data\n");
                    enet_packet_destroy(event.packet);
                    break;
                case ENET_EVENT_TYPE_DISCONNECT:
                    printf("Disconnected.\n");
                    running = 0; // Break the loop and exit gracefully
                    event.peer->data = NULL;
                    break;
                default:
                    break;
            }
        }
    }





    enet_host_destroy(client);
    return 0;
}