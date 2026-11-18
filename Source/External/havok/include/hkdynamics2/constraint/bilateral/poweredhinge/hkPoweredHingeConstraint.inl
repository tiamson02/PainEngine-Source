/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkdynamics2/entity/hkRigidBody.h>
#include <hkmath2/linear/hkVector4Util.h>
#include <hkdynamics2/constraint/bilateral/motor/hkMotorController.h>

// hkPoweredHinge set and get methods


//////////////////////////////////////////////

inline void hkPoweredHingeConstraint::setIgnoreLimits(bool state)
{
	m_ignoreLimits = state;
}

//////////////////////////////////////////////

inline hkReal hkPoweredHingeConstraint::getCurrentPos()
{
	return m_lastAngle + 2.0f*hkMath::HK_REAL_PI*m_rotations; 
}

/////////////////////////////////////////////

inline hkMotorController* hkPoweredHingeConstraint::getMotor()
{
	return m_motor;
}

/////////////////////////////////////////////

inline void hkPoweredHingeConstraint::setMotor( hkMotorController* motor )
{
	if( motor )
	{
		motor->addReference();
	}

	if( m_motor )
	{
		m_motor->removeReference();
	}

	m_motor = motor;
}

/////////////////////////////////////////////

inline bool hkPoweredHingeConstraint::getMotorActive()
{
	return m_motorActive;
}

/////////////////////////////////////////////

inline void hkPoweredHingeConstraint::setMotorActive( bool onOff )
{
	m_motorActive = onOff;
}


inline void hkPoweredHingeConstraint::setData(const hkPoweredHingeConstructionInfo& info)
{
	m_minAngle = info.m_minAngle;
	m_maxAngle = info.m_maxAngle;

	m_pivotInA = info.m_pivotInA;
	m_pivotInB = info.m_pivotInB;

	m_perpToAxleInA1 = info.m_perpToAxleInA1;
	m_perpToAxleInA2 = info.m_perpToAxleInA2;
	m_axleInB = info.m_axleInB;  
 	m_axleInA = info.m_axleInA;
	m_perp2FreeAxisInB = info.m_perp2FreeAxisInB;

	m_motor = info.m_motor;
	if( m_motor )
	{
		m_motor->addReference();
	}
	m_ignoreLimits = info.m_ignoreLimits;
	m_lastAngle = 0;
	m_rotations = 0;

	if(!isValid())
	{
		HK_ASSERT("Members of hkPoweredHingeConstraint constraint inconsistent.");
	}
}

/////////////////////////////////////////////

inline void hkPoweredHingeConstraint::getConstructionInfo(hkPoweredHingeConstructionInfo& info)
{
	info.m_bodyA = static_cast<hkRigidBody*>(getEntityA());
	info.m_bodyB = static_cast<hkRigidBody*>(getEntityB());

	info.m_minAngle = m_minAngle;
	info.m_maxAngle = m_maxAngle;

	info.m_pivotInA = m_pivotInA;
	info.m_pivotInB = m_pivotInB;

	info.m_perpToAxleInA1 = m_perpToAxleInA1;
	info.m_perpToAxleInA2 = m_perpToAxleInA2;
	info.m_axleInB = m_axleInB;  
 	info.m_axleInA = m_axleInA;
	info.m_perp2FreeAxisInB = m_perp2FreeAxisInB;

	info.m_ignoreLimits = m_ignoreLimits;
	info.m_motor = m_motor;
}

//////////////////////////////////////////////


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
