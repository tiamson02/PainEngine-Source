/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

//
// Havok 2.0 Visual Debug Viewer Display Handle
//
#include <hkmath2/hkMath.h>

// Serialization dependencies
#include <hkutilities2/visualdebugger/serialize/hkDisplaySerializeOStream.h>

// Class definition
#include <hkutilities2/visualdebugger/serialize/hkSerializingDisplayHandler.h>

#include <hkutilities2/visualdebugger/hkVisualDebuggerDebugOutput.h>
#include <hkgeometry2/display/hkDisplayGeometry.h>
#include <hkgeometry2/display/hkDisplaySphere.h>
#include <hkgeometry2/display/hkDisplayBox.h>
#include <hkgeometry2/display/hkDisplayAABB.h>
#include <hkgeometry2/display/hkDisplayCone.h>
#include <hkgeometry2/display/hkDisplayPlane.h>
#include <hkgeometry2/display/hkDisplaySemiCircle.h>
#include <hkgeometry2/display/hkDisplayCapsule.h>

hkSerializingDisplayHandler::hkSerializingDisplayHandler(hkStreambuf* stream)
	: m_connectionStreambuf(stream), m_frameNumber(0)
{
	m_connectionStreambuf->addReference();
	m_commandStream = new hkDisplaySerializeOStream(&m_commandStreambuf);
}


hkSerializingDisplayHandler::~hkSerializingDisplayHandler()
{
	m_connectionStreambuf->removeReference();
	delete m_commandStream;
}


hkDisplayHandler::Status hkSerializingDisplayHandler::sendCommand()
{
	hkArray<char>& rawData = m_commandStreambuf.getWriteBuffer();
	const int rawDataSize = rawData.getSize();

	hkDisplaySerializeOStream m_connectionStream(m_connectionStreambuf);
	m_connectionStream.write32(rawDataSize);
	m_connectionStream.write(&rawData[0], rawData.getSize());
	rawData.clear();

	return hkDisplayHandler::HK_OK;
}

// THIS HAS TO BE CHANGED. Put a getDesc() in DisplayGeometry instead. AB 13-06-03.
static char* displayGeometryTypeToString[] = 
{ 
	"HK_DISPLAY_RECTANGLE",
	"HK_DISPLAY_SPHERE",
	"HK_DISPLAY_CAPSULE",
	"HK_DISPLAY_BOX",
	"HK_DISPLAY_AABB",
	"HK_DISPLAY_CONE",
	"HK_DISPLAY_SEMICIRCLE",
	"HK_DISPLAY_CONVEX",
	"HK_DISPLAY_PLANE"
};


