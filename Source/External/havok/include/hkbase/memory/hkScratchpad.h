/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HKBASE_HKSCRATCHPAD_H
#define HKBASE_HKSCRATCHPAD_H

/// A scratchpad is a small piece of contiguous memory used for
/// temporary calculations. On supported hardware it may map to 
/// a physically faster portion of memory, for instance PS2.
class hkScratchpad : public hkSingleton<hkScratchpad>
{
	public:
	
		hkScratchpad( void* p, int sz)
			: m_address(p), m_sizeInBytes(sz), m_inUse(false)
		{
		}

			/// Acquire the scratchpad memory exclusively.
		void* acquire()
		{
			HK_ASSERT( m_inUse == false );
			m_inUse = true;
			return m_address;
		}

			/// Release the memory to allow it to be acquired again.		
		void release()
		{
			HK_ASSERT( m_inUse == true );
			m_inUse = false;
		}
		
			/// Return the size of the memory block.
		int getSizeInBytes() const
		{
			return m_sizeInBytes;
		}
		
	protected:

		void* m_address;	
		int m_sizeInBytes;
		bool m_inUse;
};

	/// 
template <typename T>
class hkLocalArray : public hkArray<T>
{
	public:

			/// Create an array with zero size and given capacity.
		hkLocalArray( int capacity )
		{
			m_data = hkAllocateStack<T>(capacity);
			m_capacityAndDeallocateFlag = capacity | DONT_DEALLOCATE_DATA_MASK;
			m_localMemory = m_data;				
		}

			/// Discard the current contents and resize to the given size. Memory is uninitialised.
		void discardAndResize(int newsize)
		{
			HK_ASSERT2( m_localMemory == m_data, "A localarray grew beyond its original specified capacity.");
			hkDeallocateStack(m_localMemory, getCapacity() );
			m_localMemory = m_data = hkAllocateStack<T>(newsize);
			m_size = newsize;
			m_capacityAndDeallocateFlag = newsize | DONT_DEALLOCATE_DATA_MASK;
		}

			/// Destroy the array.
		~hkLocalArray()
		{
			HK_ASSERT2( m_localMemory == m_data, "A localarray grew beyond its original specified capacity.");
			hkDeallocateStack<T>( m_localMemory, getCapacity() );
		}

		T* m_localMemory;
};

#endif // HKBASE_HKSCRATCHPAD_H

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
