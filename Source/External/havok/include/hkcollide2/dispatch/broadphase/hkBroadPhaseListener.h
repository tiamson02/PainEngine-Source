/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_BROAD_PHASE_LISTENER
#define HK_BROAD_PHASE_LISTENER

class hkTypedBroadPhaseHandlePair;

class hkBroadPhaseListener
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BROAD_PHASE, hkBroadPhaseListener)

		virtual ~hkBroadPhaseListener() {}

		virtual void addCollisionPair( hkTypedBroadPhaseHandlePair& pair ) = 0;

		virtual void removeCollisionPair( hkTypedBroadPhaseHandlePair& pair ) = 0;

};

#endif // HK_BROAD_PHASE_LISTENER

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
