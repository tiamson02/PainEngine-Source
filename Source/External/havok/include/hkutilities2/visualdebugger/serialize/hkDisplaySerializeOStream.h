/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


#ifndef HK_DISPLAY_SERIALIZE_O_STREAM_H
#define HK_DISPLAY_SERIALIZE_O_STREAM_H

#include<hkbase/hkBase.h>
#include<hkbase/stream/hkOArchive.h>

// non primitive datatypes which are streamable
class hkVector4;
class hkRotation;
class hkTransform;
class hkTriangleIndices;
class hkGeometry;
class hkDisplayGeometry;

/// This utility class extends the hkOArchive to provide a endian
/// safe stream interface for serializing all of the basic data 
/// types associated with the visual debugger.
class hkDisplaySerializeOStream : public hkOArchive
{
	public:
			
			/// Create a hkDisplaySerializeOStream from a basic hkStreambuf which
			/// must be writable.
		hkDisplaySerializeOStream(hkStreambuf* ioStreamBuff);

			/// Serialize a hkVector4.
		void writeQuadVector4(const hkVector4& v);

			/// Serialize a hkTransform.
		void writeTransform(const hkTransform& t);

			/// Serialize a hkTriangleIndices.
		void writeTriangleIndices(const hkTriangleIndices& ti);

			/// Serialize a hkGeometry.
		void writeGeometry(const hkGeometry& g);

			/// Serialize a hkDisplayGeometry.
		void writeDisplayGeometry(hkDisplayGeometry* dg);

		//void writeRotation(const hkRotation& r); // not used yet
};



#endif //  HK_DISPLAY_SERIALIZE_O_STREAM_H

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
