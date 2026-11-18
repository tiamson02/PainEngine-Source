/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkgeometry2/display/hkDisplaySphere.h>

hkDisplaySphere::hkDisplaySphere(const hkSphere& sphere, int xRes, int yRes) 
:  hkDisplayGeometry(HK_DISPLAY_SPHERE) , 
   m_sphere(sphere), m_xRes(xRes), m_yRes(yRes)
{

}


void hkDisplaySphere::buildGeometry()
{
	// TODO
}


hkSphere hkDisplaySphere::getSphere()
{
	return m_sphere;
}


void hkDisplaySphere::getWireframeGeometry(hkArray<hkVector4>& lines)
{
	//TODO
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
