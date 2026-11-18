/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


#ifndef HK_DYNAMICS2_RAGDOLL_CONSTRUCTION_INFO
#define HK_DYNAMICS2_RAGDOLL_CONSTRUCTION_INFO

#include <hkdynamics2/entity/hkRigidBody.h>

/// The information needed to construct a ragdoll constraint.
/// All vector members are specified in body space.
class hkRagdollConstructionInfo  
{

	public:
  
		hkRagdollConstructionInfo();

			/// Sets the construction information with world space information.
		inline void setInWorldSpace(hkRigidBody *bodyA, hkRigidBody *bodyB, 
									const hkVector4& pivot,
									const hkVector4& twistAxisW, 
									const hkVector4& planeAxisW);
		
			/// Sets the construction information with body space information.
		inline void setInBodySpace(hkRigidBody* bodyA,hkRigidBody* bodyB,
									const hkVector4& pivotA, const hkVector4& pivotB,
									const hkVector4& planeAxisA, const hkVector4& planeAxisB,
									const hkVector4& twistAxisA, const hkVector4& twistAxisB);

			/// Sets the minimum twist limit.
		inline void setTwistMinAngularLimit(hkReal rad);

			/// Sets the maximum twist limit.
		inline void setTwistMaxAngularLimit(hkReal rad);

			/// Sets the maximum plane limit.
		inline void setPlaneMaxAngularLimit(hkReal rad);

			/// Sets the minimum plane limit.
		inline void setPlaneMinAngularLimit(hkReal rad);

			/// Sets the half angle used to define the cone of possible movement.
		inline void setConeMinAngularLimit(hkReal rad);

			/// Adjusts the twist axis in B to lie in the middle of the min and max cone limit
			/// twist, cone and plane axis must be set first
		inline void setAsymmetricConeAngle( hkReal min, hkReal max );

			/// Sets the maximum angular friction impulse for the constraint.
		inline void setMaxFrictionTorque(hkReal tmag);

		// The attached and reference bodies
		hkRigidBody* m_bodyA;
		hkRigidBody* m_bodyB;

		// Pivot in body space of each entity
		hkVector4 m_pivotInA;
		hkVector4 m_pivotInB;

		// Plane axis in body space of both entities
		hkVector4 m_planeAxisInA;
		hkVector4 m_planeAxisInB;

		// Twist axis in body space of both entities
		hkVector4 m_twistAxisInA;
		hkVector4 m_twistAxisInB;

		// Angular limits
		hkReal m_coneMinAngle;
		hkReal m_planeMinAngle;
		hkReal m_planeMaxAngle;
		hkReal m_twistMinAngle;
		hkReal m_twistMaxAngle;

		// Max angular friction impulse
		hkReal m_maxFrictionTorque;
};

#include <hkdynamics2/constraint/bilateral/ragdoll/hkRagdollConstructionInfo.inl>

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
