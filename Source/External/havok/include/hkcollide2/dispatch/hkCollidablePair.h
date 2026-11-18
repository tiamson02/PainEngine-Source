/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_COLLIDE2_COLLIDABLE_PAIR_H
#define HK_COLLIDE2_COLLIDABLE_PAIR_H

#include <hkbase/hkBase.h>
#include <hkcollide2/dispatch/broadphase/hkTypedBroadPhaseHandlePair.h>
#include <hkcollide2/hkCollidable.h>

class hkCollisionAgent;
class hkContactMgr;
class hkCollidable;

/// A pair of objects that are close to each other and either colliding or potentially colliding.
/// If two object AABBs overlap with each other, a hkCollidablePair will be created for them.
class hkCollidablePair : public hkTypedBroadPhaseHandlePair
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_CDINFO, hkCollidablePair);

		inline hkCollidable* getCollidableA();
		inline hkCollidable* getCollidableB();

		hkCollisionAgent*	m_collisionAgent;
		hkContactMgr*		m_contactMgr;

		hkInt32				m_internalData;
};

#include <hkcollide2/dispatch/hkCollidablePair.inl>

#endif // HK_COLLIDE2_COLLIDABLE_PAIR_H

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
