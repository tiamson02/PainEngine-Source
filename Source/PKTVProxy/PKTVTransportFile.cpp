
#include "Engine.h"

#include "PKTVTransportFile.h"
#include "UdpMessage.h"

#include <stdio.h>

#if defined( TRANSGAMING_PSP )
# include "TNG_stdio_PSP.h"
#elif defined( TRANSGAMING_PS2 )
# include "TNG_stdio_PS2.h"
#elif defined( TRANSGAMING_WINDOWS )
# include "../TNG_WIN/TNG_stdio_WIN.h"
#endif

PKTVTransportFile::PKTVTransportFile( String filename, bool forReading )
:	mReading( forReading )
{
#ifdef TRANSGAMING
	mFile = (void*)TNG_fopen( filename.AsChar(), forReading?"rb":"wb" );
#else
	mFile = (void*)fopen( filename.AsChar(), forReading?"rb":"wb" );
#endif
}

PKTVTransportFile::~PKTVTransportFile( void )
{
	if( mFile )
#ifdef TRANSGAMING
		TNG_fclose( (FILE*)mFile );
#else
		fclose( (FILE*)mFile );
#endif
	mFile = 0;
}

bool PKTVTransportFile::ReceiveChunk( unsigned char& chunkType, UdpMessage** chunk )
{
#ifdef TRANSGAMING
	if( TNG_fread( &chunkType, 1, sizeof( unsigned char ), (FILE*)mFile ) != sizeof(unsigned char) )
#else
	if( fread( &chunkType, 1, sizeof( unsigned char ), (FILE*)mFile ) != sizeof(unsigned char) )
#endif
		return false;

	int size;
#ifdef TRANSGAMING
	if( TNG_fread( &size, 1, sizeof( int ), (FILE*)mFile ) != sizeof( int ) )
#else
	if( fread( &size, 1, sizeof( int ), (FILE*)mFile ) != sizeof( int ) )
#endif
		return false;

	if( size > 10000000 )
		return false;	// corrupt stream!!!

	char* buffer = new char[size];
#ifdef TRANSGAMING
	if( TNG_fread( buffer, 1, size, (FILE*)mFile ) != size )
#else
	if( fread( buffer, 1, size, (FILE*)mFile ) != size )
#endif
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
#ifdef TRANSGAMING
	TNG_fwrite( &chunkType, 1, sizeof(unsigned char), (FILE*)mFile );
#else
	fwrite( &chunkType, 1, sizeof(unsigned char), (FILE*)mFile );
#endif
	int size = chunk->GetSize();
#ifdef TRANSGAMING
	TNG_fwrite( &size, 1, sizeof(int), (FILE*)mFile );
	TNG_fwrite( chunk->GetBuffer(), 1, chunk->GetSize(), (FILE*)mFile );
#else
	fwrite( &size, 1, sizeof(int), (FILE*)mFile );
	fwrite( chunk->GetBuffer(), 1, chunk->GetSize(), (FILE*)mFile );
#endif
}

bool PKTVTransportFile::Ready( void ) { return ( mFile != 0 ); }
