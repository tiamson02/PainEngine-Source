/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


#ifndef HK_BASE_IARCHIVE_H
#define HK_BASE_IARCHIVE_H


/// An input archive class.
/// Inputs all data in little endian format and converts on the fly if necessary.
class hkIArchive: public hkBaseObject
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_STREAM)
		
			/// Constructs a new hkIArchive with the specified hkStreambuf.
		hkIArchive(hkStreambuf* sb);

			/// Destructor.
		~hkIArchive();

			/// Reads 8 bits.
		hkChar		read8();

			/// Reads 8 bits.
		hkUchar		read8u();
		
			/// Reads 16 bits.
		hkInt16		read16();

			/// Reads 16 bits.
		hkUint16	read16u();

			/// Reads 32 bits.
		hkInt32		read32();

			/// Reads 32 bits.
		hkUint32	read32u();

			/// Reads 64 bits
		hkInt64		read64();

			/// Reads 64 bits.
		hkUint64	read64u();

			/// Reads 32 bit floats.
		hkFloat32	readFloat32();

			/// Reads 64 bit doubles.
		hkDouble64	readDouble64();

			/// Returns the current error status of the stream.
		bool isOk() const;
		
			/// Reads raw data.
		int read( char* buf, int nbytes);

			/// Writes a linear array of hkFloat32s
		void readFloats(hkFloat32* array, int arraySize);

			/// Returns the underlying streambuf used by this hkIstream
		hkStreambuf* getStreambuf();

			/// Sets the underlying streambuf for this hkIstream
		void setStreambuf(hkStreambuf* newbuf);

	protected:

			/// The underlying stream buffer
		hkStreambuf*		m_streambuf;
};


/// An hkIArchive that is connected to a file.
class hkIfArchive : public hkIArchive
{
	public:

		hkIfArchive(const char* filename);
};


#endif // HK_BASE_IARCHIVE_H


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
