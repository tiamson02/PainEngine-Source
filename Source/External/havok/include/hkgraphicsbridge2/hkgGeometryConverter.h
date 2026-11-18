/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


#ifndef HK_GRAPHICS_GEOMETRY_CONVERTER_H
#define HK_GRAPHICS_GEOMETRY_CONVERTER_H

class hkGeometry;
class hkgGeometry;
class hkDisplaySphere;
class hkDisplayGeometry;
class hkgDisplayContext;
class hkgGeometryConverter
{
	public:

		static hkgGeometry* createGraphicsDisplay(hkDisplayGeometry* geom, hkgDisplayContext* context);

			// helpers
		static hkgGeometry* createGraphicsDisplay(hkGeometry* geom, hkgDisplayContext* context);
		static hkgGeometry* createGraphicsDisplay(hkDisplaySphere* geom, hkgDisplayContext* context);

};



#endif //  HK_GRAPHICS_GEOMETRY_CONVERTER_H

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
