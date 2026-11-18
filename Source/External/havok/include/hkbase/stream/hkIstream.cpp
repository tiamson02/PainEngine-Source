/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkbase/hkBase.h>

extern "C"
{
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
}

#include <hkbase/stream/hkIstream.h>
#include <hkbase/stream/hkStreambufFactory.h>

#define TEMP_STORAGE_SIZE 512

/// If deleteOnDestruct is true then the streambuf will be deleted when
/// this istream is destroyed. Usually this is what you want.
hkIstream::hkIstream(hkStreambuf* sb)
	:	m_streambuf(sb)
{
	m_streambuf->addReference();
	m_tempStorage = hkAllocateChunk<char>( TEMP_STORAGE_SIZE, HK_MEMORY_CLASS_BASE );
}

hkIstream::~hkIstream()
{
	if (m_tempStorage)
	{
		hkDeallocateChunk<char>(m_tempStorage, TEMP_STORAGE_SIZE, HK_MEMORY_CLASS_BASE );
	}
	if (m_streambuf)
	{
		m_streambuf->removeReference();
	}
}

bool hkIstream::isOk() const
{
	if (m_streambuf)
	{
		return m_streambuf->isOk();
	}
	return false;

}

static inline bool HK_CALL getChar(hkStreambuf* buf, char& c)
{
	int i = buf->read( &c, 1 );
	return i > 0;
}

static int HK_CALL getBufWithDelimters(hkStreambuf* buf, char* str, int maxlen, const char* delim = HK_NULL, int numdelim = 0)
{
	HK_ASSERT(buf != HK_NULL && str != HK_NULL);

	char c;
	int i = 0, j;
	int ml = maxlen - 1;
	while ( (i < ml) && getChar(buf, c) )
	{
		if ( delim )
		{
			for (j=0;j<numdelim; ++j)
			{
				if ( c == *(delim + j) )
				{
					if (c=='\r')
					{	// assume a \n in order to hide cross platform eol issues?
						getChar(buf, c);
						//HK_ASSERT( c == '\n' );
					}
					break;
				}
			}
			if (j!=numdelim)
				break;
		}
		str[i] = c;
		++i;
	}
	str[i] = '\0';
	return i;
}

#define IS_DECIMALPOINT(c) ( ((c) == '.') || ((c) == ',') )
#define IS_INTEGER(c)      ( ((c) >= '0') && ((c) <= '9') )
#define IS_SIGN(c)         ( ((c) == '+') || ((c) == '-') )
#define IS_SPACE(c)		   ( ((c) == ' ') || ((c) == '\t') )
#define IS_EXP(c)		   ( ((c) == 'E') || ((c) == 'e') )
#define IS_EOL(c)		   ( ( (c) == '\r') || ((c) == '\n') )


static int HK_CALL eatWhiteSpace( hkStreambuf* buf, char& nonWS )
{
	int i = 0;
	while ( getChar(buf, nonWS ) )
	{

		++i; // have read a character

		if ( !IS_SPACE(nonWS) ) // not space
		{
			if ( IS_EOL(nonWS) ) // but is an eol
				continue;        // so continue on

			break;
		}
	}

	return i;
}

static int HK_CALL fillSemiValidInt( hkStreambuf* buf, char* storage )
{
	int index = 0;

	if ( eatWhiteSpace( buf, storage[0] ) > 0 )
	{
		// is the char proper for an int?
		if ( !IS_INTEGER(storage[0]) &&
			 !IS_SIGN(storage[0]) )
		{
			storage[0] = '\0';
			return 0;
		}
		else
			index++;
	}

	while ( index < TEMP_STORAGE_SIZE )
	{
		if (!getChar( buf, storage[index] ))
			break;
		else
		{
			const char & c = storage[index];

			// exit on space or end of line
			if ( IS_EOL(c) )
			{
				// gobble the extra eol characters if any
				if ( c == '\r' )
				{
					// then assume we have a '\n'
					getChar( buf, storage[index] ); // can read to same place
				}
				break;
			}

			// quick exit on space
			if ( IS_SPACE(c) )
				break;

			if ( IS_INTEGER(c) ||
				 IS_SIGN(c) )
			{
				 index++;
				 continue;
			}
			else
				break; // not a valid float char
		}
	}

	storage[index] = '\0';
	return index;
}


static float HK_CALL readFloat(hkStreambuf* buf, char* storage )
{

	float f = -1.0f;
	int index = 0;

	if ( eatWhiteSpace( buf, storage[0] ) > 0 )
	{
		const char & c = storage[0];

		// is the char proper for a float?
		if ( !IS_INTEGER(c) &&
			 !IS_SIGN(c) &&
			 !IS_DECIMALPOINT(c) )
		{
			return 0;
		}
		else
			index++;
	}

	while ( index < TEMP_STORAGE_SIZE )
	{
		if ( !getChar( buf, storage[index] ))
			break;
		else
		{
			const char & c = storage[index];

			// exit on space or end of line
			if ( IS_EOL(c) )
			{
				// gobble the extra eol characters if any
				if ( c == '\r' )
				{
					// then assume we have a '\n'
					getChar( buf, storage[index] ); // can read to same place
				}
				break;
			}

			if ( IS_SPACE(c) )
				break;

			if ( IS_INTEGER(c) ||
				 IS_SIGN(c) ||
				 IS_EXP(c) ||
				 IS_DECIMALPOINT(c) )
			{
				 index++;
				 continue;
			}
			else
			{
				break; // not a valid float char
			}
		}
	}

	storage[index] = '\0';
	if (index > 0)
	{
	
		f = (float)atof(storage);
	}

	return f;
}



