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

// #pragma once
#ifndef _DYNARRAY_
#define _DYNARRAY_


class FileObject;


#include "Core/Debug.h"
#include "Core/Template.h"
#include "Core/TypeTraits.h"
#include "Core/MemoryManager.h"
//extern ENGINE_API MemoryManagerStd	GMem;
#include "FileSystem/GFile.h"


#if defined(TRANSGAMING) && defined(DEBUG)
# define DYNARRAY_TRACK_CALLS
# define ResizeArray(s)     ResizeArrayM(s, __FUNCTION__)
# define GrowArray(s)       GrowArrayM(s,   __FUNCTION__)
# define ClearArray()       ClearArrayM(0,  __FUNCTION__)
# define CreateArray()      BAD_FUNCTION!        //CreateArrayM(__FILE__, __LINE__)
#endif

template <class T>
class DynamicArray
{
#ifdef DYNARRAY_TRACK_CALLS
public:
#endif
	typedef Int2Type<ComplexType<T>::Value> IsComplexType;

public:
	DynamicArray();
//	DynamicArray(bool aligned16 = false);
#ifdef DYNARRAY_TRACK_CALLS
	DynamicArray(int size, const char *msg = NULL);
    virtual ~DynamicArray();
#else
	DynamicArray(int size);
    ~DynamicArray();
#endif
	DynamicArray(const DynamicArray& a);
    
	DynamicArray& operator=(const DynamicArray& a);

#ifdef DYNARRAY_TRACK_CALLS
	inline int  GrowArrayM  (const int count, const char *msg = NULL);
	inline void ClearArrayM (const int size,  const char *msg = NULL);
    inline void ResizeArrayM(const int size,  const char *msg = NULL);
	//inline T*	CreateArrayM(const char *file, int line);
#else
	inline int  GrowArray  (const int count = 1);
	inline void ClearArray (const int size = 0);
    inline void ResizeArray(const int size);
# ifndef TRANSGAMING
	inline T*	CreateArray();
# endif
#endif

#ifdef TRANSGAMING
    inline void Expand(const int size);
#endif
	inline void Shrink(const int size);
	inline void FastClear();
	inline void Append(const DynamicArray& a);

	inline int  Add(const T& e);
    inline int  AddUnique(const T& e);
    inline int  Find(const T& e);
	inline void Insert(const T& e, const int index);
    inline void Remove(const int index);

	inline T& operator[](const int index) const 
    { 
#if defined(TRANSGAMING) && defined(DEBUG)
        if (index < 0){
            TRACE("DynamicArray::operator[](): ERROR-> index past lower bound! (index = %d, size = %d)\n", index, mSize);
            HALT();
        }
        else if (index >= mSize){
            TRACE("DynamicArray::operator[](): ERROR-> index past upper bound! (index = %d, size = %d)\n", index, mSize);
            HALT();
        }
#else
        Assert(index>=0);
        Assert(index<mSize);
#endif

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
#ifdef DYNARRAY_TRACK_CALLS
            //printf("DynamicArray::operator>>(): buh??\n");
            array.ResizeArrayM(size, "FileObject &DynArray::operator>>()");
#else
            array.ResizeArray(size);
#endif
            array.LoadContents(file,IsComplexType());
        }

        return file;
    }

protected:
#ifdef DYNARRAY_TRACK_CALLS
	inline void ResizeBuffer(const char *msg);
#else
	inline void ResizeBuffer();

private:
#endif
#ifdef DYNARRAY_TRACK_CALLS
	inline void ResizeNoInit(const int size, const char *msg);
#else
	inline void ResizeNoInit(const int size);
#endif

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
#ifdef DYNARRAY_TRACK_CALLS
DynamicArray<T>::DynamicArray(int size, const char *msg)
#else
DynamicArray<T>::DynamicArray(int size)
#endif
:	mElems(NULL), mSize(size), mSizeAllocated(size)//, mAligned16(false)
{
#ifdef DYNARRAY_TRACK_CALLS
    ResizeBuffer(msg ? msg : "[Util]::DynamicArray::DynamicArray(int)");
#else
    ResizeBuffer();
#endif
	InitElements(IsComplexType());
}

