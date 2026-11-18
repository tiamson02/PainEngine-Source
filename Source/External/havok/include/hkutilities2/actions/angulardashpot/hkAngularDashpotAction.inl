/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

inline hkAngularDashpotActionConstructionInfo::hkAngularDashpotActionConstructionInfo()
{
	m_bodyA = HK_NULL;
	m_bodyB = HK_NULL;
	m_strength = 1;
	m_damping = .1f;
	m_rotation.setIdentity();
}

////////////////////////////////////////////////////////////////////////////

void hkAngularDashpotActionConstructionInfo::set(hkRigidBody* bodyA,hkRigidBody* bodyB,
		                                         const hkQuaternion& r)
{
	m_bodyA = bodyA;
	m_bodyB = bodyB;
	m_rotation = r;
	m_strength = 1;
	m_damping = .1f;
}

////////////////////////////////////////////////////////////////////////////

inline void hkAngularDashpotActionConstructionInfo::setStrength(hkReal str)
{
  m_strength = str;
}

////////////////////////////////////////////////////////////////////////////

inline void hkAngularDashpotActionConstructionInfo::setDamping(hkReal damp)
{
  m_damping = damp;
}

////////////////////////////////////////////////////////////////////////////

inline hkAngularDashpotAction::~hkAngularDashpotAction()
{

}


/// The rotation value specifies the relative orientation between the two bodies - or 
/// between the body and the world - that 
/// the angular dashpot tries to maintain. 
inline const hkQuaternion& hkAngularDashpotAction::getRotation() const 
{ 
	return m_rotation; 
}
	

/// The rotation value specifies the relative orientation between the two bodies - or 
/// between the body and the world - that 
/// the angular dashpot tries to maintain. The rotation 
/// value is expressed as a quaternion. 
/// \param q The relative orientation between the two bodies, or between the body and the world.
inline void hkAngularDashpotAction::setRotation(const hkQuaternion& q) 
{ 
	m_rotation = q; 
}
	

/// The default value is 
/// 1.
inline hkReal hkAngularDashpotAction::getStrength() const 
{ 
	return m_strength; 
}
	

/// The default strength value is 1 
/// . If the weight of the bodies is much greater then 1 it may be necessary to 
/// increase the strength for the  dashpot to have an appropriate affect .
/// \param s The strength of the dashpot  
inline void hkAngularDashpotAction::setStrength(hkReal s) 
{ 
	m_strength = s; 
}
	

/// The damping parameter specifies the rate of decay of the oscillations the 
/// dashpot produces. Values higher than 10% of the strength of the Dashpot can cause instability.
inline hkReal hkAngularDashpotAction::getDamping() const 
{ 
	return m_damping; 
}
	

/// The damping parameter specifies the rate of decay of the oscillations the 
/// dashpot produces. Values higher than 10% of the strength of the Dashpot can 
/// cause instability.
/// \param s The damping coefficient of the dashpot  
inline void hkAngularDashpotAction::setDamping(hkReal s) 
{ 
	m_damping = s; 
}



inline void hkAngularDashpotAction::setBodyA(hkRigidBody* ra)
{
	HK_ASSERT(ra == HK_NULL);
    m_bodyA->removeReference();
    m_bodyA->removeEntityListener(this);
	m_bodyA = ra;
	m_bodyA->addReference();
	m_bodyA->addEntityListener(this);
}


inline void hkAngularDashpotAction::setBodyB(hkRigidBody* rb)
{	
	HK_ASSERT(rb == HK_NULL);
    m_bodyB->removeReference();
    m_bodyB->removeEntityListener(this);
	m_bodyB = rb;
	m_bodyB->addReference();
	m_bodyB->addEntityListener(this);
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
