// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/04 Bart Sekura
//----------------------------------------------------------------------------

// #pragma once
#ifndef _LESTRING_
#define _LESTRING_

#include "Core/Debug.h"
#ifndef TRANSGAMING
# include "Core/DynArray.h"
#else
# include "nommgr.h"
# include <string.h>
# include "mmgr.h"
//# include "Core/DynArray.h"
# include "FileSystem/GFile.h"
#endif

typedef char CharType;

#ifdef TRANSGAMING

//#define STR_MSG  TRACE
#define STR_MSG(fmt, args...) {}

//class String: reimplementation of the string class that has no dependency on the 
//  DynamicArray<> template, and is a little nicer on the system memory.  This class
//  is highly overused as it stands, and any little bit of boost in its performance
//  can help.  
//  NOTE: a lot of the functions in this class have been copy & pasted on purpose.
//      This means there are a lot of places where new/delete are called instead
//      of having just one central allocation/deallocation point.  The reason for
//      this is to allow memory tracking to be more accurate and useful in debugging
//      situations.

class ENGINE_API String
{
private:
    //buffer that the string is stored in
    CharType *  mStr;

    //number of characters in the string.  This should be the same value as
    //would be returned from strlen(mStr)
    size_t      mSize;

    //number of characters allocated to the string.  Note that this is not
    //expressed in bytes, the actual byte size of the string would be 
    //mAllocated * sizeof(CharType)
    size_t      mAllocated;


    String(CharType *str, size_t size, size_t allocated){
        mStr = str;
        mSize = size;
        mAllocated = allocated;
    }
public:
    //********************************** Constructors & destructor ********************************//
	String(){
        STR_MSG("String::String(): creating a null string\n");
        mStr = NULL;

        Clear();
    }
    String(const String &s){
        STR_MSG("String::String(const String &): copying the string '%s'\n", s.mStr);
        if (s.mSize > 0){
            mSize = s.mSize;
            mAllocated = mSize + 1;
            mStr = new CharType[mAllocated];

            ::memcpy(mStr, s.mStr, mAllocated * sizeof(CharType));
        }

        else{
            mStr = NULL;

            Clear();
        }
    }
    String(const CharType *s){
        STR_MSG("String::String(const CharType *): copying the string '%s'\n", s);
        if (s[0]){
            mSize = strlen(s);
            mAllocated = mSize + 1;
            mStr = new CharType[mAllocated];

            ::memcpy(mStr, s, mAllocated * sizeof(CharType));
        }

        else{
            mStr = NULL;

            Clear();
        }
    }
    /*String(const char c, int count){
        STR_MSG("String::String(const char, int): creating a cleared string with character '%d' (count = %d)\n", c, count);
        if (count <= 0){
            mStr = NULL;

            Clear();
        }

        else{
            mSize = count;
            mAllocated = mSize + 1;

            mStr = new CharType[mAllocated];

            ::memset(mStr, c, mSize * sizeof(CharType));
            mStr[mSize] = 0;
        }
	}//*/

    ~String(){
        STR_MSG("String::~String(): destroying the string '%s'\r\n", mStr);
        if (m_getLoggingState())
            m_logMsg("String::~String(): destroying the string '%s'\r\n", mStr);

        Clear();
    }

    //****************************************** Accessors **********************************************//
	inline const CharType * AsChar() const{
        return mStr ? mStr : "";
    }
    inline int Len() const{
		return mSize;
    }
    inline size_t Size() const{
        return mSize;
    }
    inline void CalcLen(){
        mSize = strlen(mStr);
    }

    inline CharType &operator[](const int i) const{

        //this is a null string => return 0 iff the requested index is 0
        if (mStr == NULL){
            static CharType null = 0;
            Assert(i == 0);

            null = 0;
            return null;
        }

#if defined(TRANSGAMING) && defined(DEBUG)
        if (i < 0){
            TRACE("String::operator[](): ERROR-> index past the start of the string '%s'! (index = %d, size = %d)\n", mStr, i, mSize);
            HALT();
        }
        else if (i > mSize){
            TRACE("String::operator[](): ERROR-> index past the end of the string '%s'! (index = %d, size = %d)\n", mStr, i, mSize);
            HALT();
        }
#else
        Assert(i >= 0);
        Assert(i <= mSize);
#endif

        return mStr[i]; 
    }

