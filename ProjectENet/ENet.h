#pragma once
#include <enet/enet.h>
#include "ClientInfo.h"
#include "PacketData.h"


class ENet
{
public:
	ENet();
	~ENet();


	enum Mode {
		Unset,
		Server,
		Client
	};

public:
	int Initialize();

	int SetupClient();
	int ConnectClient();
	void DisconnectClient();

	int SetupServer();


	bool IsClient();
	bool IsServer();
	bool IsClientConnected();

protected:
	void CleanUpClient();
	void CleanUpServer();


	Mode		m_mode;
	ENetHost*   m_host;	// Client or Server, depending on the mode
	ENetPeer*   m_peer;

	void RegisterPeer(ENetPeer* _peer);
	void UnregisterPeer(ENetPeer* _peer);

	int			m_clientCount;

public:
	void Update();

	void SendPacket(bool _reliable, const char* _dataStr);

	void BroadcastPacket(bool _reliable, const char* _dataStr);


};