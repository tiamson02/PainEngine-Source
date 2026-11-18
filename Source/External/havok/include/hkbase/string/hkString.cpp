/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


#include <hkbase/hkBase.h>
#include <ctype.h>

extern "C"
{
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
}

// very small strings will likely cause reallocations
static const int MINIMUM_STRING_CAPACITY = 64 - sizeof(hkString::Rep) - 1;

// Some platforms don't have vsnprintf, so we use vsprintf instead.
// vsprintf is not safe from overflows, so we include asserts.
// Win32 calls it _vsnprintf instead of vsprintf

#if defined(HK_PLATFORM_WIN32) || defined(HK_PLATFORM_XBOX)
#	define VSNPRINTF(BUF,LEN,FMT,ARGS) ::_vsnprintf(BUF,LEN,FMT,ARGS)
#elif defined(HK_PLATFORM_PS2)
#	define VSNPRINTF(BUF,LEN,FMT,ARGS) ::vsprintf(BUF,FMT,ARGS)
#else
#	define VSNPRINTF(BUF,LEN,FMT,ARGS) ::vsnprintf(BUF,LEN,FMT,ARGS)
#endif

// all the static methods go first so that they may be inlined
// in the nonstatic ones further on.

char HK_CALL hkString::toUpper( char c )
{
	return (c>='a'&&c<='z')
			? (char)(c - ('a' - 'A'))
			: c;
}
char HK_CALL hkString::toLower( char c )
{
	return (c>='A'&&c<='Z')
			? (char)(c + ('a' - 'A'))
			: c;
}

int HK_CALL hkString::vsnprintf( char* buf, int len, const char* fmt, hk_va_list hkargs)
{
	va_list args;
	
#ifndef HK_ARCH_PPC
	args = reinterpret_cast<va_list>(hkargs);
#else
	// to get arround void* --> array[1] reverse conversion we have to bypass the compiler 
	// in mwerks anyway
	hkString::memCpy(args,hkargs,sizeof(va_list));
#endif

	int ret = VSNPRINTF(buf, len, fmt, args);	
	HK_ASSERT(ret <= int(len) && "Buffer overflow in string formatting");
	return ret;
	
}



int HK_CALL hkString::snprintf( char* buf, int len, const char* fmt, ...)
{
	va_list vlist;
	va_start(vlist, fmt);
	int ret = VSNPRINTF(buf, len, fmt, vlist);
	HK_ASSERT(ret <= int(len) && "Buffer overflow in string formatting");
	va_end(vlist);
	return ret;
}

int HK_CALL hkString::sprintf( char* buf, const char* fmt, ...)
{
	va_list vlist;
	va_start(vlist, fmt);
	int ret = ::vsprintf(buf, fmt, vlist);
	va_end(vlist);
	return ret;
}

int HK_CALL hkString::strCmp( const char* a, const char* b )
{
	HK_ASSERT(a != HK_NULL);
	HK_ASSERT(b != HK_NULL);
	return ::strcmp(a,b); 
}

int HK_CALL hkString::strNcmp( const char* a, const char* b, int n )
{
	HK_ASSERT(a != HK_NULL);
	HK_ASSERT(b != HK_NULL);
	return ::strncmp(a,b,(unsigned)n);
}

int HK_CALL hkString::strCasecmp( const char* a, const char* b )
{
	HK_ASSERT(a != HK_NULL);
	HK_ASSERT(b != HK_NULL);
	int i = 0;
	while ( a[i] != 0 || b[i] != 0 )
	{
		if( toLower(a[i]) < toLower(b[i]) )
		{
			return -1;
		}
		else if( toLower(a[i]) > toLower(b[i]) )
		{
			return 1;
		}
		++i;
	}
	
	return 0;
}

int	HK_CALL hkString::strNcasecmp(const char* a, const char* b, int n)
{
	HK_ASSERT(a != HK_NULL);
	HK_ASSERT(b != HK_NULL);
	HK_ASSERT(n >= 0);

	int i = 0;
	while ( (a[i] != 0 || b[i] != 0) && i < n )
	{
		if( toLower(a[i]) < toLower(b[i]) )
		{
			return -1;
		}
		else if( toLower(a[i]) > toLower(b[i]) )
		{
			return 1;
		}
		++i;
	}
	
	return 0;
}

void HK_CALL hkString::strCpy( char* dst, const char* src )
{
	HK_ASSERT(src != HK_NULL);
	HK_ASSERT(dst != HK_NULL);
	::strcpy(dst, src);
}

void HK_CALL hkString::strNcpy(char *dst, const char *src, int n)
{
	HK_ASSERT(src != HK_NULL);
	HK_ASSERT(dst != HK_NULL);
	::strncpy(dst, src, (unsigned)n);
}

int HK_CALL hkString::strLen( const char* src )
{
	HK_ASSERT(src != HK_NULL);
	return (int) ::strlen(src);
}

