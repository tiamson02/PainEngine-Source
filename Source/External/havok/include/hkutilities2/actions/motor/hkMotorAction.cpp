/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkutilities2/actions/motor/hkMotorAction.h>

void hkMotorAction::applyAction( const hkStepInfo& stepInfo )
{
	// Early exit if inactive.
	if (!m_active)
	{
		return;
	}

	// Work out the current angular velocity in body space.
	const hkTransform& tr = m_body->getTransform();
	hkVector4 curr;
	curr.setRotatedInverseDir(tr.getRotation(), m_body->getAngularVelocity());
		
	// Calculate the difference between the desired spin rate and the current rate of spin
	// about the desired axis 'm_axis'.
	hkReal currentRate = m_axis.dot3(curr);
	hkReal diff(m_spinRate - currentRate);

	// Calculate the newTorque to apply based on the difference and the gain. The newTorque
	// should be proportional to each of difference, gain, and inertia
	// (to make the Action mass-independent).
	hkVector4 newTorque;
	newTorque.setMul4(diff * m_gain, m_axis);
	hkMatrix3 m;
	m_body->getInertiaLocal(m);
	newTorque.setMul3(m, newTorque);

	newTorque.setRotatedDir(tr.getRotation(), newTorque);

	// Apply the new torque.
	m_body->applyTorque(newTorque);
}

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