template <class T>
DynamicArray<T>::DynamicArray(const DynamicArray<T>& a)
:	mElems(NULL), mSize(a.mSize), mSizeAllocated(a.mSizeAllocated)//, mAligned16(false)
{
#ifdef DYNARRAY_TRACK_CALLS
    ResizeBuffer("[Util]::DynamicArray::DynamicArray(const DynamicArray<> &)");
#else
    ResizeBuffer();
#endif
	CopyElements(a,IsComplexType());
}

template <class T>
DynamicArray<T>::~DynamicArray()
{

	FreeElements(IsComplexType());
	if (mElems)
	{
		//#ifdef MEMORY_TEST 
		free(mElems);
		//#else
		//m_free_a16(mElems);
		//#endif
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
#ifdef DYNARRAY_TRACK_CALLS
		ResizeNoInit(a.mSize, __FUNCTION__);
#else
		ResizeNoInit(a.mSize);
#endif
		CopyElements(a,IsComplexType());
	}
	return *this;
}

template <class T>
#ifdef DYNARRAY_TRACK_CALLS
inline int DynamicArray<T>::GrowArrayM(const int count, const char *msg)
#else
inline int DynamicArray<T>::GrowArray(const int count)
#endif
{
    Assert(count>0);
    const int index=mSize;
	mSize+=count;
	if(mSize>mSizeAllocated) {
		mSizeAllocated = Max(mSizeAllocated,16);
		while (mSizeAllocated < mSize) {
			mSizeAllocated<<=1;
		}

#ifdef DYNARRAY_TRACK_CALLS
        ResizeBuffer(msg ? msg : "[Util]::DynamicArray::Grow(const int)");
#else
        ResizeBuffer();
#endif
	}

    memset(&mElems[index], 0, sizeof(T) * count);

    return index;
}

#ifdef TRANSGAMING
template<class T>
inline void DynamicArray<T>::Expand(const int size){
    if (size <= mSizeAllocated)
        return;


    mSizeAllocated = size;

#ifdef DYNARRAY_TRACK_CALLS
    ResizeBuffer("[Util]::DynamicArray::Expand(const int)");
# else
    ResizeBuffer();
# endif

}
#endif



template <class T>
#ifdef DYNARRAY_TRACK_CALLS
inline void DynamicArray<T>::ClearArrayM(const int size, const char *msg)
#else
inline void DynamicArray<T>::ClearArray(const int size)
#endif
{
    //printf("DynamicArray::ClearArray(0x%X): clearing the array (msg = '%s', size = %d, mSize = %d, mElems = 0x%X, allocated = %d)\n", (unsigned int)this, msg, size, mSize, (unsigned int)mElems, mSizeAllocated);
    //m_logMsg("DynamicArray::ClearArray(0x%X): clearing the array (msg = '%s', size = %d, mSize = %d, mElems = 0x%X, allocated = %d)\n", (unsigned int)this, msg, size, mSize, (unsigned int)mElems, mSizeAllocated);
	FreeElements(IsComplexType());
	mSizeAllocated=size;
#ifdef DYNARRAY_TRACK_CALLS
    ResizeBuffer(msg ? msg : "[Util]::DynamicArray::ClearArray(const int)");
#else
    ResizeBuffer();
#endif
    mSize=0;
}

template <class T>
inline void DynamicArray<T>::Shrink(const int size)
{
	Assert(size <= mSize);

    //nothing to do => succeed
    if (size == mSize)
        return;


	mSize = size;

    //!!// this is bad to do - the memory allocations should be centralized in this class
    if (size == 0){
        free(mElems);

        mElems = NULL;
        mSizeAllocated = 0;
    }

    else{
        m_setTag("DynamicArray<T>::Shrink()");
        mElems = reinterpret_cast<T*>(realloc(mElems, mSize * sizeof(T)));
        mSizeAllocated = mSize;
    }
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
		GrowArray(growSize);

		AppendElements(a, origsize, IsComplexType());
    }
}

