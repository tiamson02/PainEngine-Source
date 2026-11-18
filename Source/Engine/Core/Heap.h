// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/07 Krystian Galaj
//----------------------------------------------------------------------------

// #pragma once
#ifndef _HEAP_
#define _HEAP_


// Implementing priority queue in heap - to be used in A-Star algorithm for pathfinding
// Should be very fast, I hope.

template<class T>
class Heap
{
public:
	Heap(void){};
	~Heap(void){};

	void Insert( const T& element );
	T RemoveNext( void );
	int Size( void ) { return mElements.Size(); };
	T& operator[](const int index) const { return mElements[index]; }
    void Resize(const int count) { mElements.ResizeArray(count); }

protected:
	int Father( int index ) { return (index-1)>>1; };
	int LeftSon( int index ) { return (index<<1)+1; };
	int RightSon( int index ) { return (index<<1)+2; };

	// Variables
	DynamicArray<T> mElements;
};

template<class T>
void Heap<T>::Insert( const T& element )
{
	// add element at the end of queue
	int index = mElements.Add(element);

	while( ( index != 0 ) && ( mElements[index] < mElements[Father(index)] ) ) {
		mElements.Swap( index, Father(index) );
		index = Father(index);
	};
}

// Vulnerable when no elements inside - check it first!!!

template<class T>
T Heap<T>::RemoveNext( void )
{
	T toGive = mElements[0];
	mElements.Swap( 0, mElements.Size() - 1 );
	mElements.RemoveLast();

	int index = 0;
	int largest = LeftSon(index);
	int size = mElements.Size();
	while( largest <= size ) {
		if( largest < size )	// two sons
			if( mElements[largest] > mElements[largest+1] )	// choose smaller son
				largest++;

		if( mElements[index] > mElements[largest] ) {
			mElements.Swap( index, largest );
			index = largest;
			largest = LeftSon( index );
		} else
			largest = size + 1;	// to exit the loop
	};

	return toGive;
}

#endif //_HEAP_

