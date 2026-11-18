/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

// CONSTRUCTOR
inline SkidmarkPoint::SkidmarkPoint ()
{
	m_pointLeftWS.setZero4();
	m_pointRightWS.setZero4();
}

inline SkidmarkPoint::SkidmarkPoint ( const hkVector4 &point_left_ws, const hkVector4 &point_right_ws, const hkReal &strength)
{
	m_pointLeftWS    = point_left_ws;
	m_pointLeftWS(3) = strength;
	m_pointRightWS   = point_right_ws;
	m_pointRightWS(3) = strength;
}

// ACCESSORS
inline const hkVector4 &SkidmarkPoint::getPointLeftWS() const
{
	return m_pointLeftWS;
}

inline const hkVector4 &SkidmarkPoint::getPointRightWS() const
{
	return m_pointRightWS;
}

inline const hkReal &SkidmarkPoint::getSkidStrength() const
{
	return m_pointRightWS(3);
}


inline int SkidmarksWheel::getNumPoints() const
{
	return m_numPoints;
}


inline const SkidmarkPoint &SkidmarksWheel::getSkidmarkPoint( int point) const
{
	const int index = ( m_currentPosition + point) % m_numPoints;
		// we add m_numPoints for the case m_currentPosition == point == 0
		// also due to the fact that we are using unsigned ints
	return m_skidmarkPoints[index];
}


// SKIDMARKSWHEEL

inline SkidmarksWheel::SkidmarksWheel()
{
	this->m_numPoints = 0;
	this->m_currentPosition = 0;
}

inline void SkidmarksWheel::addSkidmarkPoint(const SkidmarkPoint &point)
{
	const int previous_index = (m_currentPosition + m_numPoints - 1) % m_numPoints;

	const hkReal previous_strength = m_skidmarkPoints [previous_index].getSkidStrength();

	if ((point.getSkidStrength()==0.0f) && (previous_strength==0.0f)) 
		return; // two skidmarks == 0.0f in a row, we can ignore this one;

	m_skidmarkPoints[m_currentPosition] = point;
	m_currentPosition = ( m_currentPosition + 1 ) % m_numPoints;
}

inline int SkidmarksInfo::getNumWheels() const
{
	return m_numWheels;
}

inline const SkidmarksWheel& SkidmarksInfo::getSkidmarksWheel( int wheel) const
{
	return m_skidmarksWheel[wheel];
}

inline SkidmarksWheel& SkidmarksInfo::getSkidmarksWheel( int wheel)
{
	return m_skidmarksWheel[wheel];
}

inline const SkidmarkPoint &SkidmarksInfo::getSkidmarkPoint( int wheel, int point) const
{
	return m_skidmarksWheel[wheel].getSkidmarkPoint(point);
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
