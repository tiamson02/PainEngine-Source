/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkmath2/hkMath.h>

///\class hkVector4
/// Note that some methods have the suffix '3' or '4' - for instance, there is dot3() method and a dot4()
/// method. This suffix indicates whether the method operates
/// on the first three, or all four, elements of the hkVector4. 
/// As you can see, some methods have a '4' version but no '3' version because there is no generic hardware support for that operation.
/// Other methods, such as setMul(), have no suffixes because there is no ambiguity regarding the elements on which they operate. 

// currently all implementations share the same .cxx file
// this is left in as a sanity check.
#if HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED
#	if defined(HK_COMPILER_HAS_INTRINSICS_IA32)
#		include <hkmath2/linear/impl/hkFpuVector4.cxx>
#	elif defined(HK_COMPILER_HAS_INTRINSICS_PS2)
#		include <hkmath2/linear/impl/hkFpuVector4.cxx>
#	elif defined(HK_ARCH_PS2)
#		include <hkmath2/linear/impl/hkFpuVector4.cxx> //!me
#	else
#		error Dont know how to do simd on this platform
#	endif
#else
#	include <hkmath2/linear/impl/hkFpuVector4.cxx>
#endif

extern const hkVector4 hkVector4Zero;
const hkVector4 hkVector4Zero; // implicitly zero

bool hkVector4::isOk() const
{
	const hkReal* f = reinterpret_cast<const hkReal*>(this);
	for(int i=0; i<4; ++i)
	{
		if( hkMath::isFinite(f[i]) == false )
		{
			return false;
		}
	}
	return true;
}

void hkVector4::setMul3( const hkMatrix3& a, const hkVector4& b)
{
	this->_setMul3( a, b );
}
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
