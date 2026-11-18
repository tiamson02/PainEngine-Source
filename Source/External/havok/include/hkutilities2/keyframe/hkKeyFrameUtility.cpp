/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


#include <hkdynamics2/entity/hkRigidBody.h>
#include <hkutilities2/keyframe/hkKeyFrameUtility.h>



void hkKeyFrameUtility::KeyFrameInfo::fastSetUsingPositionOrientationPair( const hkVector4& currentPosition, const hkQuaternion& currentOrientation, const hkVector4& nextPosition, const hkQuaternion& nextOrientation, hkReal invDeltaTime )
{
	// Get lin vel required
	{
		m_position = currentPosition;
		m_linearVelocity.setSub4(nextPosition, currentPosition);
		m_linearVelocity.setMul4(invDeltaTime, m_linearVelocity);
	}

	// Get ang vel required. Use approx: sin(theta) ~ theta for small theta.
	{
		m_orientation = currentOrientation;
		hkQuaternion quatDif;	quatDif.setMulInverse(nextOrientation, currentOrientation);
		m_angularVelocity.setMul4(2.0f * invDeltaTime, quatDif.getImag());

		if ( quatDif.getReal() < 0.0f )
		{
			m_angularVelocity.setNeg4( m_angularVelocity );
		}
	}	
}

hkKeyFrameUtility::AccelerationInfo::AccelerationInfo()
{
	m_linearPositionFactor.setAll( 1.0f );
	m_angularPositionFactor.setAll( 1.0f );
	m_linearVelocityFactor.setAll( 1.0f );
	m_angularVelocityFactor.setAll( 1.0f );
	m_maxLinearAcceleration = hkMath::HK_REAL_MAX;
	m_maxAngularAcceleration = hkMath::HK_REAL_MAX;
	m_maxAllowedDistance = hkMath::HK_REAL_MAX;
}

void hkKeyFrameUtility::KeyFrameInfo::setUsingPositionOrientationPair( const hkVector4& currentPosition, const hkQuaternion& currentOrientation, const hkVector4& nextPosition, const hkQuaternion& nextOrientation, hkReal invDeltaTime )
{
	// Get lin vel required
	{
		m_position = currentPosition;
		m_linearVelocity.setSub4(nextPosition, currentPosition);
		m_linearVelocity.setMul4(invDeltaTime, m_linearVelocity);
	}

	// Get ang vel required.
	{
		m_orientation = currentOrientation;
		hkQuaternion quatDif;	quatDif.setMulInverse(nextOrientation, currentOrientation);

		quatDif.normalize();

		if ( hkMath::fabs(quatDif.getReal()) > 1.0f - hkMath::HK_REAL_EPSILON )
		{
			m_angularVelocity.setZero4();

		}
		else
		{
			hkReal angle = quatDif.getAngle();
			quatDif.getAxis(m_angularVelocity);
			m_angularVelocity.setMul4(angle * invDeltaTime, m_angularVelocity);		
		}	
	}
}



