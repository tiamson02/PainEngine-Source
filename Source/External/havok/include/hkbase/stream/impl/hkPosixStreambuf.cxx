/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkbase/hkBase.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

static const unsigned BUFFER_SIZE = 4096;

// check that we can use these interchangeably
HK_COMPILE_TIME_ASSERT(hkStreambuf::STREAM_SET==SEEK_SET
					&& hkStreambuf::STREAM_CUR==SEEK_CUR
					&& hkStreambuf::STREAM_END==SEEK_END );

class hkPosixStreambuf : public hkStreambuf
{
	public:

		hkPosixStreambuf(hkStreambuf::StdStream s)
			:	m_owned(false)
		{
			switch(s)
			{
				case STDIN:
					m_handle = 0;
					break;
				case STDOUT: // only stdout is buffered
					m_handle = 1;
					m_buffer.setSize(BUFFER_SIZE);
					initBuffer(OUTPUT, &m_buffer[0], m_buffer.getSize());
					break;
				case STDERR:
					m_handle = 2;
					break;
			}
		}
		
		hkPosixStreambuf(const char* fname, OpenMode omode)
			:	m_owned(true)
		{
			int mode;

			switch(omode)
			{
				case INPUT:
					mode = O_RDONLY;
					break;
				case OUTPUT:
					mode = O_WRONLY | O_CREAT | O_TRUNC;
					break;
				case APPEND:
					mode = O_WRONLY | O_APPEND;
					break;
				default:
					mode = O_RDONLY;
					HK_ASSERT2(false, "Unknown open mode for streambuf");
			}

			m_handle = ::open(fname, mode, 0666);
			if( m_handle >= 0)
			{
				m_buffer.setSize(BUFFER_SIZE);
				initBuffer(omode, &m_buffer[0], m_buffer.getSize());
			}
			else
			{
				m_flags |= END_OF_FILE;
			}
		}

		virtual ~hkPosixStreambuf()
		{
			flush();

			if(m_handle >= 0 && m_owned)
			{
				::close(m_handle);
			}
				
			m_handle = -1;
		}

		virtual int sysWrite( const char* buf, int nbytes)
		{
			int status = ::write( m_handle, buf, nbytes );
			return (status >= 0)
				? static_cast<unsigned>(status)
				: 0;
		}

		virtual int sysRead( char* buf, int nbytes)
		{
			int status = ::read( m_handle, buf, nbytes );
			return (status >= 0)
				? static_cast<unsigned>(status)
				: 0;
		}

		virtual void sysFlush()
		{
			::sync();
		}

		virtual hkResult sysSeek( long offset, SeekWhence whence)
		{
			flushBuffers();
			return ::lseek(m_handle, offset, whence) != -1
				? HK_SUCCESS
				: HK_FAILURE;
		}

		virtual int sysGetFileSize()
		{
			struct stat s;
			int res = fstat(m_handle, &s);
			if(res != -1)
			{
				return s.st_size;
			}
			else
			{
				return -1;
			}
		}

	protected:

		int m_handle;
		bool m_owned;
		hkArray<char> m_buffer;
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
