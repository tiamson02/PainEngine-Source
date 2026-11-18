/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkbase/hkBase.h>

extern "C"
{
#include <stdio.h>
}

class hkStdioStreambuf : public hkStreambuf
{
	public:

		hkStdioStreambuf(hkStreambuf::StdStream s)
			:	m_owned(false)
		{
			switch(s)
			{
				case STDIN:
					m_fhandle = stdin;
					break;
				case STDOUT:
					m_fhandle = stdout;
					break;
				case STDERR:
				default:
					m_fhandle = stderr;
					break;
			}
		}
		
		hkStdioStreambuf(const char* fname, OpenMode mode)
			:	m_owned(true)
		{
			const char* fmode = HK_NULL;

			switch(mode)
			{
				case INPUT:
					fmode = "rb";
					break;
				case OUTPUT:
					fmode = "wb";
					break;
				case APPEND:
					fmode = "ab";
					break;
				default:
					HK_ASSERT2(false, "Unknown open mode for streambuf");
			}

			m_fhandle = fopen(fname, fmode);
			if( m_fhandle==HK_NULL )
			{
				m_flags |= END_OF_FILE;
			}
		}

		virtual ~hkStdioStreambuf()
		{
			if(m_fhandle != HK_NULL && m_owned)
			{
				flush();
				fclose(m_fhandle);
			}
				
			m_fhandle = HK_NULL;
		}

		virtual int sysWrite( const char* buf, int nbytes)
		{
			HK_ASSERT(m_fhandle != HK_NULL);
			return fwrite(buf, 1, nbytes, m_fhandle );
		}

		virtual int sysRead( char* buf, int nbytes)
		{
			HK_ASSERT(m_fhandle != HK_NULL);
			return fread(buf, 1, nbytes, m_fhandle );
		}

		virtual void sysFlush()
		{
			HK_ASSERT(m_fhandle != HK_NULL);
			fflush(m_fhandle);
		}

		virtual hkResult sysSeek( long offset, SeekWhence whence)
		{
			return ::fseek(m_fhandle, offset, whence) != -1
				? HK_SUCCESS
				: HK_FAILURE;
		}

		virtual int sysTell()
		{
			return ::ftell(m_fhandle);
		}

		virtual int sysGetFileSize()
		{
			long saveOffset = ::ftell(m_fhandle);
			if( saveOffset != -1 )
			{
				if( sysSeek(0, STREAM_END) == HK_SUCCESS )
				{
					int length = ::ftell(m_fhandle);
					sysSeek(saveOffset, STREAM_SET);
					return length;
				}				
			}
			return -1;
		}

	protected:

		FILE* m_fhandle;
		bool  m_owned;
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
