/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

/// \class hkContactPoint
///
/// m_normal always points such that moving object A in the direction of the normal 'separates'
/// the bodies. In other words, moving object A by -distance units in the direction of the normal
/// will bring the bodies into exact (surface) contact. The direction of the normal does not change
/// if the bodies become penetrating, but the distance will become negative. Thus the normal always points
/// "into" A, and "out of" B.
///
/// m_position is the contact point. It may be anywhere between the two "closest" points, depending on
/// the agent which created it, but is usually half-way between. In the penetrating case the "closest points"
/// is the pair which determines the direction of minimum separation.
///
/// The distance (returned by getDistance()) is the distance between "closest points" on A and B.
/// If this distance is negative, the bodies are penetrating.
///
/// NOTE: To be totally precise, moving object A in the positive direction of the normal 'separates' the 
/// "closest points", but is not (in the case of concave or compound bodies) guaranteed to separate
/// the bodies themselves. 

inline const hkVector4& hkContactPoint::getPosition() const
{ 
	return m_position; 
}

inline void hkContactPoint::setPosition( const hkVector4& position )
{	
	hkReal w = m_position(3); 
	m_position = position; 
	m_position(3) = w; 
}


inline const hkVector4& hkContactPoint::getNormal() const
{ 
	return m_normal; 
}

inline void hkContactPoint::setNormal( const hkVector4& normal )
{	
	m_normal = normal;  
}


inline hkReal hkContactPoint::getDistance() const
{
	return m_position(3); 
}

inline void hkContactPoint::setDistance( hkReal d )
{ 
	m_position(3) = d; 
}


inline void hkContactPoint::set( const hkVector4& position, const hkVector4& normal, float dist )
{ 
	m_position = position; 
	m_normal = normal, m_position(3) = dist; 
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
