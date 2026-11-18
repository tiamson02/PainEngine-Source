/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DEMOFRAMEWORK2_DEFAULTGAME_H
#define HK_DEMOFRAMEWORK2_DEFAULTGAME_H

#include <hkdemoframework2/hkGame.h>
#include <hkdemoframework2/hkGameDatabase.h>



class hkgDisplayObject;
class hkgDisplayHandler;
class hkGameEnvironment;
class hkEnvironment;
class hkWorld;
class hkgCamera;
class hkViewport;
class hkgWindow;
class hkMouseSpringAction;
class hkStopwatch;

class hkDefaultGame : public hkGame
{
	public:

			/// The constructor
		hkDefaultGame(hkGameEnvironment* env);

			/// The destructor
		virtual ~hkDefaultGame();

			/// Step the game
		virtual Result stepGame();

			/// This implementation will link the graphics to the world
		void setupGraphics(hkWorld* world);

			/// set a default camera
		void setupDefaultCameras( hkGameEnvironment* env, const hkVector4& from, const hkVector4& to, const hkVector4& up, const hkReal farPlane = 5000.0f);

			/// A convinience function to open a stream. It should work on all plattforms
		hkStreambuf* openStream(const char* name, hkStreambuf::OpenMode mode, bool wantprogress=false);

			/// Gets the current path of the game
		const hkString& getPath() const { return m_path; }

			/// Start a little progress slider. E.g. good for loading or processing big files
		void progressBegin(const char* what);

			/// Update the progress slider with a value between 0 and 1.
		void progressSet(hkReal fraction);

			/// Remove progress slider.
		void progressEnd();

			/// Unprojects and calls objectPicked.
		virtual void mouseDown();

			/// Calls objectReleased.
		virtual void mouseUp();

			/// Unprojects and calls objectDragged.
		virtual void mouseDrag();

	protected:

			/// Called by mouseDown, should return true if it picks an object
		virtual bool objectPicked( const hkgDisplayObject* displayObject, const hkVector4& worldPosition );

			/// Called by mouseUp
		virtual void objectReleased();

			/// Called by mouseDrag
		virtual void objectDragged( const hkVector4& newWorldSpacePoint );

			/// Our current game environment
		hkGameEnvironment* m_env;

			/// Our world
		hkWorld* m_world;

			///
		hkReal m_timestep;

			//
			//	Mouse spring helpers
			//
			/// If true objectDragged is called, else objectPicked
		bool m_mouseActive;
			/// The mouse spring (modified by the objectXXX) functions
		hkMouseSpringAction* m_mouseSpring;
			/// The max acceleration the mouse spring will apply to objects
		hkReal m_mouseSpringMaxRelativeForce;
			/// The z-coordinate of the object picked
		hkReal m_mousePosDepth;

		hkString m_path;

			// Some helper to print progress bars while loading
		hkStopwatch* m_lastProgress;
		hkString m_progressName;
};


//
// Visual Debugger Utility Functions
//
class hkVisualDebugger;

// Setup the Visual Debugger
hkVisualDebugger* HK_CALL setupVisualDebugger(hkWorld* world, bool runServer = true, const char* captureFile = HK_NULL);

// Step the Visual Debugger
void HK_CALL stepVisualDebugger();

#endif // HK_DEMOFRAMEWORK2_DEFAULTGAME_H

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
