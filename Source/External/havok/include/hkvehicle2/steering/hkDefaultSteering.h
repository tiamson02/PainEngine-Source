/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DEFAULTSTEERING_H
#define HK_DEFAULTSTEERING_H

	/// Default implementation of SteeringComponent. See hkDefaultSteeringBP for more
	/// parameter details.
class DefaultSteering: public SteeringComponent
{
	public:
		
			/// Constructor.
		DefaultSteering (const hkDefaultSteeringBP &bp);

			/// Reset all values except pointer values with those passed in the blueprint.
		void reinitialize( const hkDefaultSteeringBP &bp );

			/// Destructor, frees all memory associated with the object.
		~DefaultSteering();

			//
			// Function overriden from VehicleComponent.
			//

			/// Overriden from VehicleComponent. In this case this method updates the value
			/// of m_mainSteeringAngle.
		void updateComponent(const hkStepInfo& stepInfo);

	public:

			//
			// SET Methods.
			//

		inline void setMaxSteeringAngle(const hkReal &angle);
		inline void setMaxSpeedFullSteeringAngle(const hkReal &speed);
		inline void setWheelDoesSteer(int wheel, const bool &does_steer);

			//
			// GET Methods.
			//

		inline const hkReal &getMaxSteeringAngle() const;
		inline const hkReal &getMaxSpeedFullSteeringAngle() const;
		inline const bool &getWheelDoesSteer(int wheel) const;

	protected:
		
		HK_FORCE_INLINE hkReal calcMainSteeringAngle() const;

	private:

		    /// Reset all values except pointer values.
		inline void reinitializeInternal( const hkDefaultSteeringBP &bp );

		bool *m_wheelsDoesSteer;
		hkReal m_maxSteeringAngle;
		hkReal m_maxSpeedFullSteeringAngle;
};

#include <hkvehicle2/steering/hkDefaultSteering.inl>

#endif // HK_DEFAULTSTEERING_H

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
