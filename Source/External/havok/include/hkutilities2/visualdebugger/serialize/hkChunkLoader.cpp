/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

// from base
#include <hkbase/hkBase.h>
#include <hkbase/stream/hkOArchive.h>

// class definition
#include <hkutilities2/visualdebugger/serialize/hkChunkLoader.h>

hkChunkLoader::hkChunkLoader(hkStreambuf* chunkStreambuf)
	: m_chunkStream(chunkStreambuf)
{
}

hkChunkLoader::~hkChunkLoader()
{

}

int hkChunkLoader::readChunk(hkStringStreambuf* chunkStreambuf)
{
	int chunkSize;
	int chunkType;

	// this code is tricker than one would hope the size information must be examined in an endian safe manner
	chunkSize = m_chunkStream.read32();
	if(!m_chunkStream.isOk())	// check if it was a good read
	{
		return -1;	// error - connection down
	}
	if(chunkSize > HK_CHUNK_LOADER_MAX_CHUNK_SIZE)
	{
		return -1;	// error - maximum chunk size has ben exceeded
	}

	hkArray<char>& chunkWriteBuffer = chunkStreambuf->getWriteBuffer();
	chunkWriteBuffer.clear();

	// just so we can forward on the size information (endian safe)
	hkOArchive chunkWriter(chunkStreambuf);  
	chunkWriter.write32(chunkSize);

	// read/write the raw data into the chuck
	chunkWriteBuffer.setSize(hkSizeOf(chunkSize) + chunkSize);
	m_chunkStream.read(&chunkWriteBuffer[sizeof(chunkSize)], chunkSize);
	if(!m_chunkStream.isOk())	// check if it was a good read
	{
		return -1;	// error - connection down
	}

	chunkType = (int)chunkWriteBuffer[sizeof(chunkSize)];

	// now we have the chunk data so swap buffers to make this data readable
	chunkStreambuf->swapBuffers();

	return chunkType;
}


int hkChunkLoader::readFrame(hkStringStreambuf* chunkStreambuf)
{
	int chunkSize;
	int chunkType;

	hkArray<char>& chunkWriteBuffer = chunkStreambuf->getWriteBuffer();
	chunkWriteBuffer.clear();

//	int chunkCount = 0;
	do
	{
		// this code is tricker than one would hope the size information must be examined in an endian safe manner
		chunkSize = m_chunkStream.read32();
		if(!m_chunkStream.isOk())	// check if it was a good read
		{
			return -1;	// error - connection down
		}
		if(chunkSize > HK_CHUNK_LOADER_MAX_CHUNK_SIZE)
		{
			return -1;	// error - maximum chunk size has ben exceeded
		}

		// just so we can forward on the size information (endian safe)
		hkOArchive chunkWriter(chunkStreambuf);  
		chunkWriter.write32(chunkSize);

		// read/write the raw data into the chuck
		int previousSize = chunkWriteBuffer.getSize();
		chunkWriteBuffer.setSize(previousSize + chunkSize);
		m_chunkStream.read(&chunkWriteBuffer[previousSize], chunkSize);
		if(!m_chunkStream.isOk()) // check if it was a good read
		{
			return -1; // error - connection down
		}

		chunkType = (int)chunkWriteBuffer[previousSize];
//		chunkCount++;
	}
	while(chunkType);

//	hkprintf("Chunks in frame: %d\n", chunkCount);

	// now we have the chunk data so swap buffers to make this data readable
	chunkStreambuf->swapBuffers();

	return chunkType;
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
