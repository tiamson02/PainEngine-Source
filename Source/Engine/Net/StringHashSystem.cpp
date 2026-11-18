
#include "Engine.h"
#include "Net/UdpMessage.h"
#include "Net/StringHashSystem.h"

#include "Engine/PCFSystem.h"
extern ENGINE_API PCFSystem* GEngine;


#if defined(TRANSGAMING) && defined(FIND_HASH_PEAK)
int StringHashSystem2::mMaxStringToNumberUsage = 0;
int StringHashSystem2::mMaxNumberToStringUsage = 0;
#endif


void StringHashSystem2::WriteString( UdpMessage* message, String& string )
{
	unsigned short* index = StringToNumber.Find(string);
	if( !index )
	{
		Assert( mNumber < 65535 );
		StringToNumber.Insert( string, mNumber );
#if defined(TRANSGAMING) && defined(FIND_HASH_PEAK)
        if (StringToNumber.Size() > mMaxStringToNumberUsage){
            mMaxStringToNumberUsage = StringToNumber.Size();
            TRACE("@@@ StringHashSystem2::WriteString(): peak StringToNumber hash usage = %d\n", mMaxStringToNumberUsage);
        }
#endif

		message->WriteUShort( mNumber++ );
		message->WriteString( string.AsChar() );
	}
	else if( *index >= mFirstUnsureNumberToSend )
	{
		message->WriteUShort( *index );
		message->WriteString( string.AsChar() );
	}
	else
		message->WriteUShort( *index );
}

String StringHashSystem2::ReadString( UdpMessage* message )
{
	unsigned short index = message->ReadUShort();
	String* string = NumberToString.Find(index);

	if( !string )
	{
		String str2 = message->ReadString();
		NumberToString.Insert( index, str2 );

#if defined(TRANSGAMING) && defined(FIND_HASH_PEAK)
        if (NumberToString.Size() > mMaxNumberToStringUsage){
            mMaxNumberToStringUsage = NumberToString.Size();
            TRACE("@@@ StringHashSystem2::ReadString(): peak NumberToString hash usage = %d\n", mMaxNumberToStringUsage);
        }
#endif

		return str2;
	}
	else if( index >= mFirstUnsureNumberToReceive )
	{
		String str2 = message->ReadString();
		Assert( str2 == (*string) );
		return *string;
	}
	else
		return *string;
}

void StringHashSystem2::Clear( void )
{
	StringToNumber.Clear();
	NumberToString.Clear();
	mNumber = 0;
	mFirstUnsureNumberToSend = 0;
	mFirstUnsureNumberToReceive = 0;
}

StringHashSystem2& StringHashSystem2::operator=( const StringHashSystem2& other )
{
	Clear();
	mNumber = other.mNumber;
	mFirstUnsureNumberToSend = other.mFirstUnsureNumberToSend;
	mFirstUnsureNumberToReceive = other.mFirstUnsureNumberToReceive;

	int size = other.StringToNumber.Size();
	for( int i = 0; i < size; ++i )
	{
#ifdef TRANSGAMING
		const HashMap<String,unsigned short,1024>::HashPair& pair = other.StringToNumber.GetPair(i);
#else
		const HashMap<String,unsigned short,1024>::HashPair& pair = other.StringToNumber.GetPair(i);
#endif
		StringToNumber.Insert( pair.Key, pair.Value );
	}


	size = other.NumberToString.Size();
	for( int i = 0; i < size; ++i )
	{
#ifdef TRANSGAMING
		const HashMap<unsigned short,String,1024>::HashPair& pair = other.NumberToString.GetPair(i);
#else
		const HashMap<unsigned short,String,1024>::HashPair& pair = other.NumberToString.GetPair(i);
#endif
		NumberToString.Insert( pair.Key, pair.Value );
	}

#if defined(TRANSGAMING) && defined(FIND_HASH_PEAK)
    if (StringToNumber.Size() > mMaxStringToNumberUsage){
        mMaxStringToNumberUsage = StringToNumber.Size();
        TRACE("@@@ StringHashSystem2::operator=(): peak StringToNumber hash usage = %d\n", mMaxStringToNumberUsage);
    }

    if (NumberToString.Size() > mMaxNumberToStringUsage){
        mMaxNumberToStringUsage = NumberToString.Size();
        TRACE("@@@ StringHashSystem2::operator=(): peak NumberToString hash usage = %d\n", mMaxNumberToStringUsage);
    }
#endif

	return *this;
}
