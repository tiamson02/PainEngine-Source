/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkutilities2/actions/angulardashpot/hkAngularDashpotAction.h>
#include <hkdynamics2/entity/hkRigidBody.h>


/////////////////////////////////////////////////////////////////////


/// \class hkAngularDashpotAction
/// When one of the dashpot's bodies is oriented away from the specified rotation, the dashpot works so that it
/// will tend to return to that rotation.  You construct the dashpot by specifying the rigid
/// body or bodies, and the desired orientation (rot).  You can also specify stiffness 
/// and damping parameters.
/// \n
/// The default strength and damping parameters are suitable for dashpots connected
/// to rigid bodies of mass 1kg.  For rigid bodies with larger masses, higher
/// strengths may be needed.\n


/////////////////////////////////////////////////////////////////////

hkAngularDashpotAction::hkAngularDashpotAction(const hkAngularDashpotActionConstructionInfo& info) : 
                        hkBinaryAction( info.m_bodyA, info.m_bodyB ) 	
{
	HK_ASSERT(info.m_bodyA != HK_NULL  && "Initialize construction info before passing to constructor!");
	HK_ASSERT(info.m_bodyB != HK_NULL  && "Initialize construction info before passing to constructor!");
	m_strength = info.m_strength;
	m_damping = info.m_damping;
	m_rotation = info.m_rotation;
}

/////////////////////////////////////////////////////////////////////

/// This method is called internally by the Havok system.
void hkAngularDashpotAction::applyAction(const hkStepInfo& stepInfo)
{

	const hkReal dtscale = 200; // to keep constants sensible around 1
	const hkReal dt = dtscale * (stepInfo.m_deltaTime);

	hkQuaternion rrot;
	hkQuaternion inv;
	hkQuaternion q;
	hkVector4 avel;

	q.setMul(m_bodyB->getRotation(),m_rotation);
	inv.setInverse(q);
	rrot.setMul(m_bodyA->getRotation(),inv);

	avel.setSub4(m_bodyA->getAngularVelocity(),m_bodyB->getAngularVelocity());

 	const hkReal angle = rrot.getAngle();
	hkVector4 axis = hkVector4::getZero();

	if( hkMath::fabs(angle) > 0.001f)
	{
		axis.setMul4(angle, rrot.getImag());
	}

	axis.mul4(dt * m_strength);
	avel.mul4(dt * m_damping);


	hkVector4 impulse;
	impulse.setAdd4(axis,avel);

	m_bodyB->applyAngularImpulse( impulse);
	impulse.mul4(-1);
	m_bodyA->applyAngularImpulse(impulse);

}

/////////////////////////////////////////////////////////////////////

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
