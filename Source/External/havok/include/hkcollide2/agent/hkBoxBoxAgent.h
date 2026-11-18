/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_COLLIDE2_BOX_BOX_AGENT_H
#define HK_COLLIDE2_BOX_BOX_AGENT_H

#include <hkcollide2/agent/hkCollisionAgent.h>
#include <hkcollide2/manifold/hkFeatureManifold.h>


	/// An hkBoxBoxAgent handles collisions between pairs of hkBoxShapes. This produces high quality manifolds, in many cases with a good CPU
class hkBoxBoxAgent : public hkCollisionAgent
{
	public:

			/// Registers this agent with the collision dispatcher.
		static void HK_CALL registerAgent(hkCollisionDispatcher* dispatcher);

			// hkCollisionAgent interface implementation.
		virtual void processCollision(const hkCollidable& collA, const hkCollidable& collB, const hkCollisionEnvironment& env, hkCollisionResult& result);

			// hkCollisionAgent interface implementation.
		virtual void getClosestPoints( const hkCollidable& collidableA, const hkCollidable& collidableB, const hkCollisionEnvironment& env, class hkClosestPointsDetails& details );
	
			// hkCollisionAgent interface implementation.
		virtual bool arePenetrating( const hkCollidable& collA, const hkCollidable& collB, const hkCollisionEnvironment& env );

			// hkCollisionAgent interface implementation.
		virtual void cleanup( const hkCollisionEnvironment& env );

			/// Returns true if aggresive manifold completion is active.  Default value is true.
			/// Aggressive manifold completion will find more manifold points during initial contact
			/// but worst case performance can be much higher than average performance
		static bool	getUseAgressiveManifoldCompletion();

			/// Aggressive manifold completion will find more manifold points during initial contact
			/// but worst case performance can be much higher than average performance
		static void	setUseAgressiveManifoldCompletion( bool useAggresiveCompletion);

	protected:
			/// Called by createBoxBoxAgent().
		hkBoxBoxAgent();

			/// Agent creation function used by the hkCollisionDispatcher. 
		static hkCollisionAgent* HK_CALL createBoxBoxAgent(	const hkCollidable& collidableA, const hkCollidable& collidableB, const hkCollisionEnvironment& environment);


	private:
		hkFeatureManifold m_manifold;
};

#endif // HK_COLLIDE2_BOX_BOX_AGENT_H

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
