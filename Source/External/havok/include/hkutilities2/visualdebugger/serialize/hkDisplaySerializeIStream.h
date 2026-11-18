/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


#ifndef HK_DISPLAY_SERIALIZE_I_STREAM_H
#define HK_DISPLAY_SERIALIZE_I_STREAM_H

#include <hkbase/hkBase.h>
#include <hkbase/stream/hkIArchive.h>

// non primitive datatypes which are streamable
class hkVector4;
class hkRotation;
class hkTransform;
class hkTriangleIndices;
class hkGeometry;
class hkDisplayGeometry;

/// This utility class extends the hkIArchive to provide a endian
/// safe stream interface for deserializing all of the basic data 
/// types associated with the visual debugger.
class hkDisplaySerializeIStream : public hkIArchive
{
	public:

			/// Create a hkDisplaySerializeIStream from a basic hkStreambuf which
			/// must be readable.
		hkDisplaySerializeIStream(hkStreambuf* ioStreamBuff);

			/// Deserialize a hkVector4.
		void readQuadVector4(hkVector4& v);

			/// Deserialize a hkTransform.
		void readTransform(hkTransform& t);

			/// Deserialize a hkTriangleIndices.
		void readTriangleIndices(hkTriangleIndices& ti);

			/// Deserialize a hkGeometry.
		void readGeometry(hkGeometry& g);

			/// Deserialize a hkDisplayGeometry.
		void readDisplayGeometry(hkDisplayGeometry*& dg);

//		void readRotation(hkRotation& r);
};



#endif //  HK_DISPLAY_SERIALIZE_I_STREAM_H

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
