/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


template <typename T>
inline void hkObjectArray<T>::construct(T* t, int n)
{
	for(int i = 0; i < n; ++i)
	{
		new (t+i) T;
	}
}

template <typename T>
inline void hkObjectArray<T>::construct(T* t, int n, const T& tcopy)
{
	for(int i = 0; i < n; ++i)
	{
		new (t+i) T(tcopy);
	}
}

template <typename T>
inline void hkObjectArray<T>::destruct(T* t, int n)
{
	for(int i = 0; i < n; ++i)
	{
		t[i].~T();
	}
}


template <typename T>
inline hkObjectArray<T>::hkObjectArray()
{
}

template <typename T>
inline hkObjectArray<T>::hkObjectArray( int n )
	:	m_array(n)
{
	construct( m_array.begin(), n);
}

template <typename T>
inline hkObjectArray<T>::hkObjectArray( int n, const T& tinit )
	: m_array( n )
{
	construct( m_array.begin(), n, tinit);
}

template <typename T>
inline hkObjectArray<T>::hkObjectArray( const hkObjectArray& a )
	:	m_array( a.getSize() )
{
	T* t = m_array.begin();
	for(int i = 0; i < a.getSize(); ++i)
	{
		new (t+i) T(a[i]);
	}
}

template <typename T>
inline hkObjectArray<T>& hkObjectArray<T>::operator= ( const hkObjectArray<T>& a )
{
	if( a.getSize() > m_array.getCapacity() )
	{
		// need to reallocate, destroy the entire array
		// and start from scratch
		destruct( m_array.begin(), m_array.getSize() );
		m_array.setSize(a.getSize());
		T* t = m_array.begin();
		for( int i = 0; i < a.getSize(); i++ )
		{
			new (t + i) T( a[i]);
		}
	}
	else if( a.getSize() > m_array.getSize() )
	{
		// have the capacity, other is bigger
		// copy as many as possible, construct the end ones.
		int i;
		for(i = 0; i < m_array.getSize(); ++i)
		{
			m_array[i] = a[i];
		}
		T* t = m_array.begin();
		for( ; i < a.getSize(); ++i)
		{
			new (t + i) T( a[i]);
		}
	}
	else //( a.getSize() < m_array.getSize() )
	{
		// have the capacity, i am bigger
		// copy as many as possible, destroy the end ones.
		int i;
		for(i = 0; i < a.getSize(); ++i)
		{
			m_array[i] = a[i];
		}
		for( ; i < m_array.getSize(); ++i)
		{
			m_array[i].~T();
		}
	}
	return *this;
}


template <typename T>
inline hkObjectArray<T>::~hkObjectArray()
{
	for( int i = 0; i < m_array.getSize(); ++i)
	{
		m_array[i].~T();
	}
}



template <typename T>
inline T& hkObjectArray<T>::operator[] (int i)
{
	HK_ASSERT(i<m_array.getSize());
	return m_array[i];
}

template <typename T>
inline const T& hkObjectArray<T>::operator[] (int i) const
{
	HK_ASSERT(i<m_array.getSize());
	return m_array[i];
}

template <typename T>
inline int hkObjectArray<T>::getSize() const
{
	return m_array.getSize();
}

template <typename T>
inline int hkObjectArray<T>::getCapacity() const
{
	return m_array.getCapacity();
}

template <typename T>
inline bool hkObjectArray<T>::isEmpty() const
{
	return 0 == getSize();
}

template <typename T>
inline void hkObjectArray<T>::clear()
{
	destruct( m_array.begin(), m_array.getSize() );
	m_array.clear();
}

template <typename T>
inline void hkObjectArray<T>::clearAndDeallocate()
{
	destruct( m_array.begin(), m_array.getSize() );
	m_array.clearAndDeallocate();
}



/// This is done by moving the last element into index i
/// and resizing by -1. This is very fast, but note that
/// the order of elements is changed.
template <typename T>
inline void hkObjectArray<T>::removeAt(int i)
{
	int s = m_array.getSize() - 1;
	m_array[i] = m_array[s];
	m_array[s].~T();
	m_array.popBack();
}

