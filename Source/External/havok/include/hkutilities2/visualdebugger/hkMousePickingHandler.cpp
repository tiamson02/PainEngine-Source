/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkmath2/hkMath.h>
#include <hkutilities2/visualdebugger/hkMousePickingHandler.h>

#include <hkcollide2/hkCollidable.h>
#include <hkdynamics2/world/hkWorld.h>
#include <hkdynamics2/entity/hkRigidBody.h>
#include <hkutilities2/mousespring/hkMouseSpringAction.h>

hkMousePickingHandler::hkMousePickingHandler( hkWorld* world )
	:	m_world(world),
		m_mouseSpring(HK_NULL),
		m_mouseSpringMaxRelativeForce(1000.0f)
{

}

hkMousePickingHandler::~hkMousePickingHandler()
{
	releaseObject();
}

bool hkMousePickingHandler::pickObject( int id, const hkVector4& worldPosition )
{
	// HACK!  We know the id is actually the address of the Collidable
	hkCollidable* col = reinterpret_cast<hkCollidable*>(id);
 	if(col->getOwner())
	{
		hkRigidBody* rb = reinterpret_cast<hkRigidBody*>(col->getOwner());

		if( rb && !rb->isFixed() )
		{
			hkVector4 positionAinA;
			positionAinA.setTransformedInversePos( rb->getTransform(), worldPosition );

			const hkReal springDamping = 0.5f;
			const hkReal springElasticity = 0.3f;
			const hkReal objectDamping = 0.95f;
			m_mouseSpring = new hkMouseSpringAction( positionAinA, worldPosition, springDamping, springElasticity, objectDamping, rb );
			m_mouseSpring->setMaxRelativeForce(m_mouseSpringMaxRelativeForce);
 			m_world->addAction( m_mouseSpring );
			return true;
		}
	}
	return true;
}

void hkMousePickingHandler::dragObject( const hkVector4& newWorldSpacePoint )
{
	if( m_mouseSpring != HK_NULL )
	{
		m_mouseSpring->setMousePosition( newWorldSpacePoint );
	}
}

void hkMousePickingHandler::releaseObject()
{
	if( m_mouseSpring != HK_NULL)
	{
		if( m_mouseSpring->getOwner() )
		{
			m_world->removeAction( m_mouseSpring );
			m_mouseSpring->getRigidBody()->forceActivate();	
		}
		m_mouseSpring->removeReference();
		m_mouseSpring = HK_NULL;
	}

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