int HK_CALL hkString::atoi( const char* in)
{
	return ::atoi(in);
}

hkReal HK_CALL hkString::atof( const char* in )
{
	return static_cast<hkReal>(::atof(in));
}

const char* HK_CALL hkString::strStr(const char* haystack, const char* needle)
{
	return ::strstr(haystack, needle);
}

const char* HK_CALL hkString::strChr(const char* haystack, int needle)
{
	return ::strchr(haystack, needle);
}

const char* HK_CALL hkString::strRchr(const char* haystack, int needle)
{
	return ::strrchr(haystack, needle);
}

char* HK_CALL hkString::strDup(const char* src)
{
	HK_ASSERT(src != HK_NULL);
	char* r = hkAllocate<char>( strLen(src)+1, HK_MEMORY_CLASS_STRING );
	hkString::strCpy(r, src);
	return r;
}

char* HK_CALL hkString::strLwr(char* s)
{
	HK_ASSERT(s != HK_NULL);
	int i=0;
	while(s[i])
	{
		s[i] = (char)(toLower(s[i]));
		i++;
	}
	return s;
}

char* HK_CALL hkString::strUpr(char* s)
{
	HK_ASSERT(s != HK_NULL);
	int i=0;
	while(s[i])
	{
		s[i] = (char)(toUpper(s[i]));
		i++;
	}
	return s;
}
void HK_CALL hkString::memCpy( void* dst, const void* src, int n)
{
	::memcpy(dst,src,(unsigned)n);
}

void HK_CALL hkString::memSet(void* dst, const int c, int n)
{
	::memset(dst,c,(unsigned)n);
}

int HK_CALL hkString::memCmp( const void *buf1, const void *buf2, int n)
{
	return ::memcmp(buf1,buf2,(unsigned)n);
}

////////////// nonstatic member functions

void hkString::Rep::freeMemory()
{
	char* charp = reinterpret_cast<char*>(this);
	hkDeallocateChunk<char>(charp, getSize(), HK_MEMORY_CLASS_STRING  );
}

hkString::Rep* hkString::Rep::clone() const
{
	Rep* r = reinterpret_cast<Rep*>( hkAllocateChunk<char>( getSize(), HK_MEMORY_CLASS_STRING) );
	r->m_length = m_length;
	r->m_capacity = m_capacity;
	r->m_references = 0;
	hkString::memCpy(r->getData(), getData(), m_length+1); //copy null too
	return r;
}

hkString::Rep* HK_CALL hkString::Rep::create(int slen)
{
	int scapacity = slen;
	if(scapacity < MINIMUM_STRING_CAPACITY)
	{
		scapacity = MINIMUM_STRING_CAPACITY;
	}

	Rep* r = reinterpret_cast<Rep*>( hkAllocateChunk<char>((int)sizeof(Rep)+scapacity+1, HK_MEMORY_CLASS_STRING) );
	r->m_length = slen;
	r->m_capacity = scapacity;
	r->m_references = 0;
	return r;
}

////////////// nonstatic member functions

char hkString::s_emptyStringRep[ sizeof(int)+sizeof(int)+sizeof(int)+sizeof(char) ];

void hkString::printf(const char* fmt, ...)
{
	// unshare ourself if necessary or we are too small
	if(getRep().m_references > 0 || getRep().getSize() < MINIMUM_STRING_CAPACITY)
	{
		getRep().unRef();
		m_string = Rep::create(255)->getData();
	}
	
	va_list args; 
	va_start(args, fmt);
	while(1)
	{
		int size = getRep().getSize();
		int nchars = hkString::vsnprintf(m_string, size, fmt, args);
		if( nchars >= 0 && nchars < size ) 
		{
			// usual case, it worked. update length
			getRep().m_length = nchars;			
			break;
		}
		else if( nchars < 0 )
		{
			// there was not enough room, double capacity
			getRep().unRef();
			m_string = Rep::create( size*2 > 255 ? size*2 : 255)->getData();
		}
		else
		{
			// there was not enough room and we were told how much
			// was needed (not including \0)
			m_string = Rep::create( nchars )->getData();
		}
	}
	va_end(args);
}

int hkString::indexOf(char c, int start, int end) const
{
	for(int i = start; i < getLength() && i < end; ++i)
	{
		if( m_string[i] == c )
		{
			return i;
		}
	}
	return -1;
}

int hkString::lastIndexOf(char c, int start, int end) const
{
	if( end > getLength() )
	{
		end = getLength();
	}
	for(int i = end - 1; i >= start ; --i)
	{
		if( m_string[i] == c )
		{
			return i;
		}
	}
	return -1;
}

hkString hkString::operator+ (const hkString& other) const
{
	int myLength = getLength();
	int otherLength = other.getLength();
	int totalLength= myLength + otherLength;
	Rep* r = Rep::create(totalLength);
	char* p = r->getData();
	hkString::memCpy(p, m_string, myLength);
	hkString::memCpy(p+myLength, other.m_string, otherLength+1); // copy null too
	return hkString(r);
}

