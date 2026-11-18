/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_GEOMETRY2_DISPLAY_AABB_H
#define HK_GEOMETRY2_DISPLAY_AABB_H

#include <hkgeometry2/display/hkDisplayGeometry.h>

class hkDisplayAABB : public hkDisplayGeometry
{
	public:

		hkDisplayAABB();
		hkDisplayAABB(const hkVector4& min, const hkVector4& max);
		
		virtual void buildGeometry();
		
		virtual void getWireframeGeometry(hkArray<hkVector4>& lines);

		void setExtents(const hkVector4& min, const hkVector4& max);

		const hkVector4& getMinExtent();
		const hkVector4& getMaxExtent();

	private:

		hkVector4 m_minExtent;
		hkVector4 m_maxExtent;
};


#endif // HK_GEOMETRY2_DISPLAY_AABB_H

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
