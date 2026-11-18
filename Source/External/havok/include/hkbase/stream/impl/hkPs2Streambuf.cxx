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

#include <sifdev.h>

//HACK disable asserts
#undef HK_ASSERT
#define HK_ASSERT(A) /*nothing*/

// 16K buffer by default
static const unsigned BUFFER_SIZE = 16384;

/// For input/output to files
class hkPs2FileStreambuf : public hkStreambuf
{
	public:

		hkPs2FileStreambuf(const char* fname, OpenMode mode)
		{
			int sceMode;
			
			switch(mode)
			{
                case INPUT:
                    sceMode = SCE_RDONLY;
                    break;
                case OUTPUT:
                    sceMode = SCE_WRONLY | SCE_CREAT | SCE_TRUNC;
                    break;
                case APPEND:
                    sceMode = SCE_WRONLY | SCE_APPEND;
                    break;
                default:
                    sceMode = SCE_RDONLY;
                    HK_ASSERT2(false, "Unknown open mode for streambuf");
			}


			m_fd = sceOpen(fname, sceMode);

			if (m_fd >= 0)
			{
				initBuffer(mode, m_buffer, BUFFER_SIZE);
				m_buffer[BUFFER_SIZE] = 0;
			}
			else
			{
				m_flags |= END_OF_FILE;
			}
		}

		virtual ~hkPs2FileStreambuf()
		{
			flush();

			if (m_fd >= 0)
			{
				sceClose(m_fd);
			}
		}

		virtual int sysWrite( const char* buf, int nbytes )
		{
			HK_ASSERT( (unsigned(buf) & 0x1f) == 0  && "Write source not multiple of 64 bytes");
			HK_ASSERT( (nbytes & 0xff) == 0  && "Write length not multiple of 512 bytes");

			int status = sceWrite(m_fd, buf, int(nbytes));
			return (status >= 0)
				? status : 0;
		}

		virtual int sysRead( char* buf, int nbytes )
		{
			HK_ASSERT( (unsigned(buf) & 0x1f) == 0  && "Read destination not multiple of 64 bytes");
			HK_ASSERT( (nbytes & 0xff) == 0  && "Read length not multiple of 512 bytes");

			int status = sceRead(m_fd, buf, int(nbytes));
			return (status >= 0)
				? status : 0;
		}

		virtual void sysFlush()
		{
		}
		
		virtual int sysGetFileSize()
		{
			int curoffset = sceLseek(m_fd, 0 ,STREAM_CUR);
			int numbytes = sceLseek(m_fd, 0 ,STREAM_END);
			sceLseek(m_fd, curoffset, STREAM_SET);
			return numbytes;
		}

		virtual hkResult sysSeek( long offset, SeekWhence whence)
		{
			//TODO the seek position may already be in the buffer
			flushBuffers();
			HK_ASSERT( (offset & 0xff) == 0  && "Seek not multiple of 512 bytes");
			return sceLseek(m_fd, int(offset), whence) != -1
				? HK_SUCCESS
				: HK_FAILURE;
		}

	protected:

		int m_fd;
		char HK_ALIGNED_VARIABLE(m_buffer[BUFFER_SIZE+1], 64);
};

/// For std{in,out,err} - currently only stdout
class hkPs2StdStreambuf : public hkStreambuf
{
	public:

		hkPs2StdStreambuf(hkStreambuf::StdStream s)
		{
		}

		virtual int sysWrite( const char* buf, int nbytes)
		{
			// careful about accessing buf[-1]
			if( nbytes == 0 )
			{
				return 0;
			}
			
			if (buf[nbytes-1] != HK_NULL)
			{
				hkArray<char> wbuf(int(nbytes+1));
				hkArray<char>::copy( &wbuf[0], buf, int(nbytes));
				wbuf[int(nbytes)] = '\0';
				printf( &wbuf[0] ); 
			}
			else
			{
				printf(buf);
			}

			return nbytes;
		}

		virtual int sysRead( char* buf, int nbytes)
		{
			HK_ASSERT(0 && "not implemented");
			return 0;
		}

		virtual void sysFlush()
		{
		}
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
