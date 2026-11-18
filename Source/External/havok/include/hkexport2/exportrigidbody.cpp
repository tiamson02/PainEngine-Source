/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


#include <hkexport2/hkexport.h>

#include <hkdynamics2/entity/hkRigidBody.h>
#include <hkdynamics2/entity/hkRigidBodyConstructionInfo.h>

#include <hkexport2/exportrigidbody.h>
#include <hkexport2/exportrigidbodycollection.h>
#include <hkexport2/exportsubspace.h>
#include <hkexport2/exportworld.h>
#include <hkcollide2/shape/hkConvexVerticesShape.h>
#include <hkcollide2/shape/hkListShape.h>
#include <hkcollide2/shape/hkTransformShape.h>
#include <hkutilities2/inertia/hkInertiaTensorComputer.h>

using namespace NS_ExportStream;

int ExportRigidBody::priv_CLASS_SerialNumber=1;

ExportRigidBody::ExportRigidBody(ExportRigidBodyCollection *rigid_body_collection) : ExportClass()
{
	PROT_AutoName();

	priv_rigid_body_collection=rigid_body_collection;
	priv_CLASS_SerialNumber++;

	priv_hk_rigid_body = HK_NULL;
	priv_collisions_disabled=false;
	priv_should_deactivate_on_import = false;
	priv_hk2Body_from_hk1Body = hkTransform::getIdentity();
	priv_display_from_local = HK_NULL;
}

ExportRigidBody::~ExportRigidBody()
{
	ExportPrimitive_VectorIterator prim_it;
	for (prim_it=priv_primitives_vector.begin(); prim_it<priv_primitives_vector.end(); prim_it++)
	{
		delete (*prim_it);
	}
	if (priv_hk_rigid_body)
	{
		priv_hk_rigid_body->removeReference();
		priv_hk_rigid_body = HK_NULL;
	}
	if ( priv_display_from_local )
	{
		delete priv_display_from_local;
	}
}

