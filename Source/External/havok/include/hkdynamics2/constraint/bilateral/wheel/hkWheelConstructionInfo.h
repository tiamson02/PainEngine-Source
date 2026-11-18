/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


#ifndef HK_DYNAMICS2_WHEEL_CONSTRUCTION_INFO_H
#define HK_DYNAMICS2_WHEEL_CONSTRUCTION_INFO_H

#include <hkdynamics2/entity/hkRigidBody.h>
#include <hkmath2/linear/hkVector4Util.h>

///The information needed to construct an hkWheelConstraint.
class hkWheelConstructionInfo  
{

	public:
  
		inline hkWheelConstructionInfo();

		/// Sets the construction information with world space information.
		inline void setInWorldSpace(hkRigidBody* bodyA, hkRigidBody* bodyB, 
								const hkVector4& pivot,
								const hkVector4& axle,
								const hkVector4& suspensionAxis,
								const hkVector4& steeringAxis ); 

		/// Sets the construction information with body space information.
		inline void setInBodySpace(hkRigidBody* bodyA, hkRigidBody* bodyB,
								const hkVector4& pivotA, const hkVector4& pivotB,
								const hkVector4& axleA, const hkVector4& axleB,
								const hkVector4& suspensionAxisB,
								const hkVector4& steeringAxisB );

			///Sets the minimum limit of vertical wheel movement along the suspension direction.
		inline void setMinSuspensionLimit(hkReal min);

			///Sets the maximum limit of vertical wheel movement along the suspension direction.
		inline void setMaxSuspensionLimit(hkReal max);

			///Gets the suspension stiffness percentage.
		inline hkReal getSuspensionStrength();

			///Sets the suspension stiffness percentage.
		inline void setSuspensionStrength( hkReal tau );

			///Gets the suspension damping percentage.
		inline hkReal getSuspensionDamping();

			///Sets the suspension damping percentage.
		inline void setSuspensionDamping( hkReal damp );

		inline void computeSecondaryValues();

		/// The attached and reference bodies
		hkRigidBody* m_bodyA;
		hkRigidBody* m_bodyB;

		/// Pivots in space of each body
		hkVector4 m_pivotInA;
		hkVector4 m_pivotInB;

		/// Steering axis in B space
		hkVector4 m_steeringAxisInB;

		/// Perp to Steering axis in B space - secondary value
		hkVector4 m_perpToSteeringAxisInB;

		/// Free axis of wheel rotation - axle in A space
		hkVector4 m_axleInA;

		/// Initial in B space
		hkVector4 m_referenceAxleInB;

		/// Suspension axis in B space
		hkVector4 m_suspensionAxisInB;
		
		/// Mutually perp vectors to  m_suspensionAxisInB in B space - secondary values
		hkVector4 m_perpToSuspensionAxisInB1;
		hkVector4 m_perpToSuspensionAxisInB2;

		/// Linear limits along suspension axis
		hkReal m_suspensionMinLimit;
		hkReal m_suspensionMaxLimit;

		/// Stiffness and damping for linear constraint along suspension axis
		hkReal m_suspensionStrength;
		hkReal m_suspensionDamping;
 
};

#include <hkdynamics2/constraint/bilateral/wheel/hkWheelConstructionInfo.inl>

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
