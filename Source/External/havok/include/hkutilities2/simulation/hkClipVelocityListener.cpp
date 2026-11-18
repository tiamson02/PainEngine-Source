/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkutilities2/simulation/hkClipVelocityListener.h>

#include <hkdynamics2/entity/hkRigidBody.h>
#include <hkdynamics2/world/hkWorld.h>
#include <hkdynamics2/world/hkSimulationIsland.h>
#include <hkdynamics2/world/listener/hkWorldPairwiseBackstepFilter.h>
#include <hkcollide2/shape/hkShapeTypes.h>
#include <hkutilities2/simulation/hkConvexSweepShape.h>



/// \class hkClipVelocityListener
/// In the postSimulateCallback, the hkClipVelocityListener sets the "stretch" value of all the moving
/// hkConvexSweepShapes in the world, so that they will pick up collisions for any object along the path
/// of their movement over the next step. For each collision, the contactPointAddedCallback checks the collision
/// pair using the hkWorld's hkWorldPairwiseBackstepFilter, to see if the pair's velocity should be clipped. If the 
/// filter returns true, then the velocity of the object is clipped if it is over the supplied threshold 
/// (the linearVelocity parameter in the constructor).



/// The class is also a hkWorldDeletionListener, and automatically removes a reference to itself when the world is deleted.
/// If you do not add any other references to this object, this means the object will delete itself when the world is deleted.
/// You must supply a linear velocity - this is the velocity that objects will be clipped to. As a rule of thumb,
/// for 20 cm objects, simulated at 60 Hz (with backstepping enabled - see hkWorld for details) should be clipped to
/// between 15 and 20 m/s.
/// Note that the hkConvexSweepShape objects are only stretched if the velocity is 5 m/s over the linear veloctiy clip value.
hkClipVelocityListener* HK_CALL hkClipVelocityListener::create( hkWorld* world, hkReal linearVelocity )
{
	return new hkClipVelocityListener( world, linearVelocity );
}


hkClipVelocityListener::hkClipVelocityListener( hkWorld* world, hkReal linearVelocity )
{
	m_world = world;
	world->addWorldDeletionListener( this );
	world->addCollisionListener( this );
	world->addWorldPostSimulationListener( this );

	m_maxLinearVelocity = linearVelocity;
	m_maxLinearVelocitySqr = linearVelocity * linearVelocity;
}


void hkClipVelocityListener::worldDeletedCallback( hkWorld* world )
{
	m_world->removeWorldDeletionListener( this );
	m_world->removeCollisionListener( this );
	m_world->removeWorldPostSimulationListener( this );
	m_world = HK_NULL;
	this->removeReference();
}


void hkClipVelocityListener::postSimulationCallback( hkWorld* world, const hkStepInfo& stepInfo )
{
	for ( int i = 0; i < world->getActiveSimulationIslands().getSize(); i++ )
	{
		hkSimulationIsland* island = world->getActiveSimulationIslands()[i];
		for ( int j = 0; j < island->getEntities().getSize(); j++ )
		{
			hkRigidBody* rb = static_cast<hkRigidBody*>( island->getEntities()[j] );
			if ( rb->getCollidable().getShape()->getPrimaryType() == HK_SHAPE_USER7 )
			{
				const hkConvexSweepShape* css = static_cast<const hkConvexSweepShape*>(rb->getCollidable().getShape());
				css->m_wantReset = false;
				if (rb->getLinearVelocity().lengthSquared3() > (m_maxLinearVelocitySqr + 25.f) )
				{
					css->m_displacementLocal._setRotatedInverseDir( rb->getTransform().getRotation(), rb->getLinearVelocity());
					css->m_displacementLocal.mul4( stepInfo.m_deltaTime );
				}
			}
		}
	}
}


hkClipVelocityListener::~hkClipVelocityListener()
{
	if(m_world)
	{
		m_world->removeWorldDeletionListener( this );
		m_world->removeCollisionListener( this );
		m_world->removeWorldPostSimulationListener( this );
	}
}


void hkClipVelocityListener::contactPointAddedCallback( hkContactPointAddedEvent& event )
{

	if (	( m_world->getPairwiseBackstepFilter() != HK_NULL ) && 
			( m_world->getPairwiseBackstepFilter()->shouldBackstep( event.m_collidableA, event.m_collidableB ) ) )
	{
		if (event.m_collidableA.getShape()->getPrimaryType() == HK_SHAPE_USER7 )
		{
			hkRigidBody* rb = static_cast<hkRigidBody*>( event.m_collidableA.getOwner() );
						
			//
			// Check the linear velocity and clip if too fast
			//
			const hkReal velSqr = rb->getLinearVelocity().lengthSquared3();
			if ( velSqr > m_maxLinearVelocitySqr )
			{
				hkVector4 v = rb->getLinearVelocity();
				const hkReal invVel = hkMath::sqrtInverse( velSqr );
				v.mul4( m_maxLinearVelocity * invVel );
				rb->setLinearVelocity( v );

				//
				// Set the restitution of this collision point to 0, and set the convex sweep shape "length" to zero
				//

				event.m_contactPointProperties->setRestitution(0);
				const hkConvexSweepShape* css = static_cast<const hkConvexSweepShape*>( event.m_collidableA.getShape() );
				css->m_displacementLocal.setZero4();
				css->m_wantReset = true;
			}
		}

		if (event.m_collidableB.getShape()->getPrimaryType() == HK_SHAPE_USER7 )
		{
			hkRigidBody* rb = static_cast<hkRigidBody*>( event.m_collidableB.getOwner() );
			
			//
			// Check the linear velocity and clip if too fast
			//
			const hkReal velSqr = rb->getLinearVelocity().lengthSquared3();
			if ( velSqr > m_maxLinearVelocitySqr )
			{
				hkVector4 v = rb->getLinearVelocity();
				const hkReal invVel = hkMath::sqrtInverse( velSqr );
				v.mul4( m_maxLinearVelocity * invVel );
				rb->setLinearVelocity( v );


				//
				// Set the restitution of this collision point to 0, and set the convex sweep shape "length" to zero
				//

				event.m_contactPointProperties->setRestitution(0);
				const hkConvexSweepShape* css = static_cast<const hkConvexSweepShape*>( event.m_collidableB.getShape() );
				css->m_displacementLocal.setZero4();
				css->m_wantReset = true;
			}
		}
	}

}

void hkClipVelocityListener::contactPointRemovedCallback( hkContactPointRemovedEvent& event )
{
}

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
