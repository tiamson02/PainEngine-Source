/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkgraphics/hkGraphics.h> // precompiled
#include <hkgraphics/hkgLightManager.h>


const float DEFAULT_SCENE_AMBIENT[] = {0.1f, 0.1f, 0.1f, 1.0f};
const float DEFAULT_LIGHT_DIFFUSE[] = {1.0f, 1.0f, 1.0f, 0.0f};

hkgLightManager* (*hkgLightManager::create)();

hkgLightManager::~hkgLightManager()
{
	int nl = m_lights.getSize();
	for (int i =0; i < nl; ++i)
		m_lights[i]->release();
}


void hkgLightManager::computeActiveSet( const float worldPointOfInterest[3])
{
	// XXX TODO: this properly
	// for the moment, choose the first enabled lights in the list

	// Before adding lights to the active set we should clean it up (M.B.) 
    m_activeSet.clear(); 


	for (int i = 0; (i < m_lights.getSize()) && (i < m_maxActiveSetSize); ++i)
	{
		if (m_lights[i]->getDesiredEnabledState())
			m_activeSet.pushBack( m_lights[i] );
	}
}

void hkgLightManager::enable(int light)
{
	HK_ASSERT( light < m_lights.getSize() );
	m_lights[light]->setDesiredEnabledState(true);
}
	
void hkgLightManager::disable(int light)
{
	HK_ASSERT( light < m_lights.getSize() );
	m_lights[light]->setDesiredEnabledState(false);
}

void hkgLightManager::enable(hkgLight* light)
{
	light->setDesiredEnabledState(true);
}

void hkgLightManager::disable(hkgLight* light)
{
	light->setDesiredEnabledState(false);
}

void hkgLightManager::addDefaultLights( float distanceFromOrigin, const float up[3], const float front[3], int numLights)
{

	float upN[3];
	float frontN[3];

	hkgVec3Copy( upN, up );
	hkgVec3Copy( frontN, front );
	hkgVec3Normalize( upN );
	hkgVec3Normalize( frontN );

	float lc[3];
	hkgVec3Copy( lc, DEFAULT_LIGHT_DIFFUSE );
	hkgVec3Div( lc, (float)numLights ); 

	float vangle = HKG_PI * 0.25f; // 45 degrees
	float height = distanceFromOrigin * hkg_sin( vangle );
	float upVec[3];

	hkgVec3Mult( upVec, upN, height );

	float hradius = distanceFromOrigin * hkg_cos( vangle );

	float towards[3];

	hkgVec3Mult( towards, frontN, hradius);
	hkgVec3Add( towards, towards, upVec );

	float hangle = HKG_PI * 2.0f / numLights; // (360 / i) degrees
	
	for (int i = 0; i < numLights; ++i)
	{
		hkgLight* light = hkgLight::create();
		
		float p[3];

		if (i > 0)
		{
			float thisHangle = hangle * i;

			float q[4];
			
			// rotate facing by thisHandle about origin and add to hvec to get pos
			hkgQuatFromAngleAxis( q, thisHangle, upN );
			hkgQuatRotateVec3( p, q, towards );
		}
		else // just copy..
		{
			hkgVec3Copy(p, towards );
		}

		light->setType( HKG_LIGHT_OMNI );
		light->setPosition( p );
		light->setDiffuse( lc );

		addLight( light );
		light->release();
	}
}


void hkgLightManager::print(hkOstream& stream) const
{
	stream << "[" << m_lights.getSize() << "] lights  (" << m_activeSet.getSize() << " in active set).\n";
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
