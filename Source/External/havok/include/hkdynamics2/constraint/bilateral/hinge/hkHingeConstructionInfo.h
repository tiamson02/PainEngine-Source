/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


#ifndef HK_DYNAMICS2_HINGE_CONSTRUCTION_INFO
#define HK_DYNAMICS2_HINGE_CONSTRUCTION_INFO

#include <hkdynamics2/entity/hkRigidBody.h>
#include <hkmath2/linear/hkVector4Util.h>

/// Construction info for hinge constraint
/// All vector members are wrt body space

/// The information needed to construct a hinge constraint.
/// All vector members are specified in body space.
class hkHingeConstructionInfo  
{

  public:
  
    inline hkHingeConstructionInfo();

	/// Sets the construction information with world space information.
	inline void setInWorldSpace(hkRigidBody* bodyA, hkRigidBody* bodyB,
								const hkVector4& pivot,
								const hkVector4& axis); 

	/// Sets the construction information with body space information.
    inline void setInBodySpace(hkRigidBody* bodyA, hkRigidBody* bodyB,
							   const hkVector4& pivotA, const hkVector4& pivotB,
							   const hkVector4& axisA, const hkVector4& axisB);
	
	// The attached and reference body
	hkRigidBody* m_bodyA;
	hkRigidBody* m_bodyB;

	
	// The pivot in A and B space
	hkVector4 m_pivotInA;
	hkVector4 m_pivotInB;

	// The hinge axis in B space
	hkVector4 m_axleInB;

	// mutually perp vectors to m_axleInA in A space
	hkVector4 m_perpToAxleInA1;
	hkVector4 m_perpToAxleInA2;
};

#include <hkdynamics2/constraint/bilateral/hinge/hkHingeConstructionInfo.inl>

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
