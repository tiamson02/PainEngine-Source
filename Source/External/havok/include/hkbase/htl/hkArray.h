/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HKBASE_HKARRAY_H
#define HKBASE_HKARRAY_H

/// Common functionality for all hkArray types.
/// These are out of line functions to avoid code bloat.
struct hkArrayUtil
{
	static void HK_CALL _reserve(void*, int numElem, int sizeElem);
};

template <typename T> class hkObjectArray;

/// The default Havok array. 
/// Note that, for performance reasons, order may not be preserved when deleting elements.<br>
///	<br>
/// This is <i>not</i> a complete replacement for the STL array, 
/// and should be used only with plain old data types
/// (or objects that have a trivial constructor, destructor,
/// copy constructor and assignment operator that are equivalent 
/// to memcopy), as constructors, destructors, etc. are not called.
/// If you need to create an array of objects, you should use an hkObjectArray.
template <typename T>
class hkArray
{
		friend struct hkArrayUtil;
		friend class hkObjectArray< hkArray<T> >;

	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_ARRAY, hkArray<T>)

			/// Creates a zero length array.
		inline hkArray();

			/// Creates an array of size n. All elements are uninitialized.
		inline hkArray(int n);

			/// Creates an array of n elements initialized to 'fill'.
		inline hkArray(int n, const T& fill);

			/// Noncopying initialization from an existing array. 
		inline hkArray(T* ptr, int size, int capacity);
	
	private:
			/// Not publicly accessible, too easy to call accidentally.
		hkArray(const hkArray& a);

	public:

			/// Copies the array a.
		hkArray& operator= (const hkArray& a);

			/// Deallocates array memory.
		inline ~hkArray();

			/// Read/write access to the i'th element.
		inline T& operator[] (int i);

			/// Read only access to the i'th element.
		inline const T& operator[] (int i) const;

			/// Read/write access to the last element.
		inline T& back();

			/// Read only access to the last element.
		inline const T& back() const;

			/// Returns the size.
		inline int getSize() const;

			/// Returns the capacity.
		inline int getCapacity() const;

			/// Checks if the size is zero.
		inline bool isEmpty() const;
		
			/// Sets the size to zero.
		inline void clear();

			/// Sets the size to zero and deallocates storage.
		void clearAndDeallocate();

			/// Removes the element at the specified index. The last array element is used to replace the removed element, and the size is reduced by 1.
		void removeAt(int index);

			/// Removes the element at the specified index, copying elements down one slot as in the STL array.
		void removeAtAndCopy(int index);

			/// Returns the index of the first occurance of t, or -1 if not found.
		int indexOf(const T& t) const;

			/// Returns index of the last occurance of t, or -1 if not found.
		int lastIndexOf(const T& t) const;
		
			/// Removes the last element.
		inline void popBack();

			/// Returns the specified subarray of this array.
		hkArray<T> getSubarray(int begin, int end);

			/// Adds an element to the end.
		inline void pushBack(const T& e);

			/// Adds an element to the end. No check for resize.
		inline void pushBackUnchecked(const T& e);

			/// Ensures no reallocation occurs until at least size n.
		inline void reserve(int n);

			/// Ensures no reallocation occurs until size n.
		inline void reserveExactly(int n);

			/// Sets the size to n.
		inline void setSize(int n);

			/// Sets the size to n. New elements are initialized to fill.
		void setSize(int n, const T& fill);

			/// Sets the size to n.
		inline void setSizeUnchecked(int n);

			/// Increments the size by n and returns a reference to the first element created.
		inline T* expandBy( int n );

			/// Increments the size by n and returns a reference to the first element created.  No check for resize!
		inline T* expandByUnchecked( int n );

			/// Expands the array by numToInsert at the specified index.
		inline T* expandAt( int index, int numToInsert );

			/// Inserts the array a at index i.
		void insertAt(int i, const hkArray<T>& a);

			/// Inserts t at index i. Elements from i to the end are copied up one place.
		void insertAt(int i, const T& t);

			/// Swaps this array's internal storage with 'a'.
		void swap(hkArray<T>& a);

			/// 
		typedef T* iterator;
			/// 
		typedef const T* const_iterator;

			/// Returns an stl-like iterator to the first element.
		inline iterator begin();
			/// Returns an stl-like iterator to the 'one past the last' element.
		inline iterator end();
			/// Returns an stl-like const iterator to the first element.
		inline const_iterator begin() const;
			/// Returns an stl-like const iterator to the 'one past the last' element.
		inline const_iterator end() const;

		static void HK_CALL copy(T* dst, const T* src, int n);

		static void HK_CALL copyBackwards(T* dst, const T* src, int n);

	protected:

		enum
		{
			CAPACITY_MASK = int(unsigned(-1) >> 1),		// 0b0111111...
			DONT_DEALLOCATE_DATA_MASK = ~CAPACITY_MASK			// 0b1000000...
		};

		void releaseMemory();

		T* m_data;
		int m_size;
		int m_capacityAndDeallocateFlag; // highest bit set if we should not free m_data.
};

/// An array that has an internal storage capacity within the class itself.
template <typename T, unsigned N>
class hkInplaceArray : public hkArray<T>
{
	public:

		typedef hkInplaceArray<T,N> mytype;

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_ARRAY, mytype )

			/// Creates an array with the specified initial size.
		inline hkInplaceArray(int size = 0);

		inline ~hkInplaceArray(){}

			/// Copies the array a.
		hkArray<T>& operator= (const hkArray<T>& a);

	private:

		T m_storage[N];
};


/// An array that has a small internal storage capacity, aligned to 16 bytes within the class itself.
template <typename T, unsigned N>
class hkInplaceArrayAligned16 : public hkArray<T>
{
	public:
	
		typedef hkInplaceArray<T,N> mytype;

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_ARRAY, mytype)

			/// Creates an array with the specified initial size.
		inline hkInplaceArrayAligned16(int size = 0);

		inline ~hkInplaceArrayAligned16(){}

			/// Copies the array a.
		hkArray<T>& operator= (const hkArray<T>& a);

	private:

		int m_padding; // sizeof(base class) + padding == 16 bytes
		// This strange declaration is made due avoid Vector4 iterating constructors
		hkReal HK_ALIGNED_VARIABLE(m_storage[sizeof(T)/sizeof(hkReal) * N], 16);
};

template <typename T>
hkOstream& operator<< (hkOstream& os, const hkArray<T>& a);

#include <hkbase/htl/hkArray.inl>

#endif // HKBASE_HKARRAY_H


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
