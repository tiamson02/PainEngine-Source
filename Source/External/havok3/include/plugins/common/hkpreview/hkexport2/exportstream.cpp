/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
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

// VERSION 1370 -> 2000 = HAVOK2 ONLY. SUPPORT FOR CAPSULES INFO IN GEOMETRIC PRIMITIVES

#define CURRENT_EXPORT_VERSION	2000 


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

void Token::SetIsCheckpoint(hkBool is_checkpoint)
{
	priv_is_checkpoint=is_checkpoint;
}

hkBool Token::GetIsCheckpoint() const
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


hkBool Token::operator==(const Token &another_token)	const { return priv_value==another_token.priv_value; }
hkBool Token::operator==(const char *a_string)		const { return priv_value==PRIV_HashFunction(a_string); }

hkBool Token::operator!=(const Token &another_token)	const { return !(another_token==*this);}
hkBool Token::operator!=(const char *a_string)		const { return !((*this)==a_string);}

hkBool Token::Equal(const Token &another_token)		const { return !hkString::strCmp(priv_string,another_token.priv_string); }
hkBool Token::Equal(const char *a_string)			const { return !hkString::strCmp(priv_string,a_string); }

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
* Havok SDK - PUBLIC RELEASE, BUILD(#20060114)
*
* Confidential Information of Havok.  (C) Copyright 1999-2006 
* Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
* Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
* rights, and intellectual property rights in the Havok software remain in
* Havok and/or its suppliers.
*
* Use of this software for evaluation purposes is subject to and indicates 
* acceptance of the End User licence Agreement for this product. A copy of 
* the license is included with this software and is also available from salesteam@havok.com.
*
*/
