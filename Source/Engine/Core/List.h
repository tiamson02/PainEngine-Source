
// #pragma once
#ifndef _LIST_H_
#define _LIST_H_


#include "Core/Pool.h"
#include "Core/DynArray.h"

// ================================================================

template <class T>
static inline void ListInsert(T*& list, T* elem)
{
    if (list)
	    list->Prev = &elem->Next;

	elem->Next = list;
	elem->Prev = &list;
	list = elem;
}

template <class T>
static inline void ListRemove(T* elem)
{
	if (elem->Next)
		elem->Next->Prev = elem->Prev;

	*(elem->Prev) = elem->Next;
}

// ================================================================
#ifndef TRANSGAMING
template <class T, const int PoolSize>
class DynamicPoolAllocator
{
    typedef Pool<T,PoolSize> TItemPool;  // pool of list items

public:
    T* Alloc()
    {
        // try existing pools
        for(int i=0; i<ItemPools.Size(); ++i)
        {
            T* item=ItemPools[i].Alloc();
            if(item)
                return item;
        }

        // all pools are busy, get a new one
        TItemPool* pool=ItemPools.Create();
        return pool->Alloc();
    }

    void Free(T* ptr)
    {
        if(ptr)
        {
            for(int i=0; i<ItemPools.Size(); ++i)
                if(ItemPools[i].Free(ptr))
                    return;
        }
    }

	void Clear() { ItemPools.ClearArray(); }

private:
    DynamicArray<TItemPool> ItemPools;
};

// ================================================================

template <class T, const int PoolSize>
class StaticPoolAllocator
{
    typedef StaticPool<T,PoolSize>  TItemPool;

public:
    T*   Alloc()        { return Pool.Alloc(); }
    void Free(T* ptr)   { Pool.Free(ptr); }
	void Clear()		{ Pool.Clear(); }

private:
	TItemPool	Pool;
};
#endif
// ================================================================

template <class T>
class ListElement : public T
{
public:
    ListElement*    Next;
    ListElement**   Prev;
};

template <class T>
class PtrListElement
{
public:
    T* Ptr;

	explicit PtrListElement(T* ptr=0) : Ptr(ptr) {}
	T& operator*()  const { return (*Get()); }
    T* operator->() const {	return (Get()); }
    T* Get() const { return Ptr; }

	PtrListElement*		Next;
	PtrListElement**	Prev;
};

// ================================================================
#ifndef TRANSGAMING
template <class T, class PoolAlloc=DynamicPoolAllocator<T,64> >
class TheList
{
public:
    TheList() : Head(0) {}

    void Insert(const T& x)
    {
        T* e=Pool.Alloc();
        if(e)
        {
            (*e)=x;
		    if(Head)
			    Head->Prev = &e->Next;
		    e->Next = Head;
		    e->Prev = &Head;
		    Head    = e;
        }
    }

    void Remove(T* e)
    {
		if(e->Next)
			e->Next->Prev = e->Prev;
		*e->Prev = e->Next;

        Pool.Free(e);
    }

	void Clear()
	{
		Head=0;
		Pool.Clear();
	}

    operator T*() const { return Head; }

private:
    T*          Head;
    PoolAlloc   Pool;
};
#endif
// ================================================================

#ifdef SHIT

template <class T>
inline bool CompareFunc(const T&, const T&)
{
    return false;
}

template <class T>
struct List
{
    struct Item
    {
        T*      Ref;
        Item*   Next;
        Item*   Prev;
    };

    DynamicArray<Item>  ItemPool;
    int                 PoolCount;
    Item*               ListFront;
    Item*               ListBack;

    List() : PoolCount(0), ListFront(NULL), ListBack(NULL) {}

    void Init(int count)
    {
        ItemPool.Resize(count);
        Reset();
    }

    void Reset()
    {
        PoolCount=0;
        ListFront=ListBack=NULL;
    }

    void Add(T* ref)
    {
		if (ItemPool.Size()<=PoolCount)
			ItemPool.Grow();

        Item* item=&ItemPool[PoolCount++];
        item->Ref=ref;

	    // insert at back
	    item->Prev = ListBack;
	    item->Next = NULL;
        if(ListFront == NULL) { // list was empty
            ListFront = item;
        }
        else { // list was nonempty
		    ListBack->Next = item;
        }

    	ListBack = item;
    }

