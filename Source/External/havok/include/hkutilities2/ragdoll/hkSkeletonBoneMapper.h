/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HKSKELETONBONEMAPPER_H
#define HKSKELETONBONEMAPPER_H

class hkSkeleton;

/// This class maps one skeleton onto another and vice versa.
/// Examples of a bone mapper derived class could be offset, low to high, 
/// one to one or onto mappings. 
class hkSkeletonBoneMapper : public hkBaseObject
{
	public:

		/// Apply skeleton mapping 
		virtual void applyMapping( const hkSkeleton& skeletonA, hkSkeleton& skeletonB ) = 0;

		/// Apply the inverse of the skeleton mapping
		virtual void applyMappingInverse( const hkSkeleton& skeletonB, hkSkeleton& skeletonA ) = 0;
};

#endif
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
