/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HKBASE_HKBASESYSTEM_H
#define HKBASE_HKBASESYSTEM_H

/// This class is used to initialize the Havok subsystems (memory, singleton, etc).
class hkBaseSystem
{
	public:

			/// Initialize the default subsystems. It is safe to call multiple times.
		static hkResult (HK_CALL *init)();

			/// Quit the subsystems. It is safe to call multiple times.
		static hkResult (HK_CALL *quit)();

			/// The default init function.
		static hkResult HK_CALL defaultInit();

			/// The default quit function.
		static hkResult HK_CALL defaultQuit();

			/// Checks if the system has already been initialized.
		static bool HK_CALL isInitialized();

			/// Utility function to initialize all singletons which are not explicitly initialized in hkBaseSystem::init().
		static void HK_CALL initSingletons();

			/// Utility function to quit all singletons which are not explicitly quit in hkBaseSystem::quit().
		static void HK_CALL quitSingletons();

	private:

			/// No implementation
		hkBaseSystem();
};

#endif // HKBASE_HKBASESYSTEM_H

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
