#include <stdio.h>
#include <stdlib.h>
#include <enet/enet.h>

void connectToServer(ENetHost *client, const char *serverAddress, int serverPort)
{
    ENetAddress address;
    ENetEvent event;
    ENetPeer *peer;

    enet_address_set_host(&address, serverAddress); // Connect to server IP
    address.port = serverPort;                      // Connect to server Port

    // Initiate connection, allocating the two channels 0 and 1.
    peer = enet_host_connect(client, &address, 2, 0);
    if (peer == NULL)
    {
        fprintf(stderr, "No available peers for initiating an ENet connection.\n");
        return; // Return without attempting to reconnect
    }

    // Wait up to 5 seconds for the connection attempt to succeed.
    if (enet_host_service(client, &event, 5000) > 0 &&
        event.type == ENET_EVENT_TYPE_CONNECT)
    {
        puts("Connection succeeded.");
        while (1)
        {
            // (Client logic and communication code)
        }
    }
    else
    {
        // 5 seconds are up OR disconnect event received.
        // Reset the peer if the 5 seconds ran out without any significant event.
        enet_peer_reset(peer);
        puts("Connection to 127.0.0.1 failed.");
    }
}

int main()
{
    if (enet_initialize() != 0)
    {
        fprintf(stderr, "Error initializing ENet.\n");
        return 1;
    }

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

    enet_host_destroy(client); // Cleanup

    // Implement delay before the next reconnection attempt
    usleep(1000000); // Sleep for 1 second

    enet_deinitialize();
    return 0;
}