/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkutilities2/mousespring/hkMouseSpringAction.h>
#include <hkdynamics2/entity/hkRigidBody.h>


hkMouseSpringAction::hkMouseSpringAction( 
	const hkVector4& positionInRbLocal, const hkVector4& mousePositionInWorld,
	const hkReal springDamping, const hkReal springElasticity, 
	const hkReal objectDamping, hkRigidBody* rb )
:	hkUnaryAction( rb ),
	m_positionInRbLocal( positionInRbLocal ),
	m_mousePositionInWorld( mousePositionInWorld ),
	m_springDamping( springDamping ),
	m_springElasticity( springElasticity ),
	m_objectDamping( objectDamping )
{
	m_maxRelativeForce = 250.0f; // that's 25 times gravity
}

void hkMouseSpringAction::setMousePosition( const hkVector4& mousePositionInWorld )
{
	if ( !mousePositionInWorld.equals3( m_mousePositionInWorld ) )
	{
		getRigidBody()->forceActivate();
	}
	m_mousePositionInWorld = mousePositionInWorld;
}

void hkMouseSpringAction::setMaxRelativeForce(hkReal newMax)
{
	m_maxRelativeForce = newMax;
}



void hkMouseSpringAction::applyAction( const hkStepInfo& stepInfo )
{
	hkRigidBody* rb = getRigidBody();

	// calculate and apply the rigid spring mouse impluse
	const hkVector4& pMouse = m_mousePositionInWorld;
	//m_positionInRbLocal.setZero(); // for centra picking

	hkVector4 pRb; pRb.setTransformedPos(rb->getTransform(), m_positionInRbLocal);

	hkVector4 ptDiff;
	ptDiff.setSub4(pRb, pMouse);

	// calculate the jacobian
	hkMatrix3 jacobian;
	{
		hkReal massInv = rb->getMassInv();

		hkVector4 r;
		r.setSub4( pRb, rb->getCenterOfMassInWorld() );

		hkMatrix3 rhat;
		rhat.setCrossSkewSymmetric(r);

		hkMatrix3 inertialInvWorld;
		rb->getInertiaInvLocal(inertialInvWorld);
		inertialInvWorld.changeBasis( rb->getTransform().getRotation() );
	
		//jacobian.setZero(); this is not necessary!
		jacobian.setDiagonal( massInv, massInv, massInv );

		// calculate: jacobian -= (rhat * inertialInvWorld * rhat)
		hkMatrix3 temp;
		temp.setMul(rhat, inertialInvWorld);
		hkMatrix3 temp2;
		temp2.setMul(temp, rhat);
		jacobian.sub(temp2);
	}

	// invert the jacobian as: jacobian * impluse = velocityDelta...
	// we want to calculate the impluse
	const hkResult jacInvertResult = jacobian.invert(0.0000001f);
	if ( jacInvertResult != HK_SUCCESS )
	{
		return;
	}

	// apply damping
	{
		hkVector4 m_linearVelocity = rb->getLinearVelocity();
		m_linearVelocity.mul4(m_objectDamping);
		rb->setLinearVelocity(m_linearVelocity);

		hkVector4 m_angularVelocity = rb->getAngularVelocity();
		m_angularVelocity.mul4(m_objectDamping);
		rb->setAngularVelocity(m_angularVelocity);
	}

	// calculate the velocity delta
	hkVector4 delta;
	{
		hkVector4 relVel;
		rb->getPointVelocity(pRb, relVel);
		delta.setMul4(m_springElasticity * stepInfo.m_invDeltaTime, ptDiff);
		delta.addMul4(m_springDamping, relVel);
	}

	// calculate the impluse
	hkVector4 impulse;
	{
		impulse.setMul3(jacobian, delta);	// jacobian is actually the jacobian inverse here!
		impulse.setNeg4(impulse);
	}

	//
	//	clip the impulse
	//
	hkReal impulseLen2 = impulse.lengthSquared3();
	hkReal maxImpulse  = rb->getMass() * stepInfo.m_deltaTime * m_maxRelativeForce; 
	if ( impulseLen2 > maxImpulse * maxImpulse )
	{
		hkReal factor = maxImpulse * hkMath::sqrtInverse( impulseLen2 );
		impulse.mul4( factor );
	}

	rb->applyPointImpulse(impulse, pRb);
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
