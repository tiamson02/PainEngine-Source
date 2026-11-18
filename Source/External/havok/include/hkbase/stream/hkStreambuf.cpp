/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkbase/hkBase.h>
#include <hkbase/stream/hkStreambufFactory.h>

//#define HK_USE_DEBUGCONSOLE
//#include <hkbase/debugutil/hkdebugutil.h>
#define DOUT(a)
#define COUT(a)

/*** STREAMBUF CLASS METHODS ***/

/// Note: Streambufs may be buffered or unbuffered, since
/// the underlying read/write may have its own buffering.
/// Buffered streams have BufAreas with nonzero sizes.
/// Some platforms need buffering because they can only
/// read/write to/from aligned memory locations and only
/// in multiples of a block size.

hkStreambuf::hkStreambuf()
	: m_flags(0)
{
}

bool hkStreambuf::isOk() const
{
	return m_flags==0;
}


void hkStreambuf::initBuffer(OpenMode mode, char* buffer, int length)
{
	BufArea* bArea = (mode == INPUT) ? &m_getArea : &m_putArea;

	bArea->m_start    = buffer;
	bArea->m_current  = 0;
	bArea->m_size     = (mode == INPUT) ? 0 : length;
	bArea->m_capacity = length;
}


void hkStreambuf::flushBuffers()
{
	// flush output buffer
	if (m_putArea.usedBytes() > 0)
	{
		int written = sysWrite(m_putArea.m_start, m_putArea.usedBytes() );
		if (written != m_putArea.usedBytes() )
		{
			m_flags |= WRITE_ERROR;
		}
		m_putArea.m_current = 0;
	}

	// flush input buffer
	m_getArea.m_current = 0;
	m_getArea.m_size = 0;
}

void hkStreambuf::flush()
{
	flushBuffers();
	sysFlush();
}

int hkStreambuf::write(const char* buf, int nbytes)
{
	if( isOk() == false )
	{
		return 0;
	}

	if(m_putArea.capacity() != 0) // buffered write
	{
		int written = 0;

		// while bytes left bigger than buffer
		while(nbytes - written > m_putArea.spareBytes() )
		{
			hkString::memCpy(m_putArea.curPointer(), buf+written, m_putArea.spareBytes() );
			written += m_putArea.spareBytes();

			int thiswrite = sysWrite(m_putArea.m_start, m_putArea.capacity() );
			m_putArea.m_current = 0;
			if( thiswrite != m_putArea.capacity() ) // write error early out
			{
				m_flags |= WRITE_ERROR;
				return written;
			}
		}

		// bytes left fit into buffer
		int bytesLeft = nbytes - written;
		HK_ASSERT(bytesLeft + written == nbytes);
		hkString::memCpy(m_putArea.curPointer(), buf+written, bytesLeft );
		m_putArea.m_current += bytesLeft;
		return nbytes;
	}
	else // unbuffered write
	{
		int written = sysWrite(buf, nbytes);
		if(written != nbytes)
		{
			m_flags |= WRITE_ERROR;
		}
		return written;
	}
}

int hkStreambuf::read(char* buf, int nbytes)
{
	if( isOk() == false )
	{
		return 0;
	}

	if( m_getArea.capacity() != 0) // buffered read
	{
		int bytesRead = 0;

		// while bytes left bigger than buffer
		while(nbytes - bytesRead > m_getArea.spareBytes() )
		{
			hkString::memCpy(buf+bytesRead, m_getArea.curPointer(), m_getArea.spareBytes() );
			bytesRead += m_getArea.spareBytes();

			int thisRead = sysRead(m_getArea.m_start, m_getArea.capacity() );
			m_getArea.m_size = thisRead;
			m_getArea.m_current = 0;

			if( thisRead==0 ) // reached eof early out
			{
				m_flags |= END_OF_FILE;
				return bytesRead;
			}
		}

		// bytes are satisfied by buffer
		int bytesLeft = nbytes - bytesRead;
		HK_ASSERT(bytesRead + bytesLeft == nbytes);
		hkString::memCpy(buf+bytesRead, m_getArea.curPointer(), bytesLeft );
		m_getArea.m_current += bytesLeft;
		return nbytes;
	}
	else // unbuffered read
	{
		int bytesRead = sysRead(buf, nbytes);
		if(bytesRead != nbytes)
		{
			m_flags |= END_OF_FILE;
		}
		return bytesRead;
	}
}

hkResult hkStreambuf::seek(long offset, SeekWhence whence)
{
	hkResult res = sysSeek(offset, whence);
	if( res != HK_FAILURE )
	{
		// a sucessful seek resets end of file
		m_flags &= (unsigned)(~END_OF_FILE);
	}
	return res;
}

int hkStreambuf::tell()
{
	return sysTell();
}

int hkStreambuf::getFileSize()
{
	return sysGetFileSize();
}

hkResult hkStreambuf::sysSeek(long, SeekWhence)
{
	return HK_FAILURE;
}

int hkStreambuf::sysTell()
{
	return -1;
}

int hkStreambuf::sysGetFileSize()
{
	return -1;
}


#ifdef HK_FOR_DOCUMENTATION_ONLY

/// Returns the number of bytes written. On error, return zero bytes.
/// Error handling is done in the caller (i.e. hkStreambuf::write)
int hkStreambuf::sysWrite(const char* buf, int nbytes)
{
}

/// Returns the number of bytes read. On error, return zero bytes.
/// Error handling is done in the caller (i.e. hkStreambuf::read)
int hkStreambuf::sysRead(char* buf, int nbytes)
{
}

/// Returns whether the seek was successful. 
int hkStreambuf::seek(long offset, SeekWhence whence)
{
}

#endif


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
