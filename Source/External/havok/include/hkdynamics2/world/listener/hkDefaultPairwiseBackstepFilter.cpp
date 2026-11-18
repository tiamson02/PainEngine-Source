/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkdynamics2/hkDynamics2.h>
#include <hkdynamics2/world/listener/hkDefaultPairwiseBackstepFilter.h>
#include <hkdynamics2/entity/hkRigidBody.h>
#include <hkcollide2/hkCollidable.h>

bool hkDefaultPairwiseBackstepFilter::shouldBackstep( const hkCollidable& collA, const hkCollidable& collB ) const
{
	const hkRigidBody* rba = static_cast<hkRigidBody*>(collA.getOwner() );
	const hkRigidBody* rbb = static_cast<hkRigidBody*>(collB.getOwner() );

	//
	// Backstep if one of the objects in the collision is keyframed or fixed
	//

	if ( rba->isFixed() || rbb->isFixed() || rba->isKeyframed() || rbb->isKeyframed() )
	{
		return true;
	}
	else
	{
		return false;
	}
}

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
