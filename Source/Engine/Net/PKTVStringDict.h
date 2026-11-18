
#ifndef _PKTV_STRING_DICT_H_
#define _PKTV_STRING_DICT_H_

#include "Core/LeString.h"
#include "Core/HashMap.h"

class UdpMessage;

class PKTVStringDictionary
{
public:
	PKTVStringDictionary( void );
	~PKTVStringDictionary( void );

	void WriteString( const String& str, UdpMessage* mesgToWriteIn );
	String ReadString( UdpMessage* mesgToReadFrom );

private:
	HashMap<String,ulong32> mStringHash;
};

#endif	// _PKTV_STRING_DICT_H_
