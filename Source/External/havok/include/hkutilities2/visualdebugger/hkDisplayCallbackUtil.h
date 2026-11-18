/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DISPLAY_CALLBACK_UTIL_H
#define HK_DISPLAY_CALLBACK_UTIL_H

#include <hkbase/hkBase.h>

class hkStepDisplayListener;

/// A helper class that fires the hkStepDisplayListener::stepDisplayCallback
/// for all hkStepDisplayListener that have been registered.  Please use the
/// macros provided instead of calling the functions in this class directly.
class hkDisplayCallbackUtil : public hkSingleton<hkDisplayCallbackUtil>
{
	friend class hkSingleton<hkDisplayCallbackUtil>;

	public:

		hkDisplayCallbackUtil();
		~hkDisplayCallbackUtil();

			/// Register a hkStepDisplayListener with the hkDisplayCallbackUtil.
			/// Please call the HK_ADD_STEP_DISPLAY_LISTENER(stepDisplayListener) macro 
			/// instead of calling this function directly.
		void addStepDisplayListener(hkStepDisplayListener* stepDisplayListener);

			/// Remove a hkStepDisplayListener from the hkDisplayCallbackUtil.
			/// Please call the HK_REMOVE_STEP_DISPLAY_LISTENER(stepDisplayListener) macro 
			/// instead of calling this function directly.
		void removeStepDisplayListener(hkStepDisplayListener* stepDisplayListener);

			/// Fire 'stepDisplayCallback' for all registered hkStepDisplayListeners.
			/// Please call the HK_STEP_DISPLAY(frameTimeInMs) macro instead of calling this function
			/// directly.  'frameTimeInMs' is the frame time in milliseconds and is used to timestamp
			/// movies so that they play back in realtime.
		void stepDisplay(hkReal frameTimeInMs);

	private:

		hkArray<hkStepDisplayListener*> m_stepDisplayListener;

};

#define HK_ADD_STEP_DISPLAY_LISTENER(stepDisplayListener) hkDisplayCallbackUtil::getInstance().addStepDisplayListener(stepDisplayListener);
#define HK_REMOVE_STEP_DISPLAY_LISTENER(stepDisplayListener) hkDisplayCallbackUtil::getInstance().removeStepDisplayListener(stepDisplayListener);

#define HK_STEP_DISPLAY(frameTimeInMs) hkDisplayCallbackUtil::getInstance().stepDisplay(frameTimeInMs);

#endif // HK_DISPLAY_CALLBACK_UTIL_H




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