    //***************************************** Serialization functions *********************************//
	friend GFile &operator<<(GFile &file, const String &str){
        STR_MSG("String::operator<<(): writing the string '%s' to file\n", str.mStr);
#ifdef DEBUG
        //make sure the correct number of bytes will be read for the string length
        if (sizeof(int) != sizeof(size_t))
            TRACE("String::operator<<(): WARNING-> size of int is not the same as the size of size_t!\n");
#endif

        //null string => just write out 0 to indicate an empty string
        if (str.mSize == 0)
            file << str.mSize;

        else{
            //write out the string's length (including the terminating null)
            size_t size = str.mSize + 1;


            file << size;


            //write the full string including terminating null
            if (str.mSize > 0)
                file.Write(str.mStr, str.mSize + 1);

            else{
                CharType tmpStr = 0;
                file.Write(&tmpStr, sizeof(tmpStr));
            }
        }
        
        return file;
    }

	friend GFile& operator>>(GFile& file, String &str){
        size_t  size;


#ifdef DEBUG
        //make sure the correct number of bytes will be read for the string length
        if (sizeof(int) != sizeof(size_t))
            STR_MSG("String::operator<<(): WARNING-> size of int is not the same as the size of size_t!\n");
#endif

        //read in the number of characters in the string (including the terminating null)
        file >> size;

        //no objects to read => skip the contents of the string
        if (size == 0)
            str.Clear();

        else{
            str.mSize = size - 1;


            //string will be empty after being read in => prevent 1 byte from being allocated
            if (str.mSize == 0){
                CharType tmpStr;

                file.Read(&tmpStr, sizeof(tmpStr));
                str.Clear();
            }

            else{
                //the current buffer isn't large enough to hold the string => resize the buffer
                if (size > str.mAllocated){
                    if (str.mStr)
                        delete []str.mStr;

                    str.mAllocated = str.mSize + 1;
                    str.mStr = new CharType[str.mAllocated];
                }

                //read in the string
                if (size)
                    file.Read(str.mStr, size);

                str.mStr[str.mSize] = 0;
            }

            STR_MSG("String::operator>>(): read the string '%s' from file\n", str.mStr);
        }

        return file;
    }


    //***************************************** Concatenation operators *********************************//
    String &operator+=(const String &str){
        STR_MSG("String::operator+=(const String &): adding the string '%s' to '%s'\n", str.mStr, mStr);
        if (this != &str && str.mSize > 0){

            //current buffer isn't large enough to accomodate the new string => resize it
            if (mAllocated < mSize + str.mSize + 1){
                CharType *tmp;


                mAllocated = mSize + str.mSize + 1;
                tmp = new CharType[mAllocated];

                if (mStr){
                    ::memcpy(tmp, mStr, mSize * sizeof(CharType));

                    delete []mStr;
                }

                mStr = tmp;
            }

            ::memcpy(&mStr[mSize], str.mStr, (str.mSize + 1) * sizeof(CharType));

            mSize += str.mSize;
        }

        return *this;
    }
	String &operator+=(const CharType *str){
        STR_MSG("String::operator+=(const CharType *): adding the string '%s' to '%s'\n", str, mStr);
		if (str && str[0]){
            size_t      len = strlen(str);


            if (mAllocated < mSize + len + 1){
                CharType *  tmp;


                mAllocated = mSize + len + 1;
                tmp = new CharType[mAllocated];

                if (mStr){
                    ::memcpy(tmp, mStr, mSize * sizeof(CharType));

                    delete []mStr;
                }

                mStr = tmp;
            }

            ::memcpy(&mStr[mSize], str, (len + 1) * sizeof(CharType));

            mSize += len;
		}
		return *this;
	}
	String &operator+=(const CharType c){
        STR_MSG("String::operator+=(const CharType): adding the character %d to '%s'\n", c, mStr);
		if (c){
            if (mAllocated < mSize + 1 + 1){
                CharType *tmp;


                if (mSize)
                    mAllocated++;

                else
                    mAllocated = 2;


                tmp = new CharType[mAllocated];

                if (mStr){
                    ::memcpy(tmp, mStr, (mSize + 1) * sizeof(CharType));

                    delete []mStr;
                }

                mStr = tmp;
            }

            mStr[mSize] = c;
            mStr[mSize + 1] = 0;

            mSize++;
		}

		return *this;
	}

