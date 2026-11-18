/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_GRAPHICS_FACE_SET_GCN_H
#define HK_GRAPHICS_FACE_SET_GCN_H

#include <hkgraphics/hkgFaceSet.h>

class hkgFaceSetGCN : public hkgFaceSet
{
	public:

		static hkgFaceSet* createFaceSetGCN(hkgDisplayContext* c) { return new hkgFaceSetGCN(c); }

		void initialize();
		void render(hkgDisplayContext* state);

	protected:
		
		inline hkgFaceSetGCN(hkgDisplayContext* context);
		virtual ~hkgFaceSetGCN();
		
		void drawWireframe( hkgDisplayContext* state, bool c, bool uv);
};

#include <hkgraphicsgcn/hkgFaceSetGCN.inl>

#endif // HK_GRAPHICS_FACE_SET_GCN_H
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
