/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

//
// Havok 2.0 Visual Debug Display Manager
//

#ifndef HKG_SERIALIZING_DISPLAY_HANDLER
#define HKG_SERIALIZING_DISPLAY_HANDLER

#include <hkmath2/hkMath.h>

#include <hkutilities2/visualdebugger/hkDisplayHandler.h>

class hkDisplaySerializeOStream;

/// This class implements the hkDisplayHandler interface.  It serializes
/// hkDisplayHandler calls so that they can be streamed over the network.
class hkSerializingDisplayHandler : public hkDisplayHandler
{
	public:
			/// Serializes hkDisplayHandler messages, the stream must be writable.
		hkSerializingDisplayHandler(hkStreambuf* stream);

			/// Destructor.
		virtual ~hkSerializingDisplayHandler();

			/// This utility function will send the data in the m_commandStreambuf down the m_connectionSteambuf.
		Status sendCommand();

		//
		// Display handler functions
		//
		virtual Status addGeometry(const hkArray<hkDisplayGeometry*>& geometries, const hkTransform& transform, int id, int tag);
		virtual Status setGeometryColor(int color, int id, int tag);
		virtual Status updateGeometry(const hkTransform& transform, int id, int tag);
		virtual Status removeGeometry(int id, int tag);

		virtual Status updateCamera(const hkVector4& from, const hkVector4& to, const hkVector4& up, hkReal nearPlane, hkReal farPlane, hkReal fov, const char* name);

		virtual Status displayPoint(const hkVector4& position, int colour, int tag);
		virtual Status displayLine(const hkVector4& start, const hkVector4& end, int color, int tag);

		virtual Status displayText(const char* text, int length, int tag);

		virtual Status displayGeometry(const hkArray<hkDisplayGeometry*>& geometries, const hkTransform& transform, int color, int tag);
		virtual Status displayGeometry(const hkArray<hkDisplayGeometry*>& geometries, int color, int tag);

		virtual Status sendStatisticsDump(const char* data, int length);

		virtual Status holdImmediate();
		virtual Status step(hkReal frameTimeInMs);

	private:
		
		hkStringStreambuf m_commandStreambuf;
		hkDisplaySerializeOStream* m_commandStream;

		hkStreambuf* m_connectionStreambuf;

		// for pathological error handling
		unsigned char m_frameNumber;

		void sendGeometryData(const hkArray<hkDisplayGeometry*>& geometries);
};

#endif // HKG_SERIALIZING_DISPLAY_HANDLER

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
