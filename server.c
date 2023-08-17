#include <stdio.h>
#include <enet/enet.h>

int main(int argc, char **argv)
{
    if (enet_initialize() != 0) // Initialise library
    {
        fprintf(stderr, "An error occurred while initializing ENet.\n");
        return EXIT_FAILURE;
    }

    ENetAddress address;
    ENetHost *server;
    /* Bind the server to the default localhost.     */
    /* A specific host address can be specified by   */
    /* enet_address_set_host (& address, "x.x.x.x"); */
    address.host = ENET_HOST_ANY;
    address.port = 1234; /* Bind the server to port 1234. */

    // Create server
    server = enet_host_create(&address /* the address to bind the server host to */,
                              2 /* allow up to 2 clients and/or outgoing connections */,
                              2 /* allow up to 2 channels to be used, 0 and 1 */,
                              0 /* assume any amount of incoming bandwidth */,
                              0 /* assume any amount of outgoing bandwidth */);
    if (server == NULL)
    {
        fprintf(stderr,
                "An error occurred while trying to create an ENet server host.\n");
        exit(EXIT_FAILURE);
    }
    printf("Server started and listening on port %d.\n", address.port);

    while (1)
    {
        ENetEvent event;

        /* Wait up to 1000 milliseconds for an event. (timeout value) */
        // Will return a 1 if event dispatched within 1000 milliseconds. Will return 0 if no event (aka return 'ENET_EVENT_TYPE_NONE)
        // Function called regularly to ensure packets are sent & recieved
        // Should be called at beginning of every frame in a game loop (with a 0 timeout)
        while (enet_host_service(server, &event, 1000) > 0)
        {
            switch (event.type)
            {
            case ENET_EVENT_TYPE_CONNECT: // If new client host connected to server host (peer field contains newly connected peer)
                printf("A new client connected from %x:%u.\n",
                       event.peer->address.host,
                       event.peer->address.port);

                event.peer->data = "Client information"; /* Store any relevant client information here. */
                break;

            case ENET_EVENT_TYPE_RECEIVE: // When packet is recieved by a connected peer.
                printf("A packet of length %u containing %s was received from %s on channel %u.\n",
                       event.packet->dataLength,
                       event.packet->data, //'packet' = packet that was sent
                       event.peer->data,   //('peer' field contains who the person that recieved the packet)
                       event.channelID);   //'channelID' = channel which packet was sent.

                enet_packet_destroy(event.packet); /* Clean up the packet now that we're done using it (must be destroyed!) */
                break;

            case ENET_EVENT_TYPE_DISCONNECT: // If peer disconnected or timed out. (peer field contains peer that disconnected)
                printf("%s disconnected.\n", event.peer->data);

                event.peer->data = NULL; /* Reset the peer's client information. ('data' is still valid so must be reset)*/
            }
        }
    }
    enet_host_destroy(server); // Destroy & free server
    atexit(enet_deinitialize); // Clean up resources
}
