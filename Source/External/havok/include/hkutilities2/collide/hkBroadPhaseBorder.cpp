/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkutilities2/collide/hkBroadPhaseBorder.h>
#include <hkcollide2/shape/hkAabbPhantomShape.h>
#include <hkcollide2/hkCollidable.h>
#include <hkdynamics2/entity/hkRigidBody.h>
#include <hkdynamics2/entity/hkRigidBodyConstructionInfo.h>
#include <hkdynamics2/world/hkWorld.h>

//////////////////////////////////////////////////////////////////////////////////////////
// Utility phantom shape
//////////////////////////////////////////////////////////////////////////////////////////

class hkBorderPhantomShape : public hkAabbPhantomShape
{
	public:

		hkBorderPhantomShape( hkBroadPhaseBorder* border, const hkVector4& mi, const hkVector4& ma );

		virtual void phantomEnterEvent(const  hkCollidable& collidableA, const hkCollidable& collidableB,	const hkCollisionEnvironment& env );

		virtual void phantomLeaveEvent( const hkCollidable& collidableA, const hkCollidable& collidableB,	const hkCollisionEnvironment& env ) {}


		hkBroadPhaseBorder* m_border;
};


hkBorderPhantomShape::hkBorderPhantomShape( hkBroadPhaseBorder* border, const hkVector4& mi, const hkVector4& ma )
:	hkAabbPhantomShape( mi, ma ),
	m_border( border )
{
}

void hkBorderPhantomShape::phantomEnterEvent( const hkCollidable& collidableA, const hkCollidable& collidableB,	const hkCollisionEnvironment& env )
{

	// it could be a HK_BROAD_PHASE_PHANTOM so then don't upcast
	if (collidableB.getType() == HK_BROAD_PHASE_ENTITY)
	{
		hkEntity* entity = reinterpret_cast<hkEntity*>(collidableB.m_owner);
		m_border->maxPositionExceededCallback( entity );
	}
}



//////////////////////////////////////////////////////////////////////////////////////////
// Broad phase border implementation
//////////////////////////////////////////////////////////////////////////////////////////


hkBroadPhaseBorder::hkBroadPhaseBorder( hkWorld* world, hkReal broadPhaseSize )
{
	m_world = world;
	addReference();
	m_world->addWorldDeletionListener( this );

	hkVector4 min;
	hkVector4 max;
	hkRigidBodyConstructionInfo rbci;
	rbci.m_motionType = HK_FIXED_MOTION;

	const hkReal m = broadPhaseSize;
	{
		min.set( m, -m, -m);
		max.set( m,  m,  m);
		rbci.m_shape = new hkBorderPhantomShape( this, min, max );
		hkRigidBody* rb = new hkRigidBody( rbci );
		rbci.m_shape->removeReference();
		world->addEntity( rb );
		rb->removeReference();
	}
	{
		min.set( -m, -m, -m);
		max.set( -m,  m,  m);
		rbci.m_shape = new hkBorderPhantomShape( this, min, max );
		hkRigidBody* rb = new hkRigidBody( rbci );
		rbci.m_shape->removeReference();
		world->addEntity( rb );
		rb->removeReference();
	}

	{
		min.set( -m, m, -m);
		max.set(  m, m,  m);
		rbci.m_shape = new hkBorderPhantomShape( this, min, max );
		hkRigidBody* rb = new hkRigidBody( rbci );
		rbci.m_shape->removeReference();
		world->addEntity( rb );
		rb->removeReference();
	}
	{
		min.set( -m, -m, -m);
		max.set(  m, -m,  m);
		rbci.m_shape = new hkBorderPhantomShape( this, min, max );
		hkRigidBody* rb = new hkRigidBody( rbci );
		rbci.m_shape->removeReference();
		world->addEntity( rb );
		rb->removeReference();
	}

	{
		min.set( -m, -m, m);
		max.set(  m,  m,  m);
		rbci.m_shape = new hkBorderPhantomShape( this, min, max );
		hkRigidBody* rb = new hkRigidBody( rbci );
		rbci.m_shape->removeReference();
		world->addEntity( rb );
		rb->removeReference();
	}

	{
		min.set( -m, -m,  -m);
		max.set(  m,  m,  -m);
		rbci.m_shape = new hkBorderPhantomShape( this, min, max );
		hkRigidBody* rb = new hkRigidBody( rbci );
		rbci.m_shape->removeReference();
		world->addEntity( rb );
		rb->removeReference();
	}
}

void hkBroadPhaseBorder::maxPositionExceededCallback( hkEntity* entity )
{
	m_world->removeEntity( entity );
}

void hkBroadPhaseBorder::worldDeletedCallback( hkWorld* world )
{
	m_world->removeWorldDeletionListener( this );
	removeReference();
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
