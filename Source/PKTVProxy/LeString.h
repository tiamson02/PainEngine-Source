// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/04 Bart Sekura
//----------------------------------------------------------------------------

#pragma once

#include "Debug.h"
#include "DynArray.h"

typedef char CharType;

class  String : public DynamicArray<CharType>
{
public:
	String()
    :	DynamicArray<CharType>()
    {}

	String(const String& s)
    :	DynamicArray<CharType>(s)
    {}

	String( const char c, int count )
    :	DynamicArray<CharType>()
    {
	    int size = count+1;
	    Resize(size);
	    ::memset(mElems,c,mSize*sizeof(CharType));
		mElems[count] = 0;
	}

	String(const CharType* s)
    :	DynamicArray<CharType>()
    {
        Assert(s);
	    int size = (int)(::strlen(s)+1);
	    Resize(size);
	    ::memcpy(mElems,s,mSize*sizeof(CharType));
    }

	const CharType* AsChar() const { return mElems; }

    String& operator+=(const String& str)
    {
        if(this != &str && str.Size()) {
            // resize buffer
            mSizeAllocated+=mSize>0 ? str.Size()-1 : str.Size();
            ResizeBuffer();

            // append string
            ::memcpy(mElems+(mSize>0 ? mSize-1 : 0),str.mElems,str.Size()*sizeof(CharType));
            mSize=mSizeAllocated;
        }

        return *this;
    }

	String& operator+=(const CharType c )
	{
		if( c )
		{
			mSizeAllocated+= (mSize>0)?1:2;
			ResizeBuffer();
			mElems[mSize>0?mSize-1:0] = c;
			mElems[mSize>0?mSize:1] = 0;
			mSize=mSizeAllocated;
		}
		return *this;
	}

    int Len() const
    {
		return mSize > 0 ? mSize-1 : 0;
    }

    String& operator+=(const CharType* str)
    {
        if(str) {
            int len=(int)::strlen(str);
            if(len>0) {
                ++len; // trailing zero

                // resize buffer
                mSizeAllocated+=mSize>0 ? len-1 : len;
                ResizeBuffer();

                // append string
                ::memcpy(mElems+(mSize>0 ? mSize-1 : 0),str,len*sizeof(CharType));
                mSize=mSizeAllocated;
            }
        }

        return *this;
    }

	String& operator=(const String& a)
	{
		if(this != &a)
		{
			Resize(a.mSize);
			if( mSize )
				::memcpy( mElems, a.mElems, mSize );
		}
		return *this;
	}

    String operator+(const String& str) const
    {
        String s(*this);
        s+=str;
        return s;
    }

	String operator+(const CharType c) const
	{
		String s(*this);
		s+=c;
		return s;
	}

    String operator+(const CharType* str) const
    {
        String s(*this);
        s+=str;
        return s;
    }

    String SubStr(const int size, const int pos=0) const
    {
        const int count=(pos+size)>mSize ? mSize-pos : size;        		
		
		if (size < 1 || count < 1)
			return "";

		String str;
        str.Resize(count+1);
        ::memcpy(str.mElems,mElems+pos,count*sizeof(CharType));
        str[count]=0;
        return str;
    }

    String& ToLower()
    {
        if(mSize>0) {
            ::strlwr(mElems);
        }
        return *this;
    }

    String LowerCase() const
    {
        String s(*this);
        s.ToLower();
        return s;
    }

    const CharType* Find(const CharType* str) const { return mSize>0?::strstr(AsChar(),str):NULL; }
    const CharType* Find(const String& str) const { return mSize>0?::strstr(AsChar(),str.AsChar()):NULL; }
	bool operator==(const String& s) const 
    { 
        if (mSize==0 && s.mSize==0) return true;
        if (mSize==0) return false;
        if (s.mSize==0) return false;
        return strcmp(s.AsChar(),mElems)==0; 
    }
	bool operator!=(const String& s) const 
    { 
        if (mSize==0 && s.mSize==0) return false;
        if (mSize==0) return true;
        if (s.mSize==0) return true;
        return strcmp(s.AsChar(),mElems)!=0; 
    }
	bool operator>(const String& s) const 
	{ 
		if (mSize==0 && s.mSize==0) return false;
		if (mSize==0) return false;
		if (s.mSize==0) return true;
		return strcmp(mElems,s.AsChar())>0; 
	}
    bool EqualsNoCase(const String& s) const 
    { 
        if (mSize==0 && s.mSize==0) return true;
        if (mSize==0) return false;
        if (s.mSize==0) return false;
        return !stricmp(s.AsChar(),mElems); 
    }
	bool GreaterNoCase(const String& s) const 
	{ 
		if (mSize==0 && s.mSize==0) return false;
		if (mSize==0) return false;
		if (s.mSize==0) return true;
		return stricmp(mElems,s.AsChar())>0; 
	}

    bool IsPrefix(const String& prefix) const { return !::strncmp(AsChar(),prefix.AsChar(), prefix.Size()-1); }
	bool ValidAsGameSpyCDKey( void );
	bool TestPrintable( void );

    // general utils
    static void Tokenize(const char* str,
                         const char* delims,
                         DynamicArray<String>& tokens);

    static void TokenizeWithLiteral(const char* str,
                         const char* delims,
                         DynamicArray<String>& tokens, const char literal );

    // file name utils
    static String BaseName(const String& fileName);
    static String StripExtension(const String& fileName);
	static String GetExtension(const String& fileName);
	static String GetPath(const String& fileName);
	static String Sprintf(const char *str, ...);
	static String GenerateRandom( void );
};
