#pragma once

typedef unsigned char NETINT8;
typedef unsigned short NETINT16;
typedef unsigned long NETINT32;

static const NETINT16 NET_PACKET_VERSION = 0x1000;

#pragma pack(1)

enum PACKET_CMD
{
	CMD_BROADCAST,
	CMD_CONNECT,
	CMD_CONNECT_REPLY,
	CMD_QUIT,
	CMD_CHAT,
	CMD_PING,
	CMD_PING_REPLY,
};

struct TNetPacketHeader 
{
	NETINT16 ver;
	NETINT16 len;
	NETINT32 addr;
	NETINT16 port;
	NETINT16 ic;
	NETINT32 cmd;
};

#pragma pack()

class CNetPacket
{
protected:
	TNetPacketHeader * header;
	char * packet;
	char * data;
	mutable int cursor;
	int	max_len;

public:
	TNetPacketHeader * GetHeader() { return header; }
	const char * GetBuf() const { return packet; }
	const char * GetData() const { return data; }

	NETINT32 GetCmd() const { return header->cmd; }
	void SetCmd(NETINT32 cmd);

	int GetDataLen() const { return header->len; }
	NETINT8  read8() const;
	NETINT16 read16() const;
	NETINT32 read32() const;
	const char * readString() const;
	const char * readBinary(int len) const; 
	void setCursor(int c) const{ if (c >= 0 && c <= header->len) cursor = c; }
	void skipCursor(int n) const{ cursor += n; if (cursor > header->len) cursor = header->len; }
	bool isEnd() const { return cursor >= header->len; };

	void write8(NETINT8 n);
	void write16(NETINT16 n);
	void write32(NETINT32 n);
	void writeString(const char * s);
	void writeBinary(const char * buf, int len);

	CNetPacket(int max_len);
	CNetPacket(const TNetPacketHeader * header, const char * data);
	CNetPacket(const char * buf, int len);

	~CNetPacket();
};