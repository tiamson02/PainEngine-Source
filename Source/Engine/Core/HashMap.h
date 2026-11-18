// #pragma once
#ifndef _HASHMAP_
#define _HASHMAP_

#include "Core/CRC32.h"
#include "Core/DynArray.h"
#include "Core/LeString.h"

template <typename T>
inline DWORD HashFunc(const T& t)
{
    return DWORD(IsPrimitive<T>::Value ? t : (T)-1);
}

template <>
inline DWORD HashFunc<String>(const String& s)
{
    return Crc32(s.AsChar());
}

// ==============================================================
// WARNING: HashSize has to be a power of 2!!!

#define M_LOGMSG(fmt, args...)   {}//{ printf(fmt, ##args); } //m_logMsg(fmt, ##args); }

template <class T_Key, class T_Value, const int HashSize=16>
class HashMap
{
public:
	struct HashPair
	{
		T_Key	Key;
		T_Value	Value;
		int		NextIndex;
	};

	HashMap() { 
        M_LOGMSG("HashMap::HashMap(): constructing (this = 0x%X)\n", (unsigned int)this);
        Clear();
    };

	void Clear()
	{
        M_LOGMSG("HashMap::Clear(0x%X): clearing the map\n", (unsigned int)this);
		for( int i = 0; i < HashSize; ++i )
			Table[i]=(DWORD)-1;

        M_LOGMSG("HashMap::Clear(): clearing the array\n");
		Array.ClearArray();
        M_LOGMSG("HashMap::Clear(): clearing the free places\n");
        FreePlaces.ClearArray();
	}

	void Insert( const T_Key& key, const T_Value& value ) {
        M_LOGMSG("HashMap::Insert(0x%X): inserting a key\n", (unsigned int)this);
        (*this)[key]=value;
    };
	int Remove( const T_Key& key )
	{
        M_LOGMSG("HashMap::Remove(0x%X): removing a key\n", (unsigned int)this);
		const DWORD hashValue = HashFunc<T_Key>(key) & (HashSize-1);
		DWORD i2 = (DWORD)-1;
		for( DWORD i = Table[hashValue]; i != (DWORD)-1; i2 = i, i = Array[i].NextIndex )
		{
			if( Array[i].Key == key )
			{
				if( i2 == (DWORD)-1 )
					Table[hashValue] = Array[i].NextIndex;
				else
					Array[i2].NextIndex = Array[i].NextIndex;


				FreePlaces.Add(i);
                Array[i].NextIndex = -1;
               
				return i;
			}
		}

        return -1;
	}

	// Krystian: removed 'const' from 'T_Value*', needed to update value field depending on insert
	T_Value* Find( const T_Key& key ) const
	{
        M_LOGMSG("HashMap::Find(0x%X): searching for a key\n", (unsigned int)this);
		const DWORD hashValue = HashFunc<T_Key>(key) & (HashSize-1);
		for( DWORD i = Table[hashValue]; i != (DWORD)-1; i = Array[i].NextIndex )
			if( Array[i].Key == key )
				return &Array[i].Value;
		return NULL;
	}

    bool KeyOf(const T_Value &val, T_Key &key){
        M_LOGMSG("HashMap::Find(0x%0X): searching for a value\n", (unsigned int)this);

        for (int i = 0; i < Array.Size(); i++){
            if (Array[i].Value == val){
                key = Array[i].Key;

                return true;
            }
        }

        return false;
    }

	void ChangeValue( const T_Key& key, const T_Value& value  )
	{
        M_LOGMSG("HashMap::ChangeValue(0x%X): changing a value\n", (unsigned int)this);
		const DWORD hashValue = HashFunc<T_Key>(key) & (HashSize-1);
		for( DWORD i = Table[hashValue]; i != (DWORD)-1; i = Array[i].NextIndex )
			if( Array[i].Key == key )
				Array[i].Value = value;
	}


	T_Value& operator[]( const T_Key& key )
	{
        M_LOGMSG("HashMap::operator[](0x%X): accessing a member\n", (unsigned int)this);
		const DWORD hashValue = HashFunc<T_Key>(key) & (HashSize-1);
		HashPair* pair;
		DWORD pairIndex;
		if( FreePlaces.Size() )
		{
            M_LOGMSG("HashMap::operator[](0x%X): accessing an existing member\n", (unsigned int)this);
			pairIndex = FreePlaces[FreePlaces.Size()-1];
			FreePlaces.RemoveLast();
			pair = &Array[pairIndex];
		}
		else
		{
            M_LOGMSG("HashMap::operator[](0x%X): allocating a new member\n", (unsigned int)this);
#ifdef TRANSGAMING
			pairIndex = Array.GrowArray(1);
            M_LOGMSG("HashMap::operator[](0x%X): allocated a new member\n", (unsigned int)this);

            pair = &Array[pairIndex];
            M_LOGMSG("HashMap::operator[](0x%X): accessed new member\n", (unsigned int)this);
#else
			pair = Array.CreateArray();
			pairIndex = Array.Size()-1;
#endif
		}
		pair->Key			= key;
		pair->NextIndex		= Table[hashValue];
		Table[hashValue]	= pairIndex;
		return pair->Value;
	}

	int Size() const { return Array.Size(); };
	HashPair& GetPair( int index ) const { return Array[index]; };

private:

	DWORD					Table[HashSize];
	DynamicArray<HashPair>	Array;
	DynamicArray<int>		FreePlaces;
};

// ==============================================================
// WARNING: HashSize has to be a power of 2!!!

template <class T_Key, class T_Key2, class T_Value, const int HashSize=16>
class HashMapSpecial
{
public:
	struct HashPair
	{
		T_Key	Key;
		T_Key2	Key2;
		T_Value	Value;
		int		NextIndex;
	};

