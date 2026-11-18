/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_COLLIDE2_CONTACT_MGR_H
#define HK_COLLIDE2_CONTACT_MGR_H

struct hkProcessCdPoint;
struct hkProcessCollisionData;
class hkCdBody;
struct hkProcessCollisionInput;
class hkCollidable;

#include <hkmath/basetypes/hkContactPoint.h>
#include <hkmath/basetypes/hkContactPointMaterial.h>

	/// The hkContactMgr is an interface used by a collision agent to report its results. 
	/// This class is used internally by hkdynamics
class hkContactMgr: public hkReferencedObject
{
	public:
		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_CONTACT );

			/// Add a new contact point. This should return an unique and persistent id for that point
			/// See hkCollisionListener for further details.
		virtual hkContactPointId addContactPoint( const hkCdBody& a, const hkCdBody& b, const hkProcessCollisionInput& input, hkContactPoint& cp ) = 0;

			/// reserve a number of contact points, if you want to release those reserve
			/// contact points, just call this function with a negative parameter
			/// Note: if this function fails, no contact points have been reserved.
			/// See hkCollisionListener for further details.
		virtual hkResult reserveContactPoints( int numPoints ) = 0;

			/// Removes a contact point with a given id (given by addContactPoint)
			/// See hkCollisionListener for further details.
		virtual void removeContactPoint( hkContactPointId cpId ) = 0;

			/// After all agents between a given pair of hkCollidables are called, all information
			/// is passed to process contact. See hkCollisionListener for further details.
		virtual void processContact( const hkCollidable& a, const hkCollidable& b, const hkProcessCollisionInput& input, hkProcessCollisionData& collisionData ) = 0;

			/// delete yourself
		virtual void cleanup() = 0;

		enum ToiAccept
		{
			TOI_ACCEPT = 0,
			TOI_REJECT = 1
		};

			/// Adds a potential toi and returns what to do with this toi.
			/// This is the only reasonable place to reject tois
			/// See hkCollisionListener for further details.
		virtual ToiAccept addToi( const hkCdBody& a, const hkCdBody& b, const hkProcessCollisionInput& input, hkContactPoint& cp, hkTime toi, hkReal projectedVelocity, hkContactPointMaterial& materialOut ) = 0;
};



#endif // HK_COLLIDE2_CONTACT_MGR_H

/*
* Havok SDK - PUBLIC RELEASE, BUILD(#20060114)
*
* Confidential Information of Havok.  (C) Copyright 1999-2006 
* Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
* Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
* rights, and intellectual property rights in the Havok software remain in
* Havok and/or its suppliers.
*
* Use of this software for evaluation purposes is subject to and indicates 
* acceptance of the End User licence Agreement for this product. A copy of 
* the license is included with this software and is also available from salesteam@havok.com.
*
*/
