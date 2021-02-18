#include <string>
#pragma once
class PacketData
{
public:
	PacketData();
	~PacketData();

	enum class Type {
		Unset,
		String,
		JSon
	};

	void SetType(Type _type);
	Type GetType();

	bool IsValid(); // false is data setup is not correct

	void SetStringContent(const char* _string);

	virtual const char* GetContent();	// returns the data hold in the packet, as a string

	virtual void* Serialize(unsigned int& _size);	// returns the data prepared for the network, according to our choices (compression, etc...)
	virtual void Deserialize(void* _data, int _size); // reads the data from the network

protected:
	Type				m_Type;
	bool				m_Valid;
	std::string			m_Content;
	void*				m_Data;
};