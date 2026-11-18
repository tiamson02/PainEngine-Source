/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkgraphicsgcn/hkgraphicsgcn.h> // precompiled
#include <hkgraphicsgcn/hkgLightGCN.h>
#include <hkgraphicsgcn/hkgViewportGCN.h>
#include <hkgraphicsgcn/hkgCameraGCN.h>

hkgLightGCN::~hkgLightGCN()
{

}

void hkgLightGCN::updateState(hkgDisplayContext* context)
{
   if ( (m_lightNum < 0) || (m_lightNum > 7 ) )
		return; // disabled or not possible

	GXColor c;
	c.r = m_diffuse[0] * 255;
	c.g = m_diffuse[1] * 255;
	c.b = m_diffuse[2] * 255;
	c.a = 255;

	// lights need to be in view space:
	float p[3];
	hkgVec3Transform(p, m_pos, context->getCurrentViewport()->getCamera()->getViewMatrix());

	GXInitLightPos(&m_gxLight, p[0], p[1], p[2]);
	GXInitLightColor(&m_gxLight, c);	
	GXLightID lid = (GXLightID)(1<< m_lightNum);
	GXInitLightDistAttn(&m_gxLight, 1, 1, GX_DA_OFF);

	GXLoadLightObjImm(&m_gxLight, lid);

	if (m_type == HKG_LIGHT_SPOT)
	{
		GXInitLightSpot(&m_gxLight, m_spotOuterAngle * 0.5f, GX_SP_COS);

		// transform world light normal		
		hkgVec3Copy(p, m_direction);
		hkgVec3Rotate(p, context->getCurrentViewport()->getCamera()->getViewMatrix());
		GXInitLightDir( &m_gxLight, p[0], p[1], p[2]);
	}
	
	context->setLightState( m_lightNum, true ); // let the context know we have created it so that it can keep track
	
/*
	unsigned int lightID = GL_LIGHT0+m_lightNum;

	float t4[4];
	t4[3] = 1.0f;

	hkgVec3Copy( t4, m_pos);
	glLightfv(lightID, GL_POSITION, t4 );

	hkgVec3Copy( t4, m_diffuse);
	glLightfv(lightID, GL_DIFFUSE, t4 );

	hkgVec3Copy( t4, m_ambient);
	glLightfv(lightID, GL_AMBIENT, t4);
		
	if (m_type == HKG_LIGHT_SPOT)
	{
		hkgVec3Copy( t4, m_direction);
		glLightfv(lightID, GL_SPOT_DIRECTION, t4);
		glLightf(lightID, GL_SPOT_CUTOFF, m_spotOuterAngle);
		glLightf(lightID, GL_SPOT_EXPONENT, m_spotOuterAngle == m_spotInnerAngle? 0 : m_spotExponent);
	}
	else
	{
		glLightf(lightID, GL_SPOT_CUTOFF, 180);
		glLightf(lightID, GL_SPOT_EXPONENT, 0);
	}

	glLightf(lightID, GL_CONSTANT_ATTENUATION,  1.0f);
	glLightf(lightID, GL_LINEAR_ATTENUATION,    0.0f);
	glLightf(lightID, GL_QUADRATIC_ATTENUATION, 0.0f);

	glEnable(lightID);
*/
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
