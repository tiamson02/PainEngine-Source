/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_CONSTRAINTSOLVER2_CONTACT_POINT_PROPERTIES_H
#define HK_CONSTRAINTSOLVER2_CONTACT_POINT_PROPERTIES_H

#include <hkconstraintsolver2/solve/hkSolverResults.h>

	/// This class is used to get and set the friction for a contact point. You can also use it to attach your own user data
	/// to a contact point.  This can be used for example to set a friction map value in when a contact point is added
	/// so that the same data can be used when the contact point is being updated (from a processContactCallback() for example)
class hkContactPointProperties : public hkSolverResults
{
	public:
		
			/// Get the friction for the contact point. This must be between 0 (no friction) and 1 (max friction)
		inline hkReal getFriction( ) const;

			/// Set the friction for the contact point. This must be between 0 (no friction) and 1 (max friction)
		inline void setFriction( hkReal r );

			/// Set the restitution for the contact point. This must be between 0 ( all energy lost in collision ) and 1 (max restitution)
		inline hkReal getRestitution( ) const;

			/// Set the restitution for the contact point. This must be between 0 ( all energy lost in collision ) and 1 (max restitution)
			/// Note: for a contact point that has been around for several frames this has virtually no effect. You need to set the
			/// restition for a contact point on creation of the contact point (when the approaching velocities are non-zero).
		inline void setRestitution( hkReal r );

			/// Get the user data	
		inline void* getUserData() const;
			
			/// Set the user data. This allows you to store any info, or a pointer with a contact point.
		inline void setUserData( void* data );
		
	protected:

		hkReal m_friction;
		hkReal m_restitution;
		void* m_userData;

	public:
		
		// internal data, not for client use
		hkReal m_internalDataA;
		hkUint16 m_internalDataB;
};


#include <hkconstraintsolver2/constraint/contact/hkContactPointProperties.inl>

#endif // HK_CONSTRAINTSOLVER2_CONTACT_POINT_PROPERTIES_H

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
