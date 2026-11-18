/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

//
// String Internal Representation
//


char* hkString::Rep::getData()
{
	return reinterpret_cast<char*>(this+1);
}

const char* hkString::Rep::getData() const
{
	return reinterpret_cast<const char*>(this+1);
}

char* hkString::Rep::refCopy()
{
	++m_references;
	return getData();
}

int hkString::Rep::getSize() const
{
	return hkSizeOf(Rep)+m_capacity+1;
}

void hkString::Rep::unRef()
{
	if(--m_references < 0)
	{
		freeMemory();
	}
}

//
// private methods
//
inline hkString::hkString(Rep* r)
	: m_string(r->getData())
{
}

// inline methods used by other inlines come first

inline hkString::Rep& hkString::getRep() const
{
	return *(reinterpret_cast<Rep*>(m_string)-1);
}

inline hkString::Rep& hkString::getEmptyStringRep()
{
	return *reinterpret_cast<Rep*>(s_emptyStringRep);
}

inline int hkString::getLength() const
{
	return getRep().m_length;
}

//
// constructors
//

inline hkString::hkString()
{
	m_string = getEmptyStringRep().refCopy();
}

inline hkString::hkString(const hkString& other)
{
	m_string = other.getRep().refCopy();
}

inline hkString::hkString(const char* s)
{
	int slen = strLen(s);
	m_string = Rep::create(slen)->getData();
	hkString::memCpy(m_string, s, slen+1); // copy null too
}

inline hkString& hkString::operator= (const hkString& other)
{
	char* newStr = other.getRep().refCopy();
	getRep().unRef();
	m_string = newStr;
	return *this;
}
		
inline hkString& hkString::operator= (const char* s)
{
	int slen = strLen(s);
	// create new rep if current one is too small or shared.
	if(getRep().m_capacity < slen || getRep().m_references > 0)
	{
		getRep().unRef();
		m_string = Rep::create(slen)->getData();
	}
	hkString::memCpy(m_string, s, slen+1); // copy null too
	getRep().m_length = slen;
	return *this;
}

inline hkString::~hkString()
{
	getRep().unRef();
}

inline char hkString::operator[] (int index) const
{
	return m_string[index];
}

inline const char* hkString::cString() const
{
	return m_string;
}

inline int hkString::compareTo(const hkString& other) const
{
	return strCmp( cString(), other.cString() );
}
inline int hkString::compareTo(const char* other) const
{
	return strCmp( cString(), other );
}

inline int hkString::compareToIgnoreCase(const hkString& other) const
{
	return strCasecmp( cString(), other.cString() );
}

inline int hkString::compareToIgnoreCase(const char* other) const
{
	return strCasecmp( cString(), other );
}

inline bool hkString::operator< (const hkString& other) const
{
	return compareTo(other) < 0;
}

inline bool hkString::operator== (const hkString& other) const
{
	return compareTo(other) == 0;
}
inline bool hkString::operator!= (const hkString& other) const
{
	return compareTo(other) != 0;
}
inline bool hkString::operator== (const char* other) const
{
	return compareTo(other) == 0;
}
inline bool hkString::operator!= (const char* other) const
{
	return compareTo(other) != 0;
}

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
