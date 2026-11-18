/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef NV_TRI_STRIP_H
#define NV_TRI_STRIP_H


//
// HAVOK NOTE
//
//
// This code has been slightly modified to use the Havok 
// memory manager and so not use standard stl or straight new / delete
//

////////////////////////////////////////////////////////////////////////////////////////
// Public interface for stripifier
////////////////////////////////////////////////////////////////////////////////////////

//GeForce1 and 2 cache size
#define NV_CACHESIZE_GEFORCE1_2 16

//GeForce3 cache size
#define NV_CACHESIZE_GEFORCE3   24

enum nvPrimType
{
	NV_PT_LIST,
	NV_PT_STRIP,
	NV_PT_FAN
};

struct nvPrimitiveGroup
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_DISPLAY, nvPrimitiveGroup );

	nvPrimType type;
	unsigned int numIndices;
	unsigned short* indices;
	bool ownIndices;

////////////////////////////////////////////////////////////////////////////////////////

	nvPrimitiveGroup() : type(NV_PT_STRIP), numIndices(0), indices(HK_NULL), ownIndices(true) {}
	~nvPrimitiveGroup()
	{
		if(indices && ownIndices)
			hkDeallocate<unsigned short>(indices);
		indices = HK_NULL;
	}
};

////////////////////////////////////////////////////////////////////////////////////////
// SetCacheSize()
//
// Sets the cache size which the stripfier uses to optimize the data.
// Controls the length of the generated individual strips.
// This is the "actual" cache size, so 24 for GeForce3 and 16 for GeForce1/2
// You may want to play around with this number to tweak performance.
//
// Default value: 16
//
void nvSetCacheSize(const unsigned int cacheSize);


////////////////////////////////////////////////////////////////////////////////////////
// SetStitchStrips()
//
// bool to indicate whether to stitch together strips into one huge strip or not.
// If set to true, you'll get back one huge strip stitched together using degenerate
//  triangles.
// If set to false, you'll get back a large number of separate strips.
//
// Default value: true
//
void nvSetStitchStrips(const bool bStitchStrips);


////////////////////////////////////////////////////////////////////////////////////////
// SetMinStripSize()
//
// Sets the minimum acceptable size for a strip, in triangles.
// All strips generated which are shorter than this will be thrown into one big, separate list.
//
// Default value: 0
//
void nvSetMinStripSize(const int minSize);


////////////////////////////////////////////////////////////////////////////////////////
// SetListsOnly()
//
// If set to true, will return an optimized list, with no strips at all.
//
// Default value: false
//
void nvSetListsOnly(const bool bListsOnly);


////////////////////////////////////////////////////////////////////////////////////////
// GenerateStrips()
//
// in_indices: input index list, the indices you would use to render
// in_numIndices: number of entries in in_indices
// primGroups: array of optimized/stripified PrimitiveGroups
// numGroups: number of groups returned
//
// Be sure to call delete[] on the returned primGroups to avoid leaking mem
//
void nvGenerateStrips(const unsigned short* in_indices, int in_numIndices,
					nvPrimitiveGroup** primGroups, unsigned short* numGroups);


////////////////////////////////////////////////////////////////////////////////////////
// RemapIndices()
//
// Function to remap your indices to improve spatial locality in your vertex buffer.
//
// in_primGroups: array of PrimitiveGroups you want remapped
// numGroups: number of entries in in_primGroups
// numVerts: number of vertices in your vertex buffer, also can be thought of as the range
//  of acceptable values for indices in your primitive groups.
// remappedGroups: array of remapped PrimitiveGroups
//
// Note that, according to the remapping handed back to you, you must reorder your 
//  vertex buffer.
//
// Credit goes to the MS Xbox crew for the idea for this interface.
//
void nvRemapIndices(const nvPrimitiveGroup* in_primGroups, const unsigned short numGroups, 
				  const unsigned short numVerts, nvPrimitiveGroup** remappedGroups);

#endif


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
