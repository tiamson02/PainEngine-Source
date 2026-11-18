// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/04 Bart Sekura
//----------------------------------------------------------------------------

// NOTE: Array assumes the elements are copyable
//       i.e. they don't depend on being at a specified memory location
//       it uses realloc mechanism, which moves memory when growing

#pragma once

class FileObject;


#include "Debug.h"
#include "Template.h"
#include "TypeTraits.h"

template <class T>
class DynamicArray
{
	typedef Int2Type<ComplexType<T>::Value> IsComplexType;

public:
	DynamicArray();
//	DynamicArray(bool aligned16 = false);
	DynamicArray(int size);
	DynamicArray(const DynamicArray& a);
	~DynamicArray();

	DynamicArray& operator=(const DynamicArray& a);

	inline int  Grow  (const int count=1);
	inline void Clear (const int size=0);
	inline void Shrink(const int size);
    inline void Resize(const int size);
	inline void FastClear();
	inline void Append(const DynamicArray& a);

	inline T*	 Create();
	inline int  Add(const T& e);
    inline int  AddUnique(const T& e);
    inline int  Find(const T& e);
	inline void Insert(const T& e, const int index);
    inline void Remove(const int index);

	inline T& operator[](const int index) const 
    { 
        Assert(index>=0);
        Assert(index<mSize);

        return mElems[index]; 
    }

	int Size() const { return mSize; }
	int MemoryTaken() const { return mSizeAllocated*sizeof(T)+sizeof(DynamicArray<T>); }
	T *Elements() const { return mElems; }

	// Added by Krystian - start - probably not working when IsComplexType!
	inline void FastRemove( const int index );
	inline void Swap( const int index1, const int index2 );
	inline void RemoveLast( void ) { if( mSize ) mSize--; };
	// Added by Krystian - end

    // serialization
	friend FileObject& operator<<(FileObject& file, const DynamicArray<T>& array)
    {
        file << array.mSize;
        if(array.mSize>0)
        {
            array.SaveContents(file,IsComplexType());
        }

        return file;
    }

	friend FileObject& operator>>(FileObject& file, DynamicArray<T>& array)
    {
        int size;
        file >> size;
        if(size>0)
        {
            array.Resize(size);
            array.LoadContents(file,IsComplexType());
        }

        return file;
    }

protected:
	inline void ResizeBuffer();

private:
	inline void ResizeNoInit(const int size);

    inline void InitElements(TrueType);
    inline void InitElements(FalseType);

	inline void FreeElements(TrueType);
	inline void FreeElements(FalseType);

	inline void CopyElements(const DynamicArray<T>& a, TrueType);
	inline void CopyElements(const DynamicArray<T>& a, FalseType);

	inline void AppendElements(const DynamicArray<T>& a, int offset, TrueType);
	inline void AppendElements(const DynamicArray<T>& a, int offset, FalseType);

	inline void RemoveElement(const int index, TrueType);
	inline void RemoveElement(const int index, FalseType);

    // serialization internal
    inline void SaveContents(FileObject& file, TrueType) const;
    inline void SaveContents(FileObject& file, FalseType) const;
    inline void LoadContents(FileObject& file, TrueType);
    inline void LoadContents(FileObject& file, FalseType);

protected:
	T*		mElems;
	int		mSize;
	int		mSizeAllocated;
//	bool	mAligned16;
};

template <class T>
DynamicArray<T>::DynamicArray()
:	mElems(NULL), mSize(0), mSizeAllocated(0)//, mAligned16(false)
{
}

/*
template <class T>
DynamicArray<T>::DynamicArray(bool aligned16)
:	mElems(NULL), mSize(0), mSizeAllocated(0), mAligned16(aligned16)
{
}
*/

template <class T>
DynamicArray<T>::DynamicArray(int size)
:	mElems(NULL), mSize(size), mSizeAllocated(size)//, mAligned16(false)
{
	ResizeBuffer();
	InitElements(IsComplexType());
}

template <class T>
DynamicArray<T>::DynamicArray(const DynamicArray<T>& a)
:	mElems(NULL), mSize(a.mSize), mSizeAllocated(a.mSizeAllocated)//, mAligned16(false)
{
	ResizeBuffer();
	CopyElements(a,IsComplexType());
}

