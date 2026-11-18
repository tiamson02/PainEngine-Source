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

#include <dolphin.h>
#include <dolphin/fio.h>


//
// Globals
//
static const int BUFFER_SIZE = 4096;
static int  g_exiChannel = -1;
static bool g_exiEnumerated  = false;
static bool g_mccInitialized = false;
static bool g_fioInitialized = false;
static OSThreadQueue fioThreadQueue;


//
//	MCC callbacks
//
static BOOL hioEnumCallback(s32 channel)
{	
	g_exiChannel = channel;
	g_exiEnumerated = true;
	OSWakeupThread(&fioThreadQueue);
	return false;
}

static void mccCallback(MCCSysEvent event)
{
	if (event == MCC_SYSEVENT_INITIALIZED)
	{
		g_mccInitialized = true;
		OSWakeupThread(&fioThreadQueue);
	}
}


/// A block buffered stream buf through DVD interface for GameCube
class hkGameCubeFileStreambuf : public hkStreambuf
{
	public:
	
		hkGameCubeFileStreambuf(const char* fname, OpenMode mode)
			:	m_fd(HK_NULL),
				m_fileOffset(0),
				m_fileLen(0)
		{
			if ( mode == INPUT )
			{
				m_fd = hkAllocate<DVDFileInfo>(1, HK_MEMORY_CLASS_BASE);
			
				if ( DVDOpen( const_cast<char*>(fname), m_fd ) == TRUE )
				{
					m_fileLen = DVDGetLength( m_fd );

					m_buffer = hkAlignedAllocate<char>(32, BUFFER_SIZE, HK_MEMORY_CLASS_BASE);

					// for now mode is always input, but leave for future.
					initBuffer(mode, m_buffer, BUFFER_SIZE);
					m_buffer[BUFFER_SIZE] = 0;
				}
				else // open failed
				{	
					hkDeallocate<DVDFileInfo>(m_fd);
					m_fd = NULL;
					m_flags |= END_OF_FILE;
				}
			}
			else // no output or append yet
			{
				m_flags |= END_OF_FILE;
			}
		}
		
		virtual ~hkGameCubeFileStreambuf()
		{	
			flush();

			if (m_fd)
			{
				DVDClose(m_fd);
				hkDeallocate<DVDFileInfo>(m_fd);
				m_fd = NULL;
				hkAlignedDeallocate<char>(m_buffer); 
			}
		}	
		
	
		virtual int sysWrite( const char* buf, int n)
		{
			return 0; // no DVD write : XXX improve to be USB link etc on dev box	
		}

		virtual int sysRead(char* buf , int nbytes)
		{
			if (m_fileOffset >= m_fileLen)
				return 0;
			
			if ( (m_fileOffset + nbytes) > m_fileLen )
			{
				nbytes = m_fileLen - m_fileOffset;
				// round it up;
				nbytes = OSRoundUp32B(nbytes);
			}

			hkInt32 r = DVDRead(m_fd, buf, nbytes, m_fileOffset);
			
			if ( r < 1 )
				return 0;
			else
			{
				m_fileOffset += r;
				return static_cast<unsigned>(r);
			}
		}

		virtual hkResult sysSeek( long offset, SeekWhence whence )
		{
			HK_ASSERT( (m_fileOffset& 0x03) == 0  && "Seek position not multiple of 32 bytes");

			long realoffset = 0;
			switch(whence)
			{
				case STREAM_SET:
					realoffset = offset;
					break;
				case STREAM_CUR:
					realoffset = m_fileOffset + offset;
					break;
				case STREAM_END:
					realoffset = m_fileLen + offset;
					break;
				default:
					HK_ASSERT(0 && "No such seek type");
			}

			HK_ASSERT(realoffset >= 0);
			HK_ASSERT(realoffset <= m_fileLen);

			hkInt32 status = DVDSeek(m_fd, realoffset);
			if(status == 0)
			{
				m_fileOffset = realoffset;
				return HK_SUCCESS;
			}
			else
			{
				return HK_FAILURE;
			}
		}

