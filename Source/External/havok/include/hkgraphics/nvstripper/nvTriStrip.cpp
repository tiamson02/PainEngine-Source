/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkgraphics/hkGraphics.h> // precompiled

#include <hkgraphics/nvstripper/nvTriStripObjects.h>
#include <hkgraphics/nvstripper/nvTriStrip.h>

#if defined(HK_COMPILER_MSVC)
	// conversion from 'type1' to 'type2', possible loss of data
#	pragma warning(disable: 4244)
#endif

////////////////////////////////////////////////////////////////////////////////////////
//private data
static unsigned int cacheSize    = NV_CACHESIZE_GEFORCE1_2;
static bool bStitchStrips        = true;
static int minStripSize = 0;
static bool bListsOnly           = false;

////////////////////////////////////////////////////////////////////////////////////////
// SetListsOnly()
//
// If set to true, will return an optimized list, with no strips at all.
//
// Default value: false
//
void nvSetListsOnly(const bool _bListsOnly)
{
	bListsOnly = _bListsOnly;
}

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
void nvSetCacheSize(const unsigned int _cacheSize)
{
	cacheSize = _cacheSize;
}


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
void nvSetStitchStrips(const bool _bStitchStrips)
{
	bStitchStrips = _bStitchStrips;
}


////////////////////////////////////////////////////////////////////////////////////////
// SetMinStripSize()
//
// Sets the minimum acceptable size for a strip, in triangles.
// All strips generated which are shorter than this will be thrown into one big, separate list.
//
// Default value: 0
//
void nvSetMinStripSize(const int _minStripSize)
{
	minStripSize = _minStripSize;
}

////////////////////////////////////////////////////////////////////////////////////////
// GenerateStrips()
//
// in_indices: input index list, the indices you would use to render
// in_numIndices: number of entries in in_indices
// primGroups: array of optimized/stripified nvPrimitiveGroups
// numGroups: number of groups returned
//
// Be sure to call delete[] on the returned primGroups to avoid leaking mem
//
void nvGenerateStrips(const unsigned short* in_indices, int in_numIndices,
					nvPrimitiveGroup** primGroups, unsigned short* numGroups)
{
	//put data in format that the stripifier likes
	nvWordVec tempIndices;
	tempIndices.setSize(in_numIndices);
	unsigned short maxIndex = 0;
	int i;
	for(i = 0; i < in_numIndices; i++)
	{
		tempIndices[i] = in_indices[i];
		if(in_indices[i] > maxIndex)
			maxIndex = in_indices[i];
	}
	nvStripInfoVec tempStrips;
	nvFaceInfoVec tempFaces;
	nvFaceInfoVec discardedDegenerates;

	nvStripifier stripifier;
	
	//do actual stripification
	stripifier.Stripify(tempIndices, cacheSize, minStripSize, maxIndex, tempStrips, tempFaces, discardedDegenerates);

	//stitch strips together
	nvIntVec stripIndices;
	int numSeparateStrips = 0;

	if(bListsOnly)
	{
		//if we're outputting only lists, we're done
		*numGroups = 1;
		(*primGroups) = new nvPrimitiveGroup[*numGroups];
		nvPrimitiveGroup* primGroupArray = *primGroups;

		//count the total number of indices
		int numIndices = 0;
		int i;
		for(i = 0; i < tempStrips.getSize(); i++)
		{
			numIndices += tempStrips[i]->m_faces.getSize() * 3;
		}

		//add in the list
		numIndices += tempFaces.getSize() * 3;

		primGroupArray[0].type       = NV_PT_LIST;
		primGroupArray[0].numIndices = numIndices;
		primGroupArray[0].indices    = hkAllocate<unsigned short>(numIndices, HK_MEMORY_CLASS_DISPLAY);

		//do strips
		unsigned int indexCtr = 0;
		for(i = 0; i < tempStrips.getSize(); i++)
		{
			for(int j = 0; j < tempStrips[i]->m_faces.getSize(); j++)
			{
				//degenerates are of no use with lists
				if(!nvStripifier::IsDegenerate(tempStrips[i]->m_faces[j]))
				{
					primGroupArray[0].indices[indexCtr++] = tempStrips[i]->m_faces[j]->m_v0;
					primGroupArray[0].indices[indexCtr++] = tempStrips[i]->m_faces[j]->m_v1;
					primGroupArray[0].indices[indexCtr++] = tempStrips[i]->m_faces[j]->m_v2;
				}
				else
				{
					//we've removed a tri, reduce the number of indices
					primGroupArray[0].numIndices -= 3;
				}
			}
		}

		//do lists
		for(i = 0; i < tempFaces.getSize(); i++)
		{
			primGroupArray[0].indices[indexCtr++] = tempFaces[i]->m_v0;
			primGroupArray[0].indices[indexCtr++] = tempFaces[i]->m_v1;
			primGroupArray[0].indices[indexCtr++] = tempFaces[i]->m_v2;
		}
	}
	else
	{
		stripifier.CreateStrips(tempStrips, stripIndices, bStitchStrips, numSeparateStrips);

		//if we're stitching strips together, we better get back only one strip from CreateStrips()
		HK_ASSERT( (bStitchStrips && (numSeparateStrips == 1)) || !bStitchStrips);
		
		//convert to output format
		*numGroups = numSeparateStrips; //for the strips
		if(tempFaces.getSize() != 0)
			(*numGroups)++;  //we've got a list as well, increment
		(*primGroups) = new nvPrimitiveGroup[*numGroups];
		
		nvPrimitiveGroup* primGroupArray = *primGroups;
		
		//first, the strips
		int startingLoc = 0;
		for(int stripCtr = 0; stripCtr < numSeparateStrips; stripCtr++)
		{
			int stripLength = 0;

			if(!bStitchStrips)
			{
				//if we've got multiple strips, we need to figure out the correct length
				int i;
				for(i = startingLoc; i < stripIndices.getSize(); i++)
				{
					if(stripIndices[i] == -1)
						break;
				}
				
				stripLength = i - startingLoc;
			}
			else
				stripLength = stripIndices.getSize();
			
			primGroupArray[stripCtr].type       = NV_PT_STRIP;
			primGroupArray[stripCtr].indices    = hkAllocate<unsigned short>(stripLength, HK_MEMORY_CLASS_DISPLAY);
			primGroupArray[stripCtr].numIndices = stripLength;
			
			int indexCtr = 0;
			for(int i = startingLoc; i < stripLength + startingLoc; i++)
				primGroupArray[stripCtr].indices[indexCtr++] = stripIndices[i];

			//we add 1 to account for the -1 separating strips
			//this doesn't break the stitched case since we'll exit the loop
			startingLoc += stripLength + 1; 
		}
		
		//next, the list
		if(tempFaces.getSize() != 0)
		{
			int faceGroupLoc = (*numGroups) - 1;    //the face group is the last one
			primGroupArray[faceGroupLoc].type       = NV_PT_LIST;
			primGroupArray[faceGroupLoc].indices    = hkAllocate<unsigned short>(tempFaces.getSize() * 3, HK_MEMORY_CLASS_DISPLAY);
			primGroupArray[faceGroupLoc].numIndices = tempFaces.getSize() * 3;
			int indexCtr = 0;
			for(int i = 0; i < tempFaces.getSize(); i++)
			{
				primGroupArray[faceGroupLoc].indices[indexCtr++] = tempFaces[i]->m_v0;
				primGroupArray[faceGroupLoc].indices[indexCtr++] = tempFaces[i]->m_v1;
				primGroupArray[faceGroupLoc].indices[indexCtr++] = tempFaces[i]->m_v2;
			}
		}
	}

	//clean up everything

	//delete strips
	for(i = 0; i < tempStrips.getSize(); i++)
	{
		for(int j = 0; j < tempStrips[i]->m_faces.getSize(); j++)
		{
			delete tempStrips[i]->m_faces[j];
			tempStrips[i]->m_faces[j] = HK_NULL;
		}
		delete tempStrips[i];
		tempStrips[i] = HK_NULL;
	}

	//delete faces
	for(i = 0; i < tempFaces.getSize(); i++)
	{
		delete tempFaces[i];
		tempFaces[i] = HK_NULL;
	}

	///XXX mem leak fix
	// delete the degenerates that never made it into final srips.
	for(i = 0; i < discardedDegenerates.getSize(); i++)
	{
		delete discardedDegenerates[i];
	}

	
}


