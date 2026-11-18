/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkutilities2/visualdebugger/hkVersionHandler.h>

#include <hkbase/config/hkConfigVersion.h>
#include <hkutilities2/visualdebugger/serialize/hkDisplaySerializeOStream.h>

// Protocal Revision Information
// Must be updated for any change to the visual debugger protocal
// Extensions to the protocal do not necessarily make the the client
// and server incompatible.
//
// Client can support more packets then the server is going to send...
// Server can send packets and the client can ignore them.
// Visa-versa for both of the above.
//
// If an existing packet changes its format then the minimum version
// must be updated to match the new protocal.
// (Unless the packet has an internal versioning system.)
//
// Date      Date      Version   Information
// (HEAD)    (Release) ver(min)
// --------- --------- --------- -----------------------------------------
// 20030430  ?         200(200)  First versioned protocal version. (BC)
//
// 

int hkVersionHandler::m_protocalMinimumCompatible = 2100;
int hkVersionHandler::m_protocalVersion = 2100;
// read as XYY -> vX.YY

#if defined(HK_PLATFORM_PS2)
#define HK_PLATFORM_INFO_STRING "PS2"
#elif defined(HK_PLATFORM_PC)
#define HK_PLATFORM_INFO_STRING "PC"
#elif defined(HK_PLATFORM_XBOX)
#define HK_PLATFORM_INFO_STRING "XBOX"
#elif defined(HK_PLATFORM_GC)
#define HK_PLATFORM_INFO_STRING "GC"
#else
#define HK_PLATFORM_INFO_STRING "UNKNOWN"
#endif

hkVersionHandler::Status hkVersionHandler::sendVersionInformation( hkStreambuf* connectionStreambuf )
{
	// send the data chunk describing the version information
	hkStringStreambuf commandStreambuf;

	hkDisplaySerializeOStream commandStream(&commandStreambuf);

	commandStream.write8u(HK_VERSION_INFORMATION);

	// version and minimum compatible version
	commandStream.write32(m_protocalVersion);
	commandStream.write32(m_protocalMinimumCompatible);

	// send magic platform string
	const char* platformInfo = HK_PLATFORM_INFO_STRING;
	int length = hkString::strLen(platformInfo);
	if(length > 65535)
	{
		length = 65535;
	}
	commandStream.write16u((unsigned short)length);
	commandStream.write(const_cast<char*>(platformInfo), length);

	// turn the data into a chunk (this only makes 2 calls to the network layer which is fast...)
	hkArray<char>& rawData = commandStreambuf.getWriteBuffer();
	const int rawDataSize = rawData.getSize();

	hkDisplaySerializeOStream connectionStream(connectionStreambuf);
	connectionStream.write32(rawDataSize);
	connectionStream.write(rawData.begin(), rawData.getSize());

	return HK_OK;
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
