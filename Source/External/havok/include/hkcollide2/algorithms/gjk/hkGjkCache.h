/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_COLLIDE2_GJK_CACHE_H
#define HK_COLLIDE2_GJK_CACHE_H

//: A structure to hold GJK caching information between calls.
class hkGjkCache
{
	public:
	
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_CDINFO, hkGjkCache)

		inline hkGjkCache();

		inline void reset ();
		inline void swap (hkVector4* M, int i, int j);
		
		hkVector4 Ra[4];				//: Vertices on simplex A.
		hkVector4 Rb[4];				//: Vertices on simplex B.
		
		int		rSize;						//: Size of simplices.

};


#include <hkcollide2/algorithms/gjk/hkGjkCache.inl>


#endif // HK_COLLIDE2_GJK_CACHE_H

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
