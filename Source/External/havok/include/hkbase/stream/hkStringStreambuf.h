/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HKBASE_HKSTRINGSTREAMBUF_H
#define HKBASE_HKSTRINGSTREAMBUF_H

/// A streambuf for in-memory reading and writing streams.
/// You can use the convenience classes hkIstringStream, hkOstringStream
/// instead of using this class directly.
class hkStringStreambuf : public hkStreambuf
{
	public:

			/// Creates a streambuf for writing.
		hkStringStreambuf();

			/// Creates a streambuf for reading.
		hkStringStreambuf(const char* buf, int len, bool copy = true);

			/// Gets the current null terminated written string.
		const char* getString();

			/// Gets the streambuf's write buffer.
		hkArray<char>& getWriteBuffer() { return m_writeBuffer; }

			/// Swaps the read and write buffers of the stream, the getArea
			/// for buffered reading will also be reset to reference the 
			/// new read buffer and the current read position will be moved
			/// to the start (of the read buffer).
		void swapBuffers();

	protected:

		virtual int sysWrite(const char* buf, int nbytes);
		virtual int sysRead(char* buf, int nbytes);
		virtual void sysFlush();

	private:

		hkArray<char> m_writeBuffer;
		hkArray<char> m_readBuffer;
};

#endif // HKBASE_HKSTRINGSTREAMBUF_H

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
