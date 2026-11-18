/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkbase/hkBase.h>
#include <hkbase/stream/hkStringStreambuf.h>

hkStringStreambuf::hkStringStreambuf()
{
}

/// The buffer uses an array for reading. The array can
/// be used in place, not copied and not deleted on destruction.
/// Or if copy is true, we maintain an internal copy which is deleted
/// on destruction.
hkStringStreambuf::hkStringStreambuf(const char* buf, int len, bool copy)
{
	if(copy)
	{
		m_readBuffer = hkArray<char>(const_cast<char*>(buf), len, len);
		m_getArea.m_start = &m_readBuffer[0];
	}
	else // inplace
	{
		m_getArea.m_start = const_cast<char*>(buf);
	}
	m_getArea.m_current = 0;
	m_getArea.m_size = len;
	m_getArea.m_capacity = len;
}

const char* hkStringStreambuf::getString()
{
	m_writeBuffer.reserve(m_writeBuffer.getSize() + 1);
	// don't use operator[] because we write one past the
	// end of the array
	*(m_writeBuffer.begin() + m_writeBuffer.getSize()) = HK_NULL;
		

	// the array the array if returned as a string will be 
	// correctly terminated, yet at the same time there will
	// be no special assumptions about the array return by
	// the getWriteBuffer call (i.e. the HK_NULL is not actually
	// officially a part of the array.
	return m_writeBuffer.begin();
}

int hkStringStreambuf::sysWrite(const char* buf, int nbytes)
{
	// use the noncopying array constructor
	hkArray<char> data( const_cast<char*>(buf), nbytes, nbytes);
	m_writeBuffer.insertAt(m_writeBuffer.getSize(), data);
	return nbytes;
}

int hkStringStreambuf::sysRead(char* buf, int nbytes)
{
	// we set up the getArea in the constructor and thats all we can read.
	return 0;
}

void hkStringStreambuf::swapBuffers()
{
	m_writeBuffer.swap(m_readBuffer);

	// set up the getArea so that we support buffered reading
	m_getArea.m_start = m_readBuffer.begin();
	m_getArea.m_current = 0;
	m_getArea.m_size = m_readBuffer.getSize();
	m_getArea.m_capacity = m_getArea.m_size;
}

void hkStringStreambuf::sysFlush()
{
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
