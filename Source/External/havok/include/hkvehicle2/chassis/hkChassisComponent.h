/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef INC_CHASSISCOMPONENT
#define INC_CHASSISCOMPONENT

#include <hkvehicle2/vehicle/hkVehicleComponent.h>
#include <hkvehicle2/chassis/hkChassisComponentBP.h>


/// An abstraction for the chassis component which stores chassis space with respect to the Rigid Body.
class ChassisComponent : public hkBaseObject
{
	public:
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_VEHICLE);

			/// Constructor.
		ChassisComponent(const hkChassisComponentBP &bp);

		/*
		 * CAR ORIENTATION FUNCTIONS
		 */

			/// Returns the coordinate system of the chassis.
		inline ChassisCoordinateSystem getCoordinateSystem() const;

			/// Returns the UP direction of the chassis in body space.
		inline const hkVector4& getUpDirCS() const;

			/// Returns the FORWARD direction of the chassis in body space.
		inline const hkVector4& getForwardDirCS()	const;

			/// Returns the RIGHT direction of the chassis in body space.
		inline const hkVector4& getRightDirCS() const;

		inline hkReal getChassisMass() const;

	private:

		ChassisCoordinateSystem m_coordinateSystem:8;
		hkReal		m_chassisMass;

		hkVector4	m_forwardDirCS;		// Calculated on construction, never changes
		hkVector4	m_upDirCS;			// ""
		hkVector4	m_rightDirCS;		// ""

};

#include <hkvehicle2/chassis/hkChassisComponent.inl>



#endif //INC_CHASSISCOMPONENT

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
