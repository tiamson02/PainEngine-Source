/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_SERVER_PLUGIN_HANDLER
#define HK_SERVER_PLUGIN_HANDLER

#include <hkutilities2/visualdebugger/hkPluginHandler.h>

#include <hkbase/hkBase.h>

class hkOArchive;

class hkWorld;
class hkDisplayHandler;

/// Server side implementation of the hkPluginHandler.  
class hkServerPluginHandler : public hkPluginHandler
{
	public:
			/// Constructor.
		hkServerPluginHandler(hkStreambuf* stream,
			hkWorld* world, hkDisplayHandler* displayHandler);

			/// Destructor.
		~hkServerPluginHandler();

		// send

		virtual Status registerViewer(const char* name, int lengthOfName, int id);
		virtual Status selectViewer(int id);

		// recieve

		virtual Status createViewer(int id);
		virtual Status deleteViewer(int id);

		// implementation specific

			/// Registers all available viewers with the hkPluginHandler.
		Status registerAllViewers();

			/// Selects a named viewer as a default.  All 'selected' viewers will
			/// be created when a new connection is established with the server.
		Status selectViewerByName(const char* name);

			/// Removes a reference from all viewers which have been created.
		void dereferenceAllViewers();

	private:

			/// Utility function to packege up information for sending accross the
			/// visual debugger client/server link.
		Status sendCommand();

	private:
		hkStringStreambuf m_commandStreambuf;
		hkOArchive* m_commandStream;

		hkStreambuf* m_connectionStreambuf;

		hkWorld* m_world;
		hkDisplayHandler* m_displayHandler;

		hkArray<hkReferencedObject*> m_viewers;
};

#endif // HK_SERVER_PLUGIN_HANDLER

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