    String operator+(const String& str) const{
        STR_MSG("String::operator+(const String &): adding the string '%s' to '%s'\n", str.mStr, mStr);
        if (str.mSize <= 0)
            return String(*this);


        size_t      size = str.mSize + mSize;
        size_t      allocated = size + 1;
        CharType *  tmp = new CharType[allocated];
        

        if (mStr)
            ::memcpy(tmp, mStr, mSize * sizeof(CharType));

        ::memcpy(&tmp[mSize], str.mStr, (str.mSize + 1) * sizeof(CharType));

        return String(tmp, size, allocated);
    }
	String operator+(const CharType c) const{
        STR_MSG("String::operator+(const CharType): adding the character %d to '%s'\n", c, mStr);
        size_t      size = mSize == 0 ? 2 : (mSize + 1);
        size_t      allocated = size + 1;
        CharType *  tmp = new CharType[allocated];
        

        if (mStr)
            ::memcpy(tmp, mStr, mSize * sizeof(CharType));

        tmp[mSize] = c;
        tmp[mSize + 1] = 0;

        return String(tmp, size, allocated);
	}
    String operator+(const CharType *str) const{
        STR_MSG("String::operator+(const CharType *): adding the string '%s' to '%s'\n", str, mStr);
        if (str == NULL || str[0] == 0)
            return String(*this);


        size_t      len = strlen(str);
        size_t      size = len + mSize;
        size_t      allocated = size + 1;
        CharType *  tmp = new CharType[allocated];
        

        if (mStr)
            ::memcpy(tmp, mStr, mSize * sizeof(CharType));

        ::memcpy(&tmp[mSize], str, (len + 1) * sizeof(CharType));

        return String(tmp, size, allocated);
    }

    //***************************************** Assignment operators ***********************************//
	String &operator=(const String &a){
        STR_MSG("String::operator=(const String &): assigning the string '%s'\n", a.mStr);
        //make sure some idiot isn't trying to assign a string to itself
        if (this != &a){

            //source string is NULL => destroy this string as well (the null string is treated specially)
            if (a.mSize == 0 || a.mStr == NULL){
                if (mStr)
                    delete []mStr;

                mStr = NULL;
                mSize = 0;
                mAllocated = 0;

                return *this;
            }


            if (mStr){

                //the new string fits in our current buffer => reuse it
                if (mAllocated >= a.mSize + 1){
                    mSize = a.mSize;

                    ::memcpy(mStr, a.mStr, (mSize + 1) * sizeof(CharType));
                    
                    return *this;
                }

                //source string is longer than the buffer => resize the buffer
                else
                    delete []mStr;
            }

            mSize = a.mSize;
            mAllocated = mSize + 1;

            mStr = new CharType[mAllocated];
            ::memcpy(mStr, a.mStr, mAllocated * sizeof(CharType));
        }

        return *this;
	}
    String &operator=(const CharType *str){
        STR_MSG("String::operator=(const CharType *): assigning the string '%s'\n", str);

        //source string is NULL => destroy this string as well (the null string is treated specially)
        if (str == NULL || str[0] == 0){
            if (mStr)
                delete []mStr;

            mStr = NULL;
            mSize = 0;
            mAllocated = 0;

            return *this;
        }


        size_t  len = strlen(str);

        if (mStr){

            //the new string fits in our current buffer => reuse it
            if (mAllocated >= len + 1){
                mSize = len;

                ::memcpy(mStr, str, (mSize + 1) * sizeof(CharType));
                
                return *this;
            }

            //source string is longer than the buffer => resize the buffer
            else
                delete []mStr;
        }

        mSize = len;
        mAllocated = mSize + 1;

        mStr = new CharType[mAllocated];
        ::memcpy(mStr, str, mAllocated * sizeof(CharType));

        return *this;
    }

