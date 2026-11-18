/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkgraphics/hkGraphics.h> // precompiled
#include <hkgraphics/math/hkgMath.h>

extern const float HKG_VEC3_ZERO[3]  = { 0.0f, 0.0f, 0.0f };
extern const float HKG_VEC3_ONE[3]  = { 1.0f, 1.0f, 1.0f };
extern const float HKG_VEC4_ZERO[4]  = { 0.0f, 0.0f, 0.0f, 0.0f };
extern const float HKG_VEC4_ONE[4]  = { 1.0f, 1.0f, 1.0f, 1.0f };
extern const float HKG_VEC3_X[3]  = { 1.0f, 0.0f, 0.0f };
extern const float HKG_VEC3_Y[3]  = { 0.0f, 1.0f, 0.0f };
extern const float HKG_VEC3_Z[3]  = { 0.0f, 0.0f, 1.0f };
extern const float HKG_VEC4_X[4]  = { 1.0f, 0.0f, 0.0f, 0.0f };
extern const float HKG_VEC4_Y[4]  = { 0.0f, 1.0f, 0.0f, 0.0f };
extern const float HKG_VEC4_Z[4]  = { 0.0f, 0.0f, 1.0f, 0.0f };
extern const float HK1G_VEC4_W[4]  = { 0.0f, 0.0f, 0.0f, 1.0f };
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
