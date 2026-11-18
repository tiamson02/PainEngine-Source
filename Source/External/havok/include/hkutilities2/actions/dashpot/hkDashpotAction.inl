/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


////////////////////////////////////////////////////////////////////////

/// \param bodyA			The first rigid body.
/// \param bodyB			The second rigid body.
/// \param pivotA			bodyA's attachment point, specified in world space.
/// \param pivotB			bodyB's attachment point, specified in world space.
inline void hkDashpotActionConstructionInfo::setInWorldSpace(hkRigidBody* bodyA,hkRigidBody* bodyB,
		                                              const hkVector4& pivotA,const hkVector4& pivotB)

{
  m_bodyA = bodyA;
  m_bodyB = bodyB;
  m_pivotA.setTransformedInversePos(bodyA->getTransform(),pivotA);
  m_pivotB.setTransformedInversePos(bodyB->getTransform(),pivotB);
}


////////////////////////////////////////////////////////////////////////

/// \param bodyA			The first rigid body.
/// \param bodyB			The second rigid body.
/// \param pivotA			bodyA's pivot point, specified in bodyA space.
/// \param pivotB			bodyB's pivot point, specified in bodyB space.	  
inline void hkDashpotActionConstructionInfo::setInBodySpace(hkRigidBody* bodyA,hkRigidBody* bodyB,
		                                             const hkVector4& pivotA,const hkVector4& pivotB)
{
  m_bodyA = bodyA;
  m_bodyB = bodyB;
  m_pivotA = pivotA;
  m_pivotB = pivotB;
}

////////////////////////////////////////////////////////////////////////

inline void hkDashpotActionConstructionInfo::setStrength(hkReal str)
{
  m_strength = str;
}

////////////////////////////////////////////////////////////////////////

inline void hkDashpotActionConstructionInfo::setDamping(hkReal damp)
{
  m_damping = damp;
}

////////////////////////////////////////////////////////////////////////

inline hkDashpotAction::~hkDashpotAction()
{

}

/// This value is the strength of the restoring force between the two points.. An appropriate strength for 
/// a hkDashpotAction is the weight of the heavier of the two rigid bodies.
inline hkReal hkDashpotAction::getStrength() const 
{ 
	return m_strength; 
}
	

/// This value is the strength of the restoring force between the two points. An appropriate strength for a 
/// hkDashpotAction is the weight of the heavier of the two rigid bodies
/// \param s The new strength  
inline void hkDashpotAction::setStrength(hkReal s)
{ 
	m_strength = s;
}
	

/// The damping parameter specifies the rate of decay of the oscillations the 
/// hkDashpotAction produces. Values higher  than 1/10 of the hkDashpotAction strength can cause 
/// instability.
inline hkReal hkDashpotAction::getDamping() const 
{ 
	return m_damping; 
}
	

/// The damping parameter specifies the rate of decay of the oscillations the 
/// hkDashpotAction produces. In general the damping should be 10% of the strength of the 
/// hkDashpotAction .
/// \param d The damping coeffecient of the hkDashpotAction  
inline void hkDashpotAction::setDamping(hkReal d) 
{ 
	m_damping = d; 
}

/////////////////////////////////////////////////////////////////////////


/// In typical usage setPointA should also be called to set the point 
/// in body space for the new body.

inline void hkDashpotAction::setBodyA(hkRigidBody* ra)
{
	HK_ASSERT(ra == HK_NULL);
    m_bodyA->removeReference();
    m_bodyA->removeEntityListener(this);
	m_bodyA = ra;
	m_bodyA->addReference();
	m_bodyA->addEntityListener(this);
}

/////////////////////////////////////////////////////////////////////////


/// In typical usage setPointB should also be called to set the point 
/// in body space for the new body.

inline void hkDashpotAction::setBodyB(hkRigidBody* rb)
{
	HK_ASSERT(rb == HK_NULL);
    m_bodyB->removeReference();
    m_bodyB->removeEntityListener(this);
	m_bodyB = rb;
	m_bodyB->addReference();
	m_bodyB->addEntityListener(this);
}

/////////////////////////////////////////////////////////////////////////

inline const hkVector4& hkDashpotAction::getPointA() const
{
  return m_point[0];
}

/////////////////////////////////////////////////////////////////////////

inline const hkVector4& hkDashpotAction::getPointB() const
{
  return m_point[1];
}

/////////////////////////////////////////////////////////////////////////

inline void hkDashpotAction::setPointA(const hkVector4& pa)
{
  m_point[0] = pa;
}

/////////////////////////////////////////////////////////////////////////

inline void hkDashpotAction::setPointB(const hkVector4& pb)
{
  m_point[1] = pb;
}

/////////////////////////////////////////////////////////////////////////


/// \param pa The hkDashpotAction application point on bodyA, in the local space of bodyA, are copied into pa.  
/// \param pb The hkDashpotAction application point on bodyB, in the local space of bodyB, are copied into pb.  
inline void hkDashpotAction::getPoints(hkVector4& pa, hkVector4& pb)
{
  pa = m_point[0];
  pb = m_point[1];
}

/////////////////////////////////////////////////////////////////////////


/// \param pa The position of the point on bodyA in the local space of bodyA  
/// \param pb The position of the point on bodyB in the local space of bodyB  
inline void hkDashpotAction::setPoints(const hkVector4& pa, const hkVector4& pb)
{
  m_point[0] = pa;
  m_point[1] = pb;
}

/////////////////////////////////////////////////////////////////////////	

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
