/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_PLUGIN_HANDLER
#define HK_PLUGIN_HANDLER

#include <hkbase/hkBase.h>

/// This class defines the interface to control the state of viewers 
/// in the visual debugger.  Implementations exist on both the client 
/// and server side.
class hkPluginHandler : public hkReferencedObject
{
	public:
		enum Status
		{
			HK_OK = 0,
			HK_FAIL
		};

		enum Command // for serialization
		{
			HK_REGISTER_VIEWER = 0xC0, // This will be moved
			HK_SELECT_VIEWER,
			HK_CREATE_VIEWER,
			HK_DELETE_VIEWER
		};

	public:
		virtual ~hkPluginHandler() {}

		//
		// Viewers
		//
			
			/// Informs the plugin handler about a viewer.  It and then be created and
			/// destroyed by calling the createViewer and deleteViewer methods.
		virtual Status registerViewer(const char* name, int lengthOfName, int id) = 0;

			/// Instructs the plugin handler to create a viewer by default when a
			/// a connection is made to the visual debugger.  When the server (or
			/// simulation) calls selectViewer(id) this will cause the client to send
			/// back a createViewer(id) without intervention from the user.  This is
			/// used to make sure that geometry data and debug points and lines are
			/// always displayed by default when a new connection is made to a visual
			/// debugger server.
		virtual Status selectViewer(int id) = 0;


			/// Creates an instance of the viewer identified by id in the server (or simulation).
			/// The viewer is only created if one doesn't already exist.
		virtual Status createViewer(int id) = 0;

			/// Deletes a viewer, identified by id, from the server.
		virtual Status deleteViewer(int id) = 0;

		//
		// Tweakers
		//
		//???

		//virtual Status registerProperty(int id, ??type??, ??value??, ??range??);
};

#endif // HK_PLUGIN_HANDLER





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
