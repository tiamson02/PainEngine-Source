/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkbase/hkBase.h>
#include <hkbase/stream/hkStreambufFactory.h>

extern "C"
{
#include <stdarg.h>
}

#if defined(HK_PLATFORM_WIN32) || defined(HK_PLATFORM_XBOX)
#	define HK_INT64_FORMAT "%I64i"
#	define HK_UINT64_FORMAT "%I64u"
#else
#	define HK_INT64_FORMAT "%Li"
#	define HK_UINT64_FORMAT "%Lu"
#endif

static const int HK_BUFSIZ = 255;

static void HK_CALL writeString(hkStreambuf* sb, const char* s)
{
	if(s)
	{
		sb->write( s, hkString::strLen(s) );
	}
	else
	{
		sb->write( "(null)", 6);
	}
}


/// If deleteOnDestruct is true then the streambuf will be deleted when
/// this ostream is destroyed. Usually this is what you want.
hkOstream::hkOstream(hkStreambuf* sb)
	: m_streambuf(sb)
{
	m_streambuf->addReference();
}

hkOstream::~hkOstream()
{
	m_streambuf->removeReference();
}

bool hkOstream::isOk() const
{
	return m_streambuf->isOk();
}

hkOstream& hkOstream::operator<< (const void* p)
{
	char buf[HK_BUFSIZ];
	hkString::snprintf(buf, HK_BUFSIZ, "%p", p);
	writeString(m_streambuf, buf);
	return *this;
}

hkOstream& hkOstream::operator<< (char c)
{
	m_streambuf->write(&c, 1);
	return *this;
}

hkOstream& hkOstream::operator<< (const char* s)
{
	writeString(m_streambuf, s);
	return *this;
}

hkOstream& hkOstream::operator<< (short s)
{
	char buf[HK_BUFSIZ];
	hkString::snprintf(buf, HK_BUFSIZ, "%i", s);
	writeString(m_streambuf, buf);
	return *this;
}

hkOstream& hkOstream::operator<< (unsigned short s)
{
	char buf[HK_BUFSIZ];
	hkString::snprintf(buf, HK_BUFSIZ, "%u", s);
	writeString(m_streambuf, buf);
	return *this;
}

hkOstream& hkOstream::operator<< (int i)
{
	char buf[HK_BUFSIZ];
	hkString::snprintf(buf, HK_BUFSIZ, "%i", i);
	writeString(m_streambuf, buf);
	return *this;
}

hkOstream& hkOstream::operator<< (unsigned int u)
{
	char buf[HK_BUFSIZ];
	hkString::snprintf(buf, HK_BUFSIZ, "%u", u);
	writeString(m_streambuf, buf);
	return *this;
}

hkOstream& hkOstream::operator<< (float f)
{
	char buf[HK_BUFSIZ];
	hkString::snprintf(buf, HK_BUFSIZ, "%f", f);
	writeString(m_streambuf, buf);
	return *this;
}

hkOstream& hkOstream::operator<< (hkInt64 i)
{
	char buf[HK_BUFSIZ];
	hkString::snprintf(buf, HK_BUFSIZ, HK_INT64_FORMAT, i);
	writeString(m_streambuf, buf);
	return *this;
}
hkOstream& hkOstream::operator<< (hkUint64 i)
{
	char buf[HK_BUFSIZ];
	hkString::snprintf(buf, HK_BUFSIZ, HK_UINT64_FORMAT, i);
	writeString(m_streambuf, buf);
	return *this;
}

void hkOstream::printf(const char *fmt, ...)
{
	char buf[HK_BUFSIZ];
	va_list args; 
	va_start(args, fmt);
	hkString::vsnprintf(buf, HK_BUFSIZ, fmt, args);
	va_end(args);
	writeString(m_streambuf, buf);
}

hkOstream& hkOstream::operator<< (const hkString& str)
{
	m_streambuf->write( str.cString(), str.getLength() ); 
	return *this;
}

void hkOstream::flush()
{
	m_streambuf->flush();
}

long hkOstream::seek(long offset, hkStreambuf::SeekWhence whence)
{
	return m_streambuf->seek(offset, whence);
}

/// Output raw data
int hkOstream::write( const char* buf, int nbytes)
{
	return m_streambuf->write(buf, nbytes);
}



/// If m_deleteOnDestruct was set when the original stream was added,
/// the original stream is deleted.  If the newDeleteOnDestruct parameter
/// is true, newBuf will be deleted when this ostream is destroyed.
void hkOstream::setStreambuf(hkStreambuf* newBuf)
{
	newBuf->addReference();
	m_streambuf->removeReference();
	m_streambuf = newBuf;	
}


hkOstream& HK_CALL hkOstreamManip::endl(hkOstream& os)
{
	os << '\n';
	os.flush();
	return os;
}

hkOstream& HK_CALL hkOstreamManip::flush(hkOstream& os)
{
	os.flush();
	return os;
}



hkOfstream::hkOfstream(const char* filename)
	: hkOstream( hkStreambufFactory::getInstance().open(filename, hkStreambuf::OUTPUT) )
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
