#include <iostream>
#include <stdio.h>
#include <string>

#include "ENet.h"

ENet::ENet()
{
	m_mode = Mode::Unset;
	m_host = nullptr;
	m_peer = nullptr;
	m_clientCount = 0;
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

int ENet::SetupClient()
{
	m_host = enet_host_create(NULL /* create a client host */,
		1 /* only allow 1 outgoing connection */,
		2 /* allow up 2 channels to be used, 0 and 1 */,
		0 /* assume any amount of incoming bandwidth */,
		0 /* assume any amount of outgoing bandwidth */);
	if (m_host == nullptr)
	{
		fprintf(stderr, "An error occurred while trying to create an ENet client host.\n");
		return 0;
	}
	m_mode = Mode::Client;
	return 1;
}

int ENet::ConnectClient()
{
	ENetAddress address;
	ENetEvent event;

	// Connection to localHost, obviously
	// some work needed if the server is not local host :)
	enet_address_set_host(&address, "127.0.0.1");
	address.port = 1234;

	/* Initiate the connection, allocating the two channels 0 and 1. */
	m_peer = enet_host_connect(m_host, &address, 2, 0);
	if (m_peer == NULL)
	{
		fprintf(stderr, "No available peers for initiating an ENet connection.\n");
		exit(EXIT_FAILURE);
	}

	/* Wait up to 5 seconds for the connection attempt to succeed. */
	if (enet_host_service(m_host, &event, 5000) > 0 &&
		event.type == ENET_EVENT_TYPE_CONNECT)
	{
		puts("Connection to some.server.net:1234 succeeded.");
		return 0;
	}
	else
	{
		/* Either the 5 seconds are up or a disconnect event was */
		/* received. Reset the peer in the event the 5 seconds   */
		/* had run out without any significant event.            */
		enet_peer_reset(m_peer);
		puts("Connection to some.server.net:1234 failed.");
		return 1;
	}
}

void ENet::DisconnectClient()
{
	ENetEvent event;

	if (m_peer == nullptr)
		return; // we were not connected anyway

	enet_peer_disconnect(m_peer, 0);

	/* Allow up to 3 seconds for the disconnect to succeed
	 * and drop any packets received packets.
	 */
	while (enet_host_service(m_host, &event, 3000) > 0)
	{
		switch (event.type)
		{
		case ENET_EVENT_TYPE_RECEIVE:
			enet_packet_destroy(event.packet);
			break;

		case ENET_EVENT_TYPE_DISCONNECT:
			puts("Disconnection succeeded.");
			return;
		}
	}
	/* We've arrived here, so the disconnect attempt didn't */
	/* succeed yet.  Force the connection down.             */
	enet_peer_reset(m_peer);
	m_peer = nullptr;
}

int ENet::SetupServer()
{
	ENetAddress address;

	/* Bind the server to the default localhost.     */
	/* A specific host address can be specified by   */
	/* enet_address_set_host (& address, "x.x.x.x"); */
	address.host = ENET_HOST_ANY;
	/* Bind the server to port 1234. */
	address.port = 1234;

	m_host = enet_host_create(&address /* the address to bind the server host to */,
		32      /* allow up to 32 clients and/or outgoing connections */,
		2      /* allow up to 2 channels to be used, 0 and 1 */,
		0      /* assume any amount of incoming bandwidth */,
		0      /* assume any amount of outgoing bandwidth */);
	if (m_host == nullptr)
	{
		fprintf(stderr, "An error occurred while trying to create an ENet server host.\n");
		return 0;
	}
	m_mode = Mode::Server;

	return 1;
}

bool ENet::IsClientConnected()
{
	return m_mode == Mode::Client && m_peer != nullptr;
}

bool ENet::IsClient()
{
	return m_mode == Mode::Client;
}

bool ENet::IsServer()
{
	return m_mode == Mode::Server;

}


void ENet::CleanUpClient()
{
	DisconnectClient(); // disconnect nicely
	enet_host_destroy(m_host);
	m_mode = Mode::Unset;
	m_host = nullptr;
}

void ENet::CleanUpServer()
{
	enet_host_destroy(m_host);
	m_mode = Mode::Unset;
	m_host = nullptr;
}

void ENet::SendPacket(bool _reliable, const char* _dataStr)
{
	// Create a reliable packet of content "_dataStr" 
	ENetPacket* packet = enet_packet_create(_dataStr, strlen(_dataStr) + 1, _reliable ? ENET_PACKET_FLAG_RELIABLE : 0);

	/* Send the packet to the peer over channel id 0. */
	/* One could also broadcast the packet by         */
	/* enet_host_broadcast (host, 0, packet);         */
	enet_peer_send(m_peer, 0, packet);

	/* One could just use enet_host_service() instead. */
	enet_host_flush(m_host);
	// enet_host_service()
}

void ENet::BroadcastPacket(bool _reliable, const char* _dataStr)
{
	// Create a reliable packet of content "_dataStr" 
	ENetPacket* packet = enet_packet_create(_dataStr, strlen(_dataStr) + 1, _reliable ? ENET_PACKET_FLAG_RELIABLE : 0);

	enet_host_broadcast(m_host, 0, packet);

	enet_host_flush(m_host);
}


void ENet::Update()
{
	ENetEvent event;

	/* Wait up to 1000 milliseconds for an event. */
	enet_uint32 timeOutValue = 1000; // that many ms before time out
	if (enet_host_service(m_host, &event, timeOutValue) > 0)
	{
		fprintf(stderr, "Event RECEIVED !!\n");

		switch (event.type)
		{
		default:
		case ENET_EVENT_TYPE_NONE:
			break;

		case ENET_EVENT_TYPE_CONNECT:
			RegisterPeer(event.peer);
			break;

		case ENET_EVENT_TYPE_RECEIVE:
		{
			ClientInfo* client = static_cast<ClientInfo*>(event.peer->data);
			const char* sender = client ? client->m_clientName.c_str() : "Server";

			printf("A packet of length %u containing %s was received from %s on channel %u.\n",
				event.packet->dataLength,
				event.packet->data,
				sender,
				event.channelID);

			/* Clean up the packet now that we're done using it. */
			enet_packet_destroy(event.packet);
		}
		break;

		case ENET_EVENT_TYPE_DISCONNECT:
			UnregisterPeer(event.peer);
			break;
		}
	}
}

void ENet::RegisterPeer(ENetPeer* _peer)
{
	ClientInfo* newClient = new ClientInfo();

	newClient->m_peer = _peer;
	_peer->data = newClient;

	char clientName[33];
	sprintf_s(clientName, "Client%02i", m_clientCount);
	newClient->m_clientName = clientName;

	printf("A new client %s connected from %x:%u.\n",
		newClient->m_clientName.c_str(),
		_peer->address.host,
		_peer->address.port);

	m_clientCount++;
}

void ENet::UnregisterPeer(ENetPeer* _peer)
{
	ClientInfo* client = static_cast<ClientInfo*>(_peer->data);

	printf("%s disconnected.\n", client->m_clientName.c_str());

	delete client;

	/* Reset the peer's client information. */
	_peer->data = nullptr;

}