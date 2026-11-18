/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

/// \class hkCollidable
/// It holds the entity's hkShape, an hkTransform defining its frame of reference, and a pointer
/// back to its owner. hkCollidable also extends the hkBroadPhaseHandle class, allowing the entity
/// to be added to the collision detection broadphase. If the entity does not have a collision
/// detection representation, the hkCollidable's hkShape member has the value HK_NULL.
/// 

inline hkCollidable::hkCollidable(const hkShape* shape,
								  const hkMotionState* ms,
								  const hkCollisionResponse* response,
								  void* owner,
								  int type)
:	hkTypedBroadPhaseHandle(type),
	m_shape(shape),
	m_motionState(ms),
	m_response( response), 
	m_owner(owner)
{
#ifdef HK_DEBUG
	checkPerformance();
#endif
}

inline hkCollidable::~hkCollidable()
{

}

inline const hkTransform* hkCollidable::getTransform() const
{ 
	return &m_motionState->m_sceneGraphTransform1; 
}

inline const hkMotionState* hkCollidable::getMotionState() const
{ 
	return m_motionState; 
}

inline void hkCollidable::setMotionState( const hkMotionState* t )
{
	m_motionState = t;
}

inline void hkCollidable::setShape(hkShape* shape)
{ 
	m_shape = shape;

	// Just duplicating the constructor behavior, checking if the new m_shape might cause
	// performance loss.
#if defined HK_DEBUG
	checkPerformance();
#endif
}

inline const hkShape* hkCollidable::getShape() const
{ 
	return m_shape; 
}

inline void* hkCollidable::getOwner() const
{ 
	return m_owner; 
}
		
inline void hkCollidable::setOwner( void* owner )
{
	m_owner = owner;
}

inline const hkCollisionResponse* hkCollidable::getResponse() const
{
	return m_response;
}

inline void hkCollidable::setResponse( const hkCollisionResponse* response ) 
{
	m_response = response;
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
