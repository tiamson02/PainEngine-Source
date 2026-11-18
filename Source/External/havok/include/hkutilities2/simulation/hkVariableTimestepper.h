/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_VARIABLETIMESTEPPER_H
#define HK_VARIABLETIMESTEPPER_H

class hkWorld;

/// attempts to keep the stiffness of the system constant when the time step varies from frame to frame
class hkVariableTimestepper
{
	public:
		hkVariableTimestepper( hkReal timePerSubstep, hkReal minSubstepCount = 2.0f );

		/// simulate forward in time
		int step( hkWorld* world, hkReal timestep );

		hkReal m_timePerSubstep;

		hkReal m_minSubstepCount;

};


#endif // HK_VARIABLETIMESTEPPER_H


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
