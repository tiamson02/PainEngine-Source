// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/08 Bart Sekura
//----------------------------------------------------------------------------

// #pragma once
#ifndef _POOL_
#define _POOL_

#include "Engine.h"

// ==============================================================

template <class T, const int Size>
class Pool
{
public:
    Pool() : RefCnt(0), Ptr(0), FreePtr(0)
    {}

    ~Pool() { Clear(); }

    T* Alloc()
    {
        if(0==RefCnt)
        {
            Ptr=FreePtr=(DWORD*)GMem.Alloc(Size*sizeof(T));
            // init pointer list
            DWORD p = (DWORD)Ptr;
            for(int i=0; i<Size-1; ++i)
            {
                *((DWORD*)p) = p + sizeof(T);
                p += sizeof(T);
            }
            *((DWORD*)p) = NULL;
        }

        DWORD addr = (DWORD)FreePtr;
        if(addr) {
            ++RefCnt;
            FreePtr = (DWORD*) *FreePtr;

            m_setTag("Util");
            return new ((void*)addr) T();
        }

        return 0;
    }

    BOOL Free(T* ptr)
    {
        Assert(ptr);
        if(ptr && ValidPtr(ptr))
        {
            ptr->~T();
            if(--RefCnt==0)
            {
                GMem.Free(Ptr);
                Ptr=FreePtr=0;
            }
            else 
            {
                DWORD addr=(DWORD)ptr;
                *((DWORD*) addr) = (DWORD) FreePtr;
                FreePtr=(DWORD*)addr;
            }

            return TRUE;
        }
        return FALSE;
    }

    void Clear()
    {
        if(Ptr) 
            GMem.Free(Ptr);

        Ptr=FreePtr=0;
        RefCnt=0;
    }

    bool ValidPtr(T* ptr) const
    {
        return (DWORD*)ptr-Ptr >= 0 && (DWORD*)ptr-Ptr < Size*sizeof(T);
    }

private:
    DWORD   RefCnt;
    DWORD*  Ptr;
    DWORD*  FreePtr;
};

// ==============================================================

template <class T, const int Size>
class StaticPool
{
public:
    StaticPool() : Ptr(0), FreePtr(0)
    {}

    T* Alloc()
    {
        if(!Ptr)
        {
            Ptr=FreePtr=(DWORD*)&Chunk[0];
            // init pointer list
            DWORD p = (DWORD)Ptr;
            for(int i=0; i<Size-1; ++i)
            {
                *((DWORD*)p) = p + sizeof(T);
                p += sizeof(T);
            }
            *((DWORD*)p) = NULL;
        }

        DWORD addr = (DWORD)FreePtr;
        if(addr) 
        {
            FreePtr = (DWORD*) *FreePtr;
            m_setTag("Util");
            return new ((void*)addr) T();
        }

        return 0;
    }

    BOOL Free(T* ptr)
    {
        if(ptr && ValidPtr(ptr))
        {
            ptr->~T();
            DWORD addr=(DWORD)ptr;
            *((DWORD*) addr) = (DWORD) FreePtr;
            FreePtr=(DWORD*)addr;

            return TRUE;
        }
        return FALSE;
    }

    void Clear() { Ptr=0; }

    bool ValidPtr(T* ptr) const
    {
        return (DWORD*)ptr-Ptr >= 0 && (DWORD*)ptr-Ptr < Size*sizeof(T);
    }


private:
    DWORD*  Ptr;
    DWORD*  FreePtr;
    BYTE    Chunk[Size*sizeof(T)];
};


#endif //_POOL_

