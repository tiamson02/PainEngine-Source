/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HK_DYNAMICS2_MATERIAL_H
#define HK_DYNAMICS2_MATERIAL_H

#include <hkcollide2/dispatch/hkCollisionResponse.h>

	/// hkMaterials allow you to add extra information to collision detection results, such as friction and restitution values.
class hkMaterial: public hkCollisionResponse
{
	public:

			/// Constructor initialises friction to .5 and restitution to .4.
		inline hkMaterial();

			/// Returns the friction coefficient (dynamic and static).
		inline hkReal getFriction() const;

			/// Returns the default restitution.
			//  restitution = bounciness (1 should give object all its energy back, 0 means it just sits there, etc.).
		inline hkReal getRestitution() const;
			
			/// Sets the friction coefficient. Note: Setting this will not update existing contact information.
		inline void setFriction( hkReal newFriction );

			/// Sets the restitution coefficient. Note: Setting this will not update existing contact information.
		inline void setRestitution( hkReal newRestitution );

			/// This returns the default way to combine two friction values.
			/// We take the geometric mean ( sqrt (frictionA * frictionB) )
		static inline hkReal HK_CALL getCombinedFriction( hkReal frictionA, hkReal frictionB);

			/// This returns the default way to combine two restitution values.
			/// We take the geometric mean ( sqrt (restitutionA * restitutionB) )
		static inline hkReal HK_CALL getCombinedRestitution( hkReal restitutionA, hkReal restitutionB);

	public:
			/// A list of some response types as initially set up by the hkWorld constructor, which can be overridden
			/// The default behavior is that a higher hkResponseType overrides a lower hkResponseType. For instance,
			/// HK_NO_RESPONSE will have a higher priority than HK_REPORTING_SIMPLE_CONTACT_RESPONSE.
		enum hkResponseType
		{
				/// Do normal collision resolution, but do not call any hkCollisionListener functions.
			HK_SIMPLE_CONTACT_RESPONSE = 0,

				/// Just do some reporting.
			HK_REPORTING_RESPONSE = 1, 

				/// Do nothing, ignore all the results - useful when using phantoms.
			HK_NO_RESPONSE = 2,

			HK_RESPONSE_MAX = 8
		};

	private:

		hkReal m_friction;
		hkReal m_restitution;
};

#include <hkdynamics2/common/hkMaterial.inl>


#endif // HK_DYNAMICS2_MATERIAL_H
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
