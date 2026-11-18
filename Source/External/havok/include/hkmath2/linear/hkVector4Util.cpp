/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkmath2/hkMath.h>
#include <hkmath2/linear/hkVector4Util.h>

#if HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED
#	if defined(HK_COMPILER_HAS_INTRINSICS_IA32)
#		include <hkmath2/linear/impl/hkSseVector4Util.cxx>
#	elif defined(HK_COMPILER_HAS_INTRINSICS_PS2)
#		include <hkmath2/linear/impl/hkFpuVector4Util.cxx>
#	elif defined(HK_ARCH_PS2)
#		include <hkmath2/linear/impl/hkFpuVector4Util.cxx>  //!me
#	else 
#		error Dont know how to do simd on this platform
#	endif
#else
#	include <hkmath2/linear/impl/hkFpuVector4Util.cxx>
#endif


hkUint32 hkVector4Util::m_reservedRegisters = 0;

const float HK_ALIGNED_VARIABLE(hkConvertMagicVector[], 16 ) = { 65536.0f, 65536.0f, 65536.0f, 65536.0f };

#if HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED
#	if defined(HK_ARCH_PS2)
#		if defined(HK_COMPILER_GCC)
			const hkUint128 hkQuadSignMask = 0x80000000800000008000000080000000ULL; // top bit ones	
#		elif defined(HK_COMPILER_MWERKS)
			const hkUint128 hkQuadSignMask = 0x80000000800000008000000080000000; // top bit ones	
#		endif
#	endif
#endif
 

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
