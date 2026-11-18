// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/07 Krystian Galaj
//----------------------------------------------------------------------------

// #pragma once
#ifndef _POINTERHEAP_
#define _POINTERHEAP_


#include "Engine.h"

// Implementing priority queue in heap - to be used in A-Star algorithm for pathfinding
// Should be very fast, I hope.

template<class T>
class PointerHeap
{
public:
	PointerHeap(void)
	{
	};

	~PointerHeap(void)
	{
	};

	void Insert( T* element );
	T* Remove( int ind = 0 );

	T* Remove( T* element )
	{
		for( int i = 0; i < mElements.Size(); i++ )
		{
			if( element == mElements[i] )
			{
				return Remove(i);
			};
		};
		return 0;
	}

	int Size( void )
	{
		return mElements.Size();
	};

	T* operator[]( const int index ) const
	{
		return mElements[index];
	};

    void Resize( const int count )	// use only at the beginning
	{
		mElements.ResizeArray(count);
	};

	void ClearContents( void );

protected:
	int Father( int index )
	{
		return (index-1)>>1;
	};

	int LeftSon( int index )
	{
		return (index<<1)+1;
	};

	int RightSon( int index )
	{
		return (index<<1)+2;
	};

	// Variables
	DynamicArray<T*> mElements;
};

template<class T>
void PointerHeap<T>::Insert( T* element )
{
	// add element at the end of queue
	mElements.Add( element );
	int index = mElements.Size()-1;

	while( ( index != 0 ) && ( *mElements[index] < *mElements[Father(index)] ) )
	{
		mElements.Swap( index, Father(index) );
		index = Father(index);
	};
}

// Vulnerable when no elements inside - check it first!!!

template<class T>
T* PointerHeap<T>::Remove( int ind )
{
	T* toGive = mElements[ind];
	mElements.Swap( ind, mElements.Size() - 1 );
	mElements.RemoveLast();

	int index = ind;
	int largest = LeftSon(index);
	int size = mElements.Size();
	while( largest < size )
	{
		if( largest < size-1 )	// two sons
		{
			if( *mElements[largest] > *mElements[largest+1] )	// choose smaller son
			{
				largest++;
			};
		};

		if( *mElements[index] > *mElements[largest] )
		{
			mElements.Swap( index, largest );
			index = largest;
			largest = LeftSon( index );
		}
		else
		{
			largest = size + 1;	// to exit the loop
		};
	};

	return toGive;
}

// Use if you want to delete the elements pointed to by heap before deleting heap

template<class T>
void PointerHeap<T>::ClearContents( void )
{
	for( int i = 0; i < mElements.Size(); i++ )
	{
		delete mElements[i];
	};
}



#endif //_POINTERHEAP_

