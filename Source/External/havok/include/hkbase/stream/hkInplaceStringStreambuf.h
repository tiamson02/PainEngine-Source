/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HKBASE_HKINPLACESTRINGSTREAMBUF_H
#define HKBASE_HKINPLACESTRINGSTREAMBUF_H


/// A streambuf for in-memory reading and writing streams.
/// You can use the convenience classes hkIstringStream, hkOstringStream
/// instead of using this class directly.
template<int N=1024>
class hkInplaceStringStreambuf : public hkStreambuf
{
	public:

			/// Create a streambuf for writing.
		hkInplaceStringStreambuf();

			/// Get the current null terminated written string.
		const char* getString() const;

	protected:

		virtual int sysWrite(const char* buf, int nbytes);
		virtual int sysRead(char* buf, int nbytes);
		virtual void sysFlush();

	private:

		hkInplaceArray<char,N> m_writeBuffer;
};

template<int N>
hkInplaceStringStreambuf<N>::hkInplaceStringStreambuf()
{
	// ensure we're null terminated
	m_writeBuffer.pushBack(HK_NULL);
}

template<int N>
const char* hkInplaceStringStreambuf<N>::getString() const
{
	return m_writeBuffer.begin();
}

template<int N>
int hkInplaceStringStreambuf<N>::sysWrite(const char* buf, int nbytes)
{
	// use the noncopying array constructor
	hkArray<char> data( const_cast<char*>(buf), nbytes, nbytes);
	m_writeBuffer.insertAt( m_writeBuffer.getSize()-1, data);
	return nbytes;
}

template<int N>
int hkInplaceStringStreambuf<N>::sysRead(char* buf, int nbytes)
{
	// we set up the getArea in the constructor and thats all we can read.
	return 0;
}

template<int N>
void hkInplaceStringStreambuf<N>::sysFlush()
{
}


#endif // HKBASE_HKINPLACESTRINGSTREAMBUF_H

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
