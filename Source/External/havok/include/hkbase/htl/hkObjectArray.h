/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HKBASE_HKOBJECTARRAY_H
#define HKBASE_HKOBJECTARRAY_H

/// The Havok object array.
/// Note that, for performance reasons, order may not be preserved when deleting elements.<br>
/// <br>
/// The following must be publicly accessible for each item in an object array:
/// <ul><li>default constructor, copy constructor, assignment, destructor
/// (either hand or compiler generated)</li>
/// <li>placement new, needed to construct objects in place. (You may need to use 
/// HK_DECLARE_CLASS_ALLOCATOR or HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR
/// for structs, nested classes, etc)</li></ul>
template <typename T>
class hkObjectArray
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_ARRAY, hkObjectArray<T>)

			/// Creates a zero length array.
		inline hkObjectArray();

			/// Creates an array of size n elements, with their default constructor.
		inline hkObjectArray(int n);

			/// Creates an array of n elements initialized to 'fill'.
		inline hkObjectArray(int n, const T& fill);

			/// Copies the array a.
		inline hkObjectArray(const hkObjectArray& a);

			/// Copies the array a.
		hkObjectArray& operator= (const hkObjectArray& a);

			/// Deallocates array memory.
		inline ~hkObjectArray();

			/// Read/write access to the i'th element.
		inline T& operator[] (int i);

			/// Read only access to the i'th element.
		inline const T& operator[] (int i) const;

			/// Returns the size of the array.
		inline int getSize() const;

			/// Returns the capacity of the array.
		int getCapacity() const;

			/// Checks if the size is zero.
		inline bool isEmpty() const;
		
			/// Sets the size to zero.
		inline void clear();

			/// Sets the size to zero and deallocate storage.
		void clearAndDeallocate();

			/// Removes the element at the specified index. The last array element is used to replace the removed element, and the size is reduced by 1.
		void removeAt(int index);

			/// Removes the element at the specified index, copying the elements down one slot.
		void removeAtAndCopy(int index);

			/// Returns the index of the first occurance of t, or -1 if not found.
		int indexOf(const T& t) const;

			/// Returns the index of the last occurance of t, or -1 if not found.
		int lastIndexOf(const T& t) const;
		
			/// Removes the last element.
		inline void popBack();

			/// Adds an element to the end.
		inline void pushBack(const T& e);

			/// Sets the capacity to n.
		inline void reserve(int n);

			/// Sets the size to n.
		inline void setSize(int n);

			/// Sets the size to n.
		void setSize(int n, const T& fill);

			/// Increments the size by n and returns a reference to the first element created
		inline T& expandBy( int n );

			/// Insert the array a at index i.
		//void insertAt(int i, const hkObjectArray<T>& a);

			/// Insert t at index i.
		//void insertAt(int i, const T& t);

			/// Swaps this array's internal storage with 'a'.
		void swap(hkObjectArray<T>& a);


		typedef T* iterator;
		typedef const T* const_iterator;

		inline iterator begin();
		inline iterator end();
		inline const_iterator begin() const;
		inline const_iterator end() const;

	private:

		void construct(T* t, int n);
		void destruct(T* t, int n);
		void construct(T* t, int n, const T& tinit);

		hkArray<T> m_array;
};


template <typename T>
hkOstream& operator<< (hkOstream& os, const hkObjectArray<T>& a);

#include <hkbase/htl/hkObjectArray.inl>

#endif // HKBASE_HKOBJECTARRAY_H

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
