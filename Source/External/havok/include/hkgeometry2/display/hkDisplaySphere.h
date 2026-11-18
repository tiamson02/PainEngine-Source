/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_GEOMETRY2_DISPLAY_SPHERE_H
#define HK_GEOMETRY2_DISPLAY_SPHERE_H

#include <hkgeometry2/display/hkDisplayGeometry.h>
#include <hkgeometry2/hkSphere.h>

class hkDisplaySphere : public hkDisplayGeometry
{
	public:

		hkDisplaySphere(const hkSphere& sphere, int xRes, int yRes);

		virtual void buildGeometry();

		virtual void getWireframeGeometry(hkArray<hkVector4>& lines);
		
		hkSphere getSphere();
		
		inline int getXRes() { return m_xRes; }
		inline int getYRes() { return m_yRes; }

	private:

		hkSphere m_sphere;

		int m_xRes;
		int m_yRes;
};


#endif // HK_GEOMETRY2_DISPLAY_SPHERE_H

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
