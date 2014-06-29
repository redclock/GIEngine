#include <assert.h>
#include "Net.h"
#include <iostream>

using namespace std;

bool InitNet()
{
	WORD wVersionRequested;
	WSADATA wsaData;

	wVersionRequested = MAKEWORD( 1, 1 );

	if ( WSAStartup( wVersionRequested, &wsaData ) != 0 ) 
	{
		return false;
	}
	return true;
}

void CleanupNet()
{
	WSACleanup();
}

u_long GetLocalhost()
{
	u_long addr = 0;

	char host[256];
	if (0 == gethostname(host, sizeof(host)))
	{
		HOSTENT* HE = gethostbyname(host);
		if (HE == 0) 
		{
			addr = 0;
		}else
		{
			addr = *((u_long*)HE->h_addr_list[0]);
		}
	}
	return addr;
}

u_long GetHostAddress(const char * host)
{
	u_long addr = inet_addr(host);   // try and parse it if it is an IP address
	if (addr == INADDR_NONE)
	{
		// Host isn't an IP address, try using DNS
		hostent* HE = gethostbyname(host);
		if (HE == 0) 
		{
			addr = 0;
		}else
		{
			addr = *((u_long*)HE->h_addr_list[0]);
		}
	}
	return addr;
	
}

CSocket::CSocket(): m_socket(INVALID_SOCKET)
{
	
}

CSocket::CSocket(const SOCKET & sock): m_socket(sock)
{

}

CSocket::CSocket(const CSocket & sock): m_socket(sock.m_socket)
{

}

CSocket & CSocket::operator =(const CSocket & sock)
{
	m_socket = sock.m_socket;
	return *this;
}

CSocket & CSocket::operator =(SOCKET sock)
{
	m_socket = sock;
	return *this;
}

CSocket::~CSocket()
{
	//Close();
}

CSocket CSocket::CreateTCP()
{
	struct protoent *ppe;
	ppe = getprotobyname("tcp");
	SOCKET sock = socket(PF_INET, SOCK_STREAM, ppe->p_proto);

	return CSocket(sock);
}

CSocket CSocket::CreateUDP()
{
	struct protoent *ppe;
	ppe = getprotobyname("udp");
	SOCKET sock = socket(PF_INET, SOCK_DGRAM, ppe->p_proto);

	return CSocket(sock);
}

bool CSocket::Bind(const SOCKADDR_IN& sockAddr)
{
	assert(m_socket != INVALID_SOCKET);
	return bind(m_socket, (LPSOCKADDR)&sockAddr, sizeof(sockAddr)) != SOCKET_ERROR;
}

bool CSocket::Bind(WORD port, u_long ip)
{
	assert(m_socket != INVALID_SOCKET);
	SOCKADDR_IN sockAddr;
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(port);
	sockAddr.sin_addr.s_addr = (ip);
	return bind(m_socket, (LPSOCKADDR)&sockAddr, sizeof(sockAddr)) != SOCKET_ERROR;
}

bool CSocket::Bind(WORD port, const char * ip)
{
	assert(m_socket != INVALID_SOCKET);
	SOCKADDR_IN sockAddr;
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(port);
	sockAddr.sin_addr.s_addr = inet_addr(ip);
	return bind(m_socket, (LPSOCKADDR)&sockAddr, sizeof(sockAddr)) != SOCKET_ERROR;

}

bool CSocket::Listen(int backlog)
{
	assert(m_socket != INVALID_SOCKET);
	return listen(m_socket, backlog) != SOCKET_ERROR;
}

bool CSocket::Connect(const SOCKADDR_IN& sockAddr)
{
	assert(m_socket != INVALID_SOCKET);
	return connect(m_socket, (LPSOCKADDR)&sockAddr, sizeof(sockAddr)) != SOCKET_ERROR;
}

