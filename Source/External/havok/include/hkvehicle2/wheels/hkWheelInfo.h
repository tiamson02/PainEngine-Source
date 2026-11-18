/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_WHEELINFO_H
#define HK_WHEELINFO_H

class	hkRigidBody;



/// The WheelInfo class holds all wheel information generated externally (from the physics engine)
/// such as each wheel's ground contact, sliding state, forces, contact friction etc.
class WheelInfo 
{
	public:
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_VEHICLE,  WheelInfo );

			/// Constructor.
		inline WheelInfo();

		/*
		** Accessors
		*/

			/// Returns true if the wheel is in contact with the ground.
		inline const bool& getIsInContact()	const;

			/// Gets the point of contact of the wheel with the ground.
		inline const hkVector4& getContactPointWS() const;

			/// Gets the normal at the point of contact wheel-ground.
		inline const hkVector4& getContactNormalWS() const;

			/// Gets the friction coefficient at the point of contact.
		inline const hkReal& getContactFriction() const;

			/// Gets the spin velocity of the wheel in rad/s.
		inline const hkReal& getSpinVelocity() const;

			/// Gets the spin angle of the wheel in rads.
		inline const hkReal& getSpinAngle() const;

			/// Gets the energy density lost when skidding.
		inline const hkReal& getSkidEnergyDensity() const;

			/// Gets the side force at the particular wheel.
		inline const hkReal& getSideForce() const;

			/// Gets the slip velocity in the forward direction for the particular wheel.
		inline const hkReal& getForwardSlipVelocity() const;

			/// Non-const method, only accesible by Vehicle.
		inline void setIsInContact (const bool &is_in_contact);

			/// Non-const method, only accesible by Vehicle.
		inline void setContactPointWS (const hkVector4 &contact_point_ws);

			/// Non-const method, only accesible by Vehicle.
		inline void setContactNormalWS (const hkVector4 &contact_normal_ws);

			/// Non-const method, only accesible by Vehicle.
		inline void setContactFriction (const hkReal &contact_friction);

			/// Non-const method, only accesible by Vehicle.
		inline void	setSpinVelocity (const hkReal &spin_velocity);

			/// Non-const method, only accesible by Vehicle.
		inline void setSpinAngle (const hkReal &spin_angle);

			/// Non-const method, only accesible by Vehicle.
		inline void setSkidEnergyDensity (const hkReal &skid_energy_density);

			/// Non-const method, only accesible by Vehicle.
		inline void setForwardSlipVelocity (const hkReal &forward_slip);

			/// Non-const method, only accesible by Vehicle.
		inline void setSideForce (const hkReal &side_force);

		inline void setGroundBody(hkRigidBody* groundbody);

			/// Returns the mass of the wheel
		inline const hkReal		getMass()	const;

			/// Sets the stored mass of the body
		inline void setMass(const hkReal mass);

		inline hkRigidBody* getGroundBody();
	public:

		hkVector4 m_hardPointWS;
		hkVector4 m_rayEndPointWS;
		hkVector4 m_contactPointWS;
		hkVector4 m_contactNormalWS;
		hkVector4 m_wheelDirectionWS;
		hkVector4 m_wheelPositionWS;
		hkVector4 m_spinAxisWS;
		hkQuaternion m_wheelRotation;

		bool	m_isInContact;
		hkReal  m_mass;
		hkReal	m_contactFriction;
		hkReal	m_spinVelocity;
		hkReal	m_spinAngle;
		hkReal	m_skidEnergyDensity;
		hkReal	m_sideForce;
		hkReal  m_forwardSlipVelocity;
		hkRigidBody* m_groundBody;
		hkReal  m_clippedInvContactDotSuspension;
	public:
		hkReal m_suspensionLength;
		hkReal m_suspensionRelativeVelocity;
};

#include <hkvehicle2/wheels/hkWheelInfo.inl>



#endif //HK_WHEELINFO_H


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
