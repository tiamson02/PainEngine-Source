/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkdynamics2/entity/hkRigidBody.h>


/// Methods to set and get angle limits and friction

////////////////////////////////////////////////////////////////////////////////////////////////

inline void hkRagdollConstraint::setTwistMinAngularLimit(hkReal rad)
{
	m_twistMinAngle = hkMath::sin(rad);
}

////////////////////////////////////////////////////////////////////////////////////////////////

inline void hkRagdollConstraint::setTwistMaxAngularLimit(hkReal rad)
{
	m_twistMaxAngle = hkMath::sin(rad);
}

////////////////////////////////////////////////////////////////////////////////////////////////

inline void hkRagdollConstraint::setPlaneMaxAngularLimit(hkReal rad)
{
	m_planeMaxAngle = hkMath::sin(rad);
}

////////////////////////////////////////////////////////////////////////////////////////////////

inline void hkRagdollConstraint::setPlaneMinAngularLimit(hkReal rad)
{
	m_planeMinAngle = hkMath::sin(rad);
}

////////////////////////////////////////////////////////////////////////////////////////////////

inline void hkRagdollConstraint::setConeMinAngularLimit(hkReal rad)
{
	m_coneMinAngle = hkMath::cos(rad);
}

///////////////////////////////////////////////////////////////////////////////////////////////

inline hkReal hkRagdollConstraint::getTwistMinAngularLimit() const
{
	return hkMath::asin(m_twistMinAngle);
}

////////////////////////////////////////////////////////////////////////////////////////////////

inline hkReal hkRagdollConstraint::getTwistMaxAngularLimit() const
{
	return hkMath::asin(m_twistMaxAngle);
}

////////////////////////////////////////////////////////////////////////////////////////////////

inline hkReal hkRagdollConstraint::getPlaneMaxAngularLimit() const
{
	return hkMath::asin(m_planeMaxAngle);
}

////////////////////////////////////////////////////////////////////////////////////////////////

inline hkReal hkRagdollConstraint::getMinPlaneAngularLimit() const
{
	return hkMath::asin(m_planeMinAngle);
}

////////////////////////////////////////////////////////////////////////////////////////////////

inline hkReal hkRagdollConstraint::getMinConeAngularLimit() const
{
	return hkMath::acos(m_coneMinAngle);
}

////////////////////////////////////////////////////////////////////////////////////////////////

inline hkReal hkRagdollConstraint::getMaxFrictionTorque() const
{
	return m_maxFrictionTorque;
}

////////////////////////////////////////////////////////////////////////////////////////////////

inline void hkRagdollConstraint::setData(const hkRagdollConstructionInfo& info)
{
  	m_pivotInA = info.m_pivotInA;
	m_pivotInB = info.m_pivotInB;
	m_twistAxisInA = info.m_twistAxisInA;
	m_twistAxisInB = info.m_twistAxisInB;

	m_planeAxisInA = info.m_planeAxisInA; 
	m_planeAxisInB = info.m_planeAxisInB;

    m_coneMinAngle =  info.m_coneMinAngle;
    m_planeMinAngle = info.m_planeMinAngle;
    m_planeMaxAngle = info.m_planeMaxAngle;
    m_twistMinAngle = info.m_twistMinAngle;
    m_twistMaxAngle = info.m_twistMaxAngle;
    m_maxFrictionTorque = info.m_maxFrictionTorque;

	HK_ASSERT2(isValid(), "Members of ragdoll constraint invalid.");
}

////////////////////////////////////////////////////////////////////////////////////////////////

inline void hkRagdollConstraint::getConstructionInfo(hkRagdollConstructionInfo& info)
{  
	info.m_bodyA = static_cast<hkRigidBody*>(getEntityA());
	info.m_bodyB = static_cast<hkRigidBody*>(getEntityB());
	info.m_pivotInA = m_pivotInA;
	info.m_pivotInB = m_pivotInB;
	info.m_twistAxisInA = m_twistAxisInA;
	info.m_twistAxisInB = m_twistAxisInB;

	info.m_planeAxisInA = m_planeAxisInA; 
	info.m_planeAxisInB = m_planeAxisInB;

    info.m_coneMinAngle = m_coneMinAngle;
    info.m_planeMinAngle = m_planeMinAngle;
    info.m_planeMaxAngle = m_planeMaxAngle;
    info.m_twistMinAngle = m_twistMinAngle;
    info.m_twistMaxAngle = m_twistMaxAngle;
    info.m_maxFrictionTorque = m_maxFrictionTorque;
}

////////////////////////////////////////////////////////////////////////////////////////////////

/// Compute the twist axis in world space for the attached and reference bodies
/// \param rbA				The attached body
/// \param rbB				The reference body
/// \param twistAxisAWS		Storage for twist axis from attached body in world space
/// \param twistAxisBWS		Storage for twist axis from reference body in world space

inline void hkRagdollConstraint::getTwistAxisWS(const hkRigidBody* rbA,const hkRigidBody* rbB,hkVector4& twistAxisAWS,hkVector4& twistAxisBWS) const
{
	twistAxisAWS._setRotatedDir(rbA->getTransform().getRotation(), this->m_twistAxisInA);
	twistAxisBWS._setRotatedDir(rbB->getTransform().getRotation(), this->m_twistAxisInB);
}

////////////////////////////////////////////////////////////////////////////////////////////////

/// Compute the plane Axis of refernece body in world space
/// \param rbB			The reference body
/// \param planeAxisWS	Storage for reference body's plane axis in world space 
inline void hkRagdollConstraint::getPlaneAxisRefWS(const hkRigidBody* rbB,hkVector4& planeAxisWS) const
{
	planeAxisWS._setRotatedDir( rbB->getTransform().getRotation(), m_planeAxisInB );
}

////////////////////////////////////////////////////////////////////////////////////////////////


inline void hkRagdollConstraint::buildLimits( const hkVector4& twist, const hkVector4& twistRef, const hkConstraintQueryIn &in, hkConstraintQueryOut &out )
{
	
	// cone

	buildConeLimit( twist, twistRef, m_coneMinAngle, 100.0f, in, out );

	// carve out apple core with plane

	hkRigidBody* rbB = getRigidBodyB();	
	hkVector4 plane_Axis_Ref_ws;
	getPlaneAxisRefWS( rbB,plane_Axis_Ref_ws);

	buildConeLimit( twist, plane_Axis_Ref_ws, m_planeMinAngle, m_planeMaxAngle, in, out );

}

//////////////////////////////////////////////////////////////////////////////////////////////////

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
