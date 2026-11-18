/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_GEOMETRY2_DISPLAY_CONE_H
#define HK_GEOMETRY2_DISPLAY_CONE_H

#include <hkgeometry2/display/hkDisplayGeometry.h>

class hkDisplayCone : public hkDisplayGeometry
{
	public:

		hkDisplayCone(const hkReal coneAngle, 
					  const hkReal coneHeight,
					  int numSegments, 
					  const hkVector4& coneAxis, 
					  const hkVector4& startPos);

		hkDisplayCone();
		
		void setParameters(const hkReal coneAngle, 
						   const hkReal coneHeight,
						   const int numSegments, 
						   const hkVector4& coneAxis, 
						   const hkVector4& startPos);
		
		virtual void buildGeometry();
		
		virtual void getWireframeGeometry(hkArray<hkVector4>& lines);

		// get apex of cone in WS
		hkVector4 getPosition();
		
		// get axis of cone in WS
		hkVector4 getAxis();
		
		// get height along axis
		hkReal getHeight();
		
		// get angle of cone
		hkReal getAngle();
		
		// get number of segments
		int getNumSegments();


	private:

		hkVector4 m_startPos;

		hkVector4 m_coneAxis;
		
		int m_numSegments;
		
		hkReal m_coneAngle;
		
		hkReal m_coneHeight;

		void generateConeVertices(hkArray<hkVector4>& conePoints);
};


#endif // HK_GEOMETRY2_DISPLAY_CONE_H

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
