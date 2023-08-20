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

// Different channels for packet passing
#define BULLETANDSTATUS_CHANNEL 0
#define PLAYER_CHANNEL 1
#define STATUS_CHANNEL 2

ENetHost *server;

bool secondClientJoined = false;

void runServer(int serverPort)
{
    int clientCount = 0;
    // int secondClientJoined = false;

    if (enet_initialize())
    {
        printf("Server: An error occurred while initializing ENet.\n");
        return;
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
        return;
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
                printf("Server: A new client connected from %x:%u.\n", event.peer->address.host, event.peer->address.port);
                clientCount++;
                if (clientCount == 2)
                {
                    // secondClientJoined = true;
                    sendSecondClientStatus(true); // Notify clients that the second client has joined
                    ENetAddress serverAddress = server->address;
                    // sendServerPointer(event.peer, &serverAddress);
                    // sendServerPointer(event.peer); // Send the server pointer to the second client
                }
                event.peer->data = (void *)"Client information";
                break;
            }
            case ENET_EVENT_TYPE_RECEIVE:
            {

                if (event.channelID == PLAYER_CHANNEL)
                {
                    printf("Server: Player Packet Request Recieved\n");
                    // Broadcast the received packet to other clients
                    enet_host_broadcast(server, PLAYER_CHANNEL, event.packet);
                    enet_packet_destroy(event.packet);
                }
                break;
            }
            case ENET_EVENT_TYPE_DISCONNECT:
            {
                printf("Server: Disconnected. Peer data: %s. Reason: %u\n", (char *)event.peer->data, event.data);
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

// Function to send status update to clients
void sendSecondClientStatus(bool status)
{
    ENetPacket *packet = enet_packet_create(&status, sizeof(bool), ENET_PACKET_FLAG_RELIABLE);
    enet_host_broadcast(server, BULLETANDSTATUS_CHANNEL, packet);
}

// void sendServerPointer(ENetPeer *clientPeer, const ENetAddress *serverAddress)
// {
//     // Create  packet containing the server pointer
//     ENetPacket *packet = enet_packet_create(&server, sizeof(ENetHost *), ENET_PACKET_FLAG_RELIABLE);

//     enet_peer_send(clientPeer, BULLETANDSTATUS_CHANNEL, packet);
// }