    void AddBefore(Item* before, T* ref)
    {
		if (ItemPool.Size()<=PoolCount)
			ItemPool.Grow();

        Item* item=&ItemPool[PoolCount++];
        item->Ref=ref;

	    item->Next = before;
	    item->Prev = before->Prev;
        if(before->Prev) {
	        before->Prev->Next = item;
        }
	    before->Prev = item;

        if(before == ListFront) {
		    ListFront = item;
        }
    }

    void AddSorted(T* ref)
    {
        for(Item* item=ListFront; item; item=item->Next) {
            if(CompareFunc<T>(*item->Ref,*ref)) {
                AddBefore(item,ref);
                return;
            }
        }
        Add(ref);
    }
};

// ===============================================================

template <class T> 
class DoubleLinkList : public T
{
public:
	DoubleLinkList*     Next;
	DoubleLinkList**    PrevLink;

	void Unlink()
	{
		if(Next)
			Next->PrevLink = PrevLink;
		*PrevLink = Next;
	}
    
	void Link(DoubleLinkList*& before)
	{
		if(before)
			before->PrevLink = &Next;
		Next     = before;
		PrevLink = &before;
		before   = this;
	}
    
    /*
    void Link(DoubleLinkList*& after)
    {
        Next     = 0;
        PrevLink = &after;
        if(after)
            after->Next=this;
        else
            after=this;
    }
    */
};

// ================================================================

template <class T, const int PoolSize>
class DynamicPoolAllocator
{
    typedef Pool<T,PoolSize> TItemPool;  // pool of list items

public:
    T* Alloc()
    {
        // try existing pools
        for(int i=0; i<ItemPools.Size(); ++i)
        {
            T* item=ItemPools[i].Alloc();
            if(item)
                return item;
        }

        // all pools are busy, get a new one
        TItemPool* pool=ItemPools.Create();
        return pool->Alloc();
    }

    void Free(T* ptr)
    {
        if(ptr)
        {
            for(int i=0; i<ItemPools.Size(); ++i)
                if(ItemPools[i].Free(ptr))
                    return;
        }
    }

	void Clear() { ItemPools.Clear(); }

private:
    DynamicArray<TItemPool> ItemPools;
};

template <class T, const int PoolSize>
class StaticPoolAllocator
{
    typedef StaticPool<T,PoolSize>  TItemPool;

public:
    T*   Alloc()        { return Pool.Alloc(); }
    void Free(T* ptr)   { Pool.Free(ptr); }
	void Clear()		{ Pool.Clear(); }

private:
	TItemPool	Pool;
};

// ================================================================

template <class T>
class ListElement : public T
{
public:
    ListElement*    Next;
    ListElement*    Prev;
};

template <class T>
class PtrListElement
{
public:
    T* Ptr;

	explicit PtrListElement(T* ptr=0) : Ptr(ptr) {}
	T& operator*()  const { return (*Get()); }
    T* operator->() const {	return (Get()); }
    T* Get() const { return Ptr; }

	PtrListElement*		Next;
	PtrListElement*		Prev;
};

template <class T, class PoolAlloc=DynamicPoolAllocator<T,64> >
class TheList
{
public:
    TheList() : Head(0) {}

    /*
    void Add(const T& elem)
    {
        T* newElem=Pool.Alloc();
        if(newElem)
        {
            (*newElem)=elem;
            newElem->Link(Head);
        }
    }

    T* Create()
    {
        T* newElem=Pool.Alloc();
        if(newElem)
            newElem->Link(Head);
        return newElem;
    }

    void Remove(T* elem)
    {
        elem->Unlink();
        Pool.Free(elem);
    }
    */

    void Insert(const T& x)
    {
        T* e=Pool.Alloc();
        if(e)
        {
            (*e)=x;
            e->Next=0;
            if(0 == Head) 
            {
                Head    = e;
                e->Prev = e;
            }
            else
            {
                e->Prev = Head->Prev;
                e->Prev->Next = e;
                Head->Prev = e;
            }
        }
    }

    void InsertBefore(T* before, const T& x)
    {
        T* e=Pool.Alloc();
        if(e)
        {
            (*e)=x;
            e->Next=before;
            e->Prev=before->Prev;
            before->Prev->Next=e;
            before->Prev=e;
            if(before == Head)
                Head=e;
        }
    }