// Import
void ExportRigidBody::Import (InExportStream &xtream, CheckLog &log)
{
	float ellasticity, friction, a_float;
	hkVector4 translation,linear_vel,angular_vel,displacement;
	hkQuaternion rotation;
	bool active;
	bool doesThisFunctionOwnTheShape = false;
	hkReal total_mass = 0.0f;
	xtream>>prot_buffer; SetName(prot_buffer);
	log.BeginLevel(LOC("RIGIDBODY"), GetName());

	xtream.TokenMustBe("ELLASTICITY"); xtream>>ellasticity; 
	xtream.TokenMustBe("STATIC_FRICTION"); xtream>>friction;
	xtream.TokenMustBe("DYNAMIC_FRICTION"); xtream>>a_float; // ignored
	xtream.TokenMustBe("ROTATION"); xtream>>rotation;
	xtream.TokenMustBe("TRANSLATION"); xtream>>translation;
	xtream.TokenMustBe("LINEAR_VELOCITY"); xtream>>linear_vel;
	xtream.TokenMustBe("ANGULAR_VELOCITY"); xtream>>angular_vel;
	xtream.TokenMustBe("COLLISIONS_DISABLED"); xtream>>priv_collisions_disabled;
	xtream.TokenMustBe("ACTIVE"); xtream>>active;
	//the displacement in havok 1 was the offset from the center of mass and the display body, the first thing we do now is align our
	//rigid bodies with their display representations by setting their position to be their old position - displacement
	//this change is then stored in the havok2Fromhavok1Transform which ensures that points and directions stored in the rigid bodies
	//original space can be mapped to this new one
	xtream.TokenMustBe("DISPLACEMENT"); xtream>>displacement;
	hkQuaternion identRot;
	identRot.setIdentity();
	SetHavok2FromHavok1Transform( hkTransform( identRot, displacement ) );
	hkTransform displayFromHK1Local( GetHavok2FromHavok1BodyTransform() );
	// Display Info if present
	xtream>>prot_token;

	if  (prot_token=="DISPLAY_BODY")
	{
		xtream>>prot_buffer;
		bool cast_shadows=false;
		if (xtream.GetVersion()>=160)
		{
			xtream.TokenMustBe("CASTS_SHADOWS"); xtream>>cast_shadows;
		}
		xtream>>prot_token;
	}

	hkRigidBodyConstructionInfo rb_construct_info;
	rb_construct_info.m_restitution = ellasticity;
	rb_construct_info.m_friction = friction;
	//now the rigid body is moved so that it lines up with its display body
	//displacement is specified in local so we need to map it to world
	hkVector4 worldDisplacement;
	worldDisplacement.setRotatedDir( rotation, displacement );
	rb_construct_info.m_position.setSub4( translation, worldDisplacement );
	rb_construct_info.m_rotation = rotation;
	rb_construct_info.m_restitution = ellasticity;
	rb_construct_info.m_mass = 0;
	//initialised to 0, or'd with all of its primitive groups to create the rigidbody mask
	rb_construct_info.m_collisionFilterInfo = 0;
	
	//this is used to store the shapes for the object
	hkArray<hkShape*> shapeArray;
	hkArray<hkTransform> transformArray;
	hkArray<hkMassElement> massArray;
	
	hkVector4 centerOfMassCalcVect = hkVector4(0,0,0);	
	while (prot_token=="BEGIN_PRIMITIVE")
	{
		xtream>>prot_token;
		ExportPrimitive *primitive = ExportPrimitiveRegistry::getInstance().CreateExportPrimitive(prot_token, this, log);
		if (!primitive)
		{
			char buffer[200];
			hkString::sprintf(buffer, LOC("Primitive not imported."),prot_token.GetString());
			log.AddError(buffer);
			PROT_WarningAndRecover("Unknown primitive type in rigid body",xtream,"END_PRIMITIVE", log);
		}
		else
		{			
			primitive->Import(xtream, log);
			AddPrimitive(primitive);
	
			if (primitive->GetHKShape())
			{
				shapeArray.pushBack(primitive->GetHKShape());
				hkTransform havok2FromPrimitive;
				//we moved each of the rigidbodies to a new space so now the offset transforms for each of the primitives should be changed by this too
				// havok2_from_prim = havok2_from_havok1 * havok1_from_primitive
				havok2FromPrimitive.setMul( GetHavok2FromHavok1BodyTransform(), primitive->GetTransform() );
				transformArray.pushBack( havok2FromPrimitive );
				if (primitive->GetMass() > 0)
				{
					hkMassElement new_mass_info;
					hkInertiaTensorComputer::computeShapeVolumeMassProperties( primitive->GetHKShape(), primitive->GetMass(), new_mass_info.m_properties );
					new_mass_info.m_transform = havok2FromPrimitive;
					massArray.pushBack( new_mass_info );
					total_mass += primitive->GetMass();
				}
			}
			rb_construct_info.m_collisionFilterInfo |= primitive->GetCollisionMask();
		}
		xtream>>prot_token; 
	}

	if ( rb_construct_info.m_collisionFilterInfo & 0x80000000 )
	{
		log.AddError( LOC("You are using collision group 32 for one or more of your rigid bodies. This has been reserved for havok 2 import of disable all collisions."));
		log.AddError( LOC("This could cause some unexpected behavior.") );
	}
	if (priv_collisions_disabled)
	{
		//we have reserved group 32 in the custom group collision filter for speedy disabling of all collisions
		log.AddError( LOC("In hkexport2, the disable all collions option is handled using bit 32 of the group collision filter.") );
		rb_construct_info.m_collisionFilterInfo |= 0x80000000;
	}
	
	//if no primitives were imported then we want to get out now and skip to the end of the Rigid Body
	if (shapeArray.getSize()==0)
	{
		char buffer[200];
		hkString::sprintf(buffer,LOC("No primitives found, this body will be ignored."));
		if (prot_token=="END_RIGID_BODY")
		{
			log.AddError(buffer);
		}
		else
		{
			PROT_WarningAndRecover(buffer, xtream, "END_RIGID_BODY", log);
		}
		log.EndLevel();
		return;
	}

	//Check if it's fixed
	if (total_mass == 0)
	{
		rb_construct_info.m_motionType = HK_FIXED_MOTION;
	}
	else
	{
		rb_construct_info.m_motionType = HK_STABILIZED_BOX_INERTIA_MOTION;
	}

	hkArray<hkTransform>::iterator transform_it;
	if (shapeArray.getSize() > 1) //create transform shapes around the current shapes if required
	{
		hkArray<hkShape*> transform_shape_array;
		hkArray<hkShape*>::iterator shape_it;
		hkArray<hkShape*> newlyCreated;
		hkArray<hkShape*>::iterator newlyIt;

		for ( shape_it = shapeArray.begin(), transform_it = transformArray.begin(); shape_it != shapeArray.end(); shape_it++, transform_it++)
		{
			if ( !( transform_it->isApproximatelyEqual( hkTransform::getIdentity() ) ) )
			{
				hkTransformShape* new_trans_shape = new hkTransformShape( *shape_it );
				new_trans_shape->setTransform( *transform_it );
				transform_shape_array.pushBack( new_trans_shape );
				//need to keep track of new shapes created in the function
				newlyCreated.pushBack( new_trans_shape );
			}
			else
			{
				transform_shape_array.pushBack( *shape_it );
			}	
		}
		//we don't clear and deallocate the shape array here as we do use it later on after the rbody has been created to
		//see if we should remove a reference from the new shape
		hkListShape* listShape = new hkListShape(&transform_shape_array[0], transform_shape_array.getSize());
		
		for ( newlyIt = newlyCreated.begin(); newlyIt != newlyCreated.end(); newlyIt++)
		{
				(*newlyIt)->removeReference();
		}
		transform_shape_array.clearAndDeallocate();
		newlyCreated.clearAndDeallocate();
		rb_construct_info.m_shape = listShape;
		doesThisFunctionOwnTheShape = true;
	}
	else 
	{	//if asked to we try to collapse the transform from any transform shapes into the rigidbody's transform and then update hk2Fromhk1Transform
		rb_construct_info.m_shape = shapeArray[0];
		hkTransform body_from_primitive( transformArray[0] );
		if (!(body_from_primitive.isApproximatelyEqual( hkTransform::getIdentity() )))
		{
			if ( GetRigidBodyCollection()->GetSubspace()->GetWorld()->GetCollapseTransforms() )
			{			
				//Note: we've already changed all primitives and bodies into their havok 2 representations
				hkTransform world_from_body_tm( rb_construct_info.m_rotation, rb_construct_info.m_position );
				hkTransform prim_from_body;
				prim_from_body.setInverse( body_from_primitive );
				//Above Note means that prim_from_body is actually prim_from_bodyInHavok2
				//the reason for the havok2_from_1_tm is so that we can map points etc stored in old body space
				//into new body space so if we collapse our body tm to primitive space then we should set havok2from1 to be prim_from_havok1
				prim_from_body.setMulEq( GetHavok2FromHavok1BodyTransform() );
				//we also have the mass information stored in the body's space after shifting from display so this should be updated 
				//or center of mass calculations will be wrong, this will become the identity as it's essentially the body to primitive
				if ( massArray.getSize() > 0 )
				{
					massArray[0].m_transform.setIdentity();
				}		
				SetHavok2FromHavok1Transform( prim_from_body );
					
				hkTransform world_from_primitive_tm;
				world_from_primitive_tm.setMul( world_from_body_tm, body_from_primitive );
				rb_construct_info.m_rotation.set( world_from_primitive_tm.getRotation() );
				rb_construct_info.m_position = world_from_primitive_tm.getTranslation();
			}
			else
			{
				hkTransformShape* new_tm_shape = new hkTransformShape( shapeArray[0] );
				new_tm_shape->setTransform( body_from_primitive );
				rb_construct_info.m_shape = new_tm_shape;
				doesThisFunctionOwnTheShape = true;
			}
		}
	}

	//setShapeVolumeMassProperties doesn't null initialise the inertial tensor so we do, otherwise the tests don't make sense
	if (total_mass != 0)
	{
		rb_construct_info.m_inertiaTensor.setIdentity();
		hkMassProperties mass_result;
		hkInertiaTensorComputer::combineMassProperties(massArray, mass_result);
		rb_construct_info.m_inertiaTensor = mass_result.m_inertiaTensor;
		rb_construct_info.m_mass = mass_result.m_mass;
		rb_construct_info.m_centerOfMass = mass_result.m_centerOfMass;
	}
	massArray.clearAndDeallocate();
	
	if ( (rb_construct_info.m_motionType != HK_FIXED_MOTION) && ((!rb_construct_info.m_inertiaTensor.isOk()) || (rb_construct_info.m_inertiaTensor(0,0)<=0)))
	{
		char buffer[200];
		hkString::sprintf(buffer,"Invalid inertial tensor for rigid body, this body will be ignored.");
		if (prot_token=="END_RIGID_BODY")
		{
			
			log.AddError(buffer);
		}
		else
		{
			PROT_WarningAndRecover(buffer, xtream, "END_RIGID_BODY", log);
		}
		log.EndLevel();
		return;
	}

	priv_hk_rigid_body = new hkRigidBody(rb_construct_info);
	//if we have collasped transforms then it's possible that Havok2FromHavok1 will not be the same as DisplayFromHK1Local
	//in which case the rigid body won't be aligned with its original display body and so we need to provide a new DisplayFromLocal transform
	//we need to see if DisplayFromLocalHavok2 != identity and if so then create a new displayFromLocalTransform
	hkTransform displayFromHavok2;
	displayFromHavok2.setMulMulInverse( displayFromHK1Local, GetHavok2FromHavok1BodyTransform() );
	if ( !displayFromHavok2.isApproximatelyEqual( hkTransform::getIdentity() ) )
	{
		priv_display_from_local = new hkTransform( displayFromHavok2 );
	}

	//if we've created a list shape for a compound object or a transform shape then the ownership of that shape is delegated to the rigidbody 
	//however, the exportprimitive classes maintain a level of ownership for the component shapes used to create these	
	if ( doesThisFunctionOwnTheShape )
	{
		rb_construct_info.m_shape->removeReference();
	}
	shapeArray.clearAndDeallocate();

	if (prot_token!="END_RIGID_BODY") 
		throw_exception("Unexpected token importing rigidbody");


	if (priv_hk_rigid_body->getMass()!=0.0f)
	{
		priv_hk_rigid_body->setLinearVelocity(linear_vel);
		priv_hk_rigid_body->setAngularVelocity(angular_vel);
	}
	//we use this flag as objects can only be force deactivated after they've been added to the world
	SetShouldDeactivateOnImport(!active);
	//we won't add our bodies to the world here as they won't work with pairwise collision filters if they're already colliding
	log.EndLevel();
}

// Primitives management
ExportPrimitive *ExportRigidBody::GetPrimitive (char *name) const
{
	ExportPrimitive_VectorIterator prim_it;
	for (prim_it=priv_primitives_vector.begin(); prim_it<priv_primitives_vector.end(); prim_it++)
	{
		if (!hkString::strCmp((*prim_it)->GetName(),name)) 
		{
			return (*prim_it);
		}
	}
	return HK_NULL;
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
