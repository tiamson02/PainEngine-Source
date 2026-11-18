/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HKBASE_HKSTREAMBUFSOURCE_H
#define HKBASE_HKSTREAMBUFSOURCE_H

/// Interface for streambuf creation.
/// When streams and archives are given a resource to open by name, they
/// ask the streambuf factory to open it for them. The user may wish to
/// replace the default factory with one which reads from packed files
/// or searches in several locations for instance.
class hkStreambufFactory : public hkSingleton<hkStreambufFactory>
{
	public:

			/// Returns a streambuf for stdstream 's' or null if unable.
		virtual hkStreambuf* open(hkStreambuf::StdStream s) = 0;

			/// Returns a streambuf for file 'name' or null if unable.
		virtual hkStreambuf* open(const char* name, hkStreambuf::OpenMode mode) = 0;
};

#endif // HKBASE_HKSTREAMBUFSOURCE_H

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
