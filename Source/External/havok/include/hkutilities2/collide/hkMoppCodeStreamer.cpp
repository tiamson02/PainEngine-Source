/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkmath2/hkMath.h>
#include <hkbase/stream/hkOArchive.h>
#include <hkbase/stream/hkIArchive.h>

#include <hkcollide2/algorithms/mopp/code/hkMoppCode.h>
#include <hkutilities2/collide/hkMoppCodeStreamer.h>

hkMoppCode* HK_CALL hkMoppCodeStreamer::readMoppCodeFromArchive(hkIArchive &inputArchive)
{
		// Read in the header info, see writeMoppCodeToArchive()
	hkVector4 header(1e10f, 1e10f, 1e10f);
	header(0) = inputArchive.readFloat32();
	header(1) = inputArchive.readFloat32();
	header(2) = inputArchive.readFloat32();
	header(3) = inputArchive.readFloat32();

		// Read in the MOPP byte code size
	int byteCodeSize = inputArchive.read32();

		// Put in some very basic "safety" checks here to detect file corruption. It is "unlikely" that
		// the offset values will be very large in magnitude.
	HK_ASSERT2((header(0) > -1e9f) && (header(0) < 1e9f), "Header of MoppCode Archive is corrupt!");
	HK_ASSERT2((header(1) > -1e9f) && (header(1) < 1e9f), "Header of MoppCode Archive is corrupt!");
	HK_ASSERT2((header(2) > -1e9f) && (header(2) < 1e9f), "Header of MoppCode Archive is corrupt!");
		// Also, assume size of data less than ~100 meg! This may be overly conservative!
	HK_ASSERT2(byteCodeSize < 10000000, "Input Archive data corrupt, byte code size is huge!");
	
	
		// Calculate the overall MOPP code size (header + byte code). The MOPP actually holds its
		// data *immediately* after the class itself, hence when allocating a hkMoppCode structure
		// it is also necessary to allocate enough space for the code at the same time. The total space
		// required is hence sizeof(hkMoppCode) + [number bytes required for byte data]
	int size = hkSizeOf(hkMoppCode) + byteCodeSize;	

		// Allocate a new MOPP code
	hkMoppCode* code = reinterpret_cast<hkMoppCode*>(hkAllocate<char>(size, HK_MEMORY_CLASS_TOOLS));
	
		// Fill in MoppCode structure data
	code->initializeExternally(size);	// You MUST call this on a hkMoppCode object if you're not using the hkMoppUtility/hkMoppCompiler
	code->m_info.offset(0) = header(0);
	code->m_info.offset(1) = header(1);
	code->m_info.offset(2) = header(2);
	code->m_info.offset(3) = header(3);
			
		// Read in the byte code
	int numBytesRead = inputArchive.read((char*) code->getProgramStart(), byteCodeSize);
	HK_ASSERT2(numBytesRead == byteCodeSize, "Input Archive data corrupt, not enough bytes read!");
	
	return code;
}

void HK_CALL hkMoppCodeStreamer::writeMoppCodeToArchive(hkMoppCode* code, hkOArchive &outputArchive)
{
		// Write out the header info
	outputArchive.writeFloat32(code->m_info.offset(0));
	outputArchive.writeFloat32(code->m_info.offset(1));
	outputArchive.writeFloat32(code->m_info.offset(2));
	outputArchive.writeFloat32(code->m_info.offset(3));
			
		// Write out the MOPP byte code size
	outputArchive.write32(code->getCodeSize());
			
		// Write out the byte code
	outputArchive.write((char*)(code->getProgramStart()), code->getCodeSize());
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
