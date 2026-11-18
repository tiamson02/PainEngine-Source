/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_GEOMETRY2_DISPLAY_CAPSULE_H
#define HK_GEOMETRY2_DISPLAY_CAPSULE_H

#include <hkgeometry2/display/hkDisplayGeometry.h>

class hkDisplayCapsule : public hkDisplayGeometry
{
	public:

		hkDisplayCapsule( const hkVector4& top, const hkVector4& bottom, hkReal radius, int numSides = 6, int numHeightSegments = 1 );

		virtual void buildGeometry();

		virtual void getWireframeGeometry(hkArray<hkVector4>& lines);
		
		const hkVector4& getTop() { return m_top; }
		const hkVector4& getBottom() { return m_bottom; }
		hkReal getRadius() { return m_radius; }
		
		inline int getNumHeightSamples() { return m_numHeightSegments; }
		inline int getNumSides() { return m_numSides; }

	private:

		hkVector4	m_top;
		hkVector4	m_bottom;
		hkReal		m_radius;

		int m_numSides;
		int m_numHeightSegments;
};


#endif // HK_GEOMETRY2_DISPLAY_CAPSULE_H

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
