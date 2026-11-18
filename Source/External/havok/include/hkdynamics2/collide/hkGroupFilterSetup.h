/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_GROUP_FILTER_SETUP_H
#define HK_DYNAMICS2_GROUP_FILTER_SETUP_H

#include <hkdynamics2/collide/hkGroupFilter.h>


	/// A helper class to create a nice predefined set of collision groups
	/// (of course you can use your own enums).
	/// There is a list of basic groups for game objects:
	/// LAYER_STATIC   collides with everything but LAYER_KEYFRAME
	/// LAYER_DYNAMIC  collides with everything
	/// LAYER_PLAYER   collides with everything
	/// LAYER_AI	      collides with everything
	/// LAYER_KEYFRAME collides with everything but LAYER_KEYFRAME, LAYER_STATIC, (good for keyframe anymation)
	/// LAYER_DEBRIS   collides with everything 
	/// LAYER_FAST_DEBRIS   collides with everything but LAYER_FAST_DEBRIS
class hkGroupFilterSetup
{
	public:

		enum DefaultCollisionGroups
		{
			// basic groups
			LAYER_STATIC = 0,
			LAYER_DYNAMIC,
			LAYER_PLAYER,
			LAYER_AI,
			LAYER_KEYFRAME,
			LAYER_DEBRIS,
			LAYER_FAST_DEBRIS
		};

			/// initialize the group filter (see class documentation for details)
		static void setupGroupFilter( hkGroupFilter* filter )
		{
			hkUint32 allBits = (1<<LAYER_STATIC) | (1<<LAYER_DYNAMIC) | (1<<LAYER_PLAYER) | (1<<LAYER_AI) | (1<<LAYER_KEYFRAME) |(1<<LAYER_DEBRIS) | (1<<LAYER_FAST_DEBRIS);
			filter->enableCollisionsUsingBitfield ( allBits, allBits );
			filter->disableCollisionsBetween( LAYER_KEYFRAME,     LAYER_KEYFRAME);
			filter->disableCollisionsBetween( LAYER_KEYFRAME,     LAYER_STATIC );
			filter->disableCollisionsBetween( LAYER_FAST_DEBRIS,  LAYER_FAST_DEBRIS );
		}
};


#endif // HK_DYNAMICS2_GROUP_FILTER_SETUP_H

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
