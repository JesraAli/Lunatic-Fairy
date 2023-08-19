#include "enet/enet.h"
#include "enet/list.h"
#include "enet/protocol.h"
#include "enet/time.h"
#include "enet/types.h"
#include "enet/utility.h"
// #include "enet/win32.h"
#include "enet/unix.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

ENetHost *server;
bool secondClientJoined;

void runServer(int serverPort)
{
    int clientCount = 0;
    // int secondClientJoined = false;

    if (enet_initialize())
    {
        printf("Server: An error occurred while initializing ENet.\n");
        return 1;
    }
    atexit(enet_deinitialize);
    ENetAddress address;
    ENetEvent event;

    address.host = ENET_HOST_ANY;
    address.port = serverPort;
    // ENetHost *server = enet_host_create(&address, 32, 0, 0);

    server = enet_host_create(&address /* the address to bind the server host to */,
                              2 /* allow up to 2 clients and/or outgoing connections */,
                              2 /* allow up to 2 channels to be used, 0 and 1 */,
                              0 /* assume any amount of incoming bandwidth */,
                              0 /* assume any amount of outgoing bandwidth */);
    if (server == NULL)
    {
        printf("Server: An error occurred while trying to create an ENet server host.\n");
        return 1;
    }

    printf("Server: started and listening on port %d.\n", address.port);

    while (1)
    {
        while (enet_host_service(server, &event, 10) > 0)
        {
            switch (event.type)
            {
            case ENET_EVENT_TYPE_CONNECT:
            {
                clientCount++;
                if (clientCount == 2)
                {
                    secondClientJoined = true;
                    printf("Client is now set to trueeeeeeeeeeee\n");
                }
                printf("Server: A new client connected from %x:%u.\n", event.peer->address.host, event.peer->address.port);
                event.peer->data = (void *)"Client information";
                break;
            }
            case ENET_EVENT_TYPE_RECEIVE:
            {
                printf("Server: recieved packet data");
                enet_packet_destroy(event.packet);
                break;
            }
            case ENET_EVENT_TYPE_DISCONNECT:
            {
                printf("Server: Disconnected.\n");
                event.peer->data = NULL;
                break;
            }
            default:
            {
            }
            }
        }
    }
    enet_host_destroy(server);
}

ENetHost *returnServerVar()
{
    return server;
}

bool returnSecondClientState()
{
    return secondClientJoined;
}