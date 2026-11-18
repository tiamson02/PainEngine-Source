
#ifndef __PKTVTRANSPORTFILE_H__
#define __PKTVTRANSPORTFILE_H__

#include "PKTVTransport.h"
#include "Core/LeString.h"

class FileObject;

class PKTVTransportFile : public PKTVTransport
{
public:
	PKTVTransportFile( String filename, bool forReading );

	// Specific file-read/write part
	bool IsForReading( void ) { return mReading; }

	virtual ~PKTVTransportFile( void );

	virtual bool Ready( void );	// file open and ready to read/write
	virtual bool ReceiveChunk( unsigned char& chunkType, UdpMessage** chunk );
	virtual void SendChunk( unsigned char chunkType, UdpMessage* chunk );
private:
	FileObject* mFile;
	bool mReading;
};

#endif	// __PKTVTRANSPORTFILE_H__

