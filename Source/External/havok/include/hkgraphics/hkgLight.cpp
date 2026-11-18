/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkgraphics/hkGraphics.h> // precompiled
#include <hkgraphics/hkgLight.h>
#include <hkgraphics/hkgLightManager.h>

hkgLight* (*hkgLight::create)();

hkgLight::~hkgLight()
{
}

void hkgLight::enable( hkgLightManager* manager )
{
	HK_ASSERT( manager->findLight( this ) >= 0 );
	
	if (m_lightNum >=0 )
		return; // already enabled as it has a light num

	manager->enable(this);
}

void hkgLight::disable( hkgLightManager* manager )
{
	HK_ASSERT( manager->findLight( this ) >= 0 );

	if (m_lightNum < 0 )
		return; // already disabled as it has no light num

	manager->disable(m_lightNum);
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
