/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_BLENDED_RAGDOLL_H
#define HK_BLENDED_RAGDOLL_H

#include <hkutilities2/ragdoll/hkRagdoll.h>

class hkHierarchy;

/// The hkBlendedRagdoll extents the base hkRagdoll by adding a number of gain values.
/// These gains are used in the overriden "updatePose" function.
/// 
/// The hierarchy gain (ranging from 0 to 1) is used in calculating world transforms
/// from the local ones stored in the skeleton. A value of 0 means that the rigid 
/// body parent transform is used. A value of 1 means that the skeleton parent 
/// transform is used. A value of 0.5 uses 50/50 blend of both parents. 
///
/// The velocity gain (ranging from 0 to 1) specifies how much of a velocity blend
/// is applied to the corresponding rigid body. A value of 0 means that the current
/// rigid body velocity is kept in its entirely. A value of 1 sets the velocity to 
/// match that required to follow the skeleton. A value of 0.5 sets a velocity 
/// that is 50% the rigid bodies current velocity and 50% of one calculated from 
/// the skeleton.
/// 
/// The max velocity values set a cap on the maximum angular and linear velocities
/// that may be set to a rigid body trying to match the skeleton. This provides 
/// some control over a rigid body which would be trying to push other objects 
/// out of its path. A balance must be struck between allowing a controlled body 
/// to be stopped by obstacles and allowing the body to follow the skeleton.
/// 
///


struct hkRagdollGainInfo
{
	hkReal	m_velocityGain;
	hkReal	m_hierarchyGain;
	hkReal	m_maxLinearVelocity;
	hkReal	m_maxAngularVelocity;
};

struct hkBlendedRagdollConstructionInfo : public hkRagdollSystemConstructionInfo
{
	hkRagdollGainInfo*	m_gains;
};

class hkBlendedRagdoll : public hkRagdoll
{
	public:
		
		hkBlendedRagdoll( hkBlendedRagdollConstructionInfo& info );

		virtual void updatePose( const hkSkeleton& skeleton, hkReal invDeltaTime );

		inline hkArray<hkRagdollGainInfo>&	getGains();

	private:

		hkArray<hkRagdollGainInfo>	m_gains;
};

#include <hkutilities2/ragdoll/hkBlendedRagdoll.inl>

#endif // HK_BLENDED_RAGDOLL_H
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
