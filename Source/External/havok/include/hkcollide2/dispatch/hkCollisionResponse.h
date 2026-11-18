/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_COLLIDE2_COLLISION_RESPONSE_H
#define HK_COLLIDE2_COLLISION_RESPONSE_H

/// Specifies a collision response type for an hkCollidable. See hkMaterial::hkResponseType for the hkWorld default response types.
/// hkCollisionResponses are used to create an appropriate hkContactMgrFactory by the hkCollisionDispatcher.
class hkCollisionResponse
{
	public:

		hkCollisionResponse():	m_primaryResponseType(0), m_secondaryResponseType(0){}

	public:

			/// The primary response type
		hkUint16 m_primaryResponseType;

			/// The secondary response type
		hkUint16 m_secondaryResponseType;
};



#endif // HK_COLLIDE2_COLLISION_RESPONSE_H

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
