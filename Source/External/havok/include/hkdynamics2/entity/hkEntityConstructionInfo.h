/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_ENTITY_CONSTRUCTION_INFO_H
#define HK_DYNAMICS2_ENTITY_CONSTRUCTION_INFO_H

#include <hkbase/hkBase.h>

class hkShape;

/// The information needed to construct an entity. 
/// You will not need to use this struct directly - only the structs that inherit from it, such as hkRigidBodyConstructionInfo.
struct hkEntityConstructionInfo
{
		/// Creates a new hkEntityConstructionInfo and initializes its data to suitable default values
	inline hkEntityConstructionInfo();

		/// The collision response. See hkMaterial::hkResponseType for hkWorld default implementations
	hkUint16	m_collisionResponse;

		/// Lowers the frequency for processContactCallbacks. A value of 5 means that a callback is risen(?) every 5th frame, default is 0xffff
	hkUint16	m_processContactCallbackDelay;
		
		/// The collision detection representation for this entity.
	const hkShape* m_shape;

	/// This value can be used by collision filters to identify the entity - for example, if a group collision filter is used,
		/// this value would specify the entity's collision group.
	hkUint32 m_collisionFilterInfo;

			
};

#include <hkdynamics2/entity/hkEntityConstructionInfo.inl>

#endif // HK_DYNAMICS2_ENTITY_CONSTRUCTION_INFO_H

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
