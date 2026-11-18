/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkgraphicsogl/hkgraphicsogl.h> // precompiled
#include <hkgraphicsogl/hkgCameraOGL.h>

#define PRINTMAT(x) { hkprintf("\nMatrix:\t[%.3f %.3f %.3f %.3f]\n\t\t[%.3f %.3f %.3f %.3f]\n\t\t[%.3f %.3f %.3f %.3f]\n\t\t[%.3f %.3f %.3f %.3f]\n",\
						x[0], x[4], x[8], x[12], \
						x[1], x[5], x[9], x[13], \
						x[2], x[6], x[10], x[14], \
						x[3], x[7], x[11], x[15] ); }


void hkgCameraOGL::setAsCurrent(hkgDisplayContext* context)
{
	glMatrixMode( GL_PROJECTION );
#ifdef HK_PLATFORM_PS2
	glLoadIdentity();	
	glMultMatrixf( m_projMat );
#else
	glLoadMatrixf( m_projMat );	
#endif

//	PRINTMAT( m_viewMat );

	glMatrixMode( GL_MODELVIEW );
#ifdef HK_PLATFORM_PS2
	glLoadIdentity();	
	glMultMatrixf( m_viewMat );
#else
	glLoadMatrixf( m_viewMat );
#endif
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
