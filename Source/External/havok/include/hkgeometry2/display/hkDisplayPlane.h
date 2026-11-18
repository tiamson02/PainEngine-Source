/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_GEOMETRY2_DISPLAY_PLANE_H
#define HK_GEOMETRY2_DISPLAY_PLANE_H

#include <hkgeometry2/display/hkDisplayGeometry.h>

class hkDisplayPlane : public hkDisplayGeometry
{
	public:

		hkDisplayPlane();
		hkDisplayPlane(const hkVector4& normal, const hkVector4& perpToNormal, 
					   const hkVector4& center, hkReal extent);
		

		virtual void buildGeometry();
		
		virtual void getWireframeGeometry(hkArray<hkVector4>& lines);
	
		void setParameters(const hkVector4& normal, const hkVector4& perpToNormal, 
						   const hkVector4& center, hkReal extent);

		hkVector4& getNormal();

		hkVector4& getCenter();

		hkVector4& getPerpToNormal();

		hkReal& getExtent();

	private:

		hkVector4 m_normal;
		hkVector4 m_center;
		hkVector4 m_perpToNormal;
		hkReal m_extent;


		void createPlanePoints(hkVector4* points);
	
};


#endif // HK_GEOMETRY2_DISPLAY_PLANE_H

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