void HK_CALL hkKeyFrameUtility::applySoftKeyFrame( const KeyFrameInfo& keyFrameInfo, AccelerationInfo& accelInfo, hkReal deltaTime,  hkReal invDeltaTime, hkRigidBody* body )
{
	HK_ASSERT2(accelInfo.m_linearPositionFactor(0) <= invDeltaTime, "SoftKeyframe will be unstable if linear position factor is greater than 1/detaTime");
	HK_ASSERT2(accelInfo.m_linearPositionFactor(1) <= invDeltaTime, "SoftKeyframe will be unstable if linear position factor is greater than 1/detaTime");
	HK_ASSERT2(accelInfo.m_linearPositionFactor(2) <= invDeltaTime, "SoftKeyframe will be unstable if linear position factor is greater than 1/detaTime");
	HK_ASSERT2(accelInfo.m_angularPositionFactor(0) <= invDeltaTime, "SoftKeyframe will be unstable if angular position factor is greater than 1/detaTime");
	HK_ASSERT2(accelInfo.m_angularPositionFactor(1) <= invDeltaTime, "SoftKeyframe will be unstable if angular position factor is greater than 1/detaTime");
	HK_ASSERT2(accelInfo.m_angularPositionFactor(2) <= invDeltaTime, "SoftKeyframe will be unstable if angular position factor is greater than 1/detaTime");

	HK_ASSERT2(accelInfo.m_linearVelocityFactor(0) <= invDeltaTime, "SoftKeyframe will be unstable if linear velocity factor is greater than 1/detaTime");
	HK_ASSERT2(accelInfo.m_linearVelocityFactor(1) <= invDeltaTime, "SoftKeyframe will be unstable if linear velocity factor is greater than 1/detaTime");
	HK_ASSERT2(accelInfo.m_linearVelocityFactor(2) <= invDeltaTime, "SoftKeyframe will be unstable if linear velocity factor is greater than 1/detaTime");
	HK_ASSERT2(accelInfo.m_angularVelocityFactor(0) <= invDeltaTime, "SoftKeyframe will be unstable if angular velocity factor is greater than 1/detaTime");
	HK_ASSERT2(accelInfo.m_angularVelocityFactor(1) <= invDeltaTime, "SoftKeyframe will be unstable if angular velocity factor is greater than 1/detaTime");
	HK_ASSERT2(accelInfo.m_angularVelocityFactor(2) <= invDeltaTime, "SoftKeyframe will be unstable if angular velocity factor is greater than 1/detaTime");

	hkVector4 deltaTime4; deltaTime4.setAll( deltaTime );

	// calculate the current delta position/orientations
	hkVector4 deltaPosition;	deltaPosition.setSub4(    keyFrameInfo.m_position, body->getPosition() );
	
	//
	// Check whether our distance gets too big, so we have to warp it
	//
	if ( deltaPosition.lengthSquared3() > accelInfo.m_maxAllowedDistance * accelInfo.m_maxAllowedDistance )
	{
		deltaPosition.setZero4();
		body->setPositionAndRotation( keyFrameInfo.m_position, keyFrameInfo.m_orientation );
		body->setAngularVelocity( keyFrameInfo.m_angularVelocity );
		body->setLinearVelocity( keyFrameInfo.m_linearVelocity );
	}


	hkQuaternion quatDif;		quatDif.setMulInverse(keyFrameInfo.m_orientation, body->getRotation());

	hkVector4 deltaOrientation; deltaOrientation.setMul4( 2.0f , quatDif.getImag() );
	if ( quatDif.getReal() < 0 )
	{
		deltaOrientation.setNeg4( deltaOrientation );
	}

	hkVector4 scaledDeltaPosition;    scaledDeltaPosition.setMul4( accelInfo.m_linearPositionFactor, deltaPosition );
	hkVector4 scaledDeltaOrientation; scaledDeltaOrientation.setMul4( accelInfo.m_angularPositionFactor, deltaOrientation );


		//
		//	Calc the part based on velocity difference
		//
	hkVector4 linVelFactor;  linVelFactor.setMul4( deltaTime4, accelInfo.m_linearVelocityFactor );
	hkVector4 angVelFactor;  angVelFactor.setMul4( deltaTime4, accelInfo.m_angularVelocityFactor );

	hkVector4 deltaLinVel; deltaLinVel.setSub4( keyFrameInfo.m_linearVelocity, body->getLinearVelocity() );
	hkVector4 deltaAngVel; deltaAngVel.setSub4( keyFrameInfo.m_angularVelocity, body->getAngularVelocity() );

	deltaLinVel.mul4( linVelFactor );
	deltaAngVel.mul4( angVelFactor );


		//
		//	Add everything together
		//
	deltaLinVel.add4( scaledDeltaPosition );
	deltaAngVel.add4( scaledDeltaOrientation );


		//
		//	clip values
		//
	{
		// clip linear
		hkReal maxLinDelta = deltaTime * accelInfo.m_maxLinearAcceleration;
		hkReal deltaLinVelLength2 = deltaLinVel.lengthSquared3();

		if ( deltaLinVelLength2 > maxLinDelta*maxLinDelta )
		{
			hkReal f = maxLinDelta * hkMath::sqrtInverse( deltaLinVelLength2 );
			deltaLinVel.mul4( f );
		}

		// clip angular
		hkReal maxAngDelta = deltaTime * accelInfo.m_maxAngularAcceleration;
		hkReal deltaAngVelLength2 = deltaAngVel.lengthSquared3();

		if ( deltaAngVelLength2 > maxAngDelta*maxAngDelta )
		{
			hkReal f = maxAngDelta * hkMath::sqrtInverse( deltaAngVelLength2 );
			deltaAngVel.mul4( f );
		}
	}
		//	apply values
		//
	{
		hkVector4 newAngVel; newAngVel.setAdd4( body->getAngularVelocity(), deltaAngVel );
		hkVector4 newLinVel; newLinVel.setAdd4( body->getLinearVelocity(),  deltaLinVel );

		body->setAngularVelocity( newAngVel );
		body->setLinearVelocity( newLinVel );
	}
}


void HK_CALL hkKeyFrameUtility::applyHardKeyFrame( hkVector4& nextPosition, hkQuaternion& nextOrientation, hkReal invDeltaTime, hkRigidBody* body)
{
	// Get lin vel required
	{
		hkVector4 linearVelocity;

		hkVector4 newCenterOfMassPosition;
		newCenterOfMassPosition.setRotatedDir( nextOrientation, body->getCenterOfMassLocal() );
		newCenterOfMassPosition.add4( nextPosition );
		linearVelocity.setSub4( newCenterOfMassPosition, body->getCenterOfMassInWorld() );

		linearVelocity.setMul4(invDeltaTime, linearVelocity);
		
		body->setLinearVelocity(linearVelocity);
	}

	// Get ang vel required
	{
		hkVector4 angularVelocity;
		hkQuaternion quatDif;
		quatDif.setMulInverse(nextOrientation, body->getRotation());
		quatDif.normalize();

		hkReal angle = quatDif.getAngle();
		if(angle < 1e-3f)
		{
			angularVelocity.setZero4();
		}
		else
		{
			quatDif.getAxis(angularVelocity);
			angularVelocity.setMul4(angle * invDeltaTime, angularVelocity);		
		}	
		body->setAngularVelocity(angularVelocity);
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
