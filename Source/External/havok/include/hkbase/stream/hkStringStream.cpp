/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkbase/hkBase.h>
#include <hkbase/stream/hkStringStream.h>
#include <hkbase/stream/hkStringStreambuf.h>


hkIstringStream::hkIstringStream(const char* buf, int len, bool copy)
	: hkIstream( new hkStringStreambuf(buf, len, copy) )
{
	getStreambuf()->removeReference();
}

hkOstringStream::hkOstringStream()
	: hkOstream( new hkStringStreambuf() )
{
	getStreambuf()->removeReference();
}

const char* hkOstringStream::getString() const
{
	return static_cast<hkStringStreambuf*>(m_streambuf)->getString();
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
