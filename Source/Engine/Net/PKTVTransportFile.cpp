
#include "Engine.h"
#include "PKTVTransportFile.h"
#include "UdpMessage.h"
#include "FileSystem/GFileManager.h"
extern ENGINE_API GFileManager  	GFileMan;

PKTVTransportFile::PKTVTransportFile( String filename, bool forReading )
:	mReading( forReading )
{
	mFile = mReading?GFileMan.CreateFileReader( filename.AsChar() ):GFileMan.CreateFileWriter( filename.AsChar() );
}

PKTVTransportFile::~PKTVTransportFile( void )
{
	if( mFile )
		delete mFile;	// this should close it
	mFile = 0;
}

bool PKTVTransportFile::ReceiveChunk( unsigned char& chunkType, UdpMessage** chunk )
{
	if( !mFile->Read( &chunkType, sizeof( unsigned char ) ) )
		return false;

	int size;
	if( !mFile->Read( &size, sizeof( int ) ) )
		return false;

	if( size > 10000000 )
		return false;	// corrupt stream!!!

	char* buffer = new char[size];
	if( !mFile->Read( buffer, size ) )
	{
		delete[] buffer;
		return false;
	}

	(*chunk) = new UdpMessage( buffer, size, true );
	
	return true;
}

void PKTVTransportFile::SendChunk( unsigned char chunkType, UdpMessage* chunk )
{
	chunk->SeekToBegin();
	mFile->Write( &chunkType, sizeof(unsigned char) );
	int size = chunk->GetSize();
	mFile->Write( &size, sizeof(int) );
	mFile->Write( chunk->GetBuffer(), chunk->GetSize() );
}

bool PKTVTransportFile::Ready( void ) { return ( mFile != 0 ); }