    void Remove(T* e)
    {
        if(Head == e)
            Head = e->Next;
        else
        {
            if(e->Next)
                e->Next->Prev = e->Prev;

            e->Prev->Next = e->Next;
        }
    }

	void Clear()
	{
		Head=0;
		Pool.Clear();
	}

    operator T*() const { return Head; }

private:
    T*          Head;
    PoolAlloc   Pool;
};

// ================================================================

class ENGINE_API ListTracker
{
public:
	ListTracker()
	{
		GMemZero(&OnList[0],sizeof(OnList));
	}

	inline bool TrackLink(const void* listPtr)
	{
		int slot=-1;
		for(int i=0; i<ARRAY_COUNT(OnList); ++i)
		{
			if(OnList[i]==listPtr)
				return false;
			if(slot==-1 && OnList[i]==0)
				slot=i;
		}

		if(slot!=-1)
			OnList[slot]=listPtr;

		return true;
	}

	inline void TrackUnlink(const void* listPtr)
	{
		for(int i=0; i<ARRAY_COUNT(OnList); ++i)
			if(OnList[i]==listPtr)
			{
				OnList[i]=0;
				break;
			}
	}

	inline bool IsOnList(const void* listPtr) const
	{
		for(int i=0; i<ARRAY_COUNT(OnList); ++i)
			if(OnList[i]==listPtr)
				return true;

		return false;
	}

	const void*	OnList[4];
};

// ================================================================

template <class T>
struct TPtr
{
    T* Ptr;

	explicit TPtr(T* ptr=0) : Ptr(ptr) {}
	T& operator*()  const { return (*Get()); }
    T* operator->() const {	return (Get()); }
    T* Get() const { return Ptr; }
};

template <class T, const int PoolSize=64>
class PtrList
{
public:

    typedef DoubleLinkList<TPtr<T> >    TPtrItem;   // list item
    typedef Pool<TPtrItem,PoolSize>     TItemPool;  // pool of list items

    typedef TPtrItem    ItemType;

    PtrList() : ItemList(0) {}

    void Add(T* ptr)
    {
        TPtrItem* item=AllocItem();
        Assert(item);
        item->Ptr=ptr;
        item->Link(ItemList);
    }

    void Remove(T* ptr)
    {
        for(TPtrItem* item=ItemList; item; item=item->Next) {
            if(item->Get()==ptr) {
                item->Unlink();
                RemoveItem(item);
                break;
            }
        }
    }

    void Clear()
    {
        ItemPools.Clear();
        ItemList=0;
    }

    operator TPtrItem*() const { return ItemList; }

private:

    TPtrItem* AllocItem()
    {
        // try existing pools
        for(int i=0; i<ItemPools.Size(); ++i)
        {
            TPtrItem* item=ItemPools[i].Alloc();
            if(item)
                return item;
        }

        // all pools are busy, get a new one
        TItemPool* pool=ItemPools.Create();
        return pool->Alloc();
    }

    void RemoveItem(TPtrItem* ptr)
    {
        for(int i=0; i<ItemPools.Size(); ++i)
            if(ItemPools[i].Free(ptr))
                return;
    }


private:
    DynamicArray<TItemPool>     ItemPools;
    TPtrItem*                   ItemList;
};

#if 0
// INTERNAL COMPILER ERROR !!!

template <class T, 
          template <class A> class TPoolAllocator, 
          const int PoolSize=64>
class PtrList
{
public:

    typedef DoubleLinkList<TPtr<T> >            TPtrItem;       // list item
    typedef TPoolAllocator<TPtrItem>            TPoolAlloc;     // pool of list items

    typedef TPtrItem    ItemType;

    PtrList() : ItemList(0) {}

    void Add(T* ptr)
    {
        TPtrItem* item=Pool.Alloc();
        Assert(item);
        item->Ptr=ptr;
        item->Link(ItemList);
    }

    void Remove(T* ptr)
    {
        for(TPtrItem* item=ItemList; item; item=item->Next) {
            if(item->Get()==ptr) {
                item->Unlink();
                Pool.Free(item);
                break;
            }
        }
    }

    void Clear()
    {
        ItemList=0;
    }

    operator TPtrItem*() const { return ItemList; }

private:
    TPoolAlloc  Pool;
    TPtrItem*   ItemList;
};

#endif

#endif // SHIT



#endif //_LIST_H_

