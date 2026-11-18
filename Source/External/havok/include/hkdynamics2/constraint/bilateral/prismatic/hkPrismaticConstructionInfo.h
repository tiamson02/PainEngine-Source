/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


#ifndef HK_DYNAMICS2_PRISMATIC_CONSTRUCTION_INFO_H
#define HK_DYNAMICS2_PRISMATIC_CONSTRUCTION_INFO_H

#include <hkdynamics2/entity/hkRigidBody.h>
#include <hkmath2/linear/hkVector4Util.h>

/// The information needed to construct a prismatic constraint.
/// All vector members are specified in body space.
class hkPrismaticConstructionInfo  
{
	public:
  
		inline hkPrismaticConstructionInfo();


		/// Sets the construction information with world space information.
		/// For best behavior the pivot point should be in body B.
		inline void setInWorldSpace(hkRigidBody* bodyA, hkRigidBody* bodyB,
									const hkVector4& pivot, const hkVector4& axis);
		
		/// Sets the construction information with body space information.    
		/// For best behavior the pivot point should be in body B.
		inline void setInBodySpace(hkRigidBody* bodyA, hkRigidBody* bodyB,
									const hkVector4& pivotA, const hkVector4& pivotB,
									const hkVector4& axisA, const hkVector4& axisB,
									const hkVector4& axisAPerp, const hkVector4& axisBPerp);

			///Sets the maximum limit for the attached body's movement along the axis.
		inline void setMaxLinearLimit(hkReal max);

			///Sets the minimum limit for the attached body's movement along the axis.
		inline void setMinLinearLimit(hkReal min);

			///Sets the friction value for this constraint.
		inline void setMaxFrictionForce(hkReal fmag);

		// The attached and reference body
		hkRigidBody* m_bodyA;
		hkRigidBody* m_bodyB;

		// Pivot in attached body and reference body space
		hkVector4 m_pivotInA;
		hkVector4 m_pivotInB;

		// Free axis in attached body and reference body space
		hkVector4 m_shaftInA;
		hkVector4 m_shaftInB;

		// Perp vectors to free axis in B space
		hkVector4 m_perpToShaftInB1;
		
		// Perp vectors to m_axleInB and m_perpToAxleInB1 in B space
		hkVector4 m_perpToShaftInB2;

		//hkVector4 m_bodyMutuallyPerpB;

		// The offset rotation of the bodies 
		hkRotation m_BtoAoffsetRotation;

		// Linear limits
		hkReal m_minLimit;
		hkReal m_maxLimit;

		// Max friction impulse along free axis
		hkReal m_maxFrictionForce;
};

#include <hkdynamics2/constraint/bilateral/prismatic/hkPrismaticConstructionInfo.inl>

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