////////////////////////////////////////////////////////////////////////////////////////
// RemapIndices()
//
// Function to remap your indices to improve spatial locality in your vertex buffer.
//
// in_primGroups: array of nvPrimitiveGroups you want remapped
// numGroups: number of entries in in_primGroups
// numVerts: number of vertices in your vertex buffer, also can be thought of as the range
//  of acceptable values for indices in your primitive groups.
// remappedGroups: array of remapped nvPrimitiveGroups
//
// Note that, according to the remapping handed back to you, you must reorder your 
//  vertex buffer.
//
void nvRemapIndices(const nvPrimitiveGroup* in_primGroups, const unsigned short numGroups,
				  const unsigned short numVerts, nvPrimitiveGroup** remappedGroups)
{
	(*remappedGroups) = new nvPrimitiveGroup[numGroups];

	//caches oldIndex --> newIndex conversion
	int *indexCache;
	indexCache = hkAllocate<int>(numVerts, HK_MEMORY_CLASS_DISPLAY);
	hkString::memSet(indexCache, -1, sizeof(int)*numVerts);
	
	//loop over primitive groups
	unsigned int indexCtr = 0;
	for(int i = 0; i < numGroups; i++)
	{
		int numIndices = in_primGroups[i].numIndices;

		//init remapped group
		(*remappedGroups)[i].type       = in_primGroups[i].type;
		(*remappedGroups)[i].numIndices = numIndices;
		(*remappedGroups)[i].indices    = hkAllocate<unsigned short>( numIndices, HK_MEMORY_CLASS_DISPLAY);

		for(int j = 0; j < numIndices; j++)
		{
			int cachedIndex = indexCache[in_primGroups[i].indices[j]];
			if(cachedIndex == -1) //we haven't seen this index before
			{
				//point to "last" vertex in VB
				(*remappedGroups)[i].indices[j] = indexCtr;

				//add to index cache, increment
				indexCache[in_primGroups[i].indices[j]] = indexCtr++;
			}
			else
			{
				//we've seen this index before
				(*remappedGroups)[i].indices[j] = cachedIndex;
			}
		}
	}

	hkDeallocate<int>(indexCache);
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
