/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

// Heap Sort, O(n log(n)), not recursive
// see http://www.csse.monash.edu.au/~lloyd/tildeAlgDS/Sort/Heap.html for explanation

namespace hkAlgorithm
{

template<typename T, typename L>
void HK_CALL downHeap(T *pArr, int k, int n, L cmpLess)
{
	// precondition: array pArr[k+1,n] is a heap
	// postcondition: array pArr[k,n] is a heap

	T temp = pArr[k - 1];
	//find them maximum of the new 'parent' and all of its children
	while (k <= n/2)
	{
		int child = 2*k;
		//don't process children 'outside' of the array
		if ((child < n) && cmpLess(pArr[child - 1], pArr[child]))
		{
			child++;
		}
		//take the maximum
		if (cmpLess(temp, pArr[child - 1]))
		{
			pArr[k - 1] = pArr[child - 1];
			k = child;
		}
		else
		{
			break;
		}
	}
	pArr[k - 1] = temp;
}


template<typename T, typename L>
void HK_CALL heapSort(T *pArr, int iSize, L cmpLess)
{
	int k;
	int n = iSize;
	//create a heap
	for (k = n/2; k > 0; k--)
	{
	    downHeap(pArr, k, n, cmpLess);
    }

	// now swap the
	while ( n>=1 )
	{
		//a[0] is now known to be largest, so put it to the end, and downheap 0..n-1
		T temp = pArr[0];
		pArr[0] = pArr[n - 1];
		pArr[n - 1] = temp;

		n = n - 1;
		downHeap(pArr, 1, n, cmpLess);
	}
}



template<typename T, typename L>
void HK_CALL quickSortRecursive(T *pArr, int d, int h, L cmpLess)
{
	int i,j;
	T str;
begin:
	i = h;
	j = d;

	str = pArr[(d+h)>>1];

	do {

		while ( cmpLess( pArr[j], str) ) j++;
		while ( cmpLess( str, pArr[i]) ) i--;

		if ( i >= j )
		{
			if ( i != j )
			{
				T zal;

				zal = pArr[i];
				pArr[i] = pArr[j];
				pArr[j] = zal;

			}

			i--;
			j++;
		}
	} while (j <= i);

	if (d < i)
	{
		quickSortRecursive(pArr,d,i, cmpLess);
	}

	if (j < h)
	{
		d = j;
		goto begin;
	}
}

template<typename T, typename L>
void HK_CALL quickSort(T *pArr, int iSize, L cmpLess)
{
	if (iSize > 1)
	{
		int	low = 0;
		int high = iSize - 1;
		quickSortRecursive(pArr,low,high, cmpLess);
	}
}




}  // namespace hkAlgorithm

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
