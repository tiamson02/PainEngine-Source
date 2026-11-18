/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_GENERIC_CONSTRAINT_SCHEME_H
#define HK_DYNAMICS2_GENERIC_CONSTRAINT_SCHEME_H

#include <hkdynamics2/hkDynamics2.h>

class hkConstraintModifier;
class hkMotorController;


class hkGenericConstraintScheme
{
	public:

		enum 
		{	
			e_endScheme = 0, 
			e_setPivotA, e_setPivotB, 
			e_setLinearDofA, e_setLinearDofB, e_setLinearDofW, e_constrainLinearW, e_constrainAllLinearW,
			e_setAngularBasisA, e_setAngularBasisB, e_setAngularBasisAidentity, e_setAngularBasisBidentity,
			e_constrainToAngularW, e_constrainAllAngularW,e_setAngularMotor, e_setLinearMotor,
			e_setLinearLimit, e_setAngularLimit, e_setConeLimit, e_setTwistLimit, e_setAngularFriction, e_setLinearFriction,
			e_setTauAndDamping, e_restoreTauAndDamping, e_doConstraintModifier, e_doRhsModifier, 
			e_numCommands 
		};
		
		hkConstraintInfo m_info;

		hkArray< hkVector4 >				m_data;
		hkArray< int >						m_commands;
		hkArray< hkConstraintModifier* >	m_modifiers;
		hkArray< hkMotorController* >		m_motors;
		hkArray< hkSolverResults >			m_results;
};

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
