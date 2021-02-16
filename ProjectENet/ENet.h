#pragma once
#include <enet/enet.h>

class ENet
{
public:
	ENet();
	~ENet();

	enum Mode
	{
		Unset,
		Server,
		Client
	};

public:
	int Initialize();
	int SetupClient();
	int ConnectClient();
	int DisconnectClient();
	int SendPacket();
	int SetupServer();

	void Update();
	bool IsClient();
	bool IsServer();
	bool IsClientConnected();

protected:
	void CleanUpServer();
	void CleanUpClient();

protected:
	ENetHost* m_host;
	ENetPeer* m_peer;
	Mode m_mode;
	bool m_isConnected;
};

