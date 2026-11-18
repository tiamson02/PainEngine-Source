/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

int hkMeshShape::getNumSubparts() const
{
	return m_subparts.getSize();
}

hkMeshShape::Subpart& hkMeshShape::getSubpartAt( int i )
{
	return m_subparts[i];
}

const hkMeshShape::Subpart& hkMeshShape::getSubpartAt( int i ) const
{
		return m_subparts[i];
}



			/// get the extra radius for every triangle
hkReal hkMeshShape::getRadius() const
{
	return m_radius;
}

		/// set the extra radius for every triangle
void hkMeshShape::setRadius(hkReal r )
{
	m_radius = r;
}

inline const hkVector4&	hkMeshShape::getScaling() const
{
	return m_scaling;
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
