/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


#include <hkbase/hkBase.h>
#include <hkbase/stream/hkStreambufFactory.h>
#include <hkbase/stream/hkOArchive.h>


//
// endianConvert16
// converts 16 bits from little to big endian and vice versa (if applicable)
// all input data is assumed to be little endian
//
static inline void HK_CALL endianConvert16(char* data)
{

// only need to convert for big endian architectures
#if defined HK_ARCH_PPC

	char swapTemp;
	swapTemp = data[0];
	data[0] = data[1];
	data[1] = swapTemp;

#endif

}


//
// endianConvert32
// converts 32 bits from little to big endian and vice versa (if applicable)
// all input data is assumed to be little endian
//
static inline void HK_CALL endianConvert32(char* data)
{

// only need to convert for big endian architectures
#if defined HK_ARCH_PPC

	char swapTemp;
	swapTemp = data[0];
	data[0] = data[3];
	data[3] = swapTemp;
	swapTemp = data[1];
	data[1] = data[2];
	data[2] = swapTemp;;

#endif

}


//
// endianConvert64
// converts 16 bits from little to big endian and vice versa (if applicable)
// all input data is assumed to be little endian
//
static inline void HK_CALL endianConvert64(char* data)
{

// only need to convert for big endian architectures
#if defined HK_ARCH_PPC

	char swapTemp;
	swapTemp = data[0];
	data[0] = data[7];
	data[7] = swapTemp;
	swapTemp = data[1];
	data[1] = data[6];
	data[6] = swapTemp;
	swapTemp = data[2];
	data[2] = data[5];
	data[5] = swapTemp;
	swapTemp = data[3];
	data[3] = data[4];
	data[4] = swapTemp;

#endif

}



// If deleteOnDestruct is true then the streambuf will be deleted when
// this istream is destroyed. Usually this is what you want.
//
// constructor
//
hkOArchive::hkOArchive(hkStreambuf* sb)
	:	m_streambuf(sb)
{
	m_streambuf->addReference();
}


//
// Destructor
//
hkOArchive::~hkOArchive()
{
	m_streambuf->removeReference();
}


//
// write8
//
void hkOArchive::write8(hkChar c)
{
	char* ptr = reinterpret_cast<char*>(&c);
	m_streambuf->write(ptr, 1);
}


//
// write8u
//
void hkOArchive::write8u(hkUchar u)
{
	char* ptr = reinterpret_cast<char*>(&u);
	m_streambuf->write(ptr, 1);
}


//
// write16
//
void hkOArchive::write16(hkInt16 i)
{
	char* ptr = reinterpret_cast<char*>(&i);
	endianConvert16(ptr);
	m_streambuf->write(ptr, 2);
}


//
// write16u
//
void hkOArchive::write16u(hkUint16 u)
{
	char* ptr = reinterpret_cast<char*>(&u);
	endianConvert16(ptr);
	m_streambuf->write(ptr, 2);
}



//
// write32
//
void hkOArchive::write32(hkInt32 i)
{
	char* ptr = reinterpret_cast<char*>(&i);
	endianConvert32(ptr);
	m_streambuf->write(ptr, 4);
}


//
// write32u
//
void hkOArchive::write32u(hkUint32 u)
{
	char* ptr = reinterpret_cast<char*>(&u);
	endianConvert32(ptr);
	m_streambuf->write(ptr, 4);
}


//
// write64
//
void hkOArchive::write64(hkInt64 i)
{
	char* ptr = reinterpret_cast<char*>(&i);
	endianConvert64(ptr);
	m_streambuf->write(ptr, 8);
}


//
// write64u
//
void hkOArchive::write64u(hkUint64 u)
{
	char* ptr = reinterpret_cast<char*>(&u);
	endianConvert64(ptr);
	m_streambuf->write(ptr, 8);
}


//
// writeFloat32
//
void hkOArchive::writeFloat32(hkFloat32 f)
{
	char* ptr = reinterpret_cast<char*>(&f);
	endianConvert32(ptr);
	m_streambuf->write(ptr, 4);
}


//
// writeDouble64
//
void hkOArchive::writeDouble64(hkDouble64 d)
{
	char* ptr = reinterpret_cast<char*>(&d);
	endianConvert64(ptr);
	m_streambuf->write(ptr, 8);
}


//
// isOK
//
bool hkOArchive::isOk() const
{
	return m_streambuf->isOk();
}



//
// write
//
int hkOArchive::write(char* buf, int nbytes)
{
	return m_streambuf->write(buf, nbytes);
}


//
// WriteFloats
//
void hkOArchive::writeFloats(const hkFloat32* array, int arraySize)
{

#if defined HK_ARCH_PPC

	// 4 byte alligned write (for 32 bit platforms)
	for (unsigned  i = 0; i < arraySize; i++)
	{
		// copy the float such that we don't destroy
		// the original array when endian converting
		hkFloat32 f = array[i];
		char* ptr = reinterpret_cast<char*>(&f);
		
		// endian swap
		endianConvert32(ptr);

		// write
		m_streambuf->write(ptr, 4);
	}

#else
	
	const char* ptr = reinterpret_cast<const char*>(array);
	m_streambuf->write(ptr, arraySize * 4);

#endif

}


//
// getStreamBuf
//
hkStreambuf* hkOArchive::getStreambuf()
{
	return m_streambuf;
}


/// If m_deleteOnDestruct was set when the original stream was added,
/// the original stream is deleted.  It the newDeleteOnDestruct parameter
/// is true, newBuf will be deleted when this ostream is destroyed.
//
// setStreambuf
//
void hkOArchive::setStreambuf(hkStreambuf* newbuf)
{
	newbuf->addReference();
	m_streambuf->removeReference();
	m_streambuf = newbuf;	
}



//
// Constructor
//
hkOfArchive::hkOfArchive(const char* filename)
	: hkOArchive( hkStreambufFactory::getInstance().open(filename, hkStreambuf::OUTPUT) )
{
	m_streambuf->removeReference(); // base class adds a ref
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
