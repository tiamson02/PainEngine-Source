/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HKBASE_HKSTL_H
#define HKBASE_HKSTL_H

//this disables warnings from stl includes
//which expect exception handling, and only
//affects win32 and xbox builds
#if defined(_MSC_VER)
#	pragma warning ( disable: 4530 )
#endif

#include <vector>
#include <memory>
#include <map>
#include <deque>
#include <queue>
#include <set>
#include <algorithm>

#define hk_find  std::find
#define hk_greater std::greater
#define hk_less std::less
#define hk_swap std::swap
#define hk_sort std::sort
#define hk_pair std::pair
#define hk_make_pair std::make_pair
#define hk_make_heap std::make_heap
#define hk_random_shuffle std::random_shuffle
#define hk_fill std::fill
#define hk_rotate std::rotate


// note that all libs needs to be rebuilt if this is changed
#if (HK_CONFIG_MEMORY_CLASS == HK_CONFIG_MEMORY_CLASS_DISABLED)

#	define hk_array std::vector
#	define hk_map   std::map
#	define hk_set   std::set
#	define hk_queue std::deque
#	define hk_priority_queue std::priority_queue
#	define hk_allocator std::allocator

#else // not HK_CONFIG_MEMORY_CLASS_DISABLED


#	if !defined(_HAS_MEMBER_TEMPLATES) && \
		( defined(_STLP_MEMBER_TEMPLATES) || \
		  defined(HK_PLATFORM_XBOX) || \
		  ( defined(HK_COMPILER_MSVC) && (HK_COMPILER_MSVC_VERSION > 1200) ) || \
		  ( defined(HK_COMPILER_MWERKS) && !defined(_MSL_NO_MEMBER_TEMPLATE) ) || \
		  defined(__STL_MEMBER_TEMPLATES) || \
		  defined(_MEMBER_TEMPLATES) || \
		  defined(HK_COMPILER_INTEL) )

#		define _HAS_MEMBER_TEMPLATES 1

#	endif

#	if (defined(HK_PLATFORM_WIN32) || defined(HK_PLATFORM_XBOX)) && !defined(_STLPORT_VERSION)
#		define PTR_CLASS _FARQ
#	else
#		define PTR_CLASS /*nothing*/
#	endif

template <typename T>
class hk_allocator : public std::allocator<T>
{
	public:

#if _HAS_MEMBER_TEMPLATES
	hk_allocator() {}
	template <typename U> hk_allocator(const hk_allocator<U>&) {}
#endif //_HAS_MEMBER_TEMPLATES

	inline char PTR_CLASS *_Charalloc(hk_size_t _HKN)
	{
		return hkAllocate<char PTR_CLASS>(_HKN, HK_MEMORY_CLASS_STL);
	}

	inline T* allocate(hk_size_t _HKN, const void * = 0)
	{
		return hkAllocate<T>( _HKN, HK_MEMORY_CLASS_STL );
	}

	inline void deallocate(void PTR_CLASS *_HKP, hk_size_t )
	{
		hkDeallocate(_HKP);
	}

#if _HAS_MEMBER_TEMPLATES
	template <typename U>
    struct rebind { typedef hk_allocator<U> other; };
#endif //_HAS_MEMBER_TEMPLATES
};

template <typename T>
class hk_array : public std::vector<T, hk_allocator<T> > 
{
	public:

	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_STL, hk_array<T>)

	// Five ctors: 
    explicit hk_array(const hk_allocator<T>& a = hk_allocator<T>() ) : std::vector<T, hk_allocator<T> >( a ) {}
	explicit hk_array(hk_size_t n) : std::vector<T, hk_allocator<T> >(n) {}
    explicit hk_array(hk_size_t n, const T& v) : std::vector<T, hk_allocator<T> >(n,v) {}
    hk_array(const hk_array& x) : std::vector<T, hk_allocator<T> >(x) {}
    
#if _HAS_MEMBER_TEMPLATES

	template <typename InputIter>
	hk_array( InputIter first, InputIter last, const hk_allocator<T>& a = hk_allocator<T>() )
		: std::vector<T, hk_allocator<T> >(f, l, a) { }
