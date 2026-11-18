/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


template <typename T>
inline void HK_CALL hkArray<T>::copy(T* dst, const T* src, int n)
{
	for(int i = 0; i < n; ++i)
	{
		dst[i] = src[i];
	}
}

template <typename T>
inline void HK_CALL hkArray<T>::copyBackwards(T* dst, const T* src, int n)
{
	for(int i = n-1; i >= 0; --i)
	{
		dst[i] = src[i];
	}
}


template <typename T>
inline hkArray<T>::hkArray()
	:	m_data(HK_NULL),
		m_size(0),
		m_capacityAndDeallocateFlag(DONT_DEALLOCATE_DATA_MASK)
{
}

template <typename T>
inline hkArray<T>::hkArray(int n)
	:	m_data(hkAllocateChunk<T>(n, HK_MEMORY_CLASS_ARRAY)),
		m_size(n),
		m_capacityAndDeallocateFlag(n)
{
}

template <typename T>
inline hkArray<T>::hkArray(int n, const T& t)
	:	m_data(hkAllocateChunk<T>(n, HK_MEMORY_CLASS_ARRAY)),
		m_size(n),
		m_capacityAndDeallocateFlag(n)
{
	for(int i = 0; i < n; ++i)
	{
		m_data[i] = t;
	}
}

template <typename T>
inline hkArray<T>::hkArray(const hkArray& a)
{
	m_data = hkAllocateChunk<T>(a.getSize(), HK_MEMORY_CLASS_ARRAY) ;
	m_size = a.getSize();
	m_capacityAndDeallocateFlag = a.getSize();
	copy(m_data, a.m_data, a.getSize());
}

template <typename T>
inline void hkArray<T>::releaseMemory()
{
	if( (m_capacityAndDeallocateFlag & DONT_DEALLOCATE_DATA_MASK) == 0)
	{
		hkDeallocateChunk<T>( m_data, getCapacity(), HK_MEMORY_CLASS_ARRAY);
	}
}

template <typename T>
inline hkArray<T>& hkArray<T>::operator= (const hkArray<T>& a)
{
	if(getCapacity() < a.getSize())
	{
		releaseMemory();
		m_data = hkAllocateChunk<T>(a.m_size, HK_MEMORY_CLASS_ARRAY);
		m_capacityAndDeallocateFlag = a.m_size;
	}
	m_size = a.m_size;
	copy(m_data, a.m_data, a.getSize());
	return *this;
}

template <typename T, unsigned N>
hkArray<T>& hkInplaceArray<T,N>::operator= (const hkArray<T>& a)
{
	return hkArray<T>::operator=(a);
}

template <typename T, unsigned N>
hkArray<T>& hkInplaceArrayAligned16<T,N>::operator= (const hkArray<T>& a)
{
	return hkArray<T>::operator=(a);
}

/// Note this does not copy the array but uses it in place. 
/// Care needs to be taken that the data ptr points to is valid for the scope
/// of this array. ptr will not be deallocated on destruction.
template <typename T>
inline hkArray<T>::hkArray(T* ptr, int size, int capacity)
	:	m_data(ptr),
		m_size(size),
		m_capacityAndDeallocateFlag(capacity | DONT_DEALLOCATE_DATA_MASK)
{
}

template <typename T>
inline hkArray<T>::~hkArray()
{
	releaseMemory();
}



template <typename T>
inline T& hkArray<T>::operator[] (int i)
{
	HK_ASSERT(i<m_size);
	return m_data[i];
}

template <typename T>
inline const T& hkArray<T>::operator[] (int i) const
{
	HK_ASSERT(i<m_size);
	return m_data[i];
}

template <typename T>
inline T& hkArray<T>::back()
{
	HK_ASSERT(m_size);
	return m_data[m_size-1];
}

template <typename T>
inline const T& hkArray<T>::back() const
{
	HK_ASSERT(m_size);
	return m_data[m_size-1];
}

template <typename T>
inline int hkArray<T>::getSize() const
{
	return m_size;
}

template <typename T>
inline int hkArray<T>::getCapacity() const
{
	return m_capacityAndDeallocateFlag & CAPACITY_MASK;
}

template <typename T>
inline bool hkArray<T>::isEmpty() const
{
	return m_size == 0;
}

template <typename T>
inline void hkArray<T>::clear()
{
	m_size = 0;
}

template <typename T>
inline void hkArray<T>::clearAndDeallocate()
{
	releaseMemory();
	m_data = HK_NULL;
	m_size = 0;
	m_capacityAndDeallocateFlag = DONT_DEALLOCATE_DATA_MASK;
}

/// This is done by moving the last element into index i
/// and resizing by -1. This is very fast, but note that
/// the order of elements is changed.
template <typename T>
inline void hkArray<T>::removeAt(int i)
{
	HK_ASSERT(i >=0 && i < m_size);
	--m_size;
	m_data[i] = m_data[m_size];
}

/// This is done by copying elements down to fill the empty
/// space. Slower than above, but the order is unchanged.
template <typename T>
inline void hkArray<T>::removeAtAndCopy(int index)
{
	--m_size;
	for(int i = index; i < m_size; ++i)
	{
		m_data[i] = m_data[i+1];
	}
}

template <typename T>
inline int hkArray<T>::indexOf(const T& t) const
{
	for(int i = 0; i < m_size ; ++i)
	{
		if( m_data[i] == t )
		{
			return i;
		}
	}
	return -1;
}

template <typename T>
inline int hkArray<T>::lastIndexOf(const T& t) const
{
	for(int i = m_size-1; i >=0; --i)
	{
		if( m_data[i] == t )
		{
			return i;
		}
	}
	return -1;
}

