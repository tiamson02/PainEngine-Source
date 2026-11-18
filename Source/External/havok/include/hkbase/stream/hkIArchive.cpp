/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkbase/hkBase.h>
#include <hkbase/stream/hkStreambufFactory.h>
#include <hkbase/stream/hkIArchive.h>

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
hkIArchive::hkIArchive(hkStreambuf* sb)
	:	m_streambuf(sb)
{
	m_streambuf->addReference();
}


//
// Destructor
//
hkIArchive::~hkIArchive()
{
	m_streambuf->removeReference();
}


//
// read8
//
hkChar hkIArchive::read8()
{
	hkChar c;
	char* ptr = reinterpret_cast<char*>(&c);
	m_streambuf->read(ptr, 1);
	return c;
}


//
// read8u
//
hkUchar hkIArchive::read8u()
{
	hkUchar u;
	char* ptr = reinterpret_cast<char*>(&u);
	m_streambuf->read(ptr, 1);
	return u;
}


//
// read16
//
hkInt16 hkIArchive::read16()
{
	hkInt16 i;
	char* ptr = reinterpret_cast<char*>(&i);
	m_streambuf->read(ptr, 2);
	endianConvert16(ptr);
	return i;
}


//
// read16u
//
hkUint16 hkIArchive::read16u()
{
	hkUint16 u;
	char* ptr = reinterpret_cast<char*>(&u);
	m_streambuf->read(ptr, 2);
	endianConvert16(ptr);
	return u;
}


//
// read32
//
hkInt32 hkIArchive::read32()
{
	hkInt32 i;
	char* ptr = reinterpret_cast<char*>(&i);
	m_streambuf->read(ptr, 4);
	endianConvert32(ptr);
	return i;
}


//
// read32u
//
hkUint32 hkIArchive::read32u()
{
	hkUint32 u;
	char* ptr = reinterpret_cast<char*>(&u);
	m_streambuf->read(ptr, 4);
	endianConvert32(ptr);
	return u;
}


//
// read64
//
hkInt64 hkIArchive::read64()
{
	hkInt64 i;
	char* ptr = reinterpret_cast<char*>(&i);
	m_streambuf->read(ptr, 8);
	endianConvert64(ptr);
	return i;
}


//
// read64u
//
hkUint64 hkIArchive::read64u()
{
	hkUint64 u;
	char* ptr = reinterpret_cast<char*>(&u);
	m_streambuf->read(ptr, 8);
	endianConvert64(ptr);
	return u;
}


//
// readFloat32
//
hkFloat32 hkIArchive::readFloat32()
{
	hkFloat32 f;
	char* ptr = reinterpret_cast<char*>(&f);
	m_streambuf->read(ptr, 4);
	endianConvert32(ptr);
	return f;
}


//
// readDouble64
//
hkDouble64 hkIArchive::readDouble64()
{
	hkDouble64 d;
	char* ptr = reinterpret_cast<char*>(&d);
	m_streambuf->read(ptr, 8);
	endianConvert64(ptr);
	return d;
}


//
// isOK
//
bool hkIArchive::isOk() const
{
	return m_streambuf->isOk();
}



//
// read
//
int hkIArchive::read(char* buf, int nbytes)
{
	return m_streambuf->read(buf, nbytes);
}


//
// ReadFloats
//
void hkIArchive::readFloats(hkFloat32* array, int arraySize)
{

#if defined HK_ARCH_PPC

	// 4 byte alligned read (for 32 bit platforms)
	for (int  i = 0; i < arraySize; i++)
	{
		char* ptr = reinterpret_cast<char*>(&array[i]);

		// write
		m_streambuf->read(ptr, 4);
		
		// endian swap
		endianConvert32(ptr);
	}

#else

	char* ptr = reinterpret_cast<char*>(array);
	m_streambuf->read(ptr, arraySize * 4);

#endif

}



//
// getStreamBuf
//
hkStreambuf* hkIArchive::getStreambuf()
{
	return m_streambuf;
}


/// If m_deleteOnDestruct was set when the original stream was added,
/// the original stream is deleted.  It the newDeleteOnDestruct parameter
/// is true, newBuf will be deleted when this ostream is destroyed.
//
// setStreambuf
//
void hkIArchive::setStreambuf(hkStreambuf* newBuf)
{
	newBuf->addReference();
	m_streambuf->removeReference();
	m_streambuf = newBuf;
}



//
// Constructor
//
hkIfArchive::hkIfArchive(const char* filename)
	: hkIArchive( hkStreambufFactory::getInstance().open(filename, hkStreambuf::INPUT) )
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
