/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_STANDARD_VEHICLE_AERODYNAMICS_BP
#define HK_STANDARD_VEHICLE_AERODYNAMICS_BP



/// Default implementation of hkAerodynamicsComponentBP
class hkDefaultAerodynamicsBP: public hkAerodynamicsComponentBP
{
	public:
		
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_VEHICLE, hkDefaultAerodynamicsBP);

		/// Constructor. 
		hkDefaultAerodynamicsBP();
		// Sets default values

	public:

		/// The density of the air that sorrounds the vehicle, usually, 1.3 kg/m3.
		hkReal	m_airDensity;

		/// The frontal area, in m2, of the car.
		hkReal	m_frontalArea;

		/// The drag coefficient of the car.
		hkReal	m_dragCoefficient;

		/// The lift coefficient of the car (it can be either positive or negative).
		hkReal	m_liftCoefficient;

		/// An extra, fictional (non-physical) gravity force, to be applied to the body.
		hkVector4	m_extraGravityws;

};



#endif /* HK_STANDARD_VEHICLE_AERODYNAMICS_BP */

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
