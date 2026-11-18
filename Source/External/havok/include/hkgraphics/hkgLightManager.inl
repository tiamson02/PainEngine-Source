/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

inline hkgLightManager::hkgLightManager()
: m_maxActiveSetSize(3)
{
	hkgVec3Zero( m_sceneAmbient );
}


inline int hkgLightManager::getNumLights() const
{
	return m_lights.getSize();
}

inline hkgLight* hkgLightManager::getLight(int i)
{
	HK_ASSERT( i < m_lights.getSize() );
	return m_lights[i];
}

inline const hkgLight* hkgLightManager::getLight(int i) const
{
	HK_ASSERT( i < m_lights.getSize() );
	return m_lights[i];
}

inline const hkgLight* hkgLightManager::getActiveSetLight(int i) const
{
	HK_ASSERT( i < m_activeSet.getSize() );
	return m_activeSet[i];
}

inline hkgLight* hkgLightManager::getActiveSetLight(int i)
{
	HK_ASSERT( i < m_activeSet.getSize() );
	return m_activeSet[i];
}

inline void hkgLightManager::setMaxActiveSetSize(int asl)
{
	int maxlen = hkgDisplayContext::getMaxNumLights();
	int rasl = HKG_MIN(asl, maxlen);
	m_maxActiveSetSize = rasl;
}


inline int hkgLightManager::addLight(hkgLight* l)
{
	m_lights.pushBack( l );
	l->reference();
	return m_lights.getSize() -1;
}


inline int hkgLightManager::findLight(hkgLight* l) const
{
	int nl = m_lights.getSize();
	for (int i =0; i < nl; ++i)
		if (m_lights[i] == l)
			return i;

	return -1;
}

inline hkgLight* hkgLightManager::removeLight(int i)
{
	HK_ASSERT( i < m_lights.getSize() );
	hkgLight* l = m_lights[i];
	m_lights.removeAt(i);

	// it may be in the active set too:
	
	int nl = m_activeSet.getSize();
	for (int j =0; j < nl; ++j)
	{
		if (m_activeSet[j] == l)
		{
			m_activeSet.removeAt(j);
			break;
		}
	}

	return l;
}

inline void hkgLightManager::realizeActiveLights(hkgDisplayContext* context) const
{
	int nl = m_activeSet.getSize();
	int j = 0;
	for (; j < nl; ++j)
	{
		m_activeSet[j]->setLightNumber(j);
		m_activeSet[j]->updateState(context);
	}

	int ml = hkgDisplayContext::getMaxNumLights();
	while (j < ml)
		context->setLightState( j++, false);
}

inline void hkgLightManager::addSceneAmbient( const float a[3] )
{
	hkgVec3Add( m_sceneAmbient, m_sceneAmbient, a );
	hkgVec3Clamp( m_sceneAmbient, 0.0f, 1.0f );
}

inline void hkgLightManager::setSceneAmbient( const float a[3] )
{
	hkgVec3Copy( m_sceneAmbient, a );
}

inline void hkgLightManager::getSceneAmbient( float a[3] ) const
{
	hkgVec3Copy( a, m_sceneAmbient );
}

inline float* hkgLightManager::getSceneAmbientPtr()
{
	return m_sceneAmbient;
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
