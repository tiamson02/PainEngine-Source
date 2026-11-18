/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_SIMPLECLOSESTPOINTDETAILS
#define HK_SIMPLECLOSESTPOINTDETAILS

#include <hkcollide2/contact/hkClosestPointsDetails.h>
#include <hkcollide2/dispatch/hkCollisionDispatcher.h>
#include <hkcollide2/dispatch/hkCollisionEnvironment.h>
#include <hkmath2/basetypes/hkContactPoint.h>


	/// This is the default implementation is hkSimpleClosestPointDetails. It implments addClosestPointCallback() 
	/// It maintains a list of the contact points gathered from addClosestPointCallback().
	/// This class is used to maintain the necessary data to calculate collisions for a given pair of collisables
class hkSimpleClosestPointsDetails : public hkClosestPointsDetails
{
	public:

			/// constructor  - takes 2 collidables, the dispatcher (used to create collision agents) and the collision environmant
			/// The dispatcher and the collision environment must be alive for the lifetime of this object
		inline hkSimpleClosestPointsDetails( const hkCollidable& colA, const hkCollidable& colB, const hkCollisionEnvironment& colEnv );

			/// This cleanup function must be instead of the destructor, because the collision environment must be present to
			/// delete the collision agent
		inline void destroy( const hkCollisionEnvironment& env );
		
			/// A wrapper to re-evaluate the closest points for the pair
		inline void updateClosestPoints( const hkCollisionEnvironment& env );
		
			/// Get the closest distance from the last set of collision points calculated
		inline hkReal getClosestDistance() const;
	
		inline	int	getClosestIndex() const;

			/// Get a list of the contact points
		inline const hkArray<hkContactPoint>& getContactPoints() const;

			/// Get collidable A
		inline const hkCollidable&	getCollidableA() const;

			/// Get collidable B
		inline const hkCollidable&	getCollidableB() const;

	public:

			//callback by the system
		virtual	void addClosestPointCallback( const ClosestPointEvent& event );

	protected:
		
		inline void resetResults();

		~hkSimpleClosestPointsDetails( );

	protected:

		hkInplaceArrayAligned16<hkContactPoint, 4> m_contactPoints;

		hkReal m_closestDistance;
		int		m_closestIndex;

		const hkCollidable&	m_collidableA;
		const hkCollidable&	m_collidableB;
		hkCollisionAgent*	m_collisionAgent;
};

#include <hkcollide2/contact/hkSimpleClosestPointsDetails.inl>

#endif //HK_SIMPLECLOSESTPOINTDETAILS

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