/// This is done by copying elements down to fill the empty
/// space. Slower than removeAt, but the order is unchanged.
template <typename T>
inline void hkObjectArray<T>::removeAtAndCopy(int index)
{
	int s = m_array.getSize() - 1;
	for( int i = index; i < s; i++ )
	{
		m_array[i] = m_array[i+1];
	}
	m_array[s].~T();
	m_array.setSizeUnchecked(s);
}

template <typename T>
inline int hkObjectArray<T>::indexOf(const T& t) const
{
	return m_array.indexOf(t);
}

template <typename T>
inline int hkObjectArray<T>::lastIndexOf(const T& t) const
{
	return m_array.lastIndexOf(t);
}

template <typename T>
inline void hkObjectArray<T>::popBack()
{
	m_array[m_array.getSize()-1].~T();
	m_array.popBack();
}

template <typename T>
inline void hkObjectArray<T>::reserve( int n )
{
	if( n > m_array.getCapacity() )
	{
		int cap2 = 2 * m_array.getCapacity();
		int newSize = (n < cap2) ? cap2 : n;

		hkObjectArray old;
		old.swap(*this);
		int oldsize = old.getSize();
		m_array.reserve(newSize);
		T* p = m_array.begin();
		int i;
		for( i = 0; i < oldsize; ++i )
		{
			new (p + i) T( old[i] );
		}

	}
	else // we have enough capacity
	{
		T* p = m_array.begin();
		for( int i = m_array.getSize(); i < n; ++i )
		{
			new (p + i) T();
		}
	}
}


template <typename T>
inline void hkObjectArray<T>::pushBack( const T& t )
{
	int size = m_array.getSize();
	if( size == m_array.getCapacity() )
	{
		m_array.reserve( size ? size*2 : 1 );
	}
	m_array.setSizeUnchecked(size+1);
	new (&m_array[size]) T(t);	
}




/// If the array is expanded
template <typename T>
inline void hkObjectArray<T>::setSize( int n )
{
	int mysize = m_array.getSize();
	if( n < mysize )
	{
		for( int i = n; i < mysize; i++ )
		{
			m_array[i].~T();
		}
	}
	else
	{
		reserve( n );
		T* p = m_array.begin();
		for( int i = mysize; i < n; ++i )
		{
			new (p + i) T();
		}
	}
	m_array.setSizeUnchecked( n );
}


/// increments the size by n and returns a reference to the first element created
template <typename T>
inline T& hkObjectArray<T>::expandBy( int n )
{
	int oldsize = m_array.getSize();
	setSize( oldsize + n );
	return (*this)[oldsize];
}



/// If the array is expanded, new elements initialized with 'fill'.
template <typename T>
inline void hkObjectArray<T>::setSize( int n, const T& fill )
{

	int oldsize = m_array.getSize();
	setSize( n );
	if( oldsize < n )
	{
		for( int i = oldsize; i < n; i++ )
		{
			m_array[i] = fill;
		}
	}
}


template <typename T>
void hkObjectArray<T>::swap(hkObjectArray<T>& a)
{
	m_array.swap(a.m_array);
}
 

template <typename T>
typename hkObjectArray<T>::iterator hkObjectArray<T>::begin() 
{
	return m_array.begin();
}


template <typename T>
typename hkObjectArray<T>::iterator hkObjectArray<T>::end()
{
	return m_array.begin() + m_array.getSize();
}


template <typename T>
typename hkObjectArray<T>::const_iterator hkObjectArray<T>::begin() const
{
	return m_array.begin();
}


template <typename T>
typename hkObjectArray<T>::const_iterator hkObjectArray<T>::end() const
{
	return m_array.begin() + m_array.getSize();
}


template <typename T>
hkOstream& operator<< (hkOstream& os, const hkObjectArray<T>& a)
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