    void Clear(){
        if (mStr){
            STR_MSG("String::Clear(): clearing the string '%s'\n", mStr);

            delete []mStr;
        }

        mStr = NULL;
        mSize = 0;
        mAllocated = 0;
    }

    /*void Resize(size_t size, bool copy = false){
        //nothing to do!
        if (size <= mAllocated)
            return;


        size_t s = mSize;

        if (size < mSize - 1)
            s = mSize;
        

        mAllocated = size;
        CharType *tmp = new CharType[mAllocated];

        if (mStr && copy){
            ::memcpy(tmp, mStr, s * sizeof(CharType));

            delete []mStr;

            mStr = tmp;
            mStr[mSize] = 0;
            mSize = s - 1;
        }

        else{
            if (mStr)
                delete []mStr;

            mStr = tmp;
            mStr[0] = 0;
            mSize = 0;
        }

    }//*/

    //**************************************** Manipulation functions *********************************//
    String SubStr(const size_t s, const int pos = 0) const{
        STR_MSG("String::SubStr(): grabbing the substring of '%s' from %d with size %d\n", mStr, pos, s);

        //invalid index, length, or string => return a null string
        if (s < 1 || pos < 0 || pos >= mSize || mSize == 0 || mStr == NULL)
            return String();

        size_t      size = s;
        size_t      allocated;
        CharType *  tmp;


        //clamp the size to the string's length
        if (size > mSize - pos)
            size = mSize - pos;

        allocated = size + 1;

        tmp = new CharType[allocated];

        ::memcpy(tmp, &mStr[pos], size * sizeof(CharType));
        tmp[size] = 0;

        return String(tmp, size, allocated);
    }

    String &ToLower(){
        if (mSize > 0 && mStr)
            ::strlwr(mStr);

        return *this;
    }

    String LowerCase() const{
        STR_MSG("String::LowerCase(): creating a lower case version of '%s'\n", mStr);

        //this string is empty => return a null string
        if (mSize == 0 || mStr == NULL)
            return String();

        size_t      size = mSize;
        size_t      allocated = mSize + 1;
        CharType *  tmp = new CharType[allocated];


        ::memcpy(tmp, mStr, allocated * sizeof(CharType));
        ::strlwr(tmp);

        return String(tmp, size, allocated);
    }

    const CharType* Find(const CharType* str) const{
        STR_MSG("String::Find(const CharType *): searching the string '%s' for '%s'\n", mStr, str);
        if (mSize <= 0 || mStr == NULL || str == NULL || str[0] == 0)
            return NULL;

        return ::strstr(mStr, str);
    }

    const CharType* Find(const String& str) const{
        STR_MSG("String::Find(const String &): searching the string '%s' for '%s'\n", mStr, str.mStr);
        if (mSize <= 0 || mStr == NULL || str.mSize == 0 || str.mStr == NULL)
            return NULL;

        return ::strstr(mStr, str.mStr);
    }

