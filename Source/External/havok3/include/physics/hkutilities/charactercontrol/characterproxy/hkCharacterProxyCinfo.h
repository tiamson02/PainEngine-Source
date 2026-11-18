/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HKUTILITIES_CHARACTERCONTROL_CHARACTERPROXY_HKCHARACTERPROXYCINFO_XML_H
#define HKUTILITIES_CHARACTERCONTROL_CHARACTERPROXY_HKCHARACTERPROXYCINFO_XML_H


#include <hkmath/hkMath.h>
class hkShapePhantom;

/// This information is used to construct a character proxy object. 	
class hkCharacterProxyCinfo
{
	public:
	
		HK_DECLARE_REFLECTION();

			/// Default constructor
		hkCharacterProxyCinfo()
		{
			m_position.set(0.0f, 0.0f, 0.0f);
			m_velocity.set(0.0f, 0.0f, 0.0f);
			m_dynamicFriction = 1.0f;
			m_staticFriction = 0.0f;
			m_keepContactTolerance = 0.1f;
			m_up.set(0.0f, 1.0f, 0.0f);
			m_extraUpStaticFriction = 0.0f;
			m_extraDownStaticFriction = 0.0f;
			m_shapePhantom = HK_NULL;
			m_keepDistance = 0.05f;
			m_characterRadius = 1.0f;
			m_userPlanes = 4;
			m_maxCharacterSpeedForSolver = 10;
			m_characterStrength = hkMath::HK_REAL_MAX;
			m_characterMass = 0;
			m_maxSlope = hkMath::HK_REAL_PI * .5f;
			m_penetrationRecoverySpeed = 1.0f;
		}
		
		//
		// Members
		//
	public:
		
			/// The initial position for the proxy in world space. 	
		hkVector4 m_position;
		
			/// The initial velocity for the proxy in world space. 	
		hkVector4 m_velocity;
		
			/// Controls the default dynamic friction for surfaces hit. A value of 0
			/// maintains character momentum. A value of 1 clips all momentum against the normal
			/// of the surface hit. 	
		hkReal m_dynamicFriction;
		
			/// Controls the default static friction for surfaces hit 	
		hkReal m_staticFriction;
		
			/// This parameter is used to determine when a contact point should be considered
			/// as part of the character's contact manifold.  The default is 0.1f, which means
			/// that points .1 above the keep distance will be considered as part of the manifold.
			/// This value should be kept at the its default, as changing it from the default 
			/// value can have undesirable effects.
		hkReal m_keepContactTolerance;

			/// Friction is split into horizontal and vertical components. This vector is used
			/// to separate the components. In general it should be a normalized, negated version
			/// of gravity. 	
		hkVector4 m_up;
		
			/// This parameter has been deprecated. It was intended to limit the maximum slope
			/// on which a character can walk. Now use m_maxSlope instead.
		hkReal m_extraUpStaticFriction;
		
			/// This parameter has been deprecated. It was intended to limit the maximum slope
			/// on which a character can walk. Now use m_maxSlope instead.
		hkReal m_extraDownStaticFriction;
		
			/// The collision detection representation for this entity.
		hkShapePhantom * m_shapePhantom;
		
			/// Attempt to keep the character at least this distance from the surface. Used
			/// internally for numerical stability and performance reasons. 	
		hkReal m_keepDistance;
		
			/// A conservative estimate of the radius of the character. This is used when
			/// deciding which planes in the manifold are duplicates.  
		hkReal m_characterRadius;
		
			/// The maximum number of extra planes the user may add to the simplex input
			/// during a processConstraints callback. 	
		hkUint32 m_userPlanes;

			/// This value is used  to clip the character's velocity when it is being "squeezed" by two
			/// moving planes. When two nearly parallel planes squeeze the character, the resultant velocity
			/// necessary to move the character so that both planes do not penetrate it can be extremely high,
			/// which can cause in the character controller to move at a very fast velocity.
			/// This value defaults to 10 (which is appropriate if your default walking speed is 10).
			/// If this velocity is exceeded by the character solver when solving parallel planes, the
			/// solver solves the planes independently. The result is that instead of moving at a high
			/// velocity, the character may penetrate one of the planes (based on plane priorities).
		hkReal m_maxCharacterSpeedForSolver;


			/// The maximum constant force that the character controller can impart onto
			/// moving objects. By default this is hkMath::HK_REAL_MAX, i.e. the character controller
			/// is infinitely strong.
		hkReal m_characterStrength;

			/// The mass of the character. This value is only used to apply an extra downward force to dynamic
			/// rigid bodies that the character is standing on.  By default this value is 0, which means no additional
			/// downward force is applied.  It should only be set to a positive value if you do not apply gravity 
			/// from your state machine when the character is on the ground.
		hkReal m_characterMass;

		
			/// The maximum slope that the character can walk up. If the character is standing on a slope that
			/// is steeper than this, checkSupport() will return false, and an additional vertical plane will be
			/// added during integrate() that will block the character's movement in the direction of the slope.
			/// This angle is measured in radians from the horizontal.
			/// By default this value is set to PI / 2, i.e. vertical, which disables this feature.
		hkReal m_maxSlope;
		
			/// When a penetrating plane is added to the manifold we set its velocity so it will push 
			/// the character away from the point of penetration. This variable controls the speed
			/// at which this recovery takes place. The velocity applied is proportional to the penetration distance
			/// This values controls the proportion.
			/// By default this value is set to 1.0
		hkReal m_penetrationRecoverySpeed;

};

#endif // HKUTILITIES_CHARACTERCONTROL_CHARACTERPROXY_HKCHARACTERPROXYCINFO_XML_H

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
