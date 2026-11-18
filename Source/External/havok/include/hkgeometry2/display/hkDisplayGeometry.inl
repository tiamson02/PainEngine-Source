/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

		
inline hkDisplayGeometry::hkDisplayGeometry(hkDisplayGeometryType type)
:	m_geometry(HK_NULL),
	m_type(type)
{
	m_transform.setIdentity();
}


inline hkDisplayGeometry::~hkDisplayGeometry()
{
	if (m_geometry != HK_NULL)
	{
		delete m_geometry;
		m_geometry = HK_NULL;
	}
}


hkGeometry* hkDisplayGeometry::getGeometry() 
{ 
	return m_geometry; 
}

inline const hkTransform& hkDisplayGeometry::getTransform() const
{
	return m_transform;
}

inline hkTransform& hkDisplayGeometry::getTransform()
{
	return m_transform;
}

inline hkDisplayGeometryType hkDisplayGeometry::getType() const
{
	return m_type;
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
