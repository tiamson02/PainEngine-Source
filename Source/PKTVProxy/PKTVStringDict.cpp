
#include "Engine.h"
#include "PKTVStringDict.h"
#include "UdpMessage.h"

#include "LogBuffer.h"

#if defined( TRANSGAMING_PSP )
# include "TNG_stdio_PSP.h"
#elif defined( TRANSGAMING_PS2 )
# include "TNG_stdio_PS2.h"
#elif defined( TRANSGAMING_WINDOWS )
# include "../TNG_WIN/TNG_stdio_WIN.h"
#endif

#define PKTV_DICT_APPEND

#ifdef PKTV_DICT_APPEND

FILE* PKTV_AppendFile = 0;

PKTVStringDictionary::PKTVStringDictionary( void )
{
	char filename[] = "PKTVStrings.dic";

	PKTV_AppendFile = 0;
	char str[65535];
	unsigned short strSize;

	// Open this file for reading, read, close file
#ifdef TRANSGAMING
	FILE* f = TNG_fopen( filename, "rb" );
#else
	FILE* f = fopen( filename, "rb" );
#endif
	if( f )
	{
#ifdef TRANSGAMING
		TNG_fseek( f, 0, SEEK_END );
		long32 size = TNG_ftell( f );
		TNG_fseek( f, 0, SEEK_SET );
#else
		fseek( f, 0, SEEK_END );
		long32 size = ftell( f );
		fseek( f, 0, SEEK_SET );
#endif
		long32 read = 0;
		while( read < size )
		{
#ifdef TRANSGAMING
			TNG_fread( &strSize, sizeof(strSize),1, f );
			read += 2;
			TNG_fread( &str, strSize, 1, f );
			read += strSize;
#else
			fread( &strSize, sizeof(strSize),1, f );
			read += 2;
			fread( &str, strSize, 1, f );
			read += strSize;
#endif
			str[strSize] = 0;

			mStringHash.Insert( String(str), mStringHash.Size() );
		}
#ifdef TRANSGAMING
		TNG_fclose( f );
#else
		fclose( f );
#endif
	}
	else if( strlen( filename ) )
		GLog->Print( "Warning: file '%s' not found!!!\n", filename );

#ifdef TRANSGAMING
	PKTV_AppendFile = TNG_fopen( filename, "ab" );
#else
	PKTV_AppendFile = fopen( filename, "ab" );
#endif
	if( !PKTV_AppendFile )
		GLog->Print( "Warning: couldn't open file '%s' for appending!\n", filename );
}

PKTVStringDictionary::~PKTVStringDictionary( void )
{
	// if file is openend for append, close it
	if( PKTV_AppendFile )
	{
#ifdef TRANSGAMING
		TNG_fclose( PKTV_AppendFile );
#else
		fclose( PKTV_AppendFile );
#endif
		PKTV_AppendFile = 0;
	}
}

void PKTVStringDictionary::WriteString( const String& str, UdpMessage* mesgToWriteIn )
{
	mesgToWriteIn->WriteString( str.AsChar() );
	if( !PKTV_AppendFile )
		return;

	if( !mStringHash.Find( str ) )
	{
		mStringHash.Insert( str, mStringHash.Size() );

		// Append the string and number to the file
		unsigned short strLen = str.Len();
#ifdef TRANSGAMING
		TNG_fwrite( &strLen, sizeof(unsigned short), 1, PKTV_AppendFile );
		TNG_fwrite( str.AsChar(), strLen, 1, PKTV_AppendFile );
#else
		fwrite( &strLen, sizeof(unsigned short), 1, PKTV_AppendFile );
		fwrite( str.AsChar(), strLen, 1, PKTV_AppendFile );
#endif
	}
}

String PKTVStringDictionary::ReadString( UdpMessage* mesgToReadFrom )
{
	String str = mesgToReadFrom->ReadString();
	if( !PKTV_AppendFile )
		return str;

	if( !mStringHash.Find( str ) )
	{
		mStringHash.Insert( str, mStringHash.Size() );

		// Append the string and number to the file
		unsigned short strLen = str.Len();
#ifdef TRANSGAMING
		TNG_fwrite( &strLen, sizeof(unsigned short), 1, PKTV_AppendFile );
		TNG_fwrite( str.AsChar(), strLen, 1, PKTV_AppendFile );
#else
		fwrite( &strLen, sizeof(unsigned short), 1, PKTV_AppendFile );
		fwrite( str.AsChar(), strLen, 1, PKTV_AppendFile );
#endif

	}
	return str;
}

#else

PKTVStringDictionary::PKTVStringDictionary( void )
{
	char filename[] = "Data\\PKTVStrings.dic";

	char str[65535];
	unsigned short strSize;

	// Open this file for reading, read, close file
#ifdef TRANSGAMING
	FILE* f = TNG_fopen( filename, "rb" );
#else
	FILE* f = fopen( filename, "rb" );
#endif
	if( f )
	{
#ifdef TRANSGAMING
		TNG_fseek( f, 0, SEEK_END );
		TNG_long size = TNG_ftell( f );
		TNG_fseek( f, 0, SEEK_SET );
#else
		fseek( f, 0, SEEK_END );
		long32 size = ftell( f );
		fseek( f, 0, SEEK_SET );
#endif

		long32 read = 0;
		while( read < size )
		{
#ifdef TRANSGAMING
			TNG_fread( &strSize, sizeof(strSize),1, f );
			read += 2;
			TNG_fread( &str, strSize, 1, f );
			read += strSize;
#else
			fread( &strSize, sizeof(strSize),1, f );
			read += 2;
			fread( &str, strSize, 1, f );
			read += strSize;
#endif
			str[strSize] = 0;

			mStringHash.Insert( String(str), mStringHash.Size() );
		}
#ifdef TRANSGAMING
		TNG_fclose( f );
#else
		fclose( f );
#endif

	}
	else if( strlen( filename ) )
		GLog->Print( "Warning: file '%s' not found!!!\n", filename );
}

PKTVStringDictionary::~PKTVStringDictionary( void ) {}

void PKTVStringDictionary::WriteString( const String& str, UdpMessage* mesgToWriteIn )
{
	ulong32* val = mStringHash.Find( str );
	if( val )
		mesgToWriteIn->WriteULongOptimized( *val );
	else
	{
		mesgToWriteIn->WriteULongOptimized( ULONG_MAX );
		mesgToWriteIn->WriteString( str.AsChar() );
	}
}

String PKTVStringDictionary::ReadString( UdpMessage* mesgToReadFrom )
{
	ulong32 index = mesgToReadFrom->ReadULongOptimized();
	return ( index == ULONG_MAX )?mesgToReadFrom->ReadString():mStringHash.GetPair(index).Key;
}
#endif
