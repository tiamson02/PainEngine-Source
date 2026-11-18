/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef INC_SUSPENSIONCOMPONENT
#define INC_SUSPENSIONCOMPONENT



/// This class controls the calculation of suspension forces and how are they applied.
class SuspensionComponent : public VehicleComponent
{
	public:
			/// Constructor.
		SuspensionComponent (const hkSuspensionComponentBP &bp);

			/// Destructor.
		~SuspensionComponent();

		/*
		** Interface
		*/

			/// Gets the force applied by the suspension of the wheel.
		inline const hkReal &getWheelSuspensionForce(int wheel) const;

	public:
		/*
		**  SET Methods
		*/

			/// Sets the hardpoint (in chassis space) of the i-th wheel.
		inline void setWheelHardpointCS (int wheel, const hkVector4 &hardpoint_cs);

			/// Sets the direction (in chassis space) of the suspension of the i-th wheel.
		inline void setWheelDirectionCS (int wheel, const hkVector4 &direction_cs);


			/// Sets the length of the suspension of the i-th wheel.
		inline void setWheelLength (int wheel, const hkReal &length);


		/*
		** GET Methods
		*/

			/// Returns the suspension hardpoint in body space of the wheel.
		inline const hkVector4 &getWheelHardpointCS(int wheel) const;


			/// Returns the direction of the suspension in body space of the wheel.
		inline const hkVector4 &getWheelDirectionCS(int wheel) const;

			/// Returns the rest length of the suspension of the wheel.
		inline const hkReal &getWheelLength(int wheel) const;


	protected:

			/// This member must be updated by the method updateComponent().
		hkReal *m_wheelsSuspensionForce;

	protected:

			/// This member must be updated by the method updateComponent().
		const int m_numWheels;

			/// This member must be updated by the method updateComponent().
		hkVector4 *m_wheelsHardpointCS;

			/// This member must be updated by the method updateComponent().
		hkVector4 *m_wheelsDirectionCS;

			/// This member must be updated by the method updateComponent().
		hkReal *m_wheelsLength;

	protected:

		    /// Reset all values except pointer values.
		void reinitializeInternal( const hkSuspensionComponentBP &bp );

};

#include <hkvehicle2/suspension/hkSuspensionComponent.inl>



#endif // INC_SUSPENSIONCOMPONENT

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
