
/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkdynamics2/hkDynamics2.h>
#include <hkdynamics2/world/hkWorld.h>
#include <hkdynamics2/entity/hkRigidBody.h>
#include <hkcollide2/shape/hkShape.h>
#include <hkcollide2/shape/hkListShape.h>
#include <hkutilities2/fractureutil/hkFractureListener.h>
#include <hkdynamics2/entity/hkRigidBodyConstructionInfo.h>
#include <hkutilities2/inertia/hkInertiaTensorComputer.h>
#include <hkcollide2/shape/hkSphereShape.h>

hkFractureListener::hkFractureListener(hkRigidBody* compoundRigidBody,hkWorld* theWorld)
{
	// Make sure the rigid body has a list shape
	hkCollidable coll = compoundRigidBody->getCollidable();

	HK_ASSERT(coll.m_shape->getPrimaryType() == HK_SHAPE_LIST);

	compoundRigidBody->addCollisionListener(this);
	m_world = theWorld; 
}

////////////////////////////////////////////////////////////////////////

hkFractureListener::~hkFractureListener()
{
	// Anything to do here???
}

////////////////////////////////////////////////////////////////////////

void hkFractureListener::contactPointAddedCallback(	hkContactPointAddedEvent& event)
{
  // Okay so this function has the glorious task of deciding what pieces break off
  // the initial body and become new bodies. The original rigid body is removed from 
  // the world and a new one with a proper intial state must be added. Each new body
  // spawned by this process must be added to this listener instance. The bodies are
  // then added to the world.

  // For a first pass, obliterate the object into all of its shapes

     hkRigidBody* bodyFragment;
	 hkListShape* listShape;
     int i,numShapes;
   	 hkRigidBody* rb;
     hkRigidBodyConstructionInfo rbInfo;
	 hkVector4 linVelChild;

	 // hkVector4 angVelChild; // One day figure out proper momenta
	 hkVector4 linVelParent;
	 hkVector4 angVelParent;
	 hkVector4 parentCOMWorld;
	 hkVector4 childCOMLocal;
	 hkVector4 parentCOMLocal;
	 hkVector4 deltaPositionLocal;
	 hkVector4 r,rCrossW; // radial arm
     hkArray<hkMassProperties>* propArray;
     hkPropertyValue propValue;

	 rb = (hkRigidBody*)(event.m_callbackFiredFrom);


  //   propValue = rb->getProperty(MASS_PROPERTY_ARRAY_TYPE); 
     propArray = (hkArray<hkMassProperties>*)propValue.asPtr; 

     // The shape is the convex vertices shape that was involved in the
	 // collision..nice for later. Need potentially all shapes in the rigid
	 // body though. Go through rigid body to get this!

	 const hkShape* theShape = rb->getCollidable().getShape();

	 // Perhaps useful later for more complex fracture process
	 // const hkShape* collidedShape = event.m_collidable?.getShape();

	 linVelParent = rb->getLinearVelocity();
     angVelParent = rb->getAngularVelocity();
     parentCOMWorld = rb->getCenterOfMassInWorld();
     parentCOMLocal = rb->getCenterOfMassLocal();

     if(theShape->getPrimaryType() == HK_SHAPE_LIST)
	 {
        listShape = (hkListShape*)theShape;

        numShapes = listShape->getNumChildShapes();

   	    // Add collision filter so that nothing collides with the
        // parent body. In demo file collisions are disabled between
		// groups 0 and 1
        rb->getCollidable().setCollisionFilterInfo(1);
        ///////////////////////////////////////////////////////// 

		// For the simple case of complete obliteration
		// each shape becomes a new rigid body
		HK_TIMER_BEGIN( "Fracture: Adding bodies", HK_NULL );
		//hkStopwatch fooWatch;

		//fooWatch.start();

		// hkSphereShape* theSphere = new hkSphereShape(1);
        for(i = 0; i < numShapes; i++)
		{
           // Need to set mass properties which should
		   // have been stored for each shape

		   rbInfo.m_inertiaTensor = (*propArray)[i].m_inertiaTensor;
          	    
		   rbInfo.m_centerOfMass =  (*propArray)[i].m_centerOfMass; 

		   rbInfo.m_mass = (*propArray)[i].m_mass;
         
           rbInfo.m_shape = listShape->getChildShape(i);

		   rbInfo.m_position = rb->getPosition();   
               
		   rbInfo.m_rotation = rb->getRotation();

	 	   // For simplicity (first pass ) and not correctness set the linear
		   // velocity of each body to v_parent + w_parent x r

		   childCOMLocal =  rbInfo.m_centerOfMass;

           deltaPositionLocal.setSub4(childCOMLocal,parentCOMLocal);
			   
		   r.setRotatedDir(rb->getRotation(),deltaPositionLocal);
    
           r.add4(parentCOMWorld); // r in world space

		   //rbInfo.m_position = r;

		   rCrossW.setCross(angVelParent,r);
			    
		   linVelChild.setAdd4(linVelParent,rCrossW);

           bodyFragment = new hkRigidBody(rbInfo);
			   
	 	   bodyFragment->setLinearVelocity(linVelChild); 
			   
		   //bodyFragment->addProperty(SPECIFIED_COLOR_PROPERTY,propValue);

		   m_world->addEntity(bodyFragment);

		   // Would also like to keep color consistent!!!! How???
			   
		   bodyFragment->removeReference();
		}
        HK_TIMER_END();

		//fooWatch.stop();
	    //hkReal time = fooWatch.getElapsedSeconds();

        // Now that all new rigid bodies have been created
   		// remove the old rigid body from the world

		rb->removeCollisionListener(this);
		m_world->removeEntity(rb);


	    // In a more complex incarnation chunks which themselves
	    // would be able to fracture would be added and therefore
	    // would be added to this instance of the listener.
	    // In the trivial case of obliteration, it does not
	    // make sense to do this since the bodies cannot break
	    // apart any further. To avoid recursive obliteration
		// however the addition of the listener will need to
		// be delayed until the bodies are out of contact..somehow!!!
	 }
	else
	{
          // Nothing to do, the object cannot be broken further if we are
          // dealing with a prefracturing paradigm
	}
	 
}

////////////////////////////////////////////////////////////////////////

// Called when an entity is deleted
void hkFractureListener::entityDeletedCallback(hkEntity* entity)
{
	entity->removeCollisionListener(this); 
}

////////////////////////////////////////////////////////////////////////

// Called when an entity is removed from the hkWorld
void hkFractureListener::entityRemovedCallback(hkEntity* entity)
{
	entity->removeCollisionListener(this); 
}

////////////////////////////////////////////////////////////////////////


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
