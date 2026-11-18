/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkbase/hkBase.h>
#include <hkbase/memory/hkScratchpad.h>

class hkPs2Scratchpad : public hkScratchpad
{
	public:

		hkPs2Scratchpad()
			: hkScratchpad( reinterpret_cast<void*>(0x70000000), 0x4000)
		{
		}
};


class hkFakeScratchpad : public hkScratchpad
{
	public:

		hkFakeScratchpad()
			: hkScratchpad( m_buffer, 0x4000)
		{
		}
		char HK_ALIGNED_VARIABLE( m_buffer[0x4000], 16 );
};


#if defined(HK_COMPILER_MWERKS)
#	pragma force_active on
#endif

#if defined (HK_ARCH_PS2)
	HK_SINGLETON_CUSTOM_IMPLEMENTATION(hkScratchpad, hkPs2Scratchpad);
#else // not PS2
	HK_SINGLETON_CUSTOM_IMPLEMENTATION(hkScratchpad, hkFakeScratchpad);
#endif // HK_ARCH_PS2

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
