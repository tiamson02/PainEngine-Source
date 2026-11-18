/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HKBASE_HKISTREAM_H
#define HKBASE_HKISTREAM_H

/// Provides functionality similar to std::istream.
/// All the usual operators are provided plus operators
/// for 64 bit integers. printf style output is also
/// supported.
#include <hkbase/hkBase.h>

class hkIstream : public hkBaseObject
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_STREAM)

			/// Constructs an hkIstream using the given hkStreambuf for reading.
		hkIstream(hkStreambuf* sb);

			/// Destroys the stream
		~hkIstream();

			/// Checks that this stream has not had a read error.
		bool isOk() const;

			/// Inputs an address (pointed to by p).
		hkIstream& operator>> (void* p);

			/// Inputs a char.
		hkIstream& operator>> (char& c);

			/// Inputs a signed char.
		hkIstream& operator>> (signed char& c);

			/// Inputs unsigned char.
		hkIstream& operator>> (unsigned char& c);

			/// Inputs a string.
			// Will assume the string pointed to by s is at least 100 characters long
		hkIstream& operator>> (char* s);

			/// Inputs a string.
		hkIstream& operator>> (signed char* s);

			/// Inputs a string.
		hkIstream& operator>> (unsigned char* s);

			/// Inputs a short.
		hkIstream& operator>> (short& s);

			/// Input an unsigned short.
		hkIstream& operator>> (unsigned short& s);

			/// Inputs an int.
		hkIstream& operator>> (int& i);

			/// Inputs an unsigned int.
		hkIstream& operator>> (unsigned int& u);

			/// Inputs a float (reads in full double and casts back)
		hkIstream& operator>> (float& f);

			/// Inputs a double. 
			// Will actually read a float
		hkIstream& operator>> (double& d); 

			/// Inputs a 64 bit int.
		hkIstream& operator>> (hkInt64& i);

			/// Input a 64 bit unsigned int
			// Will actually read an int64
		hkIstream& operator>> (hkUint64& u);

			/// Inputs an hkString
		hkIstream& operator>> (hkString& str);

			/// Gets a string with the given delimiter.
		hkIstream& getline(char* str, int maxsize, char delim = '\n');
		
			/// Reads in one character.
		hkIstream& get( char& c );
		
			/// Reads raw data.
		int read( char* buf, int nbytes );
			
			/// Seeks to the specified file position. The return value indicates whether the seek was successful.
		hkResult seek(long offset, hkStreambuf::SeekWhence whence);

			/// Gets the current file offset, or -1 on error.
		int tell();

			/// Returns the stream length or -1 if unknown.
		int getFileSize();

			/// Returns the underlying streambuf used by this hkIstream.
		hkStreambuf* getStreambuf();

			/// Sets the underlying streambuf for this hkIstream.
		void setStreambuf(hkStreambuf* newbuf);

	protected:

		hkStreambuf*	m_streambuf;
		
	private:
	
		char*			m_tempStorage;
};

/// An istream which is connected to a file.
class hkIfstream : public hkIstream
{
	public:

		hkIfstream(const char* filename);
};

#include <hkbase/stream/hkIstream.inl>

#endif // HKBASE_HKISTREAM_H

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
