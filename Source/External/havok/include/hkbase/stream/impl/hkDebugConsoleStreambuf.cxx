/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkbase/hkBase.h>

#if defined(HK_PLATFORM_WIN32)
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#elif defined(HK_PLATFORM_XBOX)
#	include <xtl.h>
#else
#	error debug console is only for win32 or xbox
#endif

extern "C"
{
#include <stdio.h>
}

/// Streambuf which prints output using OutputDebugString.
/// Unfortunately there is no easy way to check whether OutputDebugString
/// has succeeded (win9x issues) so console output will simply
/// disappear when not debugging.
class hkDebugConsoleStreambuf : public hkStreambuf
{
	public:

		hkDebugConsoleStreambuf(hkStreambuf::StdStream s)
		{
			if( s != STDIN )
			{
				m_stdio = HK_NULL;
			}
			else // fall back to stdio
			{
				m_stdio = new hkStdioStreambuf(s);
			}
		}
		
		hkDebugConsoleStreambuf(const char* fname, OpenMode mode)
		{
			if( mode != INPUT )
			{
				m_stdio = HK_NULL;
			}
			else
			{
				m_stdio = new hkStdioStreambuf(fname,mode);
			}
		}

		virtual ~hkDebugConsoleStreambuf()
		{
			if(m_stdio)
			{
				m_stdio->removeReference();
			}
		}

		virtual int sysWrite(const char* buf, int nbytes)
		{
			if(m_stdio == HK_NULL)
			{
				// careful about accessing buf[-1]
				if(nbytes==0)
				{
					return 0;
				}

				if(buf[nbytes-1] == HK_NULL)
				{
					OutputDebugString( buf );
				}
				else
				{
					hkArray<char> wbuf(nbytes+1);
					hkArray<char>::copy( &wbuf[0], buf, nbytes);
					wbuf[nbytes] = '\0';
					OutputDebugString( &wbuf[0] );
				}

				return nbytes;
			}
			else
			{
				return m_stdio->sysWrite(buf, nbytes);
			}
		}

		virtual int sysRead(char* buf, int nbytes)
		{
			HK_ASSERT(m_stdio != HK_NULL && "Tried to read from an output only stream");
			return m_stdio->sysRead(buf, nbytes);
		}

		virtual void sysFlush()
		{
			if(m_stdio != HK_NULL)
			{
				m_stdio->sysFlush();
			}
		}

	protected:

			/// If null use OutputDebugString, otherwise use stdio.
		hkStdioStreambuf* m_stdio;
};





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
