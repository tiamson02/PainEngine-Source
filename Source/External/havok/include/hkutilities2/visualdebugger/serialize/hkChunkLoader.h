/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_CHUNK_LOADER
#define HK_CHUNK_LOADER

#include <hkbase/hkBase.h>
#include <hkbase/stream/hkIArchive.h>

static const int HK_CHUNK_LOADER_MAX_CHUNK_SIZE = 10000000;

class hkStreambuf;

/// A utility class for reading chunks of data from a stream.
class hkChunkLoader
{
	public:
			/// Create a hkChunkLoader which consumes from a given hkStreambuf.
		hkChunkLoader(hkStreambuf* chunkStreambuf);

			/// Destructor.
		~hkChunkLoader();

			/// Reads in a chunk from the main chunkStreambuf, returns the chunk type, -1 if the
			/// chunk is bad.  This call blocks until a chunk is read or and error has occurred.
		int readChunk(hkStringStreambuf* chunkStreambuf);

			/// Reads in a frames worth of chunks into the chunkStreambuf, and returns the
			/// STEP chunk type, -1 if the frame is bad.  This call blocks until a chunk is read 
			/// or and error has occurred.
		int readFrame(hkStringStreambuf* chunkStreambuf);

	private:
		hkIArchive m_chunkStream;
};

#endif // HK_CHUNK_LOADER

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
