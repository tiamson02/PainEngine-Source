/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkbase/hkBase.h>

HK_COMPILE_TIME_ASSERT( sizeof(void*)==sizeof(char*) );

// hack to force this to be out of line
void HK_CALL hkArrayUtil::_reserve(void* array, int numElem, int sizeElem)
{
	HK_ASSERT2(numElem >= 0, "Number of elements must not be negative");
	HK_ASSERT2(sizeElem >= 0, "The size of an element must not be negative");

	typedef hkArray<char> hkAnyArray;

	char* p = hkAllocateChunk<char>(numElem*sizeElem, HK_MEMORY_CLASS_ARRAY);
	hkAnyArray* self = reinterpret_cast< hkAnyArray* >(array);
	hkString::memCpy(p, self->m_data, self->m_size*sizeElem);
	//cannot call self->releaseMemory(), because we're lying about the size of each element.
	if( (self->m_capacityAndDeallocateFlag & hkAnyArray::DONT_DEALLOCATE_DATA_MASK) == 0)
	{
		hkDeallocateChunk<char>( self->m_data, self->getCapacity()*sizeElem, HK_MEMORY_CLASS_ARRAY);
	}
	
	self->m_data = p;
	self->m_capacityAndDeallocateFlag = numElem;
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
