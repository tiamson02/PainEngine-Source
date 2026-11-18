/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


#ifndef HK_STEP_DISPLAY_LISTENER_H
#define HK_STEP_DISPLAY_LISTENER_H

	/// Allows an object to register itself to be called when the
	/// global HK_STEP() macro is called.  This is used by the
	/// hkVisualDebugger and the hkStatisticsViewer for example.
class hkStepDisplayListener
{
	public:
			/// Virtual districtor for base class.
		virtual ~hkStepDisplayListener() {}

			/// Called when the HK_STEP_DISPLAY(frameTimeInMs) macro is called by the user.
			/// 'frameTimeInMs' is the frame time in milliseconds and is used to timestamp
			/// movies so that they play back in realtime.
		virtual void stepDisplayCallback(hkReal frameTimeInMs) = 0;

};



#endif // HK_STEP_DISPLAY_LISTENER_H

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
