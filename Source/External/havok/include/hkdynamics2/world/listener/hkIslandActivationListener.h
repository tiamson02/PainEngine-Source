/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


#ifndef HK_DYNAMICS2_ISLAND_ACTIVATION_LISTENER_H
#define HK_DYNAMICS2_ISLAND_ACTIVATION_LISTENER_H

class hkSimulationIsland;

	/// Derive from this class, and add to hkWorld using hkWorld::addIslandActivationListener()
	/// You will then recieve callbacks when simulation islands are activated and deactivated
class hkIslandActivationListener
{
	public:

		virtual ~hkIslandActivationListener() {}

			/// Called when an island is activated
		virtual void islandActivatedCallback( hkSimulationIsland* island ) = 0;

			/// Called when an island is deactivated
		virtual void islandDeactivatedCallback( hkSimulationIsland* island ) = 0;
};


#endif // HK_DYNAMICS2_WORLD_ACTIVATION_LISTENER_H

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
