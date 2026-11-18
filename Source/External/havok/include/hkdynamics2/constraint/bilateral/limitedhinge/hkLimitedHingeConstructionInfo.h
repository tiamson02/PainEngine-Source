/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


#ifndef HK_DYNAMICS2_LIMITEDHINGE_CONSTRUCTION_INFO_H
#define HK_DYNAMICS2_LIMITEDHINGE_CONSTRUCTION_INFO_H

#include <hkdynamics2/entity/hkRigidBody.h>
#include <hkmath2/linear/hkVector4Util.h>

/// The information needed to create a limited hinge constraint.
/// All vector members are specified in body space.

class hkLimitedHingeConstructionInfo  
{

  public:
  
    inline hkLimitedHingeConstructionInfo();

	/// Sets the construction information with world space information. Note that the axis
	/// perpendicular to the hinge axis (used to specify the angular limits) is automatically
	/// calculated for you if you use this method.
	inline void setInWorldSpace(hkRigidBody* bodyA, hkRigidBody* bodyB,
								const hkVector4& pivot, const hkVector4& axis); 

	/// Set the construction information with body space information.
    inline void setInBodySpace(hkRigidBody* bodyA, hkRigidBody* bodyB, 
								const hkVector4& pivotA, const hkVector4& pivotB,
								const hkVector4& axisA, const hkVector4& axisB,
								const hkVector4& axisAPerp, const hkVector4& axisBPerp);

		/// Sets the friction value for this hinge.
	inline void setMaxFrictionTorque(hkReal mag);

		/// Sets the maximum angular limit.
    inline void setMaxAngularLimit(hkReal rad);

		/// Sets the minimum angular limit.
    inline void setMinAngularLimit(hkReal rad);

	// Attached and reference bodies
	hkRigidBody* m_bodyA;
	hkRigidBody* m_bodyB;

	// Pivot in body space of each entity
	hkVector4 m_pivotInA;
    hkVector4 m_pivotInB;

	// Free Axis in space of each entity
	hkVector4 m_axleInA;
    hkVector4 m_axleInB;

	// Vectors mutually perp to free axis in A space
	hkVector4 m_perpToAxleInA1;
    hkVector4 m_perpToAxleInA2;

	// Vector perp to free axis in B space - same vector in world space as m_perpToAxleInA2
    hkVector4 m_perp2FreeAxisInB;

	// Angular limits
    hkReal m_minAngle;
	hkReal m_maxAngle;

	// Max angular friction impulse
	hkReal m_maxFrictionTorque;
};

#include <hkdynamics2/constraint/bilateral/limitedhinge/hkLimitedHingeConstructionInfo.inl>

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
