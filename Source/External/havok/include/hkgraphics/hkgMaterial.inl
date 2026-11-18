/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

hkgMaterial::hkgMaterial()
: hkgReferencedObject(), m_texture(HK_NULL), m_isTransparent(false), m_userData(HK_NULL)
{
	m_diffuse[0] = m_diffuse[1] = m_diffuse[2] = m_diffuse[3] = 1.0f;
	m_specular[0] = m_specular[1] = m_specular[2] = 0.0f;
}

void hkgMaterial::setTexture(hkgTexture* t)
{
	if (m_texture)
		m_texture->release();

	m_texture = t;

	if (m_texture)
	{
		m_texture->reference();
		m_isTransparent = (m_diffuse[3] < 0.99f) || m_texture->isTransparent();
	}
	else
	{
		// just need to recompute transparency
		m_isTransparent = (m_diffuse[3] < 0.99f);
	}
}

const hkgTexture* hkgMaterial::getTexture() const
{
	return m_texture;
}

hkgTexture* hkgMaterial::getTexture()
{
	return m_texture;
}

inline void* hkgMaterial::getUserData() const
{
	return m_userData;
}
	
inline void hkgMaterial::setUserData(void* d)
{	
	m_userData = d;
}

inline void hkgMaterial::setDiffuseColor( float r, float g, float b, float a)
{
	m_diffuse[0] = r;
	m_diffuse[1] = g;
	m_diffuse[2] = b;
	m_diffuse[3] = a;

	m_isTransparent |= (a < 0.99f);
}

inline void hkgMaterial::getDiffuseColor( float& r, float& g, float& b, float& a) const
{
	r = m_diffuse[0];
	g = m_diffuse[1];
	b = m_diffuse[2];
	a = m_diffuse[3];
}

inline void hkgMaterial::setSpecularColor( float r, float g, float b )
{
	m_specular[0] = r;
	m_specular[1] = g;
	m_specular[2] = b;
}

inline void hkgMaterial::getSpecularColor( float& r, float& g, float& b ) const
{
	r = m_specular[0];
	g = m_specular[1];
	b = m_specular[2];
}

inline bool hkgMaterial::isTransparent() const
{
	return m_isTransparent;
}

inline HKG_MATERIAL_TRANS_SOURCE hkgMaterial::getTransparentSource() const
{
	HKG_MATERIAL_TRANS_SOURCE s = HKG_MATERIAL_TRANS_SOURCE_NONE;

	if (m_diffuse[3] < 0.99f)
		s |= HKG_MATERIAL_TRANS_SOURCE_COLOR;

	if (m_texture && m_texture->isTransparent())
		s |= HKG_MATERIAL_TRANS_SOURCE_TEXTURE;

	return s;
}

inline bool hkgMaterial::operator==( const hkgMaterial& other ) const
{
	return ( (m_texture == other.m_texture) // texture
			&& ( hkgVec3Equals( m_diffuse, other.m_diffuse, 0.01f)) // diffuse
			&& ( hkg_fabs(m_diffuse[3] - other.m_diffuse[3]) < 0.01f)  // alpha
			&& ( hkgVec3Equals( m_specular, other.m_specular, 0.01f )) ); // specular
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