template <typename T>
inline void hkArray<T>::popBack()
{
	HK_ASSERT(m_size > 0);
	--m_size;
}



/// begin and end are inclusive and exclusive respectively.
/// i.e. {0,1,2,3}.getSubarray(1,3) == { 1, 2 }
template <typename T>
inline hkArray<T> hkArray<T>::getSubarray(int begin, int end)
{
	return hkArray(m_data+begin, end-begin);
}


template <typename T>
inline void hkArray<T>::reserveExactly(int n)
{
	if(getCapacity() < n)
	{
		hkArrayUtil::_reserve(this, n, sizeof(T));
	}
}

template <typename T>
inline void hkArray<T>::reserve(int n)
{
	if(getCapacity() < n)
	{
		int cap2 = 2 * getCapacity();
		int newSize = (n < cap2) ? cap2 : n;
		hkArrayUtil::_reserve(this, newSize, sizeof(T));
	}
}

template <typename T>
inline void hkArray<T>::pushBack(const T& t)
{
	if(m_size == getCapacity())
	{
		hkArrayUtil::_reserve( this, (m_size ? m_size*2 : 1), sizeof(T) );
	}
	m_data[m_size++] = t;
}

template <typename T>
inline void hkArray<T>::pushBackUnchecked(const T& t)
{
	HK_ASSERT( m_size < getCapacity());
	m_data[m_size++] = t;
}


/// If the array is expanded, new elements are uninitialized.
template <typename T>
inline void hkArray<T>::setSize(int n)
{
	reserve(n);
	m_size = n;
}

/// If the array is expanded, new elements are uninitialized.
template <typename T>
inline void hkArray<T>::setSizeUnchecked(int n)
{
	HK_ASSERT( n <= getCapacity());
	m_size = n;
}

	/// increments the size by n and returns a reference to the first element created
template <typename T>
inline T* hkArray<T>::expandBy( int n )
{
	int oldsize = m_size;
	setSize( oldsize + n );
	return m_data+oldsize;
}


	/// increments the size by n and returns a reference to the first element created
template <typename T>
inline T* hkArray<T>::expandByUnchecked( int n )
{
	int oldsize = m_size;
	setSizeUnchecked( oldsize + n );
	return m_data+oldsize;
}


/// If the array is expanded, new elements initialized with 'fill'.
template <typename T>
inline void hkArray<T>::setSize(int n, const T& fill)
{
	if(n>m_size)
	{
		int oldsize = m_size;
		reserve(n);
		for(int i = oldsize; i < n; ++i)
		{
			m_data[i] = fill;
		}
	}
	m_size = n;
}

/// See also getSubarray() and the constructor which uses and existing
/// C style array in place.
template <typename T>
void hkArray<T>::insertAt(int index, const hkArray<T>& array)
{
	HK_ASSERT( index <= m_size );
	const int numtoinsert= array.getSize();
	const int newsize = numtoinsert + m_size;
	const int numtomove = m_size - index;
	if(newsize > getCapacity())
	{
		// note double copy from [i:end] not a problem in practice
		reserve(newsize);
	}
	copyBackwards(m_data + index + numtoinsert, m_data + index, numtomove);
	copyBackwards(m_data + index,               &array[0],      numtoinsert);
	m_size = newsize;
}


/// See also getSubarray() and the constructor which uses an existing
/// C style array in place.
template <typename T>
inline T* hkArray<T>::expandAt(int index, int numtoinsert)
{
	HK_ASSERT( index <= m_size );

	const int newsize = numtoinsert + m_size;
	const int numtomove = m_size - index;
	if(newsize > getCapacity())
	{
		// note double copy from [i:end] not a problem in practice
		reserve(newsize);
	}
	copyBackwards(m_data + index + numtoinsert, m_data + index, numtomove);
	m_size = newsize;
	return m_data + index;
}



/// Elements from i to the end are copied up one place.
template <typename T>
void hkArray<T>::insertAt(int i, const T& t)
{
	insertAt(i, hkArray( const_cast<T*>(&t), 1, 1) );
}

template <typename T>
void hkArray<T>::swap(hkArray<T>& a)
{
	hkAlgorithm::swap( m_data, a.m_data );
	hkAlgorithm::swap( m_size, a.m_size);
	hkAlgorithm::swap( m_capacityAndDeallocateFlag, a.m_capacityAndDeallocateFlag );
}


template <typename T, unsigned N>
inline hkInplaceArray<T,N>::hkInplaceArray(int size)
	: hkArray<T>(m_storage, size, N)
{
}

template <typename T, unsigned N>
inline hkInplaceArrayAligned16<T,N>::hkInplaceArrayAligned16(int size)
	: hkArray<T>( (T*)&m_storage[0], size, N)
{
}


template <typename T>
typename hkArray<T>::iterator hkArray<T>::begin() 
{
	return m_data;
}


template <typename T>
typename hkArray<T>::iterator hkArray<T>::end()
{
	return m_data + m_size;
}


template <typename T>
typename hkArray<T>::const_iterator hkArray<T>::begin() const
{
	return m_data;
}


template <typename T>
typename hkArray<T>::const_iterator hkArray<T>::end() const
{
	return m_data + m_size;
}


template <typename T>
hkOstream& operator<< (hkOstream& os, const hkArray<T>& a)
{
	os << "#[";
	for(int i=0; i < a.getSize(); ++i)
	{
		os << a[i] << ' ';
	}
	os << "]";
	return os;
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
