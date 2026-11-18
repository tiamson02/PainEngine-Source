/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


#ifndef HK_BASE_OARCHIVE_H
#define HK_BASE_OARCHIVE_H


/// An output archive class.
/// Outputs all data in little endien format and converts on the fly if necessary.
class hkOArchive: public hkBaseObject
{
	public:


		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_STREAM)
		
			/// Creates a new hkOArchive with the specified hkStreambuf.
		hkOArchive(hkStreambuf* sb);

			/// Destructor.
		~hkOArchive();

			/// Writes 8 bits.
		void write8(hkChar c);

			/// Writes 8 bits.
		void write8u(hkUchar u);
		
			/// Writes 16 bits.
		void write16(hkInt16 i);

			/// Writes 16 bits.
		void write16u(hkUint16 u);

			/// Writes 32 bits.
		void write32(hkInt32 i);

			/// Writes 32 bits.
		void write32u(hkUint32 u);

			/// Write 64 bits.
		void write64(hkInt64 i);

			/// Writes 64 bits.
		void write64u(hkUint64 u);

			/// Writes 32 bit floats.
		void writeFloat32(hkFloat32 f);

			/// Writes 64 bit doubles.
		void writeDouble64(hkDouble64 d);

			/// Returns the current error status of the stream.
		bool isOk() const;
		
			/// Writes raw data.
		int write(char* buf, int nbytes);

			/// Writes a linear array of hkFloat32s.
		void writeFloats(const hkFloat32* array, int arraySize);

			
			/// Returns the underlying streambuf used by this hkOArchive.
		hkStreambuf* getStreambuf();

			/// Set the underlying streambuf for this hkOArchive
		void setStreambuf(hkStreambuf* newbuf);

	protected:

			/// The underlying stream buffer.
		hkStreambuf*		m_streambuf;
};		


/// An oArchive which is connected to a file.
class hkOfArchive : public hkOArchive
{
	public:

		hkOfArchive(const char* filename);
};


#endif // HK_BASE_OARCHIVE_H


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
