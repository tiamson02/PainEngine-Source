/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DEFAULT_CONSOLE
#define HK_DEFAULT_CONSOLE

#include <hkbase/hkBase.h>
#include <hkbase/stream/hkStreambufFactory.h>

class hkDefaultConsole : public hkConsole
{
	public:
		
		hkDefaultConsole()
			:	m_stdout( hkStreambufFactory::getInstance().open( hkStreambuf::STDOUT) ),
				m_stderr( hkStreambufFactory::getInstance().open( hkStreambuf::STDERR) )
		{
			m_stdout.getStreambuf()->removeReference();
			m_stderr.getStreambuf()->removeReference();
		}

		virtual hkOstream& getStdout()
		{
			return m_stdout;
		}
		virtual hkOstream& getStderr() 
		{ 
			return m_stderr;
		}

		hkOstream m_stdout;
		hkOstream m_stderr;
};

#endif // HK_DEFAULT_CONSOLE


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
