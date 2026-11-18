/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_VEHICLECOMPONENT_H
#define HK_VEHICLECOMPONENT_H

#include <hkbase/baseobject/hkBaseObject.h>

class HavokVehicle;

/// An abstract vehicle component.\n
/// Contains a reference to the HavokVehicle that it is added to, and an updateComponent() method.
class VehicleComponent : public hkBaseObject
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_VEHICLE);

			/// Constructor.
		inline VehicleComponent();

			/// Destructor.
		inline virtual ~VehicleComponent();

			/// Sets the owner of the component (a vehicle).
		inline void setOwnerVehicle (HavokVehicle *m_vehicle);

			/// Virtual, called every simulation step, before anything is done in HavokVehicle::doSimulationStep().
		inline virtual void updateComponent(const hkStepInfo& stepInfo);


	protected:

		HavokVehicle *m_vehicle;

};

#include <hkvehicle2/vehicle/hkVehicleComponent.inl>



#endif //HK_VEHICLECOMPONENT_H

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
