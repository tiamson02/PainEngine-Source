/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_VERSION_HANDLER
#define HK_VERSION_HANDLER

#include <hkbase/hkBase.h>

/// This class manages version information between the server and client.
/// Currently the proto call version is the same as the build number and
/// for a client to work with a server they must have the same version
/// number.  Please always use the visual debugger client that shipped
/// with the packages you received.
class hkVersionHandler
{
	public:
		enum Status
		{
			HK_OK = 0,
			HK_FAIL
		};

		enum Command // for serialization
		{
			HK_VERSION_INFORMATION = 0x90
		};

	public:
			/// Sends a data 'chunk' which contains the version information
			/// of the Havok libs that the senders process/executable is
			/// linked with.
		static Status sendVersionInformation( hkStreambuf* connectionStreambuf );

	public:

		static int m_protocalVersion;
		static int m_protocalMinimumCompatible;
};

#endif // HK_VERSION_HANDLER



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
