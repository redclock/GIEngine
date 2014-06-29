#pragma once
#include <windows.h>
#include "NetPacket.h"

class CSocket
{
protected:
	SOCKET		m_socket;


public:
	CSocket();
	CSocket(const CSocket & sock);
	CSocket(const SOCKET & sock);
	~CSocket();

	static CSocket CreateTCP();
	static CSocket CreateUDP();
	
	void Close();
	bool Bind(const SOCKADDR_IN& sockAddr);
	bool Bind(WORD port, u_long ip = INADDR_ANY);
	bool Bind(WORD port, const char * ip);

	bool Listen(int backlog = SOMAXCONN);

	bool Connect(const SOCKADDR_IN& sockAddr);
	bool Connect(WORD port, u_long ip = INADDR_ANY);
	bool Connect(WORD port, const char * ip);

	CSocket Accept(SOCKADDR_IN * addr = NULL);

	void IOCtl(long command, UINT32* argument);

	int Receive(void * buf, int len, int flags = 0);
	int Send(const void * buf, int len, int flags = 0);
	int Send(const CNetPacket * packet);

	int ReceiveFrom(void * buf, int len, SOCKADDR_IN& sockFrom, int flags = 0);
	int SendTo(const void * buf, int len, const SOCKADDR_IN& sockTo, int flags = 0);
	int SendTo(CNetPacket * packet, const SOCKADDR_IN& sockTo);

	int SendBroadcast(const void * buf, int len, WORD port, int flags = 0);
	int SendBroadcast(CNetPacket * packet, WORD port);

	void SetNonBlock(bool nonBlock);
	void SetBroadcast(bool broadcast);

	void SetTimeout(UINT32 microSecs);
	UINT32 GetTimeout();

	CSocket & operator = (const CSocket & sock);
	CSocket & operator = (SOCKET sock);

	bool GetLocalName(u_long & addr, WORD & port);
	bool GetRemoteName(u_long & addr, WORD & port);

	bool IsAvailable() const { return m_socket != INVALID_SOCKET; }
};

bool InitNet();
void CleanupNet();

u_long GetLocalhost();
u_long GetHostAddress(const char * host);