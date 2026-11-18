/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_GEOMETRY2_DISPLAY_GEOMETRY_H
#define HK_GEOMETRY2_DISPLAY_GEOMETRY_H


#include <hkmath2/hkMath.h>
#include <hkgeometry2/hkGeometry.h>
#include <hkgeometry2/display/hkDisplayGeometryTypes.h>


class hkDisplayGeometry : public hkReferencedObject
{
	public:
		
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_DISPLAY)

		inline hkDisplayGeometry(hkDisplayGeometryType type);

		inline virtual ~hkDisplayGeometry();

		inline hkGeometry* getGeometry();

		// method returns array of points s,e,s,e,s,e - start, end ...
		virtual void getWireframeGeometry(hkArray<hkVector4>& lines) = 0;

		virtual void buildGeometry() = 0;

		inline const hkTransform& getTransform() const;

		inline hkTransform& getTransform();

		inline hkDisplayGeometryType getType() const;

	protected:

		hkTransform m_transform;
		hkGeometry* m_geometry;
		hkDisplayGeometryType m_type;
};

#include <hkgeometry2/display/hkDisplayGeometry.inl>

#endif // HK_GEOMETRY2_DISPLAY_GEOMETRY_H

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
