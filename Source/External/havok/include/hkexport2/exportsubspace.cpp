/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkexport2/hkexport.h>

#include <hkexport2/exportsubspace.h>
#include <hkexport2/exportrigidbodycollection.h>
#include <hkexport2/exportworld.h>
#include <hkexport2/exportfastconstraintsolver.h>
#include <hkexport2/exportcollisiongroups.h>

using namespace NS_ExportStream;

int	ExportSubspace::priv_CLASS_SerialNumber=1;

ExportSubspace::ExportSubspace(ExportWorld *world) : ExportClass()
{
	PROT_AutoName();
	priv_world=world;
	priv_CLASS_SerialNumber++;
}

ExportSubspace::~ExportSubspace()
{ 
	ExportRigidBodyCollection_VectorIterator ec_it;
	for (ec_it=priv_rigid_body_collections_vector.begin(); ec_it<priv_rigid_body_collections_vector.end(); ec_it++)
		delete (*ec_it);

	ExportAction_VectorIterator ac_it;
	for (ac_it=priv_actions_vector.begin(); ac_it<priv_actions_vector.end(); ac_it++)
		delete (*ac_it);
}

void ExportSubspace::Import(InExportStream &xtream, CheckLog &log)
{
	//we won't begin a level for the log as we want the subspace as transparent as possible
	hkVector4 vgravity;
	bool has_deactivator;
	float deactivator_short_frequency, deactivator_long_frequency;

	xtream>>prot_buffer; 

	xtream.TokenMustBe("GRAVITY"); xtream>>priv_gravity;
	xtream.TokenMustBe("TOLERANCE"); xtream>>priv_tolerance;	
	GetWorld()->GetHKWorld()->setGravity(priv_gravity);

	{
		float grav=priv_gravity.length3();
		if (grav<4.0f) log.AddError(LOC("Gravity strength seems too low - at least for real world"));
		if (grav>20.0f) log.AddError(LOC("Gravity strength seems too high -at least for real world"));
	}

	if (xtream.GetVersion() >=1340)
	{
		xtream.TokenMustBe("HAS_DEACTIVATOR"); xtream>>has_deactivator;
		if (has_deactivator)
		{
			xtream.TokenMustBe("DEACTIVATOR_SHORT_FREQUENCY"); xtream>>deactivator_short_frequency;
			xtream.TokenMustBe("DEACTIVATOR_LONG_FREQUENCY"); xtream>>deactivator_long_frequency;
		}
	}

	//these parameters don't map across to havok 2
//	GetWorld()->GetHKWorld()->setHighFrequencyDeactivationPeriod( deactivator_long_frequency );
//	GetWorld()->GetHKWorld()->setLowFrequencyDeactivationPeriod( deactivator_short_frequency );

	xtream.TokenMustBe("RESOLVER"); xtream>>prot_token; //this is not used in havok 2
	
	while (xtream>>prot_token, prot_token!="END_SUBSPACE")
	{
		bool parsed=false;
		if (prot_token=="BEGIN_COLLECTION")
		{
			xtream>>prot_token;

			ExportRigidBodyCollection *rigid_body_collection = 
				static_cast<ExportRigidBodyCollection*> ( ExportRigidBodyCollectionFactory::getInstance().CreateExportEntityCollection(prot_token,this) );
			if (!rigid_body_collection)
			{
				char buffer[200];
				hkString::sprintf(buffer, LOC("Entity Collection type: %s, unrecognised and ignored on import."), prot_token.GetString());
				this->PROT_WarningAndRecover(buffer,xtream,"END_COLLECTION", log);
			}
			else
			{
				rigid_body_collection->Import(xtream, log);
				AddRigidBodyCollection( rigid_body_collection );
			}
			parsed=true;
		}
		if (prot_token=="NO_COLLISION")
		{
			xtream>>prot_buffer;
			xtream>>prot_buffer;
			log.AddError(LOC("External pairwise collision disabling ignored on import."));
			// do nothing with these bodies that do not collide with one another
			parsed=true;
		}
		if (prot_token=="BEGIN_DISABLED_COLLISIONS_LIST")
		{
			PROT_WarningAndRecover( LOC("Havok 1 disabled collision list ignored on import"), xtream, "END_DISABLED_COLLISIONS_LIST", log, false );
			parsed=true;
		}

		if (prot_token==("BEGIN_ENABLED_COLLISION_GROUPS"))
		{
			ExportCollisionGroups importGroups(this);
			importGroups.Import( xtream, log );
			parsed=true;
		}

		if (prot_token == ("BEGIN_ACTION"))
		{
			xtream>>prot_token;
			ExportAction *export_action = ExportActionRegistry::getInstance().CreateExportAction(prot_token, this, log);
			if (!export_action)
			{
				char buffer[200];
				hkString::sprintf( buffer, LOC("ACTION not imported."), prot_token.GetString());
				this->PROT_WarningAndRecover( buffer, xtream, "END_ACTION", log);
			}
			else
			{
				export_action->Import(xtream, log);
				AddAction(export_action);
				if (export_action->GetHKAction())
				{
					GetWorld()->GetHKWorld()->addAction(export_action->GetHKAction());
				}
			}
			parsed=true;
		}

		if (prot_token==("BEGIN_DEFLECTOR"))
		{
			this->PROT_WarningAndRecover(const_cast<char*>(LOC("Fast Deformable import not supported and ignored.")),xtream,"END_DEFLECTOR", log );
			parsed=true;
		};

		if (!parsed) throw_exception("Unexpected token importing subspace");
	}
}