template <class T>
DynamicArray<T>::~DynamicArray()
{

	FreeElements(IsComplexType());
	if (mElems)
	{
		#ifdef MEMORY_TEST 
		free(mElems);
		#else
		m_free_a16(mElems);
		#endif
	}
	//mSizeAllocated=0;
//	if(mAligned16)
		//GMem.FreeAligned(mElems);
//	else
//		GMem.Free(mElems);
}

template <class T>
DynamicArray<T>& DynamicArray<T>::operator=(const DynamicArray<T>& a)
{
	if(this != &a) {
		ResizeNoInit(a.mSize);
		CopyElements(a,IsComplexType());
	}
	return *this;
}

template <class T>
inline int DynamicArray<T>::Grow(const int count)
{
    Assert(count>0);
    const int index=mSize;
	mSize+=count;
	if(mSize>mSizeAllocated) {
#ifdef TRANSGAMING
        if (mSizeAllocated < 16)
            mSizeAllocated = 16;
#else
		mSizeAllocated = Max(mSizeAllocated,16);
#endif

        while (mSizeAllocated < mSize) {
			mSizeAllocated<<=1;
		}
		ResizeBuffer();
	}

    return index;
}

template <class T>
inline void DynamicArray<T>::Clear(const int size)
{
	FreeElements(IsComplexType());
	mSizeAllocated=size;
	ResizeBuffer();
    mSize=0;
}

template <class T>
inline void DynamicArray<T>::Shrink(const int size)
{
	Assert(size<=mSize);
	mSize=size;
	// TODO: shrink actual memory usage
}

template <class T>
inline void DynamicArray<T>::FastClear()
{
	FreeElements(IsComplexType());
	mSize = 0;
}

template <class T>
inline void DynamicArray<T>::Append(const DynamicArray& a)
{
    if(a.Size()) {
		int growSize = a.Size();
		int origsize = Size();

		// resize buffer
		Grow(growSize);

		AppendElements(a, origsize, IsComplexType());
    }
}

template <class T>
inline void DynamicArray<T>::Resize(const int size)
{
    Clear(size);
	if(size>0)
	{
		mSize=size;
		InitElements(IsComplexType());
	}
}

template <class T>
inline void DynamicArray<T>::ResizeNoInit(const int size)
{
    Clear(size);
	if(size>0)
		mSize=size;
}

template <class T>
inline T* DynamicArray<T>::Create()
{
	return new(&mElems[Grow()]) T;
}

template <class T>
inline int DynamicArray<T>::Add(const T& e)
{
	const int index=Grow();
    new (&mElems[index]) T(e);
	return index;
}

template <class T>
inline int DynamicArray<T>::AddUnique(const T& e)
{
	for(int index=0; index<mSize; ++index) {
		if(mElems[index]==e) {
			return index;
		}
	}

    return Add(e);
}

template <class T>
inline int DynamicArray<T>::Find(const T& e)
{
	for(int index=0; index<mSize; ++index)
	{
		if(mElems[index]==e)
		{
			return index;
		}
	}

    return -1;
}

/* Added by Krystian */
/* Probably unusable when IsComplexType */
/* Krystian Start */

template <class T>
inline void DynamicArray<T>::FastRemove( const int indexToRemove )
{
	if( indexToRemove >= mSize ) return;
	mElems[indexToRemove] = mElems[mSize-1];
	mSize--;
}

template <class T>
inline void DynamicArray<T>::Swap( const int index1, const int index2 )
{
	T temp;
	temp = mElems[index1];
	mElems[index1] = mElems[index2];
	mElems[index2] = temp;
}

/* Krystian End */

template <class T>
inline void DynamicArray<T>::Insert(const T& e, const int index)
{
    Assert(index>=0);
    Assert(index<=mSize);
	if(index==mSize) {
		Add(e);
	}
	else {
        // Grow() modifies mSize, so first compute memblock size to move
		const int sz = (mSize-index)*sizeof(T);
		Grow();
		::memmove(mElems + index + 1, mElems + index, sz);
		mElems[index]=e;
	}
}

template <class T>
inline void DynamicArray<T>::Remove(const int index)
{
    Assert(index>=0);
    Assert(index<mSize);
    RemoveElement(index,IsComplexType());
    ::memmove(mElems + index, mElems + index + 1, (mSize-index-1)*sizeof(T));
    --mSize;
}

