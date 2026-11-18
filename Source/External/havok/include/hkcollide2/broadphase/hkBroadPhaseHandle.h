/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HK_COLLIDE2_BROAD_PHASE_HANDLE_H
#define HK_COLLIDE2_BROAD_PHASE_HANDLE_H

/// An identifier used to link the broadphase with the user application.
///	Each object that is to be added to the broadphase must have a hkBroadPhaseHandle.
/// For example, the hkCollidable class is a subclass of hkBroadPhaseHandle,
/// allowing Havok rigid bodies to be used with the broadphase. Alternatively, if you are
/// using the broadphase on its own, you can create your own hkBroadPhaseHandle subclasses.

class hkBroadPhaseHandle
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_CDINFO, hkBroadPhaseHandle);

		void* m_id;
		hkBroadPhaseHandle()
		{
			m_id = 0;
		}
};


#endif // HK_COLLIDE2_BROAD_PHASE_HANDLE_H

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
