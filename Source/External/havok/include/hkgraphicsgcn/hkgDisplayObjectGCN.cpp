/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkgraphicsgcn/hkgraphicsgcn.h> // precompiled
#include <hkgraphicsgcn/hkgDisplayObjectGCN.h>
#include <hkgraphicsgcn/hkgViewportGCN.h>

static float  temp[16];
static float  tempTrans[16];
static Mtx    view;
static Mtx    viewTemp;
const unsigned short FOUR_FLOATS = sizeof(float) * 4;

void hkgDisplayObjectGCN::render(hkgDisplayContext* state) const
{
	float* mv = HK_NULL;
	if (!m_identityTrans)
	{
		// get modelview, mult by our trans, and load
		
		mv = state->getCurrentViewport()->getCamera()->getViewMatrix();
		hkgMat4Mult(temp, mv, m_trans);
		hkgMat4Transpose( tempTrans, temp);
		
		// view has no bottom row in GX, so can't memcpy all at once
		hkString::memCpy( &(view[0][0]), &(tempTrans[0]), FOUR_FLOATS);
		hkString::memCpy( &(view[1][0]), &(tempTrans[4]), FOUR_FLOATS);
		hkString::memCpy( &(view[2][0]), &(tempTrans[8]), FOUR_FLOATS);
		        
	    GXLoadPosMtxImm(view, GX_PNMTX0);
	    // all we need for in inverse transpose of our RigidBody transforms (and modelview)
	    // is to wipe the trans
	 	view[0][3] = 0.0f;
	    view[1][3] = 0.0f;
	    view[2][3] = 0.0f;
		// MTXInverse(view, viewTemp); 
		// MTXTranspose(viewTemp, view);
   		GXLoadNrmMtxImm(view, GX_PNMTX0);
			
	}
	
	renderGeomsInOrder( state );

	if (mv)// then load back up the view matrix
	{
		hkgMat4Transpose( tempTrans, mv);
		// view has no bottom row in GX, so can't memcpy all at once
		hkString::memCpy( &(view[0][0]), &(tempTrans[0]), FOUR_FLOATS);
		hkString::memCpy( &(view[1][0]), &(tempTrans[4]), FOUR_FLOATS);
		hkString::memCpy( &(view[2][0]), &(tempTrans[8]), FOUR_FLOATS);
		        
	    GXLoadPosMtxImm(view, GX_PNMTX0);
	    // all we need for in inverse transpose of our RigidBody transforms (and modelview)
	    // is to wipe the trans
   		view[0][3] = 0.0f;
	    view[1][3] = 0.0f;
	    view[2][3] = 0.0f;
	    
		// MTXInverse(view, viewTemp); 
		// MTXTranspose(viewTemp, view);
   		GXLoadNrmMtxImm(view, GX_PNMTX0);
	}
}

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
