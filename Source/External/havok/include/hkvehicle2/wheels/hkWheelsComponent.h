/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef INC_WHEELSCOMPONENT
#define INC_WHEELSCOMPONENT

#include <hkvehicle2/wheels/hkWheelInfo.h>
#include <hkvehicle2/wheels/hkWheelsComponentBP.h>

/// The WheelsComponent is the vehicle component responsible for maintaining
/// the state of the wheels of the vehicle (radius, width, friction). It also stores the
/// topology of the wheels, i.e., the number of axles and what axle does each wheel
/// belong to.
class WheelsComponent: public hkBaseObject
{
	public:
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_VEHICLE);

			/// Constructor.
		WheelsComponent (const hkWheelsComponentBP &bp);

			/// Destructor.
		virtual ~WheelsComponent();

			/// Virtual, called every simulation step, after everything is done in HavokVehicle::doSimulationStep().
		inline virtual void postUpdateWheels();
		
			/// Calculate the current position and rotation of a wheel for the graphics engine
		void calcCurrentPositionAndRotation( const hkRigidBody* chassis, const SuspensionComponent* suspension,  int wheelNo, hkVector4& posOut, hkQuaternion& rotOut );

		/* TOPOLOGICAL INFORMATION */

			/// Gets the number of wheels.
		inline const int getNumWheels() const;

			/// Returns the number of axles of the vehicle.
		inline const int getNumAxles() const;

			/// Returns the number of wheels on the specified axle.
		inline const int getNumWheelsOnAxle(int axle) const;

		/* Per-Wheel Properties */

			/// Returns the wheel axis number.
		inline const int getWheelAxle(int wheel)	const;

			/// Returns the wheel radius in meters.
		inline const hkReal &getWheelRadius(int wheel) const;

			/// Returns the wheel width in meters.
		inline const hkReal &getWheelWidth(int wheel) const;

			/// Returns the wheel friction coefficient.
		inline const hkReal &getWheelFriction(int wheel)	const;

			/// Returns the wheel viscosity friction coefficient.
		inline const hkReal &getWheelViscosityFriction(int wheel) const;

			/// Returns a reference to the associated wheelinfo.
		inline const WheelInfo &getWheelInfo(int wheel) const;


	public:

		/*
		** Set Methods
		*/

			/// Sets the wheel radius in meters.
		inline void setWheelRadius (int wheel, const hkReal &radius);

			/// Sets the wheel friction coefficient.
		inline void setWheelWidth (int wheel, const hkReal &width);

			/// Sets the wheel friction coefficient.
		inline void setWheelFriction (int wheel, const hkReal &friction);

			/// Sets the wheel viscosity friction coefficient.
		inline void setWheelViscosityFriction (int wheel, const hkReal &v_friction);

	protected:

		const int m_numWheels;

		hkReal* m_wheelsRadius;
		hkReal* m_wheelsWidth;
		hkReal* m_wheelsFriction;
		hkReal* m_wheelsViscosityFriction;

	private:

		// Topology cannot be changed during runtime.
		int* m_wheelsAxle;

		int m_numAxles;			// Calculated on construction, never changes
		int* m_numWheelsPerAxle;	// ""


	private:

		hkInplaceArrayAligned16<WheelInfo,4> m_wheelsInfo;

			/// Returns a _non-const_ reference to the i-th wheel info.
			/// Only the vehicle can access it.
		inline WheelInfo &getWheelInfoRef(int wheel);

			//  Only the vehicle can access it.
			/// The vehicle is responsible to update this information.
		friend class HavokVehicle;


	protected:

		    /// Reset all values except pointer values.
		void reinitializeInternal( const hkWheelsComponentBP &bp );
};

#include <hkvehicle2/wheels/hkWheelsComponent.inl>



#endif //INC_WHEELSCOMPONENT

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
