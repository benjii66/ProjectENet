#include <iostream>
#include <stdio.h>

#include "PacketData.h"
PacketData::PacketData()
{
	m_Type = Type::Unset; // default
	m_Valid = false;
	m_Data = nullptr;
}

PacketData::~PacketData()
{
	if (m_Data != nullptr)
		free(m_Data);
	m_Data = nullptr;
}


void PacketData::SetType(Type _type)
{
	m_Type = _type;
}

PacketData::Type PacketData::GetType()
{
	return m_Type;
}

bool PacketData::IsValid()
{
	return m_Valid;
}

void PacketData::SetStringContent(const char* _string)
{
	m_Content = _string;
	m_Type = Type::String;
	m_Valid = true;
}

const char* PacketData::GetContent()
{
	return m_Content.c_str();
}


void* PacketData::Serialize(unsigned int& _size)
{
	// no content !
	if (!m_Valid)
	{
		_size = 0;
		return nullptr;
	}

	// compute the size of the packet over the network
	_size = sizeof(Type) + m_Content.length() + 1;
	// allocate the content we are preparing to send
	m_Data = malloc(_size);
	void* workBuffer = m_Data;

	// write the packet type : json, string, whatever
	Type* ptrType = static_cast<Type*>(workBuffer);
	*ptrType = m_Type;

	//workBuffer = (void *)((int)workBuffer + sizeof(Type));
	ptrType++;
	workBuffer = (void*)ptrType;


	// then write the content
	char* contentStr = static_cast<char*>(workBuffer);
	strcpy_s(contentStr, m_Content.length() + 1, m_Content.c_str());

	// return it
	return m_Data;
}

void PacketData::Deserialize(void* _data, int _size)
{
	// read the packet type : json, string, ...
	Type* ptrType = static_cast<Type*>(_data);
	m_Type = *ptrType;
	_data = (void*)((int)_data + sizeof(Type));

	if (m_Type == Type::String)
	{
		// then read the content according to the packet type
		char* contentStr = static_cast<char*>(_data);
		m_Content = contentStr;

		m_Valid = true;
	}
}