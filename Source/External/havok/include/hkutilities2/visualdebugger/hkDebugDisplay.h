/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DEBUG_DISPLAY_H
#define HK_DEBUG_DISPLAY_H

#include <hkmath2/hkMath.h>
#include <hkutilities2/visualdebugger/hkColor.h>
#include <hkutilities2/visualdebugger/viewer/hkDebugDisplayViewer.h>

class hkDebugDisplayViewer;
class hkDisplayGeometry;

/// A helper class, which enable the display of debug points and lines
/// To use this class call the HK_DISPLAY_POINT(position, color) and 
/// HK_DISPLAY_LINE(start, end, color) macros.  A HK_SET_OBJECT_COLOR(id, color)
/// macro is also defined for convience, where id is the address of the rigid 
/// body.
class hkDebugDisplay : public hkSingleton<hkDebugDisplay>
{
	friend class hkSingleton<hkDebugDisplay>;

	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_TOOLS)

		hkDebugDisplay();
		~hkDebugDisplay();

			/// Add a display viewer to the debug display.  All
			/// debug points and lines will be send to this display
			/// handler until it is removed.
		void addDebugDisplayViewer(hkDebugDisplayViewer* debugDisplayViewer);

			/// Remove a display viewer from the debug display.
			/// No more debug points and lines will be sent to the
			/// display handler.
		void removeDebugDisplayViewer(hkDebugDisplayViewer* debugDisplayViewer);

			/// Removes all display viewer from the debug display.
			/// No debug points and lines will be displayed after
			/// this function is called.
		void clear();

	public:

			/// Adds a geometry to the display world managed by this display handler.  The current convention
			/// for supplying an id is to take the address of the hkCollidable from which the geometeries were
			/// created.  
			/// Please refere to the hkDisplayHandler class description for the meaning of id and tag.
		void addGeometry(const hkArray<hkDisplayGeometry*>& geometries, const hkTransform& transform, int id, int tag);

			/// Sets the color of and object that as already been added to all display viewer (targeted by the debug display)
			/// Please refere to the hkDisplayHandler class description for the meaning of id and tag.
		void setGeometryColor(int color, int id, int tag);

			/// Updates the transform of a body in the display world.
			/// Please refere to the hkDisplayHandler class description for the meaning of id and tag.
		void updateGeometry(const hkTransform& transform, int id, int tag);

			/// Removes a geometry from the display world managed by this display handler.
			/// Please refere to the hkDisplayHandler class description for the meaning of id and tag.
		void removeGeometry(int id, int tag);

			/// updated the user camera, the user can supply as many cameras as they like and these are identified by
			/// supplying a unique id for each camera.  (NOTE: currently only one user camera is supported and the id is
			/// ignored.)
		void updateCamera(const hkVector4& from, const hkVector4& to, const hkVector4& up, hkReal nearPlane, hkReal farPlane, hkReal fov, const char* name);


			/// Displays a debug point. Please use the HK_DISPLAY_POINT macro instead of calling this function directly.
		void displayPoint(const hkVector4& position, int color, int tag);

			/// Display a debug line. Please use the HK_DISPLAY_LINE macro instead of calling this function directly.
		void displayLine(const hkVector4& start, const hkVector4& end, int color, int tag);
		
			/// Outputs user text to the display.  (The manner in which the text is displayed depends on the 
			/// implementation of the display handler.)
		void displayText(const char* text, int color, int tag);

			/// Displays the geometries	for one frame in immediate mode.
		void displayGeometry(const hkArray<hkDisplayGeometry*>& geometries, const hkTransform& transform, int color, int tag);

			/// Displays the geometries	without transform for one frame in immediate mode.
		void displayGeometry(const hkArray<hkDisplayGeometry*>& geometries, int color, int tag);

	private:

		hkArray<hkDebugDisplayViewer*> m_debugDisplayViewer;
};


// Macros for displaying debug points and lines.
// These can be compiled out if HK_DISABLE_DEBUG_DISPLAY is defined.

#if defined(HK_DISABLE_DEBUG_DISPLAY)
// No display of points and lines
#	define HK_ADD_GEOMETRY(geometries, transform, id)	/* nothing */
#	define HK_SET_OBJECT_COLOR(id, color)
#	define HK_UPDATE_GEOMETRY(transform, id)
#	define HK_REMOVE_GEOMETRY(id)

#	define HK_UPDATE_CAMERA(from, to, up, nearPlane, farPlane, fov, name)

#	define HK_DISPLAY_POINT(position, color)
#	define HK_DISPLAY_LINE(start, end, color)
#	define HK_DISPLAY_TEXT(text, color)
#	define HK_DISPLAY_GEOMETRY(geometeries, color)
#	define HK_DISPLAY_GEOMETRY_WITH_TRANSFORM(geometeries, transform, color)

#else // HK_DISABLE_DEBUG_DISPLAY
// The debug display (display points and lines)
// NOTE: Please refer to the hkDebugDisplay class definition above for
// information on these macros parameters.  These calls are essentially
// user versions of the calls in the hkDisplayHandler class interface,
// please read its class description.
#	define HK_ADD_GEOMETRY(geometries, transform, id) hkDebugDisplay::getInstance().addGeometry(geometries, transform, id, hkDebugDisplayViewer::getTag());
#	define HK_SET_OBJECT_COLOR(id, color) hkDebugDisplay::getInstance().setGeometryColor(color, id, hkDebugDisplayViewer::getTag());
#	define HK_UPDATE_GEOMETRY(transform, id) hkDebugDisplay::getInstance().updateGeometry(transform, id, hkDebugDisplayViewer::getTag());
#	define HK_REMOVE_GEOMETRY(id) hkDebugDisplay::getInstance().removeGeometry(id, hkDebugDisplayViewer::getTag());

#	define HK_UPDATE_CAMERA(from, to, up, nearPlane, farPlane, fov, name) hkDebugDisplay::getInstance().updateCamera(from, to, up, nearPlane, farPlane, fov, name);

#	define HK_DISPLAY_POINT(position, color) hkDebugDisplay::getInstance().displayPoint(position, color, hkDebugDisplayViewer::getTag());
#	define HK_DISPLAY_LINE(start, end, color) hkDebugDisplay::getInstance().displayLine(start, end, color, hkDebugDisplayViewer::getTag());
#	define HK_DISPLAY_TEXT(text, color) hkDebugDisplay::getInstance().displayText(text, color, hkDebugDisplayViewer::getTag());
#	define HK_DISPLAY_GEOMETRY(geometeries, color) hkDebugDisplay::getInstance().displayGeometry(geometeries, color, hkDebugDisplayViewer::getTag());
#	define HK_DISPLAY_GEOMETRY_WITH_TRANSFORM(geometeries, transform, color) hkDebugDisplay::getInstance().displayGeometry(geometeries, transform, color, hkDebugDisplayViewer::getTag());

#endif // HK_DISABLE_DEBUG_DISPLAY

#endif // HK_DEBUG_DISPLAY_H




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
