/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_UTILITIES2_TKL_STREAMER_H
#define HK_UTILITIES2_TKL_STREAMER_H


class hkOArchive;
class hkIArchive;
class hkOstream;
class hkIstream;
class hkStorageMeshShape;

/// These class implements a very basic streaming of a hkStorageMeshShape object to and from an
/// Archive (platform independent stream) in the tkl file format. This format is in the following form:
/// int: num vertices, triples of 32 bit floats for vertices, int: num triangles, triples of ints for triangle indices
/// btkl is the binary version of this format

class hkTklStreamer
{
public:
	
	/// Allocate and fill a hkStorageMeshShape from an Input Stream of data in the tkl format
	static hkStorageMeshShape* HK_CALL readStorageMeshFromTklStream(hkIstream &inputStream);
	
	/// Write a hkStorageMeshShape to an Output Stream in tkl format. 
	static void HK_CALL writeStorageMeshShapeToTklStream(hkStorageMeshShape* shape, hkOstream &outputStream);

	/// Allocate and fill a hkStorageMeshShape from an Input Archive of data in the btkl format
	static hkStorageMeshShape* HK_CALL readStorageMeshFromBtklArchive(hkIArchive &inputArchive);
	
	/// Write a hkStorageMeshShape to an Output Archive in btkl format. 
	static void HK_CALL writeStorageMeshShapeToBtklArchive(hkStorageMeshShape* shape, hkOArchive &outputArchive);
	
};

#endif // HK_UTILITIES2_TKL_STREAMER_H

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
