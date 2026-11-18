/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkcollide2/hkCollide2.h>

#include <hkcollide2/dispatch/broadphase/hkNullBroadPhaseListener.h>

void hkNullBroadPhaseListener::addCollisionPair( hkTypedBroadPhaseHandlePair& pair ) 
{ 
	HK_ASSERT2(0, "A broad phase pair has been created for which no handler has been registered" ); 
}

void hkNullBroadPhaseListener::removeCollisionPair( hkTypedBroadPhaseHandlePair& pair ) 
{ 
	HK_ASSERT2(0, "A broad phase pair has been deleted for which no handler has been registered" ); 
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
