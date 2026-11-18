/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

inline hkStreambuf*	hkOstream::getStreambuf()
{
	return m_streambuf;
}

inline hkOstream& hkOstream::operator<< (signed char c)
{
	return (*this) << static_cast<char>(c);
}

inline hkOstream& hkOstream::operator<< (unsigned char c)
{
	return (*this) << static_cast<char>(c);
}

inline hkOstream& hkOstream::operator<< (const signed char* s)
{
	return (*this) << reinterpret_cast<const char*>(s);
}

inline hkOstream& hkOstream::operator<< (const unsigned char* s)
{
	return (*this) << reinterpret_cast<const char*>(s);
}

inline hkOstream& hkOstream::operator<< (double d)
{
	return (*this) << static_cast<float>(d);
}

inline hkOstream& hkOstream::operator<< (hkOstreamManip::function m)
{
	return (*m)(*this);
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
