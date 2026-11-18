
#pragma once

#include "CRC32.h"
#include "LeString.h"

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

	HashMap() { Clear(); };

	void Clear()
	{
		for( int i = 0; i < HashSize; ++i )
			Table[i]=(DWORD)-1;
		Array.Clear();
        FreePlaces.Clear();
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
				return;
			}
		}
	}

	// Krystian: removed 'const' from 'T_Value*', needed to update value field depending on insert
	T_Value* Find( const T_Key& key ) const
	{
		const DWORD hashValue = HashFunc<T_Key>(key) & (HashSize-1);
		for( DWORD i = Table[hashValue]; i != (DWORD)-1; i = Array[i].NextIndex )
			if( Array[i].Key == key )
				return &Array[i].Value;
		return NULL;
	}

	void ChangeValue( const T_Key& key, const T_Value& value  )
	{
		const DWORD hashValue = HashFunc<T_Key>(key) & (HashSize-1);
		for( DWORD i = Table[hashValue]; i != (DWORD)-1; i = Array[i].NextIndex )
			if( Array[i].Key == key )
				Array[i].Value = value;
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
			pair = Array.Create();
			pairIndex = Array.Size()-1;
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
		Array.Clear();
        FreePlaces.Clear();
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
			pair = Array.Create();
			pairIndex = Array.Size()-1;
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
		Array.Clear();
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
			pair = Array.Create();
			pairIndex = Array.Size()-1;
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
