
#pragma once

#include "LeString.h"
#include "HashMap.h"

class IpAddress
{
public:
	union {
		DWORD	IP;
		struct	{ BYTE ip1,ip2,ip3,ip4; };
	};
	unsigned short Port;

	IpAddress()
	{}
	IpAddress(BYTE a, BYTE b, BYTE c, BYTE d, unsigned short port)
	:	ip1(a), ip2(b), ip3(c), ip4(d), Port(port)
	{}

	bool operator==(const IpAddress& addr) const { return (IP == addr.IP) && (Port == addr.Port); };
	IpAddress& operator=(const IpAddress& addr) { IP = addr.IP; Port = addr.Port; return *this; };
	String AsString( void ) const { return String::Sprintf( "%d.%d.%d.%d:%d", ip1, ip2, ip3, ip4, Port ); };
	String AsStringIP( void ) const { return String::Sprintf( "%d.%d.%d.%d", ip1, ip2, ip3, ip4 ); };
	void FillSockaddrIn( struct sockaddr_in& addr );
};

template <>
inline DWORD HashFunc<IpAddress>(const IpAddress& addr)
{
	char buf[4];
	buf[0] = addr.ip1;
	buf[1] = addr.ip1;
	buf[2] = addr.ip1;
	buf[3] = addr.ip1;
    return Crc32(buf,4);
}

String GetSocketErrorString( void );	// translate WSA last error to string

// For stats
struct DlUl
{
	ulong32 BytesDL;
	ulong32 BytesUL;
	ulong32 PacketsDL;
	ulong32 PacketsUL;

	DlUl() : BytesDL(0), BytesUL(0), PacketsDL(0), PacketsUL(0) {};
	inline DlUl& operator+=( const DlUl& other )
	{
		BytesDL += other.BytesDL;
		PacketsDL += other.PacketsDL;
		BytesUL += other.BytesUL;
		PacketsUL += other.PacketsUL;
		return *this;
	}
	inline DlUl& operator/=( ulong32 num )
	{
		BytesDL /= num;
		PacketsDL /= num;
		BytesUL /= num;
		PacketsUL /= num;
		return *this;
	}
};
