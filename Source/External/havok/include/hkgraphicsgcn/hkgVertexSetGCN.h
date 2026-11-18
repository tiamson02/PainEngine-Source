/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_GRAPHICS_VERTEX_SET_GCN_H
#define HK_GRAPHICS_VERTEX_SET_GCN_H

#include <hkgraphics/hkgVertexSet.h>

class hkgVertexSetGCN : public hkgVertexSet
{
public:

	static hkgVertexSet* createVertexSetGCN(hkgFaceSet* owner) { return new hkgVertexSetGCN(owner); }

	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_DISPLAY, hkgVertexSetGCN );

	virtual ~hkgVertexSetGCN();

	virtual void setNumVerts(int nv, bool uvs = true, bool colors = true);

	virtual unsigned char* lock(int offset = 0, int len = -1);  
	virtual void unlock(int offset = 0, int len = -1);

	virtual void setStreams(hkgDisplayContext* state); 
	// Set the client pointers / stream pointers to this data. Passed the current 
	// enabled streams so that it can match what it needs.

	// will assert if you try to append beyond m_numVerts)
	virtual void addPosInfo(const float* posInfo, int numPos);
	virtual void addUVInfo(const float* uvInfo, int numUVs); 
	virtual void addColorInfo(const float* colorInfo, int numColors);
	virtual void addNormalInfo(const float* normalInfo, int numNormals);

	virtual float* getPosInfo( int i );
	virtual float* getUVInfo( int i );
	virtual float* getColorInfo( int i );
	virtual float* getNormalInfo( int i );
	
protected:

	inline hkgVertexSetGCN(hkgFaceSet* owner);
	
	f32* 		m_vertBuf;
	f32* 		m_normBuf;
	u8* 		m_colorBuf;
	f32* 		m_uvBuf;
};

#include <hkgraphicsgcn/hkgVertexSetGCN.inl>

#endif //HK_GRAPHICS_VERTEX_SET_GCN_H
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
