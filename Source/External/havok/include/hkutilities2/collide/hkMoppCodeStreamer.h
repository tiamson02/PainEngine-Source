/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_UTILITIES2_MOPPCODE_STREAMER_H
#define HK_UTILITIES2_MOPPCODE_STREAMER_H


class hkMoppCode;
class hkOArchive;
class hkIArchive;


/// This class implements a very basic streaming of a hkMoppCode object to and from an
/// Archive (platform independent stream). It may be useful to use these methods as a guideline
/// for implementing your own file format, as the implementations used here read/write only the 
/// minimum data required. 
/// In particular, only basic corruption "safety" checks are included. It is up to the user to ensure 
/// any "streamed" MOPP data files are in sync with the hkMoppBvTreeShape you assign them to, otherwise 
/// collision detection will fail. It may be advisable to "hash" the shape into a checksum, and 
/// write this as additional data into the file, to avoid any such conflicts.
/// This code reads/writes only one hkMoppCodes per Archive, but you may wish to concatenate several 
/// hkMoppCodes into a single Archive using the methods below as a guideline.
class hkMoppCodeStreamer
{
	public:

			/// Allocate and fill a hkMoppCode from an Input Archive. 
		static hkMoppCode* HK_CALL readMoppCodeFromArchive(hkIArchive &inputArchive);

			/// Write a hkMoppCode to an Output Archive. 
		static void HK_CALL writeMoppCodeToArchive(hkMoppCode* code, hkOArchive &outputArchive);

};

#endif // HK_UTILITIES2_MOPPCODE_STREAMER_H

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
