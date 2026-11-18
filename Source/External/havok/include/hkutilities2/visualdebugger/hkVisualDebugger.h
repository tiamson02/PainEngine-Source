/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

//
// Havok 2.0 Visual Debugger Server
//

#ifndef HK_VISUAL_DEBUGGER
#define HK_VISUAL_DEBUGGER

#include <hkbase/hkBase.h>
#include <hkbase/stopwatch/hkStopwatch.h>

#include <hkdynamics2/world/listener/hkWorldPostSimulationListener.h>
#include <hkdynamics2/world/listener/hkWorldDeletionListener.h>

#include <hkutilities2/visualdebugger/hkVisualDebuggerDebugOutput.h>
#include <hkutilities2/visualdebugger/hkStepDisplayListener.h>

#include <hkutilities2/visualdebugger/hkDebugDisplay.h>

class hkWorld;
class hkSockServer;
class hkDisplayHandler;
class hkServerPluginHandler;
class hkServerDeserializer;

struct hkKeyboard;

static const unsigned int HK_VISUAL_DEBUGGER_DEFAULT_PORT = 25001;

/// This is the main visual debugger class on the server side.  It takes care of all
/// clients attaching them as listeners to the world and creating the appripriate
/// display handlers to send the display information over the network.  A plugin handler
/// is also created to allow the client to create and delete viewers remotely.
class hkVisualDebugger : protected hkStepDisplayListener, protected hkWorldDeletionListener, protected hkWorldPostSimulationListener
{

	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_TOOLS, hkVisualDebugger)

			/// Creates a visual debugger for a given hkWorld.
		static hkVisualDebugger* HK_CALL create(hkWorld* world, hkKeyboard* keyboard = HK_NULL);
		
			/// Instructs the visual debugger to start listening for client connections
			/// from the given TCP/IP port number.  For the server to actually work the
			/// world associated with this hkVisualDebugger instance must have its
			/// hkWorld::stepDeltaTime called as it uses the post simulation callback
			/// to poll for new clients.
		void serve(int listenPort = HK_VISUAL_DEBUGGER_DEFAULT_PORT);

			/// Instructs the visual debugger to start capturing the output of all
			/// of the default viewers to the specified filename.  Note you do not
			/// have to call hkVisualDebugger::serve this is completely independent
			/// and can be used without any network serving.
		void capture(const char* captureFilename);

			/// Steps every client of every instance of the visual debugger.
			/// See also hkVisualDebugger::stepDisplayCallback.
			/// NOTE: If you have disabled hkStopwatch you may need to supply
			/// the frame time as calculated by your own timer implementation.
			/// The default value of 0.0f for frameTimeInMs causes hkStopwatch
			/// (form hkbase) to be used to approximate the frame time.
			/// Supplying you own non zero value overrides this behaviour, it
			/// should be in milliseconds.
			/// When a movie is recorded from the visual debugger client the
			/// actual frame arrival time on the client is measured and this
			/// value is ignored.  The server side frame time value is used when
			/// recording a movie directly to a file.
		static void HK_CALL step(hkReal frameTimeInMs = 0.0f);

			/// Added a viewer to the list of default viewers.  All viewers in this
			/// list will be selected by a client when a new connection is established.
		void addViewer(const char* viewerName);

			/// Removes a viewer from the list of default viewers.  If a viewer is not
			/// in this list it will not be seleted by a client when a new connection 
			/// is established.
		void removeViewer(const char* viewerName);


			/// Called when the macro HK_STEP_DISPLAY(frameTimeInMs) is called, this
			/// macro must be called for the visual debugger to function properly.
		virtual void stepDisplayCallback(hkReal frameTimeInMs); // advances all clients to the next frame

			/// Called at the end of a world simulation step.
		virtual void postSimulationCallback(hkWorld* world, const hkStepInfo& stepInfo);

			/// Called when the world is deleted.
		virtual void worldDeletedCallback(hkWorld* world);

			/// Destructor.
		virtual ~hkVisualDebugger();

			/// Get access to the internal keyboard
		void setKeyboard(hkKeyboard* keyboard);

	private:
		void addClient(hkStreambuf* stream);

			/// Constructor. See the hkVisualDebugger::create.
		hkVisualDebugger(hkWorld* world, hkKeyboard* keyboard);

	private:

		hkWorld* m_world;

		hkSockServer* m_server;

		// client tracking (to handle cleanup in the event of network errors)
		struct Client
		{
			hkStreambuf* m_stream;
			hkDisplayHandler* m_dh;
			hkServerPluginHandler* m_sph;
			hkServerDeserializer* m_sd;
		};

		hkArray<Client> m_clients;
		hkArray<hkString*> m_requiredViewers;

		hkKeyboard* m_keyboard;

		bool m_amTimingFrame;
		hkStopwatch m_frameTimer;
};

#endif // HK_VISUAL_DEBUGGER

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
