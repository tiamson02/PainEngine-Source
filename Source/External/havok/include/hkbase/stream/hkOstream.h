/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HKBASE_HKOSTREAM_H
#define HKBASE_HKOSTREAM_H

class hkOstream;

namespace hkOstreamManip
{
		/// Ostream manipulation operator.
	typedef hkOstream& (HK_CALL *function)(hkOstream&);

		/// Output 'endl'
	hkOstream& HK_CALL endl(hkOstream& os);

		/// Flush stream
	hkOstream& HK_CALL flush(hkOstream& os);	
}



/// Provides functionality similar to std::ostream.
/// All the usual operators are provided plus operators
/// for 64 bit integers. printf style output is also
/// supported.
class hkOstream : public hkBaseObject
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_STREAM)

			/// Constructs an hkOstream using the given hkStreambuf for writing.
		hkOstream(hkStreambuf* sb);

			/// Destroys the stream.
		~hkOstream();

			/// Checks the error status of the stream.
		bool isOk() const;

			/// Outputs an address (pointed to by p).
		hkOstream& operator<< (const void* p);

			/// Outputs a char.
		hkOstream& operator<< (char c);

			/// Outputs a signed char.
		hkOstream& operator<< (signed char c);

			/// Outputs an unsigned char.
		hkOstream& operator<< (unsigned char c);

			/// Outputs a string.
		hkOstream& operator<< (const char* s);

			/// Outputs a string.
		hkOstream& operator<< (const signed char* s);

			/// Outputs a string.
		hkOstream& operator<< (const unsigned char* s);

			/// Outputs a short.
		hkOstream& operator<< (short s);

			/// Outputs an unsigned short
		hkOstream& operator<< (unsigned short s);

			/// Outputs an int
		hkOstream& operator<< (int i);

			/// Outputs an unsigned int.
		hkOstream& operator<< (unsigned int u);

			/// Outputs a float (cast to float as output is set pres anyway)
		hkOstream& operator<< (float f);

			/// Outputs a double.
		hkOstream& operator<< (double d);

			/// Outputs a 64 bit int.
		hkOstream& operator<< (hkInt64 i);

			/// Outputs a 64 bit unsigned int.
		hkOstream& operator<< (hkUint64 u);
			
			/// Outputs an hkString.
		hkOstream& operator<< (const hkString& str);

			/// Outputs raw data.
		int write( const char* buf, int nbytes);

			/// Outputs formatted data.
		void printf( const char *fmt, ...);

			/// Flushes printf stream.
		void flush();

			/// Seeks to the specified file position.
		long seek(long offset, hkStreambuf::SeekWhence whence);

			/// Manipulates the stream format.
		hkOstream& operator<< (hkOstreamManip::function f);

			/// Returns the underlying streambuf used by this hkOstream.
		hkStreambuf* getStreambuf();

			/// Set the underlying streambuf for this hkOstream
		void setStreambuf(hkStreambuf* newbuf);
		
	protected:

		hkStreambuf*	m_streambuf;
};

class hkOfstream : public hkOstream
{
	public:

		hkOfstream(const char* filename);
};


#define hkendl hkOstreamManip::endl
#define hkflush hkOstreamManip::flush

#include <hkbase/stream/hkOstream.inl>

#endif // HKBASE_HKOSTREAM_H

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
