/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkmath2/hkMath.h>

/// \class hkRotation
/// <p>This is the fastest way to represent a rotation in Havok.
/// If using an hkQuaternion is more convenient, it is possible to convert
/// back and forth between an hkRotation and an hkQuaternion.</p>
/// <p>hkRotations are assumed to be orthonormal i.e. that any given hkRotation r
/// should satisfy the property transpose(r) = (r^-1).</p>

#include <hkmath2/linear/impl/hkFpuRotation.cxx>


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
