/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HKLOTOHIBONEMAPPER_H
#define HKLOTOHIBONEMAPPER_H

#include <hkutilities2/ragdoll/hkSkeletonBoneMapper.h>
#include <hkbase/htl/hkMap.h>

/// The class defines a mapper that links a low resolution skeleton to a
/// higher resolution equilvalent. Primarily the low to high mapper is used to
/// match an animation skeleton with a ragdoll skeleton (which has only a 
/// subset of the bones).
/// This mapper is similar to a one to one mapping in functionality.
/// It assumes that every bone in the lo-rez skeleton has a twin in the hi, 
/// otherwise why is the bone there?
/// It also assumes that each lo-rez bone aligns exactly with is hi-rez twin, 
/// saving on any extra offset transform.
///
class hkLoToHiBoneMapper : public hkSkeletonBoneMapper
{
	public:

		/// Construct a mapper with the specified mappings
		hkLoToHiBoneMapper( const hkArray<int>& mappings );

		/// Get the array of mappings.
		inline const hkArray<int>& getMappings() const;

		/// Get the equivalent index in the hi-rez skeleton for the specified lo-rez bone
		int	getImage( int iLo );

		/// Get the equivalent index in the lo-rez skeleton for the specified hi-rez bone
		/// Function returns -1 if no inverse mapping exists.
		int	getPreImage( int iHi );

		/// Map a high resolution skeleton onto a low resolution version
		virtual void applyMapping( const hkSkeleton& skeletonHi, hkSkeleton& skeletonLo );

		/// Map a low resolution skeleton onto a high resolution version
		virtual void applyMappingInverse( const hkSkeleton& skeletonLo, hkSkeleton& skeletonHi );

	private:

		hkArray<int>	m_mappings;
};

#include <hkutilities2/ragdoll/hkLoToHiBoneMapper.inl>

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