bool CSocket::Connect(WORD port, u_long ip)
{
	assert(m_socket != INVALID_SOCKET);
	SOCKADDR_IN sockAddr;
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(port);
	sockAddr.sin_addr.s_addr = (ip);
	
	return connect(m_socket, (LPSOCKADDR)&sockAddr, sizeof(sockAddr)) != SOCKET_ERROR;
}

bool CSocket::Connect(WORD port, const char * ip)
{
	assert(m_socket != INVALID_SOCKET);
	SOCKADDR_IN sockAddr;
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(port);
	sockAddr.sin_addr.s_addr = inet_addr(ip);
	return connect(m_socket, (LPSOCKADDR)&sockAddr, sizeof(sockAddr)) != SOCKET_ERROR;
}

CSocket CSocket::Accept(SOCKADDR_IN * addr)
{
	CSocket sock;
	int len = sizeof(SOCKADDR_IN);
	sock.m_socket = accept(m_socket, (sockaddr *)addr, &len);
	return sock;
}

int CSocket::Send(const void * buf, int len, int flags)
{
	assert(m_socket != INVALID_SOCKET);
	return send(m_socket, (const char *) buf, len, flags);
}

int CSocket::Send(const CNetPacket *packet)
{
	return Send(packet->GetBuf(), packet->GetDataLen() + sizeof(TNetPacketHeader));
}

int CSocket::Receive(void * buf, int len, int flags)
{
	assert(m_socket != INVALID_SOCKET);
	return recv(m_socket, (char *) buf, len, flags);
}

int CSocket::SendTo(const void * buf, int len, const SOCKADDR_IN& sockTo, int flags)
{
	assert(m_socket != INVALID_SOCKET);
	return sendto(m_socket, (char *)buf, len, flags, (sockaddr *)&sockTo, sizeof(SOCKADDR_IN));
}

int CSocket::SendTo(CNetPacket * packet, const SOCKADDR_IN& sockTo)
{
	u_long ip;
	WORD port;
	GetLocalName(ip, port);
	packet->GetHeader()->addr = ip;
	packet->GetHeader()->port = port;
	return SendTo(packet->GetBuf(), packet->GetDataLen() + sizeof(TNetPacketHeader), sockTo);
}

int CSocket::SendBroadcast(const void * buf, int len, WORD port, int flags)
{
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
	addr.sin_port = htons(port);
	return SendTo(buf, len, addr, flags);
}

int CSocket::SendBroadcast(CNetPacket * packet, WORD port)
{
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
	addr.sin_port = htons(port);
	return SendTo(packet, addr);
}

int CSocket::ReceiveFrom(void * buf, int len, SOCKADDR_IN& sockFrom, int flags)
{
	assert(m_socket != INVALID_SOCKET);
	int addr_len = sizeof(SOCKADDR_IN);
	return recvfrom(m_socket, (char *)buf, len, flags, (sockaddr *)&sockFrom, &addr_len);
}

void CSocket::Close()
{
	if (m_socket != INVALID_SOCKET)
	{
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}
}

bool CSocket::GetLocalName(u_long & addr, WORD & port)
{
	assert(m_socket != INVALID_SOCKET);
	SOCKADDR_IN saddr;
	int len = sizeof(saddr);
	bool r = getsockname(m_socket, (sockaddr *)&saddr, &len) != SOCKET_ERROR;
	addr = saddr.sin_addr.s_addr;
	port = ntohs(saddr.sin_port);
	return r;
}

bool CSocket::GetRemoteName(u_long & addr, WORD & port)
{
	assert(m_socket != INVALID_SOCKET);
	SOCKADDR_IN saddr;
	int len = sizeof(saddr);
	bool r = getpeername(m_socket, (sockaddr *)&saddr, &len) != SOCKET_ERROR;
	addr = saddr.sin_addr.s_addr;
	port = ntohs(port);
	return r;
}

void CSocket::SetBroadcast(bool broadcast)
{
	BOOL fBroadcast = (BOOL)broadcast;
	setsockopt(m_socket, SOL_SOCKET, SO_BROADCAST, (char*)&fBroadcast, sizeof(BOOL)); 
}