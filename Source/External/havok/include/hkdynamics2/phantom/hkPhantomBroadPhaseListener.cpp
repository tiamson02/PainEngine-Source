/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkdynamics2/hkDynamics2.h>
#include <hkdynamics2/phantom/hkPhantomBroadPhaseListener.h>
#include <hkdynamics2/phantom/hkPhantom.h>
#include <hkdynamics2/world/hkBroadPhaseTypes.h>

#include <hkcollide2/dispatch/broadphase/hkTypedBroadPhaseHandlePair.h>

void hkPhantomBroadPhaseListener::addCollisionPair( hkTypedBroadPhaseHandlePair& pair )
{
	if ( pair.getElementA()->getType() == HK_BROAD_PHASE_PHANTOM )
	{
		static_cast<hkPhantom*>(static_cast<hkCollidable*>(pair.m_a)->getOwner())->addOverlappingCollidable( static_cast<hkCollidable&>(*pair.m_b) );
	}

	if ( pair.getElementB()->getType() == HK_BROAD_PHASE_PHANTOM )
	{
		static_cast<hkPhantom*>(static_cast<hkCollidable*>(pair.m_b)->getOwner())->addOverlappingCollidable( static_cast<hkCollidable&>(*pair.m_a) );
	}
}


void hkPhantomBroadPhaseListener::removeCollisionPair( hkTypedBroadPhaseHandlePair& pair )
{
	if ( pair.getElementA()->getType() == HK_BROAD_PHASE_PHANTOM )
	{
		static_cast<hkPhantom*>(static_cast<hkCollidable*>(pair.m_a)->getOwner())->removeOverlappingCollidable( static_cast<hkCollidable&>(*pair.m_b) );
	}

	if ( pair.getElementB()->getType() == HK_BROAD_PHASE_PHANTOM )
	{
		static_cast<hkPhantom*>(static_cast<hkCollidable*>(pair.m_b)->getOwner())->removeOverlappingCollidable( static_cast<hkCollidable&>(*pair.m_a) );
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
