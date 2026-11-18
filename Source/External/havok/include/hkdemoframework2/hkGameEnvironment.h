/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_GAME_ENVIRONMENT
#define HK_GAME_ENVIRONMENT

class hkTextDisplay;
class hkgWindow;
class hkgDisplayHandler;
class hkgDisplayWorld;
class hkgPad;
class hkPerformanceCounterUtility;

struct hkDemoFrameworkOptions;

class hkGameEnvironment
{
	public:

		hkGameEnvironment()
			:	m_window(0), 
				m_displayHandler(0),	
				m_displayWorld(0),  
				m_textDisplay(0),
				m_gamePad(0),
				m_performanceCounter(HK_NULL),
				m_options(0),
				//m_path()
				m_parameters(0)
		{
		}

		hkgWindow*	m_window;
		hkgDisplayHandler* m_displayHandler;
		hkgDisplayWorld* m_displayWorld;
		hkTextDisplay* m_textDisplay;
		hkgPad* m_gamePad;
		hkPerformanceCounterUtility* m_performanceCounter;
		const hkDemoFrameworkOptions* m_options;
		
		hkString m_path;
		hkArray<hkGame::hkParameter>* m_parameters;
};


#endif //HK_GAME_ENVIRONMENT

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