#else

	hk_array(const_iterator f, const_iterator l, const hk_allocator<T>& a = hk_allocator<T>() )
		: std::vector<T, hk_allocator<T> >(f, l, a) { }

#endif	
};

// Different allocator semantics for CW on Mac and Dev Studio on PC:
template <typename K, typename T, typename P = std::less<K> > 
#ifdef HK_COMPILER_MWERKS
class hk_map : public std::map<K,T,P,hk_allocator< std::pair< const K, T> > >
{
	public:
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR_BY_SIZE( HK_MEMORY_CLASS_STL, sizeof(hk_map<K,T,P>) );
	hk_map() : std::map<K,T,P,hk_allocator< std::pair< const K, T> > >() {}
    hk_map(const hk_map& x) : std::map<K,T,P,hk_allocator< std::pair< const K, T> > >(x) {}
	
};
#else
class hk_map : public std::map<K,T,P,hk_allocator< T > >{
	public:
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR_BY_SIZE(HK_MEMORY_CLASS_STL, sizeof(hk_map<K,T,P>) )
	hk_map() : std::map<K,T,P,hk_allocator<T> >() {}
    hk_map(const hk_map& x) : std::map<K,T,P,hk_allocator<T> >(x) {}
};
#endif

template <typename T> 
class hk_queue : public std::deque<T, hk_allocator<T> > 
{
	public:
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_STL, hk_queue<T> )
	hk_queue() : std::deque<T, hk_allocator<T> >() {}
    hk_queue(const hk_queue& x) : std::deque<T, hk_allocator<T> >(x) {}
};

template <typename T, typename C = hk_array<T>, typename P = hk_less< typename C::value_type > >  
class hk_priority_queue : public std::priority_queue<T, C, P> 
{
	public:

	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR_BY_SIZE(HK_MEMORY_CLASS_STL, sizeof(hk_priority_queue<T,C,P>) )

	// Does the priority queue accept the Container or the Allocator:
	// All member template compatable STL so far tested and also the GNU STL
#if _HAS_MEMBER_TEMPLATES || defined(HK_COMPILER_GCC)

	explicit hk_priority_queue(const P& p = P(), const C& c = C() ) 
		: std::priority_queue<T,C,P>(p,c) {}

#else

	typedef typename C::allocator_type      hkAllocType;
	
	explicit hk_priority_queue(const P& p = P(), const hkAllocType& a = hkAllocType()) 
		: std::priority_queue<T,C,P>(p,a) {}

#endif 

	// Does the priority queue accept the Iterator type or the Value type
	// Must support member templates
#if _HAS_MEMBER_TEMPLATES 

	// Not supported yet

//	template <class Iter>
//	hk_priority_queue(Iter _F, Iter _L, const P& _P = P() )
//		: std::priority_queue<T,C,P>( _F, _L, _P ) {}

//	template <class Iter>
//	hk_priority_queue(Iter _F, Iter _L, const P& _P, const C& _C )
//		: std::priority_queue<T,C,P>( _F, _L, _P, _C ) {}
//
#elif defined(HK_COMPILER_GCC)

	typedef typename C::value_type			hkValueType;

	hk_priority_queue(	const hkValueType *f, const hkValueType *l,
						const P& p = P(), const C& c = C(f,l))
		: std::priority_queue<T,C,P>(f,l,p,c) {}

#else

	typedef typename C::value_type			hkValueType;

	hk_priority_queue(	const hkValueType *f, const hkValueType *l,
						const P& p = P(), const hkAllocType& a = hkAllocType())
		: std::priority_queue<T,C,P>(f,l,p,a) {}

#endif 

};

template <typename K, typename P = hk_less<K> > 
class hk_set : public std::set<K,P,hk_allocator<K> >
{
	public:
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR_BY_SIZE(HK_MEMORY_CLASS_STL, sizeof(hk_set<K,P>) )
	hk_set() : std::set<K,P,hk_allocator<K> > () {}
};

#endif // HK_CONFIG_MEMORY_CLASS_DISABLED
#endif // HKBASE_HKSTL_H

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
