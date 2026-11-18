/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HKBASE_HKCONSOLE_H
#define HKBASE_HKCONSOLE_H

/// Contains the standard output and standard error used by Havok.
class hkConsole : public hkSingleton<hkConsole>
{
	public:

			/// Standard output as used by hkprintf and hkcout.
		virtual hkOstream& getStdout() = 0;
			/// Standard error as used by hkcerr
		virtual hkOstream& getStderr() = 0;

	private:

		friend class hkSingleton<hkConsole>;
};

/// Convenience macro to print to stdout
#define hkprintf	hkConsole::getInstance().getStdout().printf
/// Convenience macro to access havok stdout
#define hkcout		hkConsole::getInstance().getStdout()
/// Convenience macro to access havok stderr
#define hkcerr		hkConsole::getInstance().getStderr()

#endif // HKBASE_HKCONSOLE_H

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
