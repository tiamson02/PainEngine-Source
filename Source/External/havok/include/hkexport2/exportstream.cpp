/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkexport2/hkexport.h>
#include <hkexport2/exportstream.h>

using namespace NS_ExportStream;

// VERSION 1310 -> 1320  = MARKERS WERE ADDED & CONSTRAINTS WERE CHANGED
// VERSION 1320 -> 1330  = EXPORT POINT TO POINT HAS A SINGLE BODY BOOL EXPORTED
// VERSION 1330 -> 1340 = EXPORT NEW DEACTIVATOR AND CONSTRAINTS STUFF PROPERLY
// VERSION 1340 -> 1350 = EXPORT COLLISION GROUPS INFORMATION ~ EXP_PRIMITIVES, new EXP_COLLISIONGROUPS, Fast_Subspace
// VERSION 1350 -> 1360 = DEFLECTORS AND FAST SOFT ADDED
// VERSION 1360 -> 1370 = Constraints updated to handle constraint spaces, breakable constraints and new strength/tau

#define CURRENT_EXPORT_VERSION	1370


// Constructors
Token::Token()
{
	priv_string=HK_NULL;
}

Token::Token(const char *string, Token::TokenType type)
{
	priv_string=hkString::strDup(string);
	priv_value=PRIV_HashFunction(string);
	priv_is_checkpoint= (type==CHECKPOINT);

}

Token::Token(int value, Token::TokenType type)
{ 
	priv_string=HK_NULL;
	priv_value=value;
	priv_is_checkpoint= (type==CHECKPOINT);
}

// Destructors
Token::~Token()
{
	if (priv_string) hkDeallocate(priv_string);
}

//

const char* Token::GetString() const { return priv_string; }

void Token::SetString(const char *string)
{
	if (priv_string) hkDeallocate(priv_string);
	priv_string=hkString::strDup(string);
	priv_value=PRIV_HashFunction(string);
}

void Token::SetValue(int value)
{
	if (priv_string) { hkDeallocate(priv_string); priv_string=HK_NULL; }
	priv_value=value;
}

int Token::GetValue() const { return priv_value; }

void Token::SetIsCheckpoint(bool is_checkpoint)
{
	priv_is_checkpoint=is_checkpoint;
}

bool Token::GetIsCheckpoint() const
{
	return priv_is_checkpoint;
}

Token &Token::operator=(const Token &another_token)
{
	priv_is_checkpoint = another_token.priv_is_checkpoint;
	if (priv_string) hkDeallocate(priv_string);
	priv_string = hkString::strDup(another_token.priv_string);
	priv_value = another_token.priv_value;
	
	return *this;
}


bool Token::operator==(const Token &another_token)	const { return priv_value==another_token.priv_value; }
bool Token::operator==(const char *a_string)		const { return priv_value==PRIV_HashFunction(a_string); }

bool Token::operator!=(const Token &another_token)	const { return !(another_token==*this);}
bool Token::operator!=(const char *a_string)		const { return !((*this)==a_string);}

bool Token::Equal(const Token &another_token)		const { return !hkString::strCmp(priv_string,another_token.priv_string); }
bool Token::Equal(const char *a_string)			const { return !hkString::strCmp(priv_string,a_string); }

int Token::PRIV_HashFunction(const char *string) const 
{
    unsigned long   h = 0, g;
    static int M = 2147483647; // 2^31 -1

    while (*string)
    {
        h = ( h << 4 ) + *string++;
        if ((g = h & 0xF0000000L) != 0)
            h ^= g >> 24;

        h &= ~g;
    }
    return int(h % M);
}

void ExportStream::SpacesIn(char *str)
{
	for (;*str;str++)
	{
		if ((unsigned char) *str== (unsigned char) 128) *str=(unsigned char) 32;
	}
}

char *ExportStream::SpacesOut(const char *str)
{
	char* result=hkString::strDup(str);

	unsigned char* cit = reinterpret_cast<unsigned char*>(result);
	for ( ; *cit; ++cit)
	{
		if ( *cit < 33)
		{
			*cit = 128;
		}
	}
	return result;
}

ExportStream::ExportStream(enum ExportStreamType type)
{
	prot_type = type;
	prot_version = CURRENT_EXPORT_VERSION;
}

/*
* Havok Game Dynamics SDK - PUBLIC RELEASE, BUILD(#20030622)
*
* (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. The Telekinesys
* Logo, Havok and the Havok buzzsaw logo are trademarks of Telekinesys
* Research Limited.  Title, ownership
* rights, and intellectual property rights in the Software remain in
* Telekinesys Research Limited and/or its suppliers.
*
* Use of this software is subject to and indicates acceptance of the End User licence
* Agreement for this product. A copy of the license is included with this software and
* is also available from info@havok.com.
*
*/ 
