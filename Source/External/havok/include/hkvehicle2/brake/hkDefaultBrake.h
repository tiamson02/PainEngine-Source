/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef INC_DEFAULTBRAKE
#define INC_DEFAULTBRAKE



/// Default implementation of BrakeComponent. See hkDefaultBrakeBP for more parameter details.
class DefaultBrake : public BrakeComponent
{
	public:

		/// Constructor
		DefaultBrake (const hkDefaultBrakeBP &bp);

		/// reset all values except pointer values
		void reinitialize( const hkDefaultBrakeBP &bp );

		/// Destructor
		~DefaultBrake ();

		/*
		** Function overriden from VehicleComponent
		*/

		/// Overriden from VehicleComponent, in this case this method updates the value
		void updateComponent(const hkStepInfo& stepInfo);
		// of m_wheelBrakingTorque and m_wheelIsFixed for every wheel
	
	public:

		/*
		** GET Methods
		*/
		inline const hkReal &getWheelMaxBrakingTorque(int wheel) const;
		inline const hkReal &getWheelMinPedalInputToBlock(int wheel) const;
		inline const hkReal &getWheelMinTimeToBlock() const;
		inline const bool &getWheelIsConnectedToHandbrake(int wheel) const;

		/*
		** SET Methods
		*/
		inline void setWheelMaxBrakingTorque(int wheel, const hkReal &max_braking_torque);
		inline void setWheelMinPedalInputToBlock(int wheel, const hkReal &min_pedal_input_to_block);
		inline void setWheelMinTimeToBlock(const hkReal &min_time_to_block);
		inline void setWheelIsConnectedToHandbrake(int wheel, const bool &is_connected_to_handbrake);


	protected:

		hkReal *m_wheelsMaxBrakingTorque;
		hkReal *m_wheelsMinPedalInputToBlock;
		hkReal m_wheelsMinTimeToBlock;
		bool *m_wheelsIsConnectedToHandbrake;

	protected:

		/// Last time that the torque applied was maximum. In order to implement wheel blockage
		//warning this is a float right now, a time struct would be better
		// because over time the accuracy will get less and less
		hkReal	m_wheelsLastTimeMaxPedalInput;


	private:

		    /// reset all values except pointer values
		void reinitializeInternal( const hkDefaultBrakeBP &bp );
};

#include <hkvehicle2/brake/hkDefaultBrake.inl>



#endif // INC_DEFAULTBRAKE

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