		virtual void sysFlush()
		{
		}
			
	protected:
	
		DVDFileInfo* m_fd;
		int m_fileOffset;
		int m_fileLen;
		char* m_buffer;
};


/// For std{in,out,err} - currently only stdout
class hkGameCubeStdStreambuf : public hkStreambuf
{
	public:

		hkGameCubeStdStreambuf(hkStreambuf::StdStream s)
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
				hkArray<char> wbuf(nbytes+1);
				hkArray<char>::copy( &wbuf[0], buf, nbytes);
				wbuf[nbytes] = '\0';
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


/// An EXI2USB based File IO Streambuf for Gamecube
class hkGameCubeUSBFileStreambuf : public hkStreambuf
{
	public:

		hkGameCubeUSBFileStreambuf(const char* fname, OpenMode mode)
			:	m_fileHandle(HK_NULL),
				m_fileOffset(0),
				m_fileLength(0)	
		{
			// determin open type
			switch(mode)
			{
				case INPUT:
				{
					m_fileMode = FIO_OPEN_RDONLY;
					break;
				}
				case OUTPUT:
				{
					m_fileMode = FIO_OPEN_WRONLY | FIO_OPEN_CREAT;
					break;
				}
				default:
				{
					HK_ASSERT2(false, "HK GCN FIO: Unknown open mode for streambuf");
				}
			}

			// make sure MCC is initialized
			if (!g_mccInitialized)
			{
				// enum EXI interface
				if (!MCCEnumDevices(hioEnumCallback))
				{
					hkcerr <<"HK GCN FIO: Can't detect EXI Adapter!" << hkendl;
					m_flags |= END_OF_FILE;
					return;				
				}
				else
				{
					// init thread queue
					OSInitThreadQueue(&fioThreadQueue);
					
					// spin until EXI enumerated
					hkprintf("HK GCN FIO: Waiting for EXI enumeration...\n");
					while (!g_exiEnumerated)
					{
						// spin until enumerated
						OSSleepThread(&fioThreadQueue);
					}
					hkprintf("HK GCN FIO: EXI enumeration complete, EXI channel# = %d! \n", g_exiChannel);
				}
				
				// init MCC communication layer
				if (!MCCInit((MCCExiChannel) g_exiChannel, 0, mccCallback))
				{
					hkcerr << "HK GCN FIO: MCC communication not available" << hkendl;
					m_flags |= END_OF_FILE;
					return;
				}
				else
				{
					// spin until MCC initialised
					hkprintf("HK GCN FIO: Waiting for MCC init by host...\n");
					while (!g_mccInitialized)
					{
						// spin until initialized
						OSSleepThread(&fioThreadQueue);
					}
					hkprintf("HK GCN FIO: MCC initialized!\n");
				}
			}
			
			// make sure FIO is initialized
			if (!g_fioInitialized)
			{
				// init FIO communication layer
				if (!FIOInit((MCCExiChannel) g_exiChannel, MCC_CHANNEL_1, 10))
				{
					hkcerr << "HK GCN FIO: FIO communication not available." << hkendl;
					m_flags |= END_OF_FILE;
					return;
				}
				else			
				{
					// spin util host available
					hkprintf("HK GCN FIO: Waiting for FIO connection by host...\n");
					while (!g_fioInitialized)
					{
						// wait until FIO connected
						g_fioInitialized = FIOQuery();
					}
					hkprintf("HK GCN FIO: FIO Connection established!\n");
				}
			}
							
			// open file
			m_fileHandle = FIOFopen(fname, m_fileMode);
			if (m_fileHandle == FIO_INVALID_HANDLE)
			{
				hkcerr << "HK GCN FIO: Can't open file " << fname << hkendl;
				hkcerr << "HK GCN FIO: Note: Files can only be opened once!" << hkendl;
				m_flags |= END_OF_FILE;
			}
			else
			{
				// obtain file size
				if (m_fileMode == INPUT)
				{
					m_fileLength = FIOFseek(m_fileHandle, 0, FIO_SEEK_LAST);
				
					// seek to start of file
					FIOFseek(m_fileHandle, 0, FIO_SEEK_TOP);
				}
			
				m_buffer = hkAlignedAllocate<char>(32, BUFFER_SIZE, HK_MEMORY_CLASS_BASE);			
												
				// tie streambuf buffers to our buffer	
				initBuffer(mode, m_buffer, BUFFER_SIZE);
				m_buffer[BUFFER_SIZE] = 0;
			}	
		}
    

