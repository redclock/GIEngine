#include <stdlib.h>
#include <string.h>
#include "NetPacket.h"

CNetPacket::CNetPacket(int max_len)
{
	this->max_len = max_len;
	packet = new char[sizeof(TNetPacketHeader) + max_len];
	header = (TNetPacketHeader *) packet;
	data = packet + sizeof(TNetPacketHeader);

	header->ver = NET_PACKET_VERSION;
	header->len = 0;

	cursor = 0;
}

CNetPacket::CNetPacket(const TNetPacketHeader * header, const char * data)
{
	max_len = header->len;
	packet = new char[sizeof(TNetPacketHeader) + max_len];
	this->header = (TNetPacketHeader *) packet;
	this->data = packet + sizeof(TNetPacketHeader);
	
	memcpy(this->header, header, sizeof(TNetPacketHeader));
	memcpy(this->data, data, header->len);

	cursor = 0;
}

CNetPacket::CNetPacket(const char * buf, int len)
{
	max_len = len - sizeof(TNetPacketHeader);
	packet = new char[sizeof(TNetPacketHeader) + max_len];
	header = (TNetPacketHeader *) packet;
	data = packet + sizeof(TNetPacketHeader);
	
	memcpy(packet, buf, len);

	cursor = 0;
}

CNetPacket::~CNetPacket()
{
	if (packet) delete [] packet;
}

NETINT8 CNetPacket::read8() const
{
	NETINT8 n = 0;
	if (cursor <= header->len - 1) 
	{
		n = *(NETINT8 *) (data + cursor);
		cursor += 1;
	}
	return n;
}

NETINT16 CNetPacket::read16() const
{
	NETINT16 n = 0;
	if (cursor <= header->len - 2) 
	{
		n = *(NETINT16 *) (data + cursor);
		cursor += 2;
	}
	return n;
}

NETINT32 CNetPacket::read32() const
{
	NETINT32 n = 0;
	if (cursor <= header->len - 4) 
	{
		n = * (NETINT32 *) (data + cursor);
		cursor += 4;
	}
	return n;
}

const char * CNetPacket::readString() const
{
	char * s = data + cursor;
	int slen = (int) strlen(data + cursor) + 1;
	if (cursor <= header->len - slen)
	{
		cursor += slen; 
	}else
	{
		cursor = header->len;
		data[cursor] = 0;
	}
	return s;
}

const char * CNetPacket::readBinary(int len) const
{
	char * s = data + cursor;
	if (cursor <= header->len - len)
	{
		cursor += len; 
	}else
	{
		cursor = header->len;
	}
	return s;
}

void CNetPacket::write8(NETINT8 n)
{
	if (header->len < max_len - 1)
	{
		*(NETINT8 *)(data + header->len) = n;
		header->len += 1;
	}
}

void CNetPacket::write16(NETINT16 n)
{
	if (header->len < max_len - 2)
	{
		*(NETINT16 *)(data + header->len) = n;
		header->len += 2;
	}
}

void CNetPacket::write32(NETINT32 n)
{
	if (header->len < max_len - 4)
	{
		*(NETINT32 *)(data + header->len) = n;
		header->len += 4;
	}
}

void CNetPacket::writeString(const char *s)
{
	int slen = (int)strlen(s);
	if (header->len >= max_len - slen - 1)
	{
		slen = max_len - header->len - 2;
	}
	strncpy(data + header->len, s, slen);
	header->len += slen + 1;
	data[header->len - 1] = 0;
}

void CNetPacket::writeBinary(const char *buf, int len)
{
	if (header->len >= max_len - len)
	{
		len = max_len - header->len - 1;
	}
	memcpy(data + header->len, buf, len);
	header->len += len;
}