/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkdynamics2/entity/hkRigidBody.h>
//////////////////////////////////////////////////////////////////

/// \param bodyA			The first rigid body.
/// \param bodyB			The second rigid body.
/// \param pivotA			bodyA's attachment point, specified in world space.
/// \param pivotB			bodyB's attachment point, specified in world space.
inline void hkSpringActionConstructionInfo::setInWorldSpace(hkRigidBody* bodyA,hkRigidBody* bodyB,
		                                                    const hkVector4& pivotA,const hkVector4& pivotB)
{
  m_bodyA = bodyA;
  m_bodyB = bodyB;
  m_pivotA.setTransformedInversePos(bodyA->getTransform(),pivotA);
  m_pivotB.setTransformedInversePos(bodyB->getTransform(),pivotB);
  hkVector4 dist;
  dist.setSub4(pivotA,pivotB);
  m_restLength = dist.length3();
}

//////////////////////////////////////////////////////////////////

/// \param bodyA			The first rigid body.
/// \param bodyB			The second rigid body.
/// \param pivotA			bodyA's attachment point, specified in bodyA space.
/// \param pivotB			bodyB's attachment point, specified in bodyB space.
inline void hkSpringActionConstructionInfo::setInBodySpace(hkRigidBody* bodyA,hkRigidBody* bodyB,
		                                                   const hkVector4& pivotA,const hkVector4& pivotB)
{
  m_bodyA = bodyA;
  m_bodyB = bodyB;
  m_pivotA = pivotA;
  m_pivotB = pivotB;
  
  hkVector4 dist;
  hkVector4 pivotAW;
  hkVector4 pivotBW;

  // transform point to world space
  pivotAW.setTransformedPos(bodyA->getTransform(),pivotA);
  pivotBW.setTransformedPos(bodyB->getTransform(),pivotB);
  dist.setSub4(pivotAW,pivotBW);
  m_restLength = dist.length3();
}

//////////////////////////////////////////////////////////////////

inline void hkSpringActionConstructionInfo::setStrength(hkReal str)
{
  m_strength = str;
}

//////////////////////////////////////////////////////////////////

inline void hkSpringActionConstructionInfo::setDamping(hkReal damp)
{
  m_damping = damp;
}

//////////////////////////////////////////////////////////////////

inline void hkSpringActionConstructionInfo::setRestLength(hkReal restLength)
{
  m_restLength = restLength;
}

//////////////////////////////////////////////////////////////////

inline void hkSpringActionConstructionInfo::setOnCompression(bool onCompression)
{
  m_onCompression = onCompression;
}

//////////////////////////////////////////////////////////////////

inline void hkSpringActionConstructionInfo::setOnExtension(bool onExtension)
{
  m_onExtension = onExtension;
}

//////////////////////////////////////////////////////////////////

///\param ra			The rigid body
///\param pivotAinA		The attachment point in ra space.
inline void hkSpringAction::setBodyA(hkRigidBody* ra,const hkVector4& pivotAinA)
{
	HK_ASSERT(ra == HK_NULL);
    m_bodyA->removeReference();
    m_bodyA->removeEntityListener(this);
	m_bodyA = ra;
	m_bodyA->addReference();
	m_bodyA->addEntityListener(this);
}

//////////////////////////////////////////////////////////////////

///\param rb			The rigid body
///\param pivotBinB		The attachment point in rb space.
inline void hkSpringAction::setBodyB(hkRigidBody* rb,const hkVector4& pivotBinB)
{
	HK_ASSERT(rb == HK_NULL);
    m_bodyB->removeReference();
    m_bodyB->removeEntityListener(this);
	m_bodyB = rb;
	m_bodyB->addReference();
	m_bodyB->addEntityListener(this);	
}

//////////////////////////////////////////////////////////////////

inline const hkVector4& hkSpringAction::getPositionAinA()
{
	return m_positionAinA;
}

//////////////////////////////////////////////////////////////////

inline const hkVector4& hkSpringAction::getPositionBinB()
{
	return m_positionBinB;
}

//////////////////////////////////////////////////////////////////

inline void hkSpringAction::setStrength(hkReal str)
{
  m_strength = str;
}

//////////////////////////////////////////////////////////////////

inline hkReal hkSpringAction::getStrength()
{
	return m_strength;
}

//////////////////////////////////////////////////////////////////

inline void hkSpringAction::setDamping(hkReal damp)
{
  m_damping = damp;
}

//////////////////////////////////////////////////////////////////

inline hkReal hkSpringAction::getDamping()
{
	return m_damping;
}

//////////////////////////////////////////////////////////////////

inline void hkSpringAction::setRestLength(hkReal restLength)
{
  m_restLength = restLength;
}

//////////////////////////////////////////////////////////////////

inline hkReal hkSpringAction::getRestLength()
{
	return m_restLength;
}

//////////////////////////////////////////////////////////////////

inline void hkSpringAction::setOnCompression(bool onCompression)
{
  m_onCompression = onCompression;
}

//////////////////////////////////////////////////////////////////

inline bool hkSpringAction::getOnCompression()
{
	return m_onCompression;
}

//////////////////////////////////////////////////////////////////

inline void hkSpringAction::setOnExtension(bool onExtension)
{
  m_onExtension = onExtension;
}

//////////////////////////////////////////////////////////////////

inline bool hkSpringAction::getOnExtension()
{
	return m_onExtension;
}

//////////////////////////////////////////////////////////////////

inline const hkVector4& hkSpringAction::getLastForce() 
{ 
	return m_lastForce; 
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
