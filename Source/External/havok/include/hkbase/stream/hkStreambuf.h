/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HKBASE_HKSTREAMBUF_H
#define HKBASE_HKSTREAMBUF_H

class hkStreambufSource;

/// A generic interface for reading and writing a stream of bytes.
/// If the bufarea has zero capacity, then read and write pass straight
/// through to sysRead/sysWrite. When the bufarea has nonzero capacity,
/// we try to sysRead/sysWrite a complete buffer at a time because of
/// alignment and block size issues on consoles.
class hkStreambuf : public hkReferencedObject
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_STREAM)

			/// Havok has three 'standard' streams - stdin, stdout and stderr.
		enum StdStream { STDIN=0, STDOUT, STDERR };

			/// Havok streams can currently be opened for input or output, but not both.
			/// Append mode assumes output.
		enum OpenMode { INPUT=1, OUTPUT=2, APPEND=3 };

			/// Parameter for seek method.
		enum SeekWhence { STREAM_SET=0, STREAM_CUR=1, STREAM_END=2 };



			///
		hkStreambuf();

			/// Flushes any internal write buffers of this hkStreambuf.
		void flush();

			/// Writes nbytes. Returns the number of bytes written.
		int write(const char* buf, int nbytes);

			/// Reads nbytes bytes into the array buf. Returns the number of bytes read.
		int read(char* buf, int nbytes);

			/// Seeks to the specified offset, returning the absolute offset or -1 on failure.
		hkResult seek(long offset, SeekWhence whence);

			/// Gets the current file offset, or -1 on error.
		int tell();

			/// Returns the stream length, or -1 if unknown.
		int getFileSize();

			/// Checks that this streambuf has not tried to read past end of file.
		bool isOk() const;


	protected:

			/// System write of buffer.
		virtual int sysWrite(const char* buf, int nbytes) = 0;

			/// System write into buffer.
		virtual int sysRead(char* buf, int nbytes) = 0;

			/// Flushes system buffers.
		virtual void sysFlush() = 0;

			/// Seeks to stream position, by default not supported.
		virtual hkResult sysSeek(long offset, SeekWhence whence);

			/// 
		virtual int sysTell();

		virtual int sysGetFileSize();

			/// Sets up a buffer for the streambuf.  The mode indicates
			/// whether or not an input or and output buffer is used.
			/// 
		void initBuffer(OpenMode mode, char* buffer, int length);

			/// Flushes this streambuf's input and output buffers.
		void flushBuffers();

		enum FlagMask { END_OF_FILE=1, WRITE_ERROR=2, NETWORK_ERROR=4, NETWORK_DISCONNECTED=8 };

			/// Represents a buffer used by an hkStreambuf when reading or writing data. By default, a BufArea has zero size.
		struct BufArea
		{
			BufArea() : m_start(0), m_current(0), m_size(0), m_capacity(0) { }
			int usedBytes() const { return m_current; }
			int spareBytes() const { return m_size - m_current; }
			char* curPointer() const { return m_start + m_current; }
			int capacity() const { return m_capacity; }
			int size() const { return m_size; }
			char* m_start;
			int m_current;
			int m_size; // usable buffer size - only less than capacity on short read
			int m_capacity; // buffer region is m_start,m_start+m_capacity
		};

		unsigned m_flags;
		BufArea m_getArea;
		BufArea m_putArea;
};

#endif // HKBASE_HKSTREAMBUF_H

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