template <class T>
#ifdef DYNARRAY_TRACK_CALLS
inline void DynamicArray<T>::ResizeArrayM(const int size, const char *msg)
#else
inline void DynamicArray<T>::ResizeArray(const int size)
#endif
{
#ifdef DYNARRAY_TRACK_CALLS
    //printf("DynamicArray::ResizeArray(0x%X): clearing the array (msg = '%s', size = %d, mSize = %d, mElems = 0x%X, allocated = %d)\n", (unsigned int)this, msg, size, mSize, (unsigned int)mElems, mSizeAllocated);
    //m_logMsg("DynamicArray::ResizeArray(0x%X): clearing the array (msg = '%s', size = %d, mSize = %d, mElems = 0x%X, allocated = %d)\n", (unsigned int)this, msg, size, mSize, (unsigned int)mElems, mSizeAllocated);

    ClearArrayM(size, msg ? msg : "[Util]::DynamicArray<>::ResizeArray()");
#else
    ClearArray(size);
#endif
	if(size>0)
	{
		mSize=size;
		InitElements(IsComplexType());
	}
}

template <class T>
#ifdef DYNARRAY_TRACK_CALLS
inline void DynamicArray<T>::ResizeNoInit(const int size, const char *msg)
#else
inline void DynamicArray<T>::ResizeNoInit(const int size)
#endif
{
#ifdef DYNARRAY_TRACK_CALLS
    //printf("DynamicArray::ResizeNoInit(0x%X): clearing the array (msg = '%s', size = %d, mSize = %d, mElems = 0x%X, allocated = %d)\n", (unsigned int)this, msg, size, mSize, (unsigned int)mElems, mSizeAllocated);
    //m_logMsg("DynamicArray::ResizeNoInit(0x%X): clearing the array (msg = '%s', size = %d, mSize = %d, mElems = 0x%X, allocated = %d)\n", (unsigned int)this, msg, size, mSize, (unsigned int)mElems, mSizeAllocated);
    ClearArrayM(size, msg ? msg : __FUNCTION__);
#else
    ClearArray(size);
#endif
	if(size>0)
		mSize=size;
}

#ifndef TRANSGAMING
template <class T>
# ifdef DYNARRAY_TRACK_CALLS
inline T* DynamicArray<T>::CreateArrayM(const char *file, int line)
# else
inline T* DynamicArray<T>::CreateArray()
# endif
{
# ifdef DYNARRAY_TRACK_CALLS
    TRACE("CreateArray(): creating a new element in the array at %d (sizeof(T) = %d, source = '%s' (%d))\n", mSize, sizeof(T), file, line);
# endif
    return new(&mElems[GrowArray(1)]) T;
}
#endif

template <class T>
inline int DynamicArray<T>::Add(const T& e)
{
#ifdef DYNARRAY_TRACK_CALLS
    const int index = GrowArrayM(1, "DynamicArray<>::Add()");
#else
	const int index = GrowArray(1);
#endif
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
    RemoveElement(indexToRemove, IsComplexType());
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
		GrowArray(1);
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
#ifdef DYNARRAY_TRACK_CALLS
inline void DynamicArray<T>::ResizeBuffer(const char *msg = NULL)
#else
inline void DynamicArray<T>::ResizeBuffer()
#endif
{
	int size = mSizeAllocated*sizeof(T);
	if( mElems )
	{
		if( !size )
		{
			//#ifdef MEMORY_TEST 
			free(mElems);
			//#else
			//m_free_a16(mElems);
			//#endif
			
			mElems = 0;
		}
		else
		{
			//#ifdef MEMORY_TEST 
			mElems = reinterpret_cast<T*> (realloc(mElems,size));
			//#else
			//mElems = reinterpret_cast<T*> (m_realloc_a16(mElems,size));
			//#endif
			Assert(mElems);
		}
	}
	else if( size )
	{
		//#ifdef MEMORY_TEST 
#ifdef DYNARRAY_TRACK_CALLS
        if (msg)
            m_setTag(msg);

        else
            m_setTag("Util");
#endif
		mElems = reinterpret_cast<T*> (malloc(size));
		//#else
		//mElems = reinterpret_cast<T*> (m_malloc_a16(size));
		//#endif
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

#endif //_DYNARRAY_