static int HK_CALL readInteger(hkStreambuf* buf, char* storage)
{
	int i = -1;
	int index = fillSemiValidInt( buf, storage );

	if (index > 0)
		i = atoi(storage);

	return i;
}

static hkInt64 HK_CALL readLargeInteger(hkStreambuf* buf, char* storage)
{
	hkInt64 li = -1;
	int index = fillSemiValidInt( buf, storage );

	if (index > 0)
	{
#if defined(HK_PLATFORM_WIN32) || defined(HK_PLATORM_XBOX)
		li = _atoi64(storage);
#elif defined(HK_COMPILER_GNU)
		li = atoll(storage);
#else
		li = atol(storage);
#endif
	}

	return li;
}

hkIstream& hkIstream::operator>> (void* p)
{
	unsigned int pi = 0;
	(*this) >> pi;
	p = (void*) pi;
	return *this;
}

hkIstream& hkIstream::operator>> (char& c)
{
	eatWhiteSpace( m_streambuf, c );
	return *this;
}

hkIstream& hkIstream::operator>> (char* str)
{
	int ate = eatWhiteSpace( m_streambuf, m_tempStorage[0] );

	if (ate > 0)
		getBufWithDelimters( m_streambuf, (m_tempStorage+1), TEMP_STORAGE_SIZE-1, " \t\r\n\0", 5);
	else
		getBufWithDelimters( m_streambuf, m_tempStorage, TEMP_STORAGE_SIZE, " \t\r\n\0", 5);

	strncpy( str, m_tempStorage, 100);

	return *this;
}

hkIstream& hkIstream::operator>> (short& s)
{
	int i = readInteger(m_streambuf, m_tempStorage);
	s = static_cast<short>(i);
	return *this;
}

hkIstream& hkIstream::operator>> (unsigned short& s)
{
	int i = readInteger(m_streambuf, m_tempStorage);
	s = static_cast<unsigned short>(i);
	return *this;
}

hkIstream& hkIstream::operator>> (int& i)
{
	i = readInteger(m_streambuf, m_tempStorage);
	return *this;
}
hkIstream& hkIstream::operator>> (unsigned int& i)
{
	// XXX Will have problems with large unsigned ints ( > 2147483648 etc )
	i = static_cast<unsigned int>( readInteger(m_streambuf, m_tempStorage) );
	return *this;
}

hkIstream& hkIstream::operator>> (float & f)
{
	f = readFloat(m_streambuf, m_tempStorage);
	return (*this);
}

hkIstream& hkIstream::operator>> (double& d)
{
	// XXX down size as Havok doesn't use doubles usually,
	// but change this if doubles required
	d = static_cast<float>( readFloat(m_streambuf, m_tempStorage) );
	return *this;
}

hkIstream& hkIstream::operator>> (hkInt64& i)
{
	i = readLargeInteger(m_streambuf, m_tempStorage);
	return *this;
}

hkIstream& hkIstream::operator>> (hkUint64& i)
{
	// XXX Will have problems with very large unsigned ints ( > 9223372036854775808 etc )
	i = hkUint64(readLargeInteger(m_streambuf, m_tempStorage));
	return *this;
}

hkIstream& hkIstream::operator>> (hkString& str)
{
	hkArray<char> buf;
	buf.reserve(64);
	char c;
	eatWhiteSpace( m_streambuf, c );
	buf.pushBack(c);
	while( getChar(m_streambuf, c) )
	{
		if( ! IS_SPACE(c) )
		{
			buf.pushBack(c);
		}
		else
		{
			break;
		}
	}
	buf.pushBack(0);
	str = &buf[0];
	return *this;
}

hkIstream& hkIstream::getline(char* str, int maxlen, char delim)
{
	getBufWithDelimters( m_streambuf, str, maxlen, &delim, 1);
	return *this;
}

int hkIstream::read( char* buf, int nbytes )
{
	return m_streambuf->read(buf, nbytes);
}

hkResult hkIstream::seek(long offset, hkStreambuf::SeekWhence whence)
{
	return m_streambuf->seek(offset, whence);
}

int hkIstream::tell()
{
	return m_streambuf->tell();
}

int hkIstream::getFileSize()
{
	return m_streambuf->getFileSize();
}

/// If m_deleteOnDestruct was set when the original stream was added,
/// the original stream is deleted.  It the newDeleteOnDestruct parameter
/// is true, newBuf will be deleted when this ostream is destroyed.
inline void hkIstream::setStreambuf(hkStreambuf* newBuf)
{
	newBuf->addReference();
	m_streambuf->removeReference();
	m_streambuf = newBuf;
}

hkIfstream::hkIfstream(const char* fname)
	: hkIstream( hkStreambufFactory::getInstance().open(fname, hkStreambuf::INPUT) )
{
	m_streambuf->removeReference(); // base class adds a ref
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