    int Find(const CharType e){
        STR_MSG("String::Find(const CharType): searching the string '%s' for %d\n", mStr, e);
        if (mStr == NULL || mSize == 0)
            return -1;

        const char *val = strchr(mStr, e);

        if (val == NULL)
            return -1;

        return val - mStr;
    }
    //************************************ Comparison Operators **************************************//
    bool operator==(const CharType *s) const{ 
        if (mSize == 0 && s[0] == 0)
            return true;

        if (mStr == s)
            return true;

        if (mSize == 0)
            return false;

        if (s[0] == 0)
            return false;

        return strcmp(s, mStr) == 0; 
    }
	bool operator!=(const CharType *s) const{ 
        if (mSize == 0 && s[0] == 0)
            return false;

        if (mStr == s)
            return false;

        if (mSize == 0)
            return true;

        if (s[0] == 0)
            return true;

        return strcmp(s, mStr) != 0; 
    }
	bool operator>(const CharType *s) const{ 
        if (mSize == 0 && s[0] == 0)
            return false;

        if (mStr == s)
            return false;

        if (mSize == 0)
            return false;

        if (s[0] == 0)
            return true;
        
		return strcmp(mStr, s) > 0; 
	}
    bool EqualsNoCase(const CharType *s) const{ 
        if (mSize == 0 && s[0] == 0)
            return true;

        if (mStr == s)
            return true;

        if (mSize == 0)
            return false;

        if (s[0] == 0)
            return false;

        return !stricmp(s, mStr); 
    }
	bool GreaterNoCase(const CharType *s) const{ 
        if (mSize == 0 && s[0] == 0)
            return false;

        if (mStr == s)
            return false;

        if (mSize == 0)
            return false;

        if (s[0] == 0)
            return true;

		return stricmp(mStr, s) > 0; 
	}
    bool operator==(const String& s) const{ 
        if (mSize == 0 && s.mSize == 0)
            return true;

        if (mStr == s.mStr)
            return true;

        if (mSize != s.mSize)
            return false;

        if (mSize == 0)
            return false;

        if (s.mSize == 0)
            return false;

        return strcmp(mStr, s.mStr) == 0; 
    }
	bool operator!=(const String& s) const{ 
        if (mSize == 0 && s.mSize == 0)
            return false;

        if (mStr == s.mStr)
            return false;

        if (mSize == s.mSize)
            return false;

        if (mSize == 0)
            return true;

        if (s.mSize==0)
            return true;

        return strcmp(s.mStr, mStr) != 0; 
    }
	bool operator>(const String& s) const{ 
		if (mSize == 0 && s.mSize == 0)
            return false;

        if (mStr == s.mStr)
            return false;

		if (mSize == 0)
            return false;

		if (s.mSize == 0)
            return true;

		return strcmp(mStr, s.mStr) > 0; 
	}
    bool EqualsNoCase(const String& s) const{ 
        if (mSize == 0 && s.mSize == 0)
            return true;

        if (mStr == s.mStr)
            return true;

        if (mSize != s.mSize)
            return false;

        if (mSize == 0)
            return false;

        if (s.mSize == 0)
            return false;

        return !stricmp(s.mStr, mStr); 
    }
	bool GreaterNoCase(const String& s) const{ 
		if (mSize == 0 && s.mSize==0)
            return false;

        if (mStr == s.mStr)
            return false;

		if (mSize == 0)
            return false;

		if (s.mSize == 0)
            return true;

		return stricmp(mStr,s.mStr) > 0; 
	}

    bool IsNull() const{
        return mStr == NULL || mStr[0] == 0;
    }

    bool IsPrefix(const String& prefix) const{
        if (prefix.mSize > mSize)
            return false;

        return !::strncmp(mStr, prefix.mStr, prefix.mSize);
    }

    bool IsPrefix(const char *prefix) const{
        size_t len = strlen(prefix);


        if (len > mSize)
            return false;

        return !::strncmp(mStr, prefix, len);
    }

    bool TestPrintable();

    //****************************** Global comparison operators **********************************//
    friend bool operator==(const CharType *s1, const String &s2){ 
        TRACE("String::operator==(const char *, const String &): comparing '%s' to '%s'\n", s1, s2.mStr);
        if (s2.mSize == 0 && s1[0] == 0)
            return true;

        if (s2.mStr == s1)
            return true;

        if (s2.mSize == 0)
            return false;

        if (s1[0] == 0)
            return false;

        return strcmp(s1, s2.mStr) == 0; 
    }

    friend bool operator!=(const CharType *s1, const String &s2){ 
        TRACE("String::operator!=(const char *, const String &): comparing '%s' to '%s'\n", s1, s2.mStr);
        if (s2.mSize == 0 && s1[0] == 0)
            return false;

        if (s2.mStr == s1)
            return false;

        if (s2.mSize == 0)
            return true;

        if (s1[0] == 0)
            return true;

        return strcmp(s1, s2.mStr) != 0; 
    }
    friend bool operator>(const CharType *s1, const String &s2){ 
        TRACE("String::operator>(const char *, const String &): comparing '%s' to '%s'\n", s1, s2.mStr);
        if (s2.mSize == 0 && s1[0] == 0)
            return false;

        if (s2.mStr == s1)
            return false;

        if (s2.mSize == 0)
            return false;

        if (s1[0] == 0)
            return true;
        
	    return strcmp(s2.mStr, s1) > 0; 
    }

