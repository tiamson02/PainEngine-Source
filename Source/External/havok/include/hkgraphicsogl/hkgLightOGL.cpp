/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkgraphicsogl/hkgraphicsogl.h> // precompiled
#include <hkgraphicsogl/hkgLightOGL.h>

hkgLightOGL::~hkgLightOGL()
{

}

void updateState()
{

}

void hkgLightOGL::updateState(hkgDisplayContext* context)
{
	if ( (m_lightNum < 0) || (m_lightNum > 7 ) )
		return; // disabled or not possible

	GLenum lightID = (GLenum)(GL_LIGHT0+m_lightNum);

	float t4[4];
	t4[3] = 1.0f;

	hkgVec3Copy( t4, m_pos);
	glLightfv(lightID, GL_POSITION, t4 );

	hkgVec3Copy( t4, m_diffuse);
	glLightfv(lightID, GL_DIFFUSE, t4 );

	hkgVec3Copy( t4, m_ambient);
	glLightfv(lightID, GL_AMBIENT, t4);

// PS2GL don't like these spots too much yet.
#ifndef HK_PLATFORM_PS2		
	if (m_type == HKG_LIGHT_SPOT)
	{
		hkgVec3Copy( t4, m_direction);
		glLightfv(lightID, GL_SPOT_DIRECTION, t4);
		glLightf(lightID, GL_SPOT_CUTOFF, m_spotOuterAngle);
		glLightf(lightID, GL_SPOT_EXPONENT, m_spotOuterAngle == m_spotInnerAngle? 0 : m_spotExponent);
	}
	else
#endif
	{
		glLightf(lightID, GL_SPOT_CUTOFF, 180);
		glLightf(lightID, GL_SPOT_EXPONENT, 0);
	}

	glLightf(lightID, GL_CONSTANT_ATTENUATION,  1.0f);
	glLightf(lightID, GL_LINEAR_ATTENUATION,    0.0f);
	glLightf(lightID, GL_QUADRATIC_ATTENUATION, 0.0f);

	glEnable(lightID);
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
