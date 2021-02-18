#pragma once
#include "PacketData.h"
#include "json.hpp"
class PacketDataJson :
	public PacketData
{
public:
	PacketDataJson();
	~PacketDataJson();

	void SetType(Type _type);
	Type GetType();

	bool IsValid(); // false is data setup is not correct
	void SetJSonContent(const char* _string);

	const char* GetContent();	// returns the data hold in the packet, as a string


	void* Serialize(unsigned int& _size);
	void Deserialize(void* _data, int& _size);

protected:
	Type				m_JSonType;
	bool				m_JSonValid;
	std::string			m_JSonContent;
	void*               m_JSonData;

};

