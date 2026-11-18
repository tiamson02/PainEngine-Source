
#include "Engine.h"
#include "PKTVStringDict.h"
#include "UdpMessage.h"

#include "Core/FileSystem/FileManagerStandard.h"
extern ENGINE_API FileManagerStd GFileMan;

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

#define PKTV_DICT_APPEND

#ifdef PKTV_DICT_APPEND

GFile* PKTV_AppendFile = 0;

extern void PKTV_Log( const char* fmt, ... );
extern void PKTV_Error( const char* fmt, ... );

PKTVStringDictionary::PKTVStringDictionary( void )
{
	char filename[] = "..\\Data\\PKTVStrings.dic";

	PKTV_AppendFile = 0;
	char str[65535];
	unsigned short strSize;

	// Open this file for reading, read, close file
	FileObject* f = GFileMan.CreateFileReader( filename );
	if( f )
	{
		long32 size = f->GetSize();
		long32 read = 0;
		while( read < size )
		{
			f->Read( &strSize, sizeof(strSize) );
			read += 2;
			f->Read( &str, strSize );
			read += strSize;
			str[strSize] = 0;

			mStringHash.Insert( String(str), mStringHash.Size() );
		}
		delete f;
	}
	else if( strlen( filename ) )
		GLog.Print( "Warning: file '%s' not found!!!\n", filename );

	PKTV_AppendFile = GFileMan.CreateFileWriter( filename, "ab" );
	if( !PKTV_AppendFile )
		GLog.Print( "Warning: couldn't open file '%s' for appending!\n", filename );
}

PKTVStringDictionary::~PKTVStringDictionary( void )
{
	// if file is openend for append, close it
	if( PKTV_AppendFile )
	{
		delete PKTV_AppendFile;
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
		PKTV_AppendFile->Write( &strLen, sizeof(unsigned short) );
		PKTV_AppendFile->Write( str.AsChar(), strLen );
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
		PKTV_AppendFile->Write( &strLen, sizeof(unsigned short) );
		PKTV_AppendFile->Write( str.AsChar(), strLen );
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
	FileObject* f = GFileMan.CreateFileReader( filename );
	if( f )
	{
		long32 size = f->GetSize();
		long32 read = 0;
		while( read < size )
		{
			f->Read( &strSize, sizeof(strSize) );
			read += 2;
			f->Read( &str, strSize );
			read += strSize;
			str[strSize] = 0;

			mStringHash.Insert( String(str), mStringHash.Size() );
		}
		delete f;
	}
	else if( strlen( filename ) )
		PKTV_Log( "Error: dictionary file '%s' not found!!!", filename );
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
