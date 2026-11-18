/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

hkgDisplayWorld::hkgDisplayWorld()
: m_materialManager(HK_NULL),
  m_lightManager(HK_NULL),
  m_culled(0)
{

}



inline int hkgDisplayWorld::getNumDisplayObjects() const
{
	return ( m_solidObjects.getSize() + m_alphaObjects.getSize() + m_mixedObjects.getSize() );
}

inline hkgDisplayObject* hkgDisplayWorld::getDisplayObject( int i)
{
	HK_ASSERT(  i < getNumDisplayObjects() );
	int ss = m_solidObjects.getSize();
	if (i < ss)
		return m_solidObjects[i]; 
	else if ( i < (ss + m_alphaObjects.getSize()) )
		return m_alphaObjects[i-ss]; 
	else
		return m_mixedObjects[i-(ss+m_alphaObjects.getSize())]; 
}

inline const hkgDisplayObject* hkgDisplayWorld::getDisplayObject( int i) const
{
	return const_cast<hkgDisplayWorld*>(this)->getDisplayObject(i);
}


inline const hkgMaterialManager* hkgDisplayWorld::getMaterialManager() const
{
	return m_materialManager;
}

inline hkgMaterialManager* hkgDisplayWorld::getMaterialManager()
{
	return m_materialManager;
}



inline const hkgLightManager* hkgDisplayWorld::getLightManager() const
{
	return m_lightManager;
}

inline hkgLightManager* hkgDisplayWorld::getLightManager()
{
	return m_lightManager;
}


inline int hkgDisplayWorld::getNumCulledLastDraw() const
{
	return m_culled;
}

inline int hkgDisplayWorld::getNumAlphaObjects() const
{
	return m_alphaObjects.getSize();
}

inline int hkgDisplayWorld::getNumSolidObjects() const
{
	return m_solidObjects.getSize();
}

inline int hkgDisplayWorld::getNumMixedObjects() const
{
	return m_mixedObjects.getSize();
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
