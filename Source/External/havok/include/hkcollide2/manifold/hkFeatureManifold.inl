/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


inline int hkFeatureManifold::getNumPoints()
{
	return m_numPoints;
}

inline hkFeatureContactPoint& hkFeatureManifold::operator[]( int index )
{
	return m_contactPoints[index];
}

inline bool hkFeatureManifold::findInManifold( const hkFeatureContactPoint& fcp )
{
	for (int i = m_numPoints-1; i>=0; i--)
	{
		if ( m_contactPoints[i].m_featureIdA == fcp.m_featureIdA && m_contactPoints[i].m_featureIdB == fcp.m_featureIdB )
		{
			return true;
		}
	}
	return false;
}

inline bool hkFeatureManifold::isComplete()
{
	return m_isComplete; 
}

inline void hkFeatureManifold::setComplete( bool complete )
{
	m_isComplete = complete;	
}

inline bool hkFeatureManifold::hasNoPointsLeft()
{
	return (m_numPoints >= HK_BOXBOX_MANIFOLD_MAX_POINTS); 
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
