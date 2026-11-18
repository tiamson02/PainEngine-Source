
// #pragma once
#ifndef _STRINGHASHSYSTEM_
#define _STRINGHASHSYSTEM_


#include "Core/HashMap.h"
#include "Net/UdpMessage.h"

class ENGINE_API StringHashSystem2
{
public:
	StringHashSystem2() : mNumber( 0 ), mFirstUnsureNumberToSend( 0 ), mFirstUnsureNumberToReceive( 0 ) {};
	~StringHashSystem2() {};

	void SetFirstUnsureStringNumberToSend( unsigned short number ) { mFirstUnsureNumberToSend = number; };
	void SetFirstUnsureStringNumberToReceive( unsigned short number ) { mFirstUnsureNumberToReceive = number; };
	unsigned short GetFirstUnusedStringNumber( void ) { return mNumber; };

	void WriteString( UdpMessage* message, String& string );
	String ReadString( UdpMessage* message );

	StringHashSystem2& operator=( const StringHashSystem2& other );
	void Clear( void );

protected:
	unsigned short mNumber;
	unsigned short mFirstUnsureNumberToSend;
	unsigned short mFirstUnsureNumberToReceive;
    
#ifdef TRANSGAMING
    //@@@// eric-> reduced the hash count on these tables for the PSP and PS2.  They shouldn't need to
    //@@@//   be too large on these platforms hopefully
    HashMap<String,unsigned short,1024> StringToNumber;
	HashMap<unsigned short,String,1024> NumberToString;
# ifdef FIND_HASH_PEAK
    static int mMaxStringToNumberUsage;
    static int mMaxNumberToStringUsage;
# endif
#else
    HashMap<String,unsigned short,1024> StringToNumber;
	HashMap<unsigned short,String,1024> NumberToString;
#endif

};


#endif //_STRINGHASHSYSTEM_

