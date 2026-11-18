/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

hkgTexture::hkgTexture()
: hkgReferencedObject(), m_clut(HK_NULL), m_data(HK_NULL), m_userData(HK_NULL)
{
}
	
inline bool hkgTexture::hasData() const
{
	return (m_bpp!= 0) && (m_data != HK_NULL);
}

inline const unsigned char* hkgTexture::getDataPointer() const
{
	return m_data;
}

inline const unsigned char* hkgTexture::getClutPointer() const
{
	return m_clut;
}

inline unsigned char* hkgTexture::getDataPointer()
{
	return m_data;
}

inline unsigned char* hkgTexture::getClutPointer()
{
	return m_clut;
}

inline unsigned int hkgTexture::getWidth() const
{
	return m_width;
}

inline unsigned int hkgTexture::getHeight() const
{
	return m_height;
}

inline unsigned int hkgTexture::getBPP() const
{
	return m_bpp;
}

inline bool hkgTexture::isTransparent() const
{
	return m_isTransparent;
}

inline bool hkgTexture::hasAlphaChannel() const
{
	return m_hasAlpha;
}

inline void* hkgTexture::getUserData() const
{
	return m_userData;
}

inline void hkgTexture::setUserData(void* d)
{
	m_userData = d;
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
