/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


#ifndef HK_DYNAMICS2_WORLD_PAIRWISE_BACKSTEP_FILTER_H
#define HK_DYNAMICS2_WORLD_PAIRWISE_BACKSTEP_FILTER_H

#include <hkbase/hkBase.h>

class hkCollidable;


	/// Implement this class to say whether a pair of collidables should backstep or not.
class hkWorldPairwiseBackstepFilter : public hkReferencedObject
{
	public:

			/// Returns true if the pair of objects should backstep, false if they should be dealt with purely by
			/// penetration depth calculations. 
			/// Note that keyframed - keyframed and keyframed - fixed collisions are filtered automatically before 
			/// this method is called, as these types of collisions should never be backstepped.
		virtual bool shouldBackstep( const hkCollidable& collA, const hkCollidable& collB ) const = 0;
};


#endif // HK_DYNAMICS2_WORLD_PAIRWISE_BACKSTEP_FILTER_H

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
