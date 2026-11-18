/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkexport2/hkexport.h>

#include <hkexport2/exportrigidbodycollection.h>
#include <hkexport2/exportsubspace.h>
#include <hkexport2/exportworld.h>
//#include <hkdynamics2/entity/primitive.h>

using namespace NS_ExportStream;

// FACTORY ----------------------------------------------------------------------------------------------

EXPORT_SINGLETON_IMPLEMENTATION(ExportRigidBodyCollectionFactory);

// OVERRIDE 
ExportEntityCollection* ExportRigidBodyCollectionFactory::CreateExportEntityCollection(Token &token, ExportSubspace *subspace)
{
	if (token=="RBCollection") return new ExportRigidBodyCollection(subspace);
	return HK_NULL;
}
// ------------------------------------------------------------------------------------------------------

int ExportRigidBodyCollection::priv_CLASS_SerialNumber=1;

// Constructor
ExportRigidBodyCollection::ExportRigidBodyCollection(ExportSubspace *subspace): ExportEntityCollection (subspace)
{
	PROT_AutoName();
	priv_CLASS_SerialNumber++;
}

// Destructor
ExportRigidBodyCollection::~ExportRigidBodyCollection()
{
	ExportRigidBody_VectorIterator rb_it;
	for (rb_it=priv_rigid_bodies_vector.begin();rb_it<priv_rigid_bodies_vector.end(); rb_it++)
	{
	   delete (*rb_it);
	}
}

// Import
void ExportRigidBodyCollection::Import (InExportStream &xtream, CheckLog &log)
{
	float a_float;
	hkPairwiseCollisionFilter* pairwise_collision_filter = GetSubspace()->GetWorld()->GetHKPairwiseCollisionFilter();

	// name
	xtream>>prot_buffer;
	SetName(prot_buffer);
	log.BeginLevel(LOC("RIGIDBODY COLLECTION"),prot_buffer);

	xtream.TokenMustBe("ODE_SOLVER");
	xtream>>prot_token; //no longer valid

	xtream.TokenMustBe("REFRESH_RATE");
	xtream>>a_float; //no longer valid

	// RigidBodies
	while (xtream>>prot_token, prot_token=="BEGIN_RIGID_BODY")
	{
		ExportRigidBody *rbody=new ExportRigidBody(this);
		rbody->Import(xtream, log);
		if (rbody->GetHKRigidBody())
		{
			AddRigidBody(rbody);
		}
		else
		{
			delete rbody;
		}
	}

	// DISABLED COLLISIONS
	while (prot_token=="NO_COLLISION")
	{
		xtream>>prot_buffer;
		ExportRigidBody *rbodyA=GetRigidBody(prot_buffer);
		char buffer[200];
		bool is_valid_pair = true;
		if (!rbodyA) 
		{
			hkString::sprintf(buffer, LOC("Unknown rigidbody: %s in disabled collision pair."), prot_buffer);
			log.AddError(buffer);
			is_valid_pair = false;
		}
		xtream>>prot_buffer;
		ExportRigidBody *rbodyB=GetRigidBody(prot_buffer);
		if (!rbodyB)
		{
			hkString::sprintf(buffer, LOC("Unknown rigidbody: %s in disabled collision pair."), prot_buffer);
			log.AddError(buffer);
			is_valid_pair = false;
		}
		xtream>>prot_token;
		if (is_valid_pair)
		{
			pairwise_collision_filter->disableCollisionPair(rbodyA->GetHKRigidBody(), rbodyB->GetHKRigidBody());
		}
	 }

	//we add the bodies here as if we add them before they're added to the pairwise collision disabler they can cause
	//collisions and so if, as in the case of a constraint they're held in contact they'll always be resolved which isn't nice
	for (int i=0; i < priv_rigid_bodies_vector.getSize(); i++)
	{
		hkRigidBody* newly_created_rigidbody = HK_NULL;
		newly_created_rigidbody = priv_rigid_bodies_vector[i]->GetHKRigidBody();
		if (newly_created_rigidbody)
		{
			GetSubspace()->GetWorld()->GetHKWorld()->addEntity(newly_created_rigidbody);
			if ( priv_rigid_bodies_vector[i]->GetShouldDeactivateOnImport() )
			{
				newly_created_rigidbody->forceDeactivate();
			}
		}
	}

	if (prot_token!="END_COLLECTION") throw_exception("Unexpected token in RIGIDBODY COLLECTION");
	log.EndLevel();
}

// Management of the rigid body list
ExportRigidBody *ExportRigidBodyCollection::GetRigidBody(char *name) const
{
	ExportRigidBody_VectorIterator rb_it;
	for (rb_it=priv_rigid_bodies_vector.begin(); rb_it<priv_rigid_bodies_vector.end(); rb_it++)
	{
		// all lower case compare too. 
		if (hkString::strCmp((*rb_it)->GetName(),name) == 0) return (*rb_it);

		char * lower = hkString::strLwr(hkString::strDup(name));

		if (hkString::strCmp((*rb_it)->GetName(),lower) == 0) 
		{	
			hkDeallocate(lower);
			return (*rb_it);
		}
		else
		{
			hkDeallocate(lower);
		}
	}
	return HK_NULL;
}

ExportRigidBody *ExportRigidBodyCollection::GetRigidBody(hkRigidBody *hk_rigid_body) const
{
	ExportRigidBody_VectorIterator rb_it;
	for (rb_it=priv_rigid_bodies_vector.begin(); rb_it<priv_rigid_bodies_vector.end(); rb_it++)
	{
		if (hk_rigid_body==(*rb_it)->GetHKRigidBody()) 
		{
			return (*rb_it);
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
