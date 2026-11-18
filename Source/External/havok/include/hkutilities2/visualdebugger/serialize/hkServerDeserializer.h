/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_COMMAND_DESERIALIZER
#define HK_COMMAND_DESERIALIZER

#include <hkbase/hkBase.h>

class hkStreambuf;
class hkDisplaySerializeIStream;

class hkPluginHandler;
class hkMousePickingHandler;

struct hkKeyboard;

/// A class to deserialize chunks of data that have come from the client to the server.
/// For example the creation and deletion of viewers.
class hkServerDeserializer : public hkReferencedObject
{
	public:

		hkServerDeserializer(hkStreambuf* stream, hkPluginHandler* pluginHandler, hkMousePickingHandler* mph, hkKeyboard* vkb);
		~hkServerDeserializer();

		bool consumeCommands();

	private:
		//
		// Viewers
		//

			/// Deserializes a createViewer command as declared in the hkPluginHandler interface.
		void processCreateViewer();

			/// Deserializes a deleteViewer command as declared in the hkPluginHandler interface.
		void processDeleteViewer();

		//
		// Tweakers
		//
		//???

		//
		// Mouse Picking
		//
		void processPickObject();
		void processDragObject();
		void processReleaseObject();

		//
		// Virtual Keyboard
		//
		void processVirtualKeyboardDump();


			/// Deserializes a step command as declared hkCommandHandler.h
		bool processStep();

	private:
		hkDisplaySerializeIStream* m_commandStream;
		hkPluginHandler* m_pluginHandler;
		hkMousePickingHandler* m_mousePickingHandler;
		hkKeyboard* m_keyboard;
};

#endif // HK_COMMAND_DESERIALIZER

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
