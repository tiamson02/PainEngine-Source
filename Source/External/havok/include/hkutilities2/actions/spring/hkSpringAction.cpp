/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkutilities2/actions/spring/hkSpringAction.h>
#include <hkdynamics2/entity/hkRigidBody.h>

///////////////////////////////////////////////////////////////////////////

hkSpringActionConstructionInfo::hkSpringActionConstructionInfo()
{
	m_bodyA = HK_NULL;
	m_bodyB = HK_NULL;
	m_strength = 1000;
	m_damping = 0.1f;
	m_restLength = 1;
	m_onCompression = true;
	m_onExtension = true;
}

///////////////////////////////////////////////////////////////////////////

hkSpringAction::hkSpringAction( hkSpringActionConstructionInfo& info ) :
	hkBinaryAction( info.m_bodyA, info.m_bodyB )
{
	m_restLength = info.m_restLength;
	m_strength = info.m_strength;
	m_damping = info.m_damping;
	m_positionAinA = info.m_pivotA;
	m_positionBinB = info.m_pivotB;
	m_onCompression = info.m_onCompression;
	m_onExtension = info.m_onExtension;
}

void hkSpringAction::applyAction( const hkStepInfo& stepInfo )
{
	if( 0 == m_strength )
	{
		return;
	}

	hkRigidBody* ra = getRigidBodyA();	
	hkRigidBody* rb = getRigidBodyB();

	hkVector4 posA;
	posA.setTransformedPos( ra->getTransform(), m_positionAinA );
	hkVector4 posB;
	posB.setTransformedPos( rb->getTransform(), m_positionBinB );

	hkVector4 posAB;
	posAB.setSub4( posB, posA );
	hkVector4 dirAB = posAB;
	
	hkReal	length = dirAB.length3();
	if( length < 0.001f )	// can't normalize the zero vector!
	{
		// what if rest length is not zero?
		return;
	}

	// normalise
	dirAB.mul4( 1.0f / length );
	
	{
		if( !m_onCompression && (length < m_restLength) )
		{
			return;
		}
		if( !m_onExtension && (length > m_restLength) )
		{
			return;
		}

		hkVector4 velA;
		ra->getPointVelocity( posA, velA );
        hkVector4 velB;
		rb->getPointVelocity( posB, velB );
		hkVector4 velAB;
		velAB.setSub4( velB, velA );

		hkReal relVel = velAB.dot3( dirAB );

		hkReal force = (relVel * m_damping) + ((length - m_restLength) * m_strength);

		m_lastForce.setMul4( -force, dirAB );
		rb->applyForce( m_lastForce, posB );
		
		m_lastForce.setMul4( force, dirAB );
		ra->applyForce( m_lastForce, posA );
	}
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
