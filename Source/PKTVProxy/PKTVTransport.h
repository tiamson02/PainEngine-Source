
#ifndef __PKTVTRANSPORT_H__
#define __PKTVTRANSPORT_H__

#include <winsock2.h>

class UdpMessage;

class PKTVTransport
{
public:

	enum
	{
		CHUNK_FRAME = 0,
		CHUNK_MESSAGE = 1,
		CHUNK_COMMAND = 2,
		CHUNK_HEADER = 3,
	};

	PKTVTransport( void ) {};
	virtual ~PKTVTransport( void ) {};

	virtual bool Ready( void ) = 0;	// ready to stream?
	virtual bool ReceiveChunk( unsigned char& chunkType, UdpMessage** chunk ) = 0;
	virtual void SendChunk( unsigned char chunkType, UdpMessage* chunk ) = 0;
};

#endif	// __PKTVTRANSPORT_H__

