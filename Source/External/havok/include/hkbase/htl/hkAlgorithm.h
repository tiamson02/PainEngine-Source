/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HKBASE_HKALGORITHM_H
#define HKBASE_HKALGORITHM_H

#include <hkbase/hkBase.h>

namespace hkAlgorithm
{

	/// swap the elements
	template<typename T>
	inline void HK_CALL swap(T& x, T& y)
	{
		T t(x);
		x = y;
		y = t;
	}


	/// swap the 16 byte aligned elements
	template<typename T>
	inline void HK_CALL swap16(T& x, T& y)
	{
		T HK_ALIGNED_VARIABLE(t,16) = x;
		x = y;
		y = t;
	}


	/// function object that routes calls to operator<
	template<typename T>
	class less
	{
		public:

			bool operator() ( const T& a, const T& b )
			{
				return ( a < b );
			}
	};


	/// heap sort.  you supply the functor, see hkAlgorithm::less for an example functor.
	template<typename T, typename L>
	void HK_CALL heapSort(T *pArr, int iSize, L cmpLess);


	/// Heap sort for the elements of the specified array.
	///
	/// \param *pArr		A pointer to the array to sort.
	/// \param iSize		The size of the array pointed to by *pArr.
	///
	template<typename T>
	void HK_CALL heapSort(T *pArr, int iSize)
	{
		heapSort( pArr, iSize, less<T>() );
	}

	// used by heapSort
	template<typename T, typename L>
	void HK_CALL downHeap(T *pArr, int k, int n, L cmpLess);


	/// quick sort.  you supply the functor, see hkAlgorithm::less for an example functor.
	template<typename T, typename L>
	void HK_CALL quickSort(T *pArr, int iSize, L cmpLess);


	/// Quick sort for the elements of the specified array.
	///
	/// \param *pArr		A pointer to the array to sort.
	/// \param iSize		The size of the array pointed to by *pArr.
	///
	template<typename T>
	void HK_CALL quickSort(T *pArr, int iSize)
	{
		quickSort( pArr, iSize, less<T>() );
	}

	template< typename T, typename L >
	void HK_CALL quickSortRecursive(T *pArr, int d, int h, L cmpLess);


}

#ifdef HK_PS2
	#define hkSort hkAlgorithm::heapSort
#else
	#define hkSort hkAlgorithm::quickSort
#endif



#include <hkbase/htl/hkAlgorithm.inl>

#endif // HKBASE_HKALGORITHM_H

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
