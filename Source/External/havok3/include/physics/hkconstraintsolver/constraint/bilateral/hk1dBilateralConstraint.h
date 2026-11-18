/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
 
#ifndef HK_CONSTRAINTSOLVER_1D_BILATERAL_CONSTRAINT_PUBLIC_H
#define HK_CONSTRAINTSOLVER_1D_BILATERAL_CONSTRAINT_PUBLIC_H


class hkConstraintQueryIn;
class hkConstraintQueryOut;
class hkSolverResults;

	/// A structure which is used for building jacobian elements
class hk1dBilateralConstraintStatus
{
	public:
			/// The relative mass of the two objects in constraint space
		hkReal			m_virtualMass;
};

	/// The low level input structure to set up linear and angular motors for the constraint solver
class hk1dConstraintMotorInfo
{
	public:

// <todo> not used in position motor
			/// position target for the current frame.
		hkReal				m_targetPosition;  

			/// The target velocity
		hkReal				m_targetVelocity;

			/// max force that can be applied
		hkReal				m_maxForce;

			/// max force that can be applied in the reverse direction
		hkReal				m_minForce;

			/// The relative stiffness of the motor between 0..1
		hkReal				m_tau;

			/// The relative damping of the motor between 0..1
		hkReal				m_damping;

};


#endif // HK_CONSTRAINTSOLVER_1D_BILATERAL_CONSTRAINT_PUBLIC_H

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
