/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_GEOMETRY2_DISPLAY_BOX_H
#define HK_GEOMETRY2_DISPLAY_BOX_H

#include <hkgeometry2/display/hkDisplayGeometry.h>

class hkDisplayBox : public hkDisplayGeometry
{
	public:

		hkDisplayBox(const hkVector4& halfExtents);
		hkDisplayBox();

		void setParameters(const hkVector4& halfExtents, const hkTransform& t);
		
		virtual void buildGeometry();

		virtual void getWireframeGeometry(hkArray<hkVector4>& lines);
		
		inline const hkVector4& getHalfExtents() { return m_halfExtents; }

	private:

		hkVector4 m_halfExtents;
};


#endif // HK_GEOMETRY2_DISPLAY_BOX_H

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
