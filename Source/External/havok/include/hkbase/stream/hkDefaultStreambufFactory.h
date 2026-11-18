/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DEAFULT_STREAMBUF_FACTORY
#define HK_DEAFULT_STREAMBUF_FACTORY 

#include <hkbase/hkBase.h>
#include <hkbase/stream/hkStreambufFactory.h>

#if defined(HK_PLATFORM_GC)
#	include <hkbase/stream/impl/hkNgcStreambuf.cxx>
	typedef hkGameCubeStdStreambuf DefaultStdStreambuf;
	typedef hkGameCubeFileStreambuf DefaultFileStreambuf;
#elif defined(HK_PLATFORM_PS2)
#	include <hkbase/stream/impl/hkPs2Streambuf.cxx>
	typedef hkPs2StdStreambuf DefaultStdStreambuf;
	typedef hkPs2FileStreambuf DefaultFileStreambuf;
#elif defined(HK_PLATFORM_UNIX)
#	include <hkbase/stream/impl/hkPosixStreambuf.cxx>
	typedef hkPosixStreambuf DefaultStdStreambuf;
	typedef hkPosixStreambuf DefaultFileStreambuf;
#elif defined(HK_PLATFORM_WIN32)
#	include <hkbase/stream/impl/hkStdioStreambuf.cxx>
#	if 0 // see caveat before enabling debug console
		typedef hkStdioStreambuf DefaultStdStreambuf;
#	else
#		include <hkbase/stream/impl/hkDebugConsoleStreambuf.cxx>
		typedef hkDebugConsoleStreambuf DefaultStdStreambuf;
#	endif	
	typedef hkStdioStreambuf DefaultFileStreambuf;
#elif defined(HK_PLATFORM_XBOX)
#	include <hkbase/stream/impl/hkStdioStreambuf.cxx>
#	include <hkbase/stream/impl/hkDebugConsoleStreambuf.cxx>
	typedef hkDebugConsoleStreambuf DefaultStdStreambuf;
	typedef hkStdioStreambuf DefaultFileStreambuf;
#else
#	include <hkbase/stream/impl/hkStdioStreambuf.cxx>
	typedef hkStdioStreambuf DefaultStdStreambuf;
	typedef hkStdioStreambuf DefaultFileStreambuf;
#endif
	
class hkDefaultStreambufFactory : public hkStreambufFactory
{
	public:

		virtual hkStreambuf* open(hkStreambuf::StdStream s)
		{
			return new DefaultStdStreambuf(s);
		}
		virtual hkStreambuf* open(const char* name, hkStreambuf::OpenMode mode)
		{
			return new DefaultFileStreambuf(name, mode);
		}
		static hkReferencedObject* create() { return new hkDefaultStreambufFactory(); }
};

#endif // HK_DEAFULT_STREAMBUF_FACTORY 


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
