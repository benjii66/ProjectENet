#include "ENet.h"

#include <cstdio>
#include <string.h>

ENet::ENet()
{
	m_mode = Mode::Unset;
	m_host = nullptr;
	m_peer = nullptr;
}


ENet::~ENet()
{
	switch (m_mode)
	{
	case Mode::Server:
		CleanUpServer();
		break;
	case Mode::Client:
		CleanUpClient();
		break;
	default:
		break;
	}
}

int ENet::Initialize()
{
	if (enet_initialize() != 0)
	{
		fprintf(stderr, "An error occurred while initializing ENet.\n");
		return 0;
	}
	atexit(enet_deinitialize);
	return 1;
}

int ENet::SetupServer()
{
	ENetAddress address;
	ENetHost* server;
	address.host = ENET_HOST_ANY;
	address.port = 1234;

	m_host = enet_host_create(&address, 32, 2, 0, 0);
	if (m_host == nullptr)
	{
		fprintf(stderr, "An error occurred while trying to create an ENet server host.\n");
		exit(EXIT_FAILURE);
	}

	m_mode = Mode::Server;
	//enet_host_destroy(server);
	return 1;
}

void ENet::Update()
{
	ENetEvent event;
	/* Wait up to 1000 milliseconds for an event. */
	while (enet_host_service(m_host, &event, 1000) > 0)
	{
		switch (event.type)
		{
		case ENET_EVENT_TYPE_CONNECT:
			printf("A new client connected from %x:%u.\n",
				event.peer->address.host,
				event.peer->address.port);
			/* Store any relevant client information here. */
			event.peer->data = (void*)"Client information";
			break;
		case ENET_EVENT_TYPE_RECEIVE:
			printf("A packet of length %u containing %s was received from %s on channel %u.\n",
				event.packet->dataLength,
				event.packet->data,
				event.peer->data,
				event.channelID);
			/* Clean up the packet now that we're done using it. */
			enet_packet_destroy(event.packet);

			break;

		case ENET_EVENT_TYPE_DISCONNECT:
			printf("%s disconnected.\n", event.peer->data);
			/* Reset the peer's client information. */
			event.peer->data = NULL;
		}
	}
}

bool ENet::IsClient()
{
	if (m_mode == Mode::Server)return true;
	return false;
}

bool ENet::IsServer()
{
	if (m_mode == Mode::Server)return true;
	return false;
}

bool ENet::IsClientConnected()
{
	if (m_isConnected == true)return true;
	return false;
}

void ENet::CleanUpServer()
{
	enet_host_destroy(m_host);
	m_mode = Mode::Unset;
	m_host = nullptr;
}

void ENet::CleanUpClient()
{
	m_mode = Mode::Unset;
}

int ENet::SetupClient()
{
	m_host = enet_host_create(NULL,
		32,
		2,
		0,
		0);
	if (m_host == NULL)
	{
		fprintf(stderr, "An error occurred while trying to create an ENet client host.\n");
		exit(EXIT_FAILURE);
	}
	m_mode = Mode::Client;
	return 1;
}

int ENet::ConnectClient()
{

	ENetAddress address;
	ENetEvent event;
	enet_address_set_host(&address, "some.server.net");
	address.port = 1234;

	m_peer = enet_host_connect(m_host, &address, 2, 0);
	if (m_peer == NULL)
	{
		fprintf(stderr,
			"No available peers for initiating an ENet connection.\n");
		exit(EXIT_FAILURE);
	}
	if (enet_host_service(m_host, &event, 5000) > 0 &&
		event.type == ENET_EVENT_TYPE_CONNECT)
	{
		puts("Connection to some.server.net:1234 succeeded.");
		IsClientConnected();
		return 0;
	}
	else
	{
		enet_peer_reset(m_peer);
		puts("Connection to some.server.net:1234 failed.");
		return 1;
	}
	m_mode = Mode::Client;
	return 1;
}

int ENet::DisconnectClient()
{
	ENetEvent event;
	enet_peer_disconnect(m_peer, 0);
	while (enet_host_service(m_host, &event, 3000) > 0)
	{
		switch (event.type)
		{
		case ENET_EVENT_TYPE_RECEIVE:
			enet_packet_destroy(event.packet);
			break;
		case ENET_EVENT_TYPE_DISCONNECT:
			puts("Disconnection succeeded.");
			return 1;
		}
	}
	enet_peer_reset(m_peer);
	return 0;
}

int ENet::SendPacket()
{
	//spécifie la taille en 2nd args
	ENetPacket* packet = enet_packet_create("packet", strlen("packet") + 1, ENET_PACKET_FLAG_RELIABLE);
	enet_packet_resize(packet, strlen("packetfoo") + 1);
	strcpy_s((char*)&packet->data[strlen("packet")],packet->dataLength, "foo");
	enet_peer_send(m_peer, 0, packet);
	enet_host_flush(m_host);
	return 0;
}
