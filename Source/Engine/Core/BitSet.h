
// #pragma once
#ifndef _BITSEC_
#define _BITSEC_

#include "mmgr.h"

class ENGINE_API BitSet 
{
public:
    BitSet() : Bits(0), Size(0) {}
	~BitSet() { Release(); }

    void Release() 
    { 
        delete [] Bits, Bits=0;
        Size=0; 
    }
	void Resize(int size) 
    { 
        Release();
		Size = size/32+1;
        m_setTag("Util");
		Bits = new DWORD[Size];
		ClearAll();
	}

	void Set(int i) 
    { 
        Bits[i>>5] |= (1<<(i&31)); 
    }
	void Clear(int i) 
    { 
        Bits[i>>5] &= ~(1<<(i&31)); 
    }
    int Test(int i) const
    { 
        return Bits[i>>5]&(1<<(i&31)); 
    }
	void ClearAll() 
    { 
        memset(Bits,0,Size*sizeof(DWORD)); 
    }

	int GetSize( void )
	{
		return Size;
	}

	void CopyFrom( BitSet& other )
	{
		int min = (Size > other.Size)?other.Size:Size;
		::memcpy( Bits, other.Bits, min*sizeof(DWORD) );
	}

	void MakeCompleteCopy( BitSet& other )
	{
        Release();
		Size = other.Size;
        m_setTag("Util");
		Bits = new DWORD[Size];
		CopyFrom( other );
	}

	DWORD* GetPtrToData( void )	// for load/save, for instance, so I won't put fread() here; use carefully
	{
		return Bits;
	}

private:
	DWORD*  Bits;
	int     Size;
};

template <const int Size>
class StaticBitSet
{
public:
	void Set(int i) 
    { 
        Bits[i>>5] |= (1<<(i&31)); 
    }
	void Clear(int i) 
    { 
        Bits[i>>5] &= ~(1<<(i&31)); 
    }
    int Test(int i) const
    { 
        return Bits[i>>5]&(1<<(i&31)); 
    }
	void ClearAll() 
    { 
        GMemZero(Bits,sizeof(Bits)); 
    }

private:
	DWORD	Bits[Size/32+1];
};

#endif //_BITSEC_

