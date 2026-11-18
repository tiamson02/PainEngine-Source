/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DEFAULTAERODYNAMICS_H
#define HK_DEFAULTAERODYNAMICS_H



/// Default implementation of AerodynamicsComponent. See hkDefaultAerodynamicsBP for more parameter details.
class DefaultAerodynamics : public AerodynamicsComponent
{
	public:
			/// Constructor.
		DefaultAerodynamics(const hkDefaultAerodynamicsBP &bp);

		    /// Reset all values except pointer values.
		void reinitialize( const hkDefaultAerodynamicsBP &bp );


		/*
		** Overrided from VehicleComponent
		*/

			/// Overriden from VehicleComponent, this method updates the values of
			/// the total torque and forces on chassis (protected members).
		inline void updateComponent(const hkStepInfo& stepInfo);

	public:

		/*
		** GET Methods
		*/
			/// Get the air density.
		inline const hkReal	 &getAirDensity() const;

			/// Get the frontal area.
		inline const hkReal	 &getFrontalArea() const;

			/// Get the drag coefficient.
		inline const hkReal	 &getDragCoefficient() const;

			/// Get the lift coefficient.
		inline const hkReal	 &getLiftCoefficient() const;

			/// Get the extra gravity.
		inline const hkVector4 &getExtraGravity() const;

		/*
		** SET Methods
		*/
			/// Set the air density.
		inline void setAirDensity(const hkReal &air_density);

			/// Set the frontal area.
		inline void setFrontalArea(const hkReal &frontal_area);

			/// Set the drag coefficient.
		inline void setDragCoefficient(const hkReal &drag_coefficient);

			/// Set the lift coefficient.
		inline void setLiftCoefficient(const hkReal &lift_coefficient);

			/// Set the extra gravity.
		inline void setExtraGravity(const hkVector4 &extra_gravity);


	protected:
		hkReal calcAerodynamicDrag(hkReal forwardVelocity) const;
		hkReal calcAerodynamicLift(hkReal forwardVelocity) const;

		hkReal	m_airDensity;
		hkReal	m_frontalArea;
		hkReal	m_dragCoefficient;
		hkReal	m_liftCoefficient;

		hkVector4	m_extraGravityws;

	private:

		    /// Reset all values except pointer values.
		void reinitializeInternal( const hkDefaultAerodynamicsBP &bp );
};

#include <hkvehicle2/aerodynamics/hkDefaultAeroDynamics.inl>



#endif //HK_DEFAULTAERODYNAMICS_H

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
