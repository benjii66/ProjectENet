#include "PacketDataJson.h"
#include <iostream>


PacketDataJson::PacketDataJson()
{
	m_JSonType = Type::Unset;
	m_JSonValid = false;
	m_JSonData = nullptr;
}

PacketDataJson::~PacketDataJson()
{
	if (m_JSonData != nullptr)
		free(m_JSonData);
	m_JSonData = nullptr;
}

void PacketDataJson::SetType(Type _type)
{
	m_JSonType = _type;
}

PacketDataJson::Type PacketDataJson::GetType()
{
	return m_JSonType;
}

bool PacketDataJson::IsValid()
{
	return m_JSonValid;
}

void PacketDataJson::SetJSonContent(const char* _string)
{
	m_Content = _string;
	m_Type = Type::JSon;  //CHANGES LE TYPE
	m_Valid = true;

}

const char* PacketDataJson::GetContent()
{
	return m_JSonContent.c_str();
}

void* PacketDataJson::Serialize(unsigned int& _size)
{
	if (!m_JSonValid)
	{
		_size = 0;
		return nullptr;
	}


	JSON obj = JSON::Load(m_JSonContent);
	m_JSonContent = obj.ToString();
	//obj.size();
	_size = sizeof(Type) + m_JSonContent.length() + 1;

	m_JSonData = malloc(_size);

	void* workBuffer = m_JSonData;

	Type* ptrType = static_cast<Type*>(workBuffer);
	*ptrType = m_JSonType;

	//workBuffer = (void *)((int)workBuffer + sizeof(Type));
	ptrType++;
	workBuffer = (void*)ptrType;

	// then write the content
	char* contentStr = static_cast<char*>(workBuffer);
	strcpy_s(contentStr, m_JSonContent.length() + 1, m_JSonContent.c_str());

	return m_JSonData;
}

void PacketDataJson::Deserialize(void* _data, int& _size)
{
	JSON Str = JSON::Load("\"String\"");

	std::cout << Str << std::endl;

	Type* ptrType = static_cast<Type*>(_data);
	m_JSonType = *ptrType;
	_data = (void*)((int)_data + sizeof(Type));

	if (m_JSonType == Type::String)
	{
		// then read the content according to the packet type
		JSON* contentJSon = static_cast<JSON*>(_data);

		char* contentStr = static_cast<char*>(_data);
		m_JSonContent = contentStr;

		m_JSonValid = true;
	}
}
