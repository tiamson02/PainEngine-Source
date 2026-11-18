/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


#ifndef HK_STIFFSPRING_CONSTRUCTION_INFO_H
#define HK_STIFFSPRING_CONSTRUCTION_INFO_H

class hkRigidBody;

/// The information needed to construct an hkStiffSpringConstraint.
class hkStiffSpringConstructionInfo  
{

  public:
  
    inline hkStiffSpringConstructionInfo();

	/// Sets the construction information with world space information.
	inline void setInWorldSpace(hkRigidBody* bodyA, hkRigidBody* bodyB,
								const hkVector4& pivotA, const hkVector4& pivotB);
	
	/// Set the construction information with body space information.
    inline void setInBodySpace(hkRigidBody* bodyA, hkRigidBody* bodyB,
								const hkVector4& pivotA, const hkVector4& pivotB);

		///Gets the length of the stiff spring.
	inline hkReal getSpringLength();

		///Sets the length of the stiff spring.
    inline void setSpringLength( hkReal length);

	// The attached and reference body
	hkRigidBody* m_bodyA;
	hkRigidBody* m_bodyB;

	// Pivot in body space
	hkVector4 m_pivotInA;
    hkVector4 m_pivotInB;

	// Rest length of the constraint
    hkReal m_springLength;
};

#include <hkdynamics2/constraint/bilateral/stiffspring/hkStiffSpringConstructionInfo.inl>

///////////////////////////////////////////////////////////

#endif

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
