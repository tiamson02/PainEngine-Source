/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkgraphicsdx8/hkgraphicsdx8.h> // precompiled
#include <hkgraphicsdx8/hkgLightDX8.h>
#include <hkgraphicsdx8/hkgDisplayContextDX8.h>

const float ROOT_FLTMAX = (hkg_sqrt(FLT_MAX) - 1.0f);

hkgLightDX8::~hkgLightDX8()
{

}

void hkgLightDX8::updateState(hkgDisplayContext* context)
{
	if ( (m_lightNum < 0) || (m_lightNum > 7 ) )
		return; // disabled or not possible


	D3DLIGHT8 light;

	light.Type = (m_type == HKG_LIGHT_SPOT)? D3DLIGHT_SPOT :
				( (m_type == HKG_LIGHT_DIRECTIONAL)? D3DLIGHT_DIRECTIONAL :
				 D3DLIGHT_POINT );         		

    hkgVec3Copy( (float*)&(light.Diffuse), m_diffuse);
	light.Diffuse.a = 1.0f;

    hkgVec3Copy( (float*)&(light.Specular), m_specular);
	light.Specular.a = 1.0f;

	hkgVec3Copy( (float*)&(light.Ambient), m_ambient);
    light.Ambient.a = 1.0f;

	hkgVec3Copy( (float*)&(light.Position), m_pos);
	hkgVec3Copy( (float*)&(light.Direction), m_direction);
	
	light.Attenuation0 = 1.0f; 
	light.Attenuation1 = 0.0f;
	light.Attenuation2 = 0.0f;

	light.Range = ROOT_FLTMAX;
	light.Falloff = 1.0f; // falloff curve shape

	light.Theta	= m_spotInnerAngle * 3.14f / 90.0f; // inner cone
	light.Phi	= /*d3dLight.Theta + */( m_spotOuterAngle * 3.14f / 90.0f );
		
	LPDIRECT3DDEVICE8 dev = static_cast<hkgDisplayContextDX8*>( context )->getDevice();
	dev->SetLight( m_lightNum, &light);

	dev->LightEnable( m_lightNum, TRUE );
}


/*
	if ( (m_lightNum < 0) || (m_lightNum > 7 ) )
		return; // disabled or not possible

	float t4[4];
	t4[3] = 1.0f;

	hkgVec3Copy( t4, m_pos);
	glLightfv(GL_LIGHT0+m_lightNum, GL_POSITION, t4 );

	hkgVec3Copy( t4, m_diffuse);
	glLightfv(GL_LIGHT0+m_lightNum, GL_DIFFUSE, t4 );

	hkgVec3Copy( t4, m_ambient);
	glLightfv (GL_LIGHT0+m_lightNum, GL_AMBIENT, t4);
		
	if (m_type == HKG_LIGHT_SPOT)
	{
		hkgVec3Copy( t4, m_direction);
		glLightfv(GL_LIGHT0+m_lightNum, GL_SPOT_DIRECTION, t4);
		glLightf(GL_LIGHT0+m_lightNum, GL_SPOT_CUTOFF, m_spotOuterAngle);
		glLightf(GL_LIGHT0+m_lightNum, GL_SPOT_EXPONENT, m_spotOuterAngle == m_spotInnerAngle? 0 : m_spotExponent);
	}
	else
	{
		glLightf(GL_LIGHT0+m_lightNum, GL_SPOT_CUTOFF, 180);
		glLightf(GL_LIGHT0+m_lightNum, GL_SPOT_EXPONENT, 0);
	}

	glEnable( GL_LIGHT0+m_lightNum );
*/
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