ExportRigidBodyCollection* ExportSubspace::GetRigidBodyCollection(char *name) const
{
	ExportRigidBodyCollection_VectorIterator ec_it;
	for (ec_it=priv_rigid_body_collections_vector.begin(); ec_it < priv_rigid_body_collections_vector.end(); ec_it++)
		if(!hkString::strCmp((*ec_it)->GetName(),name)) return (*ec_it);

	return HK_NULL;
}


ExportAction *ExportSubspace::GetAction(char *name) const
{
	ExportAction_VectorIterator ac_it;
	for (ac_it=priv_actions_vector.begin(); ac_it<priv_actions_vector.end(); ac_it++)
		if(!hkString::strCmp((*ac_it)->GetName(),name)) return (*ac_it);

	return HK_NULL;
}

int	ExportSubspace::GetNumRigidBodies() const
{
	int result = 0;

	ExportRigidBodyCollection_VectorIterator ec_it;
	for (ec_it=priv_rigid_body_collections_vector.begin(); ec_it<priv_rigid_body_collections_vector.end(); ec_it++)
	{
		result += (*ec_it)->GetNumRigidBodies();
	}

	return result;
}

ExportRigidBody *ExportSubspace::GetRigidBody(char *name) const
{
	ExportRigidBody *result=HK_NULL;

	ExportRigidBodyCollection_VectorIterator ec_it;
	for (ec_it=priv_rigid_body_collections_vector.begin(); ec_it<priv_rigid_body_collections_vector.end(); ec_it++)
	{
		result=(*ec_it)->GetRigidBody(name);
		if (result) break;
	}
	return result;
}

ExportRigidBody *ExportSubspace::GetRigidBody(hkRigidBody *hk_rigid_body) const
{
	ExportRigidBody *result=HK_NULL;

	ExportRigidBodyCollection_VectorIterator ec_it;
	for (ec_it=priv_rigid_body_collections_vector.begin(); ec_it<priv_rigid_body_collections_vector.end(); ec_it++)
	{
		result=(*ec_it)->GetRigidBody(hk_rigid_body);
		if (result) break;
	}
	return result;
}

ExportRigidBody *ExportSubspace::GetRigidBody( int index ) const
{
	int current_index = index;
	ExportRigidBody *result=HK_NULL;

	ExportRigidBodyCollection_VectorIterator ec_it;
	for (ec_it=priv_rigid_body_collections_vector.begin(); ec_it<priv_rigid_body_collections_vector.end(); ec_it++)
	{
		if ( current_index < 0 )
		{
			break;
		}
		int numBodiesInCollection = (*ec_it)->GetNumRigidBodies();
		if ( current_index < numBodiesInCollection )
		{
			result = (*ec_it)->GetRigidBody( current_index );
			break;
		}
		current_index -= numBodiesInCollection;
	}
	return result;
}

int ExportSubspace::GetNumConstraints() const
{
	ExportAction_VectorIterator ac_it;
	int result = 0;
	ExportFastConstraintSolver* solver = HK_NULL;
	for (ac_it=priv_actions_vector.begin(); ac_it<priv_actions_vector.end(); ac_it++)
	{
		if (!hkString::strCmp((*ac_it)->ClassName(),"FastConstraintSolver"))
		{
			solver = static_cast<ExportFastConstraintSolver*>(*ac_it);
			result += solver->GetNumConstraints();
		}
	}
	return result;
}

ExportConstraint *ExportSubspace::GetConstraint(char *name) const
{
	ExportAction_VectorIterator ac_it;
	ExportConstraint* result = HK_NULL;
	ExportFastConstraintSolver* solver = HK_NULL;
	for (ac_it=priv_actions_vector.begin(); ac_it<priv_actions_vector.end(); ac_it++)
	{
		if (!hkString::strCmp((*ac_it)->ClassName(),"FastConstraintSolver"))
		{
			solver = static_cast<ExportFastConstraintSolver*>(*ac_it);
			result = solver->GetConstraint(name);
			if (result)
			{
				break;
			}
		}
	}
	return result;
}

ExportConstraint *ExportSubspace::GetConstraint( int index ) const
{
	int current_index = index;
	ExportAction_VectorIterator ac_it;
	ExportConstraint* result = HK_NULL;
	ExportFastConstraintSolver* solver = HK_NULL;
	for (ac_it=priv_actions_vector.begin(); ac_it<priv_actions_vector.end(); ac_it++)
	{
		if ( current_index < 0 )
		{
			break;
		}
		if (!hkString::strCmp((*ac_it)->ClassName(),"FastConstraintSolver"))
		{
			solver = static_cast<ExportFastConstraintSolver*>(*ac_it);
			int numConstraintsInSolver = solver->GetNumConstraints();
			if ( current_index < numConstraintsInSolver )
			{
				result = solver->GetConstraint( current_index );
				break;
			}
			current_index -= numConstraintsInSolver;
		}
	}
	return result;
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