void hkSerializingDisplayHandler::sendGeometryData(const hkArray<hkDisplayGeometry*>& geometries)
{
	m_commandStream->write32(geometries.getSize());

	for(int i = 0; i < geometries.getSize(); i++)
	{
		switch(geometries[i]->getType())
		{
			case HK_DISPLAY_CONVEX:
				{
					const int numVertices = geometries[i]->getGeometry()->getNumVertices();
					const int numTriangles = geometries[i]->getGeometry()->getNumTriangles();
					HK_VISUAL_DEBUGGER_INFO(2, " Sending sub geometry of type " << displayGeometryTypeToString[geometries[i]->getType()]
						<< ". It has " << numVertices << " vertices and " << numTriangles << " triangles.");
				}
				break;

			case HK_DISPLAY_SPHERE:
				{
					const hkReal radius = static_cast<hkDisplaySphere*>(geometries[i])->getSphere().getRadius();
					HK_VISUAL_DEBUGGER_INFO(2, " Sending sub geometry of type " << displayGeometryTypeToString[geometries[i]->getType()]
						<< ". Its radius is " << radius);
				}
				break;

			case HK_DISPLAY_CAPSULE:
				{
					const hkVector4& bottom = static_cast<hkDisplayCapsule*>(geometries[i])->getBottom();
					const hkVector4& top = static_cast<hkDisplayCapsule*>(geometries[i])->getTop();
					const hkReal radius = static_cast<hkDisplayCapsule*>(geometries[i])->getRadius();

					HK_VISUAL_DEBUGGER_INFO(2, " Sending sub geometry of type " << displayGeometryTypeToString[geometries[i]->getType()]
						<< ". Its radius is " << radius);
				}
				break;

			case HK_DISPLAY_BOX:
				{
					const hkVector4& halfExtents = static_cast<hkDisplayBox*>(geometries[i])->getHalfExtents();
					HK_VISUAL_DEBUGGER_INFO(2, " Sending sub geometry of type " << displayGeometryTypeToString[geometries[i]->getType()]
						<< ". Its half extents are " << halfExtents(0) << halfExtents(1) << halfExtents(2));
				}
				break;

			case HK_DISPLAY_AABB:
				{
					hkVector4 minExtent = static_cast<hkDisplayAABB*>(geometries[i])->getMinExtent();
					hkVector4 maxExtent = static_cast<hkDisplayAABB*>(geometries[i])->getMaxExtent();

					HK_VISUAL_DEBUGGER_INFO(2, " Sending geometry of type " << displayGeometryTypeToString[geometries[i]->getType()]
						<< ". Min Extent =  " << minExtent(0) << minExtent(1) << minExtent(2) << "Max Extent = "  << maxExtent(0) << maxExtent(1) << maxExtent(2) );
				}
				break;

			case HK_DISPLAY_CONE:
				{
					const hkVector4& position = static_cast<hkDisplayCone*>( geometries[i] )->getPosition();
				
				    HK_VISUAL_DEBUGGER_INFO(2, " Sending geometry of type " << displayGeometryTypeToString[geometries[i]->getType()] << "position : " << position(0) << position(1) << position(2) );
				}
				break;


			case HK_DISPLAY_PLANE:
				{
					const hkVector4& center = static_cast<hkDisplayPlane*>( geometries[i] )->getCenter();
				
				    HK_VISUAL_DEBUGGER_INFO(2, " Sending geometry of type " << displayGeometryTypeToString[geometries[i]->getType()] << "center : " << center(0) << center(1) << center(2) );
				}
				break;
		
			case HK_DISPLAY_SEMICIRCLE:
				{
					const hkVector4& center = static_cast<hkDisplaySemiCircle*>( geometries[i] )->getCenter();
				
					HK_VISUAL_DEBUGGER_INFO(2, " Sending geometry of type " << displayGeometryTypeToString[geometries[i]->getType()] << "center : " << center(0) << center(1) << center(2) );
				}
				break;

			case HK_DISPLAY_RECTANGLE:
			default:
				{
					HK_VISUAL_DEBUGGER_INFO(0, "Trying to send unknown geometry type");
				}
		}

		m_commandStream->writeDisplayGeometry(geometries[i]);
	}
}



hkDisplayHandler::Status hkSerializingDisplayHandler::addGeometry(const hkArray<hkDisplayGeometry*>& geometries, const hkTransform& transform, int id, int tag)
{
	HK_VISUAL_DEBUGGER_INFO(1, "Sending geometry with id " << id 
		<< ". It has " << geometries.getSize() << " sub geometery(s) and is located at position "
		<< transform.getTranslation()(0) << ", " 
		<< transform.getTranslation()(1) << ", "
		<< transform.getTranslation()(2));

	// send a serialized version of the displayObject
	m_commandStream->write8u(hkDisplayHandler::HK_ADD_GEOMETRY);

	sendGeometryData(geometries);

	m_commandStream->writeTransform(transform);
	m_commandStream->write32(id);
	m_commandStream->write32(tag);

	return sendCommand();
}


hkDisplayHandler::Status hkSerializingDisplayHandler::displayGeometry(const hkArray<hkDisplayGeometry*>& geometries, const hkTransform& transform, int color, int tag)
{
	HK_VISUAL_DEBUGGER_INFO(1, "Sending immmediate display geometry and transform. It has " << geometries.getSize() << " sub geometery(s) and is located at position "
		<< transform.getTranslation()(0) << ", " 
		<< transform.getTranslation()(1) << ", "
		<< transform.getTranslation()(2));

	// send a serialized version of the displayObject
	m_commandStream->write8u(hkDisplayHandler::HK_DISPLAY_GEOMETRY_WITH_TRANSFORM);
	
	sendGeometryData(geometries);

	m_commandStream->writeTransform(transform);

	m_commandStream->write32u(color);
	m_commandStream->write32(tag);

	return sendCommand();
}

hkDisplayHandler::Status hkSerializingDisplayHandler::displayGeometry(const hkArray<hkDisplayGeometry*>& geometries, int color, int tag)
{
	HK_VISUAL_DEBUGGER_INFO(1, "Sending immediate display geometry data. numGeometries =  " << geometries.getSize() );

	// send a serialized version of the displayObject
	m_commandStream->write8u(hkDisplayHandler::HK_DISPLAY_GEOMETRY);

	sendGeometryData(geometries);

	m_commandStream->write32u(color);
	m_commandStream->write32(tag);

	return sendCommand();
}