hkString hkString::operator+ (const char* other) const
{
	int myLength = getLength();
	int otherLength = strLen(other);
	int totalLength= myLength + otherLength;
	Rep* r = Rep::create(totalLength);
	char* p = r->getData();
	hkString::memCpy(p, m_string, myLength);
	hkString::memCpy(p+myLength, other, otherLength+1); // copy null too
	return hkString(r);
}

hkString& hkString::operator+= (const hkString& other)
{
	int myLength = getLength();
	int otherLength = other.getLength();
	int totalLength= myLength + otherLength;
	// need a bigger rep?
	if(totalLength > getRep().m_capacity || getRep().m_references > 0)
	{
		// allocate a little more space for next time
		int longer = (totalLength > myLength ? totalLength : myLength);
		Rep* r = Rep::create( 2 * longer);
		char* p = r->getData();
		hkString::memCpy(p, m_string, myLength);
		getRep().unRef();
		m_string = r->getData();
	}
	hkString::memCpy(m_string+myLength, other.m_string, otherLength+1); // copy null too
	getRep().m_length = totalLength;
	return *this;
}

hkString& hkString::operator+= (const char* other)
{
	int myLength = getLength();
	int otherLength = strLen(other);
	int totalLength= myLength + otherLength;
	// need a bigger rep?
	if(totalLength > getRep().m_capacity || getRep().m_references > 0)
	{
		// allocate a little more space for next time
		int longer = (totalLength > myLength ? totalLength : myLength);
		Rep* r = Rep::create( 2 * longer);
		char* p = r->getData();
		hkString::memCpy(p, m_string, myLength);
		getRep().unRef();
		m_string = r->getData();
	}
	hkString::memCpy(m_string+myLength, other, otherLength+1); // copy null too
	getRep().m_length = totalLength;
	return *this;
}

hkString hkString::substr(int index, int numChars) const
{
	if(numChars - index > getLength())
	{
		numChars = getLength() - index;
	}

	Rep* r = Rep::create(numChars);
	char* p = r->getData();
	hkString::memCpy(p, m_string+index, numChars);
	p[numChars] = '\0';

	return hkString(r);
}


hkString hkString::asUpperCase() const
{
	Rep* r = getRep().clone();
	char* p = r->getData();
	// dont use strupr because that doesn't deal with embedded nulls
	for(int i = 0; i < getLength(); ++i)
	{
		p[i] = toUpper(p[i]);
	}
	return hkString(r);
}

hkString hkString::asLowerCase() const
{
	Rep* r = getRep().clone();
	char* p = r->getData();
	// dont use strlwr because that doesn't deal with embedded nulls
	for(int i = 0; i < getLength(); ++i)
	{
		p[i] = toLower(p[i]);
	}
	return hkString(r);
}

bool hkString::beginsWith (const hkString& other) const
{
	return this->beginsWith( other.cString() );
}

bool hkString::beginsWith (const char* other) const
{
	for(int i=0; other[i] != 0; ++i)
	{
		if( i >= getLength() || m_string[i] != other[i] )
		{
			return false;
		}
	}
	return true;
}

bool hkString::endsWith (const hkString& other) const
{
	if( getLength() < other.getLength() )
	{
		return false;
	}
	int offset = getLength() - other.getLength();

	for(int i=0; i < other.getLength(); ++i)
	{
		if( m_string[i+offset] != other[i] )
		{
			return false;
		}
	}
	return true;
}

bool hkString::endsWith (const char* other) const
{
	int offset = getLength() - hkString::strLen(other);
	if(offset < 0)
	{
		return false;
	}
	for(int i=0; other[i] != 0; ++i)
	{
		if( m_string[i+offset] != other[i] )
		{
			return false;
		}
	}
	return true;
}

hkString hkString::replace(char oldchar, char newchar, hkString::ReplaceType rtype ) const
{
	Rep* r = getRep().clone();
	char* p = r->getData();
	for(int i = 0; i < getLength(); ++i)
	{
		if(p[i] == oldchar)
		{
			p[i] = newchar;
			if(rtype == REPLACE_ONE)
			{
				break;
			}
		}
	}
	return hkString(r);
}

hkString hkString::replace(const hkString& from, const hkString& to, hkString::ReplaceType rtype) const
{
	hkString ret;
	int current = 0;

	const char* thisp = this->cString();
	const char* fromp = from.cString();

	const char* p = strStr(thisp, fromp);
	while(p)
	{
		int i = p - thisp - current;
		ret += this->substr(current, i);
		ret += to;
		current += i + from.getLength();

		if(rtype == REPLACE_ONE)
		{
			break;
		}

		p = strStr(thisp + current, fromp);
	}
	ret += (thisp + current);
	return ret;
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
