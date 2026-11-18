/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkgraphicsgcn/hkgraphicsgcn.h> // precompiled
#include <hkgraphicsgcn/hkgCameraGCN.h>

#define PRINTMAT(x) { hkprintf("\nMatrix:\t[%.3f %.3f %.3f %.3f]\n\t\t[%.3f %.3f %.3f %.3f]\n\t\t[%.3f %.3f %.3f %.3f]\n\t\t[%.3f %.3f %.3f %.3f]\n",\
						x[0], x[4], x[8], x[12], \
						x[1], x[5], x[9], x[13], \
						x[2], x[6], x[10], x[14], \
						x[3], x[7], x[11], x[15] ); }


void hkgCameraGCN::setAsCurrent(hkgDisplayContext* context)
{
	Mtx44  hkgproj;
 	Mtx    hkgview;
 	Mtx    viewTemp;
 	
 #define TEST_MATRICES
 #ifdef TEST_MATRICES
 	
 	Mtx    gxview;
 	Mtx44  gxproj;
 		
	Vec location;
	Vec up;
	Vec at;
	
	hkgVec3Copy( (float*)&location, m_from);
	hkgVec3Copy( (float*)&up, m_up);
	hkgVec3Copy( (float*)&at, m_to);
	
	MTXLookAt(
        gxview,
        &location ,
        &up,
        &at );
     
    if (m_mode == HKG_CAMERA_PERSPECTIVE)
    {
	    MTXPerspective (
	        gxproj,
	        m_fov,
	       	m_aspect,
	        m_near,
	        m_far );
	}
	else	        
	{
		MTXOrtho (
		    gxproj,
		    m_otop,
		    m_obottom,
		    m_oleft,
		    m_oright,
		    m_onear,
		    m_ofar );
	}
	
#endif
        
   	// Transpose to change storage order and memcpy (as values are correct handidness etc)
	float vt[16];
	hkgMat4Transpose( vt, m_viewMat);
	// view has no bottom row in GX, so can't memcpy all at once
	hkString::memCpy( &(hkgview[0][0]), &(vt[0]), sizeof(float) * 4);
	hkString::memCpy( &(hkgview[1][0]), &(vt[4]), sizeof(float) * 4);
	hkString::memCpy( &(hkgview[2][0]), &(vt[8]), sizeof(float) * 4);
	        
    GXLoadPosMtxImm(hkgview, GX_PNMTX0);
   
   	// The inverse transpose of our rigidbody transforms
   	// and our modelview is simply the rotational part, no trans:
    hkgview[0][3] = 0.0f;
	hkgview[1][3] = 0.0f;
	hkgview[2][3] = 0.0f;
	    
  //  MTXInverse(hkgview, viewTemp); 
  //  MTXTranspose(viewTemp, hkgview);
	GXLoadNrmMtxImm(hkgview, GX_PNMTX0);
	
    // have to change z range to -1 to 0 (from -1 to 1)  
    hkgMat4Transpose( (float*)&(hkgproj[0][0]), m_projMat);
    if (m_mode == HKG_CAMERA_PERSPECTIVE)
    {
        float fmn =  1.0f / (m_far - m_near);

    	hkgproj[2][2] = -(m_near * fmn);
    	hkgproj[2][3] = -(m_near * m_far * fmn);
    	
   	    GXSetProjection(hkgproj, GX_PERSPECTIVE);
    }
    else // ortho
    {
        float fmn =  1.0f / (m_ofar - m_onear);

    	hkgproj[2][2] *= 0.5f;
    	hkgproj[2][3] = -(m_ofar * fmn);
    	
   	    GXSetProjection(gxproj, GX_ORTHOGRAPHIC);
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
