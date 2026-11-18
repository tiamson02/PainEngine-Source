/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_STANDARD_VEHICLE_SUSPENSION_BP
#define HK_STANDARD_VEHICLE_SUSPENSION_BP



/// Default implementation of hkSuspensionComponentBP
class hkDefaultSuspensionBP: public hkSuspensionComponentBP
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_VEHICLE, hkDefaultSuspensionBP);

		/// Constructor. Sets defaults (no suspension).
		hkDefaultSuspensionBP (int num_wheels);

		/// Destructor
		~hkDefaultSuspensionBP();

	public:

		/*
		** Per-wheel information
		*/

		/// The strenght [N/m] of the suspension at each wheel.
		hkReal* m_wheelsStrength;

		/// The damping force [N/(m/sec)] of the suspension at each wheel.
		hkReal* m_wheelsDampingCompression;

		/// The damping force [N/(m/sec)] of the suspension at each wheel.
		hkReal* m_wheelsDampingRelaxation;

};



#endif /* HK_STANDARD_VEHICLE_SUSPENSION_BP */


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