    //*********************************** Static Manipulators *****************************************//
    static char *   StripBaseName(const char *str, char *buf, size_t len);
    static char *   StripExtension(const char *str, char *buf, size_t len);
    static bool     StrSuffix(const char *str, const char *suffix);
    static bool     StrCaseSuffix(const char *str, const char *suffix);
    //static String BaseName(const String& fileName);
    //static String StripExtension(const String& fileName);
	static String GetPath(const String& fileName);
    static char * GetPath(const char *filename, char *buf, size_t len);
    static const char *GetFilename(const char *path);
	static String Sprintf(const char *str, ...);
	static String GenerateRandom();
    static const char *Strcasestr(const char *str1, const char *str2);
};
#else


class ENGINE_API String : public DynamicArray<CharType>
{
public:
	String()
    :	DynamicArray<CharType>()
    {}

#ifdef DYNARRAY_TRACK_CALLS
    String(const String& s): 
        DynamicArray<CharType>(s.Size(), "[Util]::String::String(const String &)"){

        CopyElements(s, IsComplexType());
    }
#else
	String(const String& s)
    :	DynamicArray<CharType>(s)
    {}
#endif

	String( const char c, int count )
    :	DynamicArray<CharType>()
    {
	    int size = count+1;
#ifdef DYNARRAY_TRACK_CALLS
        ResizeArrayM(size, "[Util]::String::String(const char, int)");
#else
	    ResizeArray(size);
#endif
	    ::memset(mElems,c,mSize*sizeof(CharType));
		mElems[count] = 0;
	}

	String(const CharType* s)
    :	DynamicArray<CharType>()
    {
        Assert(s);
	    int size = (int)(::strlen(s)+1);
#ifdef DYNARRAY_TRACK_CALLS
        ResizeArrayM(size, "[Util]::String::String(const CharType)");
#else
	    ResizeArray(size);
#endif
	    ::memcpy(mElems,s,mSize*sizeof(CharType));
    }

#ifdef DYNARRAY_TRACK_CALLS
    ~String(){
        if (m_getLoggingState())
            m_logMsg("String::~String(): destroying the string '%s'\r\n", mElems);
    }
#endif

    const CharType* AsChar() const { return mElems; }

    String& operator+=(const String& str)
    {
        if(this != &str && str.Size()) {
            // resize buffer
            mSizeAllocated+=mSize>0 ? str.Size()-1 : str.Size();
#ifdef DYNARRAY_TRACK_CALLS
            ResizeBuffer("[Util]::String::operator+=(const String &)");
#else
            ResizeBuffer();
#endif

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
#ifdef DYNARRAY_TRACK_CALLS
            ResizeBuffer("[Util]::String::operator+=(const CharType)");
#else
            ResizeBuffer();
#endif
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
#ifdef DYNARRAY_TRACK_CALLS
                ResizeBuffer("[Util]::String::operator+=(const CharType *)");
#else
                ResizeBuffer();
#endif

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
#ifdef DYNARRAY_TRACK_CALLS
            TRACE("String::operator=(const String &): copying the string '%s'\n", a.AsChar());
            ResizeArrayM(a.mSize, "[Util]::String::operator=(const String &)");
#else
			ResizeArray(a.mSize);
#endif
			if( mSize )
				::memcpy( mElems, a.mElems, mSize );
		}
		return *this;
	}
    /*String &operator=(const CharType *s){
        if (s == NULL || s[0] == 0)

        size_t len = strlen s;

        return *this;
    }//*/

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
#ifdef DYNARRAY_TRACK_CALLS
        str.ResizeArrayM(count + 1, "[Util]::String::SubStr()");
#else
        str.ResizeArray(count+1);
#endif
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

    bool IsNull() const{
        return mElems == NULL || mElems[0] == 0;
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
	static String GetPath(const String& fileName);
	static String GetExtension(const String& fileName);
	static String Sprintf(const char *str, ...);
	static String GenerateRandom( void );
};
#endif  //ifdef TRANSGAMING

#endif //_LESTRING_