	HashMapSpecial() { Clear(); };

	void Clear()
	{
		for( int i = 0; i < HashSize; ++i )
			Table[i]=(DWORD)-1;
		Array.ClearArray();
        FreePlaces.ClearArray();
	}

	void Insert( const T_Key& key, const T_Key2& key2, const T_Value& value ) { this->Get(key,key2) = value; }

	void Remove( const T_Key& key, const T_Key2& key2 )
	{
		const DWORD hashValue = (HashFunc<T_Key>(key)+HashFunc<T_Key2>(key2)) & (HashSize-1);
		DWORD i2 = (DWORD)-1;
		for( DWORD i = Table[hashValue]; i != (DWORD)-1; i2 = i, i = Array[i].NextIndex )
		{
			if( ( Array[i].Key == key ) && ( Array[i].Key2 == key2 ) )
			{
				if( i2 == (DWORD)-1 )
					Table[hashValue] = Array[i].NextIndex;
				else
					Array[i2].NextIndex = Array[i].NextIndex;
				FreePlaces.Add(i);
				return;
			}
		}
	}

	T_Value* Find( const T_Key& key, const T_Key2& key2 ) const
	{
		const DWORD hashValue = (HashFunc<T_Key>(key)+HashFunc<T_Key2>(key2)) & (HashSize-1);
		for( DWORD i = Table[hashValue]; i != (DWORD)-1; i = Array[i].NextIndex )
			if( ( Array[i].Key == key ) && ( Array[i].Key2 == key2 ) )
				return &Array[i].Value;
		return NULL;
	}

	T_Value& Get( const T_Key& key, const T_Key2& key2 )
	{
		const DWORD hashValue = (HashFunc<T_Key>(key)+HashFunc<T_Key2>(key2)) & (HashSize-1);
		HashPair* pair;
		DWORD pairIndex;
		if( FreePlaces.Size() )
		{
			pairIndex = FreePlaces[FreePlaces.Size()-1];
			FreePlaces.RemoveLast();
			pair = &Array[pairIndex];
		}
		else
		{
#ifdef TRANSGAMING
			pairIndex = Array.GrowArray(1);

            pair = &Array[pairIndex];
#else
			pair = Array.CreateArray();
			pairIndex = Array.Size()-1;
#endif
		}
		pair->Key			= key;
		pair->Key2			= key2;
		pair->NextIndex		= Table[hashValue];
		Table[hashValue]	= pairIndex;
		return pair->Value;
	}

	int Size() const { return Array.Size(); };
	HashPair& GetPair( int index ) const { return Array[index]; };

private:

	DWORD					Table[HashSize];
	DynamicArray<HashPair>	Array;
	DynamicArray<int>		FreePlaces;
};

// ==============================================================
// WARNING: HashSize has to be a power of 2!!!

template <class T_Key, class T_Type, const int HashSize=16>
class HashMapPtr
{
public:
	typedef T_Type* T_Value;

	struct HashPair
	{
		T_Key		Key;
		T_Value		Value;
		int			NextIndex;
	};

	HashMapPtr() { Clear(); };
	~HashMapPtr() 
	{
		int fsize = FreePlaces.Size();

		for (int i=0; i<fsize; i++) 
			Array[FreePlaces[i]].NextIndex = (DWORD)-2;

		int asize = Array.Size();

		for (int i=0; i<asize; i++) 
			if (Array[i].NextIndex != (DWORD)-2)
				delete Array[i].Value;
	}

	void Clear()
	{
		for( int i = 0; i < HashSize; ++i )
			Table[i]=(DWORD)-1;
		Array.ClearArray();
	}

	void Insert( const T_Key& key, const T_Value& value ) { (*this)[key]=value; };
	void Remove( const T_Key& key )
	{
		const DWORD hashValue = HashFunc<T_Key>(key) & (HashSize-1);
		DWORD i2 = (DWORD)-1;
		for( DWORD i = Table[hashValue]; i != (DWORD)-1; i2 = i, i = Array[i].NextIndex )
		{
			if( Array[i].Key == key )
			{
				if( i2 == (DWORD)-1 )
					Table[hashValue] = Array[i].NextIndex;
				else
					Array[i2].NextIndex = Array[i].NextIndex;
				

				FreePlaces.Add(i);

				delete Array[i].Value;

				return;
			}
		}
	}

	T_Type* Find( const T_Key& key ) const
	{
		const DWORD hashValue = HashFunc<T_Key>(key) & (HashSize-1);
		for( DWORD i = Table[hashValue]; i != (DWORD)-1; i = Array[i].NextIndex )
			if( Array[i].Key == key )
				return Array[i].Value;
		return NULL;
	}

	T_Value& operator[]( const T_Key& key )
	{
		const DWORD hashValue = HashFunc<T_Key>(key) & (HashSize-1);
		HashPair* pair;
		DWORD pairIndex;
		if( FreePlaces.Size() )
		{
			pairIndex = FreePlaces[FreePlaces.Size()-1];
			FreePlaces.RemoveLast();
			pair = &Array[pairIndex];
		}
		else
		{
#ifdef TRANSGAMING
			pairIndex = Array.GrowArray(1);

            pair = &Array[pairIndex];
#else
			pair = Array.CreateArray();
			pairIndex = Array.Size()-1;
#endif
		}
		pair->Key			= key;
		pair->NextIndex		= Table[hashValue];
		Table[hashValue]	= pairIndex;
		return pair->Value;
	}

	int Size() const { return Array.Size(); };
	HashPair& GetPair( int index ) const { return Array[index]; };

private:

	DWORD					Table[HashSize];
	DynamicArray<HashPair>	Array;
	DynamicArray<int>		FreePlaces;
};

#endif //_HASHMAP_

