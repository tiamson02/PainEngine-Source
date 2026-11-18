/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

//
// Havok 2.0 Display Handler - Defines an interface for viewing
// serializing and deserializing graphical information
//

#ifndef HK_DISPLAY_HANDLER
#define HK_DISPLAY_HANDLER

#include <hkbase/hkBase.h>
#include <hkmath2/hkMath.h>

class hkDisplayGeometry;
class hkCollidable;
struct hkKeyboard;

/// This class defines the graphical interface to the visual debugger.
/// It is also used to display worlds with the Havok demos.
///
/// IDs: IDs are used to manage persistent objects which are added/updated
/// and removed.  It is the responsibility of the user to supply a unique
/// id for every persistent object added and to use this id consistently when
/// updating and removing objects.
/// The current convention when supplying ids is to supply the
/// address of the hkCollidable where the geometeries came from.
/// Geometeries are built using the hkShapeDisplayBuilder class in
/// hkutilities2/visualdebugger/viewer/hkShapeDisplayBuilder.h/cpp.
///
/// TAGs: Unique tags can be obtained form the hkViewerFactory::getTag().
/// These can are used for separating display information into different
/// catagories so that they can be configured and filtered independently.
/// For example, each Viewer has one or more unique tags.
///
class hkDisplayHandler : public hkReferencedObject
{
	public:
		enum Status
		{
			HK_OK = 0,
			HK_FAIL
		};

			/// Defines a set of identifiers associated with each command/method in the display handler.
			/// NOTE: Step must have the numeric value 0.
		enum Command // for serialization
		{
			HK_STEP = 0x00,
			HK_ADD_GEOMETRY,
			HK_UPDATE_GEOMETRY,
			HK_SET_COLOR_GEOMETRY,
			HK_REMOVE_GEOMETRY,
			HK_DISPLAY_GEOMETRY_WITH_TRANSFORM,
			HK_DISPLAY_GEOMETRY,
			HK_DISPLAY_POINT,
			HK_DISPLAY_LINE,
			HK_DISPLAY_TEXT,	// These may move into a separate interface later

			HK_SEND_STATISTICS_DUMP,

			HK_HOLD_IMMEDIATE,

			HK_ADD_CAMERA,
			HK_UPDATE_CAMERA,
			HK_REMOVE_CAMERA
		};

	public:

			/// Destructor
		virtual ~hkDisplayHandler() {}

		//
		// Geometry functions
		//
			/// Adds a geometry to the display world managed by this display handler.
			/// Please refere to the class description for the meaning of id and tag.
		virtual Status addGeometry(const hkArray<hkDisplayGeometry*>& geometries, const hkTransform& transform, int id, int tag) = 0;

			/// Sets the color of a geometry previously added to the display world.
			/// Please refere to the class description for the meaning of id and tag.
		virtual Status setGeometryColor(int color, int id, int tag) = 0;

			/// Updates the transform of a body in the display world.
			/// Please refere to the class description for the meaning of id and tag.
		virtual Status updateGeometry(const hkTransform& transform, int id, int tag) = 0;

			/// Removes a geometry from the display world managed by this display handler.
			/// Please refere to the class description for the meaning of id and tag.
		virtual Status removeGeometry(int id, int tag) = 0;

		
		//
		// Camera Functionality
		//
		virtual Status updateCamera(const hkVector4& from, const hkVector4& to, const hkVector4& up, hkReal nearPlane, hkReal farPlane, hkReal fov, const char* name) = 0;


		//
		// Immediate Mode Functions
		//

			/// Puts a display point into the display buffer for display in the next frame.
		virtual Status displayPoint(const hkVector4& position, int colour, int tag) = 0;

			/// Puts a display line into the display buffer for display in the next frame.
		virtual Status displayLine(const hkVector4& start, const hkVector4& end, int color, int tag) = 0;

			/// Outputs user text to the display.  (The manner in which the text
			/// is displayed depends on the implementation of the display handler.)
		virtual Status displayText(const char* text, int color, int tag) = 0;

			/// Displays the geometries	
		virtual Status displayGeometry(const hkArray<hkDisplayGeometry*>& geometries, const hkTransform& transform, int color, int tag) = 0;

			/// Displays the geometries	without transform
		virtual Status displayGeometry(const hkArray<hkDisplayGeometry*>& geometries, int color, int tag) = 0;


		//
		// Statistics functions (ideally these would be in a separate interface to the display handler)
		//
			/// Sends statistical information to display handler.  This is a chunk of 
			/// binary data which can be interpreted by the hkMonitorCaptureStreamUtility.
			/// This information is not currently displayed in real time, please use
			/// the visual debugger client to capture the stats in ascii readable format.
		virtual Status sendStatisticsDump(const char* data, int length) = 0;


		//
		// Step (End of frame) and status functions
		//

			/// Ensures that the current immediate mode display information
			/// will be preserved and merged with all new immediate mode data
			/// for the next step/frame.
		virtual Status holdImmediate() = 0;

			/// Steps the display world by moving it to the next frame.  Note: This
			/// does not render the display world it merely swaps the front and back
			/// buffers of the display data such as the debug points and lines.
			/// 'frameTimeInMs' is the frame time in milliseconds and is used
			/// to timestamp movies so that they play back in realtime.
		virtual Status step(hkReal frameTimeInMs) = 0;


		// Immediate mode geometeries and immediate mode markers - **proposed**
//		virtual Status displayGeometry(const hkArray<hkDisplayGeometry*>& geometries, const hkTransform& transform, const int id) = 0;
//		virtual Status sendMarker(const char* name) = 0;


		// Camera - **proposed**
//		virtual Status addCamera(const char* cameraName, hkVector4 from, hkVector4 up, hkVector4 up, hkReal fieldOfViewInDegrees, int id) = 0;
//		virtual Status addCamera(const char* cameraName, hkTransform, hkReal fieldOfViewInDegrees, int id) = 0;
//		virtual Status updateCamera(const hkTransform& transform, int id) = 0;
//		virtual Status removeCamera(int id) = 0;

};


#endif // HK_DISPLAY_HANDLER

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