template <class T>
inline void DynamicArray<T>::ResizeBuffer()
{
	int size = mSizeAllocated*sizeof(T);
	if( mElems )
	{
		if( !size )
		{
			#ifdef MEMORY_TEST 
			free(mElems);
			#else
			m_free_a16(mElems);
			#endif
			
			mElems = 0;
		}
		else
		{
			#ifdef MEMORY_TEST 
			mElems = reinterpret_cast<T*> (realloc(mElems,size));
			#else
			mElems = reinterpret_cast<T*> (m_realloc_a16(mElems,size));
			#endif
			Assert(mElems);
		}
	}
	else if( size )
	{
		#ifdef MEMORY_TEST 
		mElems = reinterpret_cast<T*> (malloc(size));
		#else
		mElems = reinterpret_cast<T*> (m_malloc_a16(size));
		#endif
		Assert(mElems);
	}

//	if(mAligned16)
//		mElems=reinterpret_cast<T*>(GMem.ReAllocAligned(mElems,mSizeAllocated*sizeof(T),16));
//	else
//		mElems=reinterpret_cast<T*>(GMem.ReAlloc(mElems,mSizeAllocated*sizeof(T)));
}

template <class T>
inline void DynamicArray<T>::InitElements(TrueType /* IsComplexType */)
{
	for(int i=0; i<mSize; ++i)
		new (&mElems[i]) T;
}

template <class T>
inline void DynamicArray<T>::InitElements(FalseType /* IsComplexType */)
{
}

template <class T>
inline void DynamicArray<T>::FreeElements(TrueType /* IsComplexType */)
{
	for(int i=0; i<mSize; ++i) {
		mElems[i].~T();
	}
}

template <class T>
inline void DynamicArray<T>::FreeElements(FalseType /* IsComplexType */)
{
}

template <class T>
inline void DynamicArray<T>::CopyElements(const DynamicArray<T>& a, TrueType /* IsComplexType */)
{
	for(int i=0; i<mSize; ++i) {
        new (&mElems[i]) T(a[i]);
	}
}

template <class T>
inline void DynamicArray<T>::CopyElements(const DynamicArray<T>& a, FalseType /* IsComplexType */)
{
	::memcpy(mElems,a.mElems,mSize*sizeof(T));
}

template <class T>
inline void DynamicArray<T>::AppendElements(const DynamicArray<T>& a, int offset, TrueType /* IsComplexType */)
{
	for(int i=0; i<a.mSize; ++i) {
        new (&mElems[i+offset]) T(a[i]);
	}
}

template <class T>
inline void DynamicArray<T>::AppendElements(const DynamicArray<T>& a, int offset, FalseType /* IsComplexType */)
{
	::memcpy(&mElems[offset],a.mElems,a.mSize*sizeof(T));
}

template <class T>
inline void DynamicArray<T>::RemoveElement(const int index, TrueType /* IsComplexType */)
{
    mElems[index].~T();
}

template <class T>
inline void DynamicArray<T>::RemoveElement(const int /*index*/, FalseType /* IsComplexType */)
{
}

template <class T>
inline void DynamicArray<T>::SaveContents(FileObject& file, TrueType) const
{
    for(int i=0; i<mSize; ++i)
        file << mElems[i];
}

template <class T>
inline void DynamicArray<T>::SaveContents(FileObject& file, FalseType) const
{
    file.Write(&mElems[0],mSize*sizeof(T));
}

template <class T>
inline void DynamicArray<T>::LoadContents(FileObject& file, TrueType)
{
    for(int i=0; i<mSize; ++i)
        file >> mElems[i];
}

template <class T>
inline void DynamicArray<T>::LoadContents(FileObject& file, FalseType)
{
    file.Read(&mElems[0],mSizeAllocated*sizeof(T));
}

// ==============================================================

template <class T>
class PtrArray : public DynamicArray<T*>
{
public:
	PtrArray() : DynamicArray<T*>() {}
	~PtrArray()
	{
		for(int i=0; i<mSize; ++i)
			delete mElems[i];
	}
};

// ==============================================================

template <class T>
inline int SlotAdd(DynamicArray<T*>& array, T* t)
{
	for (int i=0; i<array.Size(); ++i)
		if (!array[i])
		{
			array[i]=t;
			return i;
		}

	return array.Add(t);
}