		virtual ~hkGameCubeUSBFileStreambuf()
		{
			// close file
			if(m_fileHandle != HK_NULL)
			{
				flush();
				FIOFclose(m_fileHandle);
				m_fileHandle = HK_NULL;
				hkAlignedDeallocate<char>(m_buffer);
			}
		}


		virtual int sysWrite( const char* buf, int nbytes)
		{
			HK_ASSERT(m_fileHandle != HK_NULL);
			HK_ASSERT(m_fileMode == (FIO_OPEN_WRONLY | FIO_OPEN_CREAT));
			
			// 32 byte allign
			int nbytesToWrite = OSRoundUp32B(nbytes);
			
			// flush CPU cache
			DCFlushRange((void*) buf, BUFFER_SIZE);
			
			int numWritten = FIOFwrite(m_fileHandle, (void*) buf, nbytesToWrite);
			if (numWritten < nbytes)
			{
				int fioError = FIOGetLastError();
				hkcerr << "HK GCN FIO: Write failed with error #" << fioError << hkendl;
				return 0;
			}
			else
			{
				// only move fprward by the original number of bytes
				m_fileOffset += nbytes;
				
				// move the write pointer back to the original number of buyes 
				FIOFseek(m_fileHandle, m_fileOffset, FIO_SEEK_TOP);
				
				return static_cast<unsigned>(numWritten);
			}
		}


		virtual int sysRead( char* buf, int nbytes)
		{
			HK_ASSERT(m_fileHandle != HK_NULL);
			HK_ASSERT(m_fileMode == FIO_OPEN_RDONLY);

			// check for EOF
			if (m_fileOffset >= m_fileLength)
			{
				m_flags |= END_OF_FILE;
				return 0;
			}

			// check for overshoot
			if ((m_fileOffset + nbytes) > m_fileLength)
			{
				nbytes = m_fileLength - m_fileOffset;
				
				// 32 byte allign
				nbytes = OSRoundUp32B(nbytes);
			}
			
			int numRead = FIOFread(m_fileHandle, buf, nbytes);
			if (numRead < 0)
			{
				int fioError = FIOGetLastError();
				hkcerr << "HK GCN FIO: Read failed with error #" << fioError << hkendl;
				return 0;
			}
			else
			{
				m_fileOffset += numRead;
				return static_cast<unsigned>(numRead);
			}
		}


		virtual void sysFlush()
		{
			HK_ASSERT(m_fileHandle != HK_NULL);
			FIOFflush(m_fileHandle);
		}


		virtual hkResult sysSeek( long offset, SeekWhence whence)
		{
			HK_ASSERT(m_fileHandle != HK_NULL);
			
			int mode = 0;
			switch(whence)
			{
				case STREAM_SET:
					mode = FIO_SEEK_TOP;
					break;
				case STREAM_CUR:
					mode = FIO_SEEK_CUR;
					break;
				case STREAM_END:
					mode = FIO_SEEK_LAST;
					break;
				default:
					HK_ASSERT(0 && "HK GCN FIO: No such seek type");
			}
			
			flushBuffers();
			m_fileOffset = FIOFseek(m_fileHandle, offset, FIO_SEEK_TOP);
		
			return m_fileOffset != -1 ? HK_SUCCESS : HK_FAILURE;
		}


	protected:

		FIOHandle		m_fileHandle;
		unsigned int	m_fileOffset;
		unsigned int	m_fileLength;
		int				m_fileMode;
		char* 			m_buffer;
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
