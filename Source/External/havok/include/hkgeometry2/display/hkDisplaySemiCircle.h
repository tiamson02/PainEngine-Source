/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_GEOMETRY2_DISPLAY_SEMICIRCLE_H
#define HK_GEOMETRY2_DISPLAY_SEMICIRCLE_H

#include <hkgeometry2/display/hkDisplayGeometry.h>

class hkDisplaySemiCircle : public hkDisplayGeometry
{
	public:

		hkDisplaySemiCircle(const hkVector4& center, 
							const hkVector4& normal, 
							const hkVector4& startPerp,
							const hkReal thetaMin,
							const hkReal thetaMax,
							const hkReal radius,
							int numSegments);

		hkDisplaySemiCircle();
		
		virtual void buildGeometry();
		
		virtual void getWireframeGeometry(hkArray<hkVector4>& lines);

		void setParameters(const hkReal radius, 
						   const hkReal thetaMin,
						   const hkReal thetaMax,
						   int numSegments, 
						   const hkVector4& center,
						   const hkVector4& normal, 
						   const hkVector4& startPerp);

		// get normal to plane containing semi-circle
		hkVector4 getNormal();
	
		// get center of semi-circle in WS
		hkVector4 getCenter();
		
		// get perp to normal in WS
		hkVector4 getPerp();
		
		// get min angular extent
		hkReal getThetaMin();
		
		// get max angular extent
		hkReal getThetaMax();
		
		// get radius
		hkReal getRadius();

		int getNumSegments();

	private:

		hkVector4 m_normal;
		
		hkVector4 m_perp;
		
		hkVector4 m_center;
		
		hkReal m_thetaMin;
		
		hkReal m_thetaMax;
		
		hkReal m_radius;
		
		int m_numSegments;

		void generatePoints(hkArray<hkVector4>& points);
};


#endif // HK_GEOMETRY2_DISPLAY_SEMICIRCLE_H

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
