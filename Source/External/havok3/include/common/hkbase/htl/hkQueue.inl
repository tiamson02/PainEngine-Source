/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


template <typename T>
hkQueue<T>::hkQueue(int guaranteedExtraCapacity)
	:	m_data( HK_NULL ),
		m_capacity( guaranteedExtraCapacity ),
		m_guaranteedExtraCapacity( guaranteedExtraCapacity ),
		m_head(0),
		m_tail(0),
		m_elementsInUse(0)
{
	if (guaranteedExtraCapacity > 0)
	{
		m_data = hkAllocateChunk<T>( guaranteedExtraCapacity, HK_MEMORY_CLASS_ARRAY );
	}
}


template <typename T>
hkQueue<T>::hkQueue( int n, int guaranteedExtraCapacity )
	:	m_data(hkAllocateChunk<T>( n + guaranteedExtraCapacity, HK_MEMORY_CLASS_ARRAY )),
		m_capacity(n + guaranteedExtraCapacity),
		m_head(0),
		m_tail(0),
		m_elementsInUse(0)
{
}

template <typename T>
inline void hkQueue<T>::setInitialCapacity( int capacity )
{
	HK_ASSERT2(0xefa09786, m_head == 0 && m_tail == 0, "setInititalCapacity must be called before any elements have been added to the queue.");

	int newCapacity = capacity > m_guaranteedExtraCapacity ? capacity : m_guaranteedExtraCapacity;

	if (m_data != HK_NULL)
	{
		hkDeallocateChunk<T>( m_data, m_capacity, HK_MEMORY_CLASS_ARRAY );
	}
	m_data = hkAllocateChunk<T>( newCapacity, HK_MEMORY_CLASS_ARRAY );
	m_capacity = newCapacity;
}



template <typename T>
hkQueue<T>::~hkQueue()
{
	releaseMemory();
}



template <typename T>
inline hkBool hkQueue<T>::isEmpty() const
{
	return m_elementsInUse == 0;
}


template <typename T>
inline int hkQueue<T>::getSize() const
{
	return m_elementsInUse;
}


template <typename T>
inline int hkQueue<T>::getCapacity() const
{
	return m_capacity;
}


template <typename T>
inline void hkQueue<T>::clear()
{
	m_head = 0;
	m_tail = 0;
	m_elementsInUse = 0;
}


template <typename T>
inline void hkQueue<T>::releaseMemory()
{
	if( m_capacity )
	{
		hkDeallocateChunk<T>( m_data, m_capacity, HK_MEMORY_CLASS_ARRAY );
	}
}


template <typename T>
inline void hkQueue<T>::reserve( int n )
{
	if( m_capacity < n)
	{
		// allocate a new buffer and copy existing data over
		char* p = hkAllocateChunk<char>( (n * sizeof(T)), HK_MEMORY_CLASS_ARRAY );

		if( m_data != HK_NULL )
		{
			hkString::memCpy( p, m_data, ( sizeof(T) * m_capacity ) );
		}

		// release the old buffer
		releaseMemory();

		// assign to new buffer and set capacity
		m_data = (T*)p;
		m_capacity = n;
	}
}

template <typename T>
inline void hkQueue<T>::increaseCapacity( )
{
	// double storage and add an extra 'm_tail' to move leading elements to new storage
	int capacity = m_elementsInUse;
	int extra = ( m_tail <= m_head ? m_tail : 0 );

	if( m_capacity == 0 )
	{
		// if there is no current capacity default to 8 elements
		int initialSize = m_guaranteedExtraCapacity < 8 ? 8 : m_guaranteedExtraCapacity;
		reserve( initialSize );
	}

	else
	{
		// allocate to the nearest power of two
		unsigned int memory = ( ( m_capacity * 2 ) + extra + m_guaranteedExtraCapacity ) * sizeof(T);

		unsigned int allocate = 1;

		while( memory > allocate )
		{
			allocate <<= 1;
		}

		reserve( allocate / sizeof(T) );
	}

	int i = 0;
	
	if( extra )
	{
		// copy leading elements
		for( i = 0; i < extra; i++ )
		{
			m_data[capacity + i] = m_data[i];
		}
	}

	// place tail to start of new storage
	m_tail = capacity + i;
}

// Places a new element to the back of the queue and expand storage if necessary.
template <typename T>
inline void hkQueue<T>::enqueue( const T& element )
{	

	//if (m_tail >= m_head) HK_ASSERT(0, m_tail - m_head == m_elementsInUse);
	//else HK_ASSERT(0, m_capacity - m_head + m_tail == m_elementsInUse);

	if( ( m_elementsInUse + m_guaranteedExtraCapacity ) >= m_capacity )
	{
		increaseCapacity();
	}
	else if( m_tail == m_capacity )
	{
		m_tail = 0;
	}

	m_data[m_tail++] = element;
	m_elementsInUse++;

	//if (m_tail >= m_head) HK_ASSERT(0, m_tail - m_head == m_elementsInUse);
	//else HK_ASSERT(0, m_capacity - m_head + m_tail == m_elementsInUse);

}

// Places a new element to the back of the queue and expand storage if necessary.
template <typename T>
inline void hkQueue<T>::enqueueInFront( const T& element )
{	
	if( ( m_elementsInUse + m_guaranteedExtraCapacity ) >= m_capacity )
	{
		increaseCapacity();
	}

	if( m_head == 0 )
	{
		m_head = m_capacity;
	}

	m_data[--m_head] = element;
	m_elementsInUse++;
}


template <typename T>
inline void hkQueue<T>::dequeue( T& data )
{
	HK_ASSERT(0xf032ed23, m_elementsInUse );
	{
		data = m_data[m_head];

		if( ++m_head == m_capacity )
		{
			m_head = 0;
		}
		m_elementsInUse--;
	}
}

template <typename T>
inline void hkQueue<T>::dequeue(  )
{
	HK_ASSERT(0xf032ed23, m_elementsInUse );
	{
		if( ++m_head == m_capacity )
		{
			m_head = 0;
		}
		m_elementsInUse--;
	}
}

template <typename T>
inline T* hkQueue<T>::probeDequeue(  )
{
	HK_ASSERT(0xf032ed23, m_elementsInUse );
	return &m_data[m_head];
}

template <typename T>
inline int hkQueue<T>::getGuaranteedExtraCapacity()
{
	return m_guaranteedExtraCapacity;
}
			
template <typename T>
inline void hkQueue<T>::setGuaranteedExtraCapacity( int guaranteedExtraCapacity )
{
	m_guaranteedExtraCapacity = guaranteedExtraCapacity;
	if( ( m_elementsInUse + m_guaranteedExtraCapacity ) >= m_capacity )
	{
		increaseCapacity();
	}
}



/*
* Havok SDK - PUBLIC RELEASE, BUILD(#20060114)
*
* Confidential Information of Havok.  (C) Copyright 1999-2006 
* Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
* Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
* rights, and intellectual property rights in the Havok software remain in
* Havok and/or its suppliers.
*
* Use of this software for evaluation purposes is subject to and indicates 
* acceptance of the End User licence Agreement for this product. A copy of 
* the license is included with this software and is also available from salesteam@havok.com.
*
*/
