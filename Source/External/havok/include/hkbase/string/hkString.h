/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HKBASE_HKSTRING_H
#define HKBASE_HKSTRING_H

#define HK_INT_MAX int(unsigned(-1)>>1)

/// String and buffer functions normally found in libc.
class hkString
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_STRING, hkString);
		struct Rep
		{
			inline char* getData();
			inline const char* getData() const;
			inline char* refCopy();
			inline int getSize() const;
			inline void unRef();
			void freeMemory();
			Rep* clone() const;
			static Rep* create(int slen);
			int m_length;
			int m_capacity;
			int	m_references;
		};

	public: // object methods

			/// Creates an empty string.
		hkString();

			/// Creates a new string as a copy from a null terminated character array.
		hkString(const char* s);

			/// Creates a new string as a copy of an existing one.
		hkString(const hkString& s);

			/// Copy of an existing string.
		hkString& operator=(const hkString& s);

			/// Copy of an existing c string.
		hkString& operator=(const char* s);

			/// Destroys this string.
		~hkString();

			/// Read only access the i'th character.
		char operator[] (int i) const;

			/// Overwrite the current value. Arguments are the same as for ::sprintf.
		void printf(const char* format, ...);
	
			/// Returns the first index of c given range, or -1 if not found.
		int indexOf (char c, int startIndex=0, int endIndex=HK_INT_MAX) const;

			/// Returns the last index of c, or -1 if not found.
		int lastIndexOf (char c, int startIndex=0, int endIndex=HK_INT_MAX) const;

			/// Read only access the internal buffer.
		const char* cString() const;

			// overload each comparison for char* to avoid creating temporary string objects.

			/// Returns <0,0,>0 if *this is lexicographically less than, equal to or greater than other.
		int compareTo(const hkString& other) const;

			/// Returns <0,0,>0 if *this is lexicographically less than, equal to or greater than other.
		int compareTo(const char* other) const;

			/// Returns <0,0,>0 if *this is lexicographically less than, equal to or greater than other, ignoring case.
		int compareToIgnoreCase(const hkString& other) const;

			/// Returns <0,0,>0 if *this is lexicographically less than, equal to or greater than other, ignoring case.
		int compareToIgnoreCase(const char* other) const;
	
			/// Convenience operator for use in map<>
		bool operator< (const hkString& s) const;

			/// Returns compareTo(s)==0
		bool operator== (const hkString& s) const;

			/// Returns compareTo(s)!=0
		bool operator!= (const hkString& s) const;

			/// Returns compareTo(s)==0
		bool operator== (const char* s) const;

			/// Returns compareTo(s)!=0
		bool operator!= (const char* s) const;

			/// Does this string begin with s?
		bool beginsWith(const hkString& s) const;

			/// Does this string begin with s?
		bool beginsWith(const char* s) const;

			/// Does this string end with s?
		bool endsWith(const hkString& s) const;

			/// Does this string end with s?
		bool endsWith(const char* s) const;

			/// Returns the concatenation of *this and other.
		hkString operator+ (const hkString& other) const;

			/// Returns the concatenation of *this and other.
		hkString operator+ (const char* other) const;

			/// Sets *this as the concatenation of *this and other.
		hkString& operator+= (const hkString& other);

			/// Sets *this as the concatenation of *this and other.
		hkString& operator+= (const char* other);


			/// Returns the number of characters in this string excluding the trailing NULL
		int getLength() const;

			/// Parameter to hkString::replace functions
		enum ReplaceType
		{
			REPLACE_ONE,
			REPLACE_ALL
		};

			/// Replaces all occurences of 'from' with 'to'.
		hkString replace(char from, char to, ReplaceType=REPLACE_ALL) const;

			/// Replaces all occurences of 'from' with 'to'.
		hkString replace(const hkString& from, const hkString& to, ReplaceType=REPLACE_ALL) const;

			/// Returns a copy of the string as lower case.
		hkString asUpperCase() const;

			/// Returns a copy of the string as upper case.
		hkString asLowerCase() const;

			/// Returns a copy of the substring from the index onward.
			/// The index begins at 0.
		hkString substr(int index, int maxChars=HK_INT_MAX) const;

	private:

		explicit hkString(Rep* r);
		Rep& getRep() const;
		char* m_string;
		static char s_emptyStringRep[ sizeof(int)+sizeof(int)+sizeof(int)+sizeof(char) ];
		static Rep& getEmptyStringRep();

	public: // static methods

			/// Return the upper case of character c
		static char HK_CALL toUpper( char c );
			/// Return the lower case of character c
		static char HK_CALL toLower( char c );

			/// Equivalent to sprintf except at most n characters are written.
		static int HK_CALL snprintf(char* buf, int n, const char* fmt, ...);
			/// Equivalent to snprintf, but with a va_list of arguments.
		static int HK_CALL vsnprintf(char* buf, int len, const char* fmt, hk_va_list hkargs);
			/// Printf formatting to a string buffer.
		static int HK_CALL sprintf(char* buf, const char* fmt, ...);
			/// Returns <=1,0,>=1 if s1 is lexicographically less than, equal to or greater than s2.
		static int HK_CALL strCmp(const char* s1, const char* s2 );
			/// Returns <=1,0,>=1 if s1 is lexicographically less than, equal to or greater than s2. Comparison is done using at most the first n characters.
		static int HK_CALL strNcmp(const char* s1, const char* s2, int n);
			/// Returns <=1,0,>=1 if s1 is lexicographically less than, equal to or greater than s2, ignoring case.
		static int HK_CALL strCasecmp(const char* s1, const char* s2 );
			/// Returns <=1,0,>=1 if s1 is lexicographically less than, equal to or greater than s2, ignoring case. Comparison is done using at most the first n characters.
		static int HK_CALL strNcasecmp(const char* s1, const char* s2, int n);
			/// Copy null terminated src into dst. dst must be large enough to hold src.
		static void HK_CALL strCpy(char* dst, const char* src);
			/// Copy at most n characters of null terminated src into dst. dst must be large enough to hold src.
		static void HK_CALL strNcpy(char* dst, const char* src, int n);
			/// Return the length of null terminated string src.
		static int HK_CALL strLen(const char* src);

			/// Return an integer representing the string (signed, undefined for invalid input)
		static int HK_CALL atoi(const char* in);

			/// Return a hkReal representing the string.
		static hkReal HK_CALL atof(const char* in); 

			/// Return the first occurance of needle in haystack or null if not found.
		static const char* HK_CALL strStr(const char* haystack, const char* needle);

			/// Return the first occurance of needle in haystack or null if not found.
		static const char* HK_CALL strChr(const char* haystack, int needle);
			/// Return the last occurance of needle in haystack or null if not found.
		static const char* HK_CALL strRchr(const char* haystack, int needle);

			/// Return a copy of string src. This copy is allocated using hkAllocate<char> and therefore it should be deallocated using hkDeallocate<char>
		static char* HK_CALL strDup(const char* src);
			/// Change src to lower case in place.
		static char* HK_CALL strLwr(char* src);
			/// Change src to upper case in place.
		static char* HK_CALL strUpr(char* src);

			/// Copy n bytes of src into dst.
		static void HK_CALL memCpy(void* dst, const void* src, int n);
			/// Set n bytes of dst to c.
		static void HK_CALL memSet(void* dst, const int c, int n);
			/// Returns <=1,0,>=1 if n bytes of buf1 is less than, equal to or greater than those of buf2.
		static int HK_CALL memCmp(const void* buf1, const void* buf2, int n);
};

#undef HK_INT_MAX

#include <hkbase/string/hkString.inl>

#endif // HKBASE_HKSTRING_H

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
