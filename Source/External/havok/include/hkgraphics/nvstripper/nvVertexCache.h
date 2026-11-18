/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef NV_VERTEX_CACHE_H
#define NV_VERTEX_CACHE_H

class nvVertexCache
{
public:
	
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_DISPLAY, nvVertexCache );

	nvVertexCache(int size=16)
	{
		numEntries = size;
		
		entries = hkAllocate<int>(numEntries, HK_MEMORY_CLASS_DISPLAY);
		
		for(int i = 0; i < numEntries; i++)
			entries[i] = -1;
	}
		
	~nvVertexCache()
	{
		hkDeallocate<int>(entries);
		entries = 0;
	}
	
	bool InCache(int entry)
	{
		bool returnVal = false;
		for(int i = 0; i < numEntries; i++)
		{
			if(entries[i] == entry)
			{
				returnVal = true;
				break;
			}
		}
		
		return returnVal;
	}
	
	int AddEntry(int entry)
	{
		int removed;
		
		removed = entries[numEntries - 1];
		
		//push everything right one
		for(int i = numEntries - 2; i >= 0; i--)
		{
			entries[i + 1] = entries[i];
		}
		
		entries[0] = entry;
		
		return removed;
	}

	void Clear()
	{
		hkString::memSet(entries, -1, hkSizeOf(int) * numEntries);
	}
	
	int At(int index) { return entries[index]; }
	void Set(int index, int value) { entries[index] = value; }

	void Copy(nvVertexCache* inVcache) 
	{
		for(int i = 0; i < numEntries; i++)
		{
			inVcache->Set(i, entries[i]);
		}
	}

private:

  int *entries;
  int numEntries;

};

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
