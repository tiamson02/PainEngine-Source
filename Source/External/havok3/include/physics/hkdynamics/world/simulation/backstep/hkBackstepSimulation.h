/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_BACKSTEP_SIMULATION_H
#define HK_DYNAMICS2_BACKSTEP_SIMULATION_H

#include <hkinternal/dynamics/world/simulation/continuous/hkContinuousSimulation.h>



	/// Simplified continuous simulation 
class hkBackstepSimulation : public hkContinuousSimulation
{
	public:
			/// Backstepping mode
		enum BackstepMode
		{
				/// Performs one call to continuous collision detection, and multiple calls to discrete collision detection.
			SIMPLE,
				/// Performs multiple calls to continuous collision detection. Guarantees non-penetration according to surface qualities.
			NON_PENETRATING
		};

		hkBackstepSimulation( enum BackstepMode backstepMode = NON_PENETRATING) : hkContinuousSimulation(hkContinuousSimulation::STEPPING_MODE_HALF_STEP), m_backsteppingMode(backstepMode) {}

	protected:
		void simulateToi( hkWorld* world, hkToiEvent& event, hkReal physicsDeltaTime );

		BackstepMode m_backsteppingMode;
};


#endif // HK_DYNAMICS2_BACKSTEP_SIMULATION_H



/*
* Havok SDK - PUBLIC RELEASE, BUILD(#20060114)
*
* Confidential Information of Havok.  (C) Copyright 1999-2006 
* Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
* Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
* rights, and intellectual property rights in the Havok software remain in
* Havok and/or its suppliers.
*
* Use of this software for evaluation purposes is subject to and indicates 
* acceptance of the End User licence Agreement for this product. A copy of 
* the license is included with this software and is also available from salesteam@havok.com.
*
*/
