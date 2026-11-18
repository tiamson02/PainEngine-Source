/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_HAVOKVEHICLE_H
#define HK_HAVOKVEHICLE_H

#include <hkdynamics2/common/hkStepInfo.h>


class hkHavokVehicleBP;
/// The main class, performs the vehicle simulation.
class HavokVehicle : public hkBaseObject
{

	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_VEHICLE);

			/// Constructor. The vehicle takes ownership of the components, i.e.,
			/// the components will be destroyed with the vehicle.
		HavokVehicle(const hkHavokVehicleBP& bp);

			/// Destructor. Destroys the vehicle and its components.
		virtual ~HavokVehicle();


		/* TIME INFO : This is refreshed every simulation step */

			/// Gets a reference to timing information.
		//inline const TimeInfo& getTimeInfo() const;
		inline const hkReal	getCurrentTime() const;

		// const version for read access to motionstate
		inline const class hkRigidBody* getChassisMotionState() const;

		// non-const version for write/read access to motionstate
		inline class hkRigidBody* getChassisMotionState();

		

		/* ACCESS TO THE DIFFERENT COMPONENTS */

			/// Gets (a const reference to) the wheels component.
		inline const class WheelsComponent* getWheelsComponent() const;

		inline class WheelsComponent* getWheelsComponent() ;

			/// Gets (a const reference to) the chassis component.
		inline const class ChassisComponent* getChassisComponent() const;

			/// Gets (a const reference to) the driver input component.
		inline const class DriverInputComponent* getDriverInputComponent() const;

			/// Gets (a const reference to) the steering component.
		inline const class SteeringComponent* getSteeringComponent() const;

			/// Gets (a const reference to) the engine component.
		inline const class EngineComponent* getEngineComponent() const;

			/// Gets (a const reference to) the transmission component.
		inline const class TransmissionComponent*	getTransmissionComponent() const;

			/// Gets (a const reference to) the brake component.
		inline const class BrakeComponent* getBrakeComponent() const;

			/// Gets (a const reference to) the suspension component.
		inline const class SuspensionComponent* getSuspensionComponent() const;

			/// Gets (a const reference to) the aerodynamics component.
		inline const class AerodynamicsComponent*	getAerodynamicsComponent() const;

			/// Applies the vehicle controller at a given time / timestep.
		void applyVehicle(const hkStepInfo& stepInfo);

	protected:

			/// Calls update on all components.
		inline void updateComponents(const hkStepInfo& stepInfo) const;


		// **** THESE ARE THE ONLY FUNCTIONS THAT NEED TO BE IMPLEMENTED BY HAVOKVEHICLES ****

			/// Needs to be implemented by Havok vehicles.
		virtual void updateBodiesInfo(const hkStepInfo& stepInfo) = 0;


			// state of the simulation (i.e., query the simulation)

			/// Needs to be implemented by Havok vehicles.
			/// This function should query the different components and apply the car behavior.
		virtual void doSimulationStep(const hkStepInfo& stepInfo) = 0;

		// (i.e., affect the simulation)
		// ***********************************************************************************

	protected:

		// These functions are needed since class friendship is not inherited.

			/// Retrieves a non-const bodyinfo reference from the chassis.
		inline class BodyInfo &getChassisBodyInfoRef();

			/// Retrieves a non-const bodyinfo reference of the given wheel.
		inline class WheelInfo &getWheelBodyInfoRef(int wheel);


	private:
		//warning this hkReal is inaccurate over time !
		hkReal					m_currentTime;

		WheelsComponent*		m_wheels;
		ChassisComponent*		m_chassis;
		DriverInputComponent*	m_driverInput;
		SteeringComponent*		m_steering;
		EngineComponent*		m_engine;
		TransmissionComponent*	m_transmission;
		BrakeComponent*			m_brake;
		SuspensionComponent*	m_suspension;
		AerodynamicsComponent*	m_aerodynamics;
		class hkRigidBody*		m_chassisMotionState;
		
};

#include <hkvehicle2/vehicle/hkHavokVehicle.inl>



#endif

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
