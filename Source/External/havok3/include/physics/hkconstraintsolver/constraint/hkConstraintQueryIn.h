/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_CONSTRAINTSOLVER2_CONSTRAINT_QUERY_IN_H
#define HK_CONSTRAINTSOLVER2_CONSTRAINT_QUERY_IN_H

#include <hkmath/hkMath.h>
class hkVelocityAccumulator;
class hkJacobianElement;


	/// The time information, the constraints get access to
class hkConstraintQueryStepInfo
{
	public:
			/// The delta time of each solvers substep
		float		m_substepDeltaTime;

			/// The 1.0f/m_substepDeltaTime
		float		m_substepInvDeltaTime;
		float		m_frameDeltaTime;
		float		m_frameInvDeltaTime;
		float		m_invNumSteps;

			// a factor all rhs should be multiplied
		float		m_rhsFactor;

			// a factor all invMasses should be multiplied
		float		m_virtMassFactor;

			// a factor for all rhs used for friction calculations
		float m_frictionRhsFactor;
};


	/// The input structure to hkConstraints::buildJacobian.
class hkConstraintQueryIn: public hkConstraintQueryStepInfo
{
	public:
	
		hkConstraintQueryIn() {m_accumulatorBufferRoot = HK_NULL; m_jacobianBufferRoot = HK_NULL;}

			/// A pointer to the buffer used to store the velocity accumulators
		hkVelocityAccumulator* m_accumulatorBufferRoot;
		
			/// A pointer to the buffer used to store the jacobian elements
		hkJacobianElement* m_jacobianBufferRoot;

			/// The velocity accumulator of bodyA. Typically this is a hkRigidAccumulator
		hkVelocityAccumulator*	m_bodyA;

			/// The velocity accumulator of bodyB. Typically this is a hkRigidAccumulator
		hkVelocityAccumulator*	m_bodyB;

			/// the transform of rigid body A, identity transform if not available
		const hkTransform*	m_transformA;

			/// the transform of rigid body N, identity transform if not available
		const hkTransform*	m_transformB;

			/// the current global solver settings
		hkReal m_tau;

			/// the current global solver settings
		hkReal m_damping;

			/// if this class is used with the hkdynamics library, this points to an hkConstraintInstance
		void*	m_constraintInstance;

			/// if this class is used with the hkdynamics library, this points to an hkConstraintRuntime
		void*	m_constraintRuntime;
};



#endif // HK_CONSTRAINTSOLVER2_CONSTRAINT_QUERY_IN_H

/*
* Havok SDK - PUBLIC RELEASE, BUILD(#20060114)
*
* Confidential Information of Havok.  (C) Copyright 1999-2006 
* Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
* Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
* rights, and intellectual property rights in the Havok software remain in
* Havok and/or its suppliers.
*
* Use of this software for evaluation purposes is subject to and indicates 
* acceptance of the End User licence Agreement for this product. A copy of 
* the license is included with this software and is also available from salesteam@havok.com.
*
*/
