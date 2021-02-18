#pragma once

class ClientInfo
{
public:
	ClientInfo() { m_clientName = ""; m_peer = nullptr; };

	std::string m_clientName;
	ENetPeer* m_peer;
};
