/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_RAGDOLLDRAWER_H
#define HK_RAGDOLLDRAWER_H

#include <hkutilities2/visualdebugger/viewer/constraint/hkConstraintDrawer.h>
#include <hkgeometry2/display/hkDisplayCone.h>
#include <hkgeometry2/display/hkDisplayPlane.h>

/// Displays information about the ballSocket constraint.
class hkRagdollDrawer : public hkConstraintDrawer
{
	public:
	
		void drawConstraint(hkConstraint* constraint, hkDisplayHandler* displayHandler, int tag);

	private:

		hkDisplayCone m_twistCone;
		hkDisplayCone m_planeCone1;
		hkDisplayCone m_planeCone2;
		hkDisplayPlane m_plane;
};

#endif //HK_RAGDOLLDRAWER_H






















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
