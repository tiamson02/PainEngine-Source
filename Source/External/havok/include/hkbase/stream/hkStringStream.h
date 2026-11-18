/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HKBASE_HKSTRINGSTREAM_H
#define HKBASE_HKSTRINGSTREAM_H

#include <hkbase/stream/hkIstream.h>

/// An istream that is connected to a pre-existing buffer.
class hkIstringStream : public hkIstream
{
	public:

			///Creates a new hkIstringStream. 
		hkIstringStream(const char* buf, int buflen, bool copy = true);
};

/// An ostream that is connected to an in-memory buffer.
class hkOstringStream : public hkOstream
{
	public:

			/// Creates a new hkOstringStream.
		hkOstringStream();

			/// Returns the null-terminated buffer so far.
		const char* getString() const;
};

#endif // HKBASE_HKSTRINGSTREAM_H

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