hkDisplayHandler::Status hkSerializingDisplayHandler::setGeometryColor(int color, int id, int tag)
{
	m_commandStream->write8u(hkDisplayHandler::HK_SET_COLOR_GEOMETRY);

	m_commandStream->write32u(color);
	m_commandStream->write32(id);
	m_commandStream->write32(tag);

	return sendCommand();
}


hkDisplayHandler::Status hkSerializingDisplayHandler::updateGeometry(const hkTransform& transform, int id, int tag)
{
	// update a display object identified by id
	m_commandStream->write8u(hkDisplayHandler::HK_UPDATE_GEOMETRY);

	m_commandStream->writeTransform(transform);
	m_commandStream->write32(id);
	m_commandStream->write32(tag);

	return sendCommand();
}


hkDisplayHandler::Status hkSerializingDisplayHandler::removeGeometry(int id, int tag)
{
	HK_VISUAL_DEBUGGER_INFO(1, "Sending remove geometry reqest with id " << id);

	// remove a display object identified by id
	m_commandStream->write8u(hkDisplayHandler::HK_REMOVE_GEOMETRY);

	m_commandStream->write32(id);
	m_commandStream->write32(tag);

	return sendCommand();
}



hkDisplayHandler::Status hkSerializingDisplayHandler::updateCamera(const hkVector4& from, const hkVector4& to, const hkVector4& up, hkReal nearPlane, hkReal farPlane, hkReal fov, const char* name)
{
	// update a display object identified by id
	m_commandStream->write8u(hkDisplayHandler::HK_UPDATE_CAMERA);

	m_commandStream->writeQuadVector4(from);
	m_commandStream->writeQuadVector4(to);
	m_commandStream->writeQuadVector4(up);

	m_commandStream->writeFloat32(nearPlane);
	m_commandStream->writeFloat32(farPlane);
	m_commandStream->writeFloat32(fov);

	// send the name
	unsigned short length = hkString::strLen(name);
	if(length > 65535)
	{
		length = 65535;
	}
	m_commandStream->write16u((unsigned short)length);
	m_commandStream->write(const_cast<char*>(name), length);

	return sendCommand();
}


hkDisplayHandler::Status hkSerializingDisplayHandler::displayPoint(const hkVector4& position, int color, int tag)
{
	m_commandStream->write8u(hkDisplayHandler::HK_DISPLAY_POINT);

	m_commandStream->writeQuadVector4(position);
	m_commandStream->write32u(color);

	m_commandStream->write32(tag);

	return sendCommand();
}

hkDisplayHandler::Status hkSerializingDisplayHandler::displayLine(const hkVector4& start, const hkVector4& end, int color, int tag)
{
	m_commandStream->write8u(hkDisplayHandler::HK_DISPLAY_LINE);

	m_commandStream->writeQuadVector4(start);
	m_commandStream->writeQuadVector4(end);

	m_commandStream->write32u(color);

	m_commandStream->write32(tag);

	return sendCommand();
}


hkDisplayHandler::Status hkSerializingDisplayHandler::displayText(const char* text, int color, int tag)
{
	m_commandStream->write8u(hkDisplayHandler::HK_DISPLAY_TEXT);

	int length = hkString::strLen(text);

	// send length (and truncate if necessary)
	if(length > 65535)
	{
		length = 65535;
	}
	m_commandStream->write16u((unsigned short)length);

	m_commandStream->write(const_cast<char*>(text), length);

	m_commandStream->write32(color);

	m_commandStream->write32(tag);

	return sendCommand();
}

		
hkDisplayHandler::Status hkSerializingDisplayHandler::sendStatisticsDump(const char* data, int length)
{
	m_commandStream->write8u(hkDisplayHandler::HK_SEND_STATISTICS_DUMP);

	m_commandStream->write32(length);

	m_commandStream->write(const_cast<char*>(data), length);

	return sendCommand();
}

hkDisplayHandler::Status hkSerializingDisplayHandler::holdImmediate()
{
	m_commandStream->write8u(hkDisplayHandler::HK_HOLD_IMMEDIATE);

	return sendCommand();
}

hkDisplayHandler::Status hkSerializingDisplayHandler::step(hkReal frameTimeInMs)
{
	m_commandStream->write8u(hkDisplayHandler::HK_STEP);

	m_commandStream->writeFloat32(frameTimeInMs);

	const hkDisplayHandler::Status status = sendCommand();

	m_connectionStreambuf->flush();

	return status;
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
