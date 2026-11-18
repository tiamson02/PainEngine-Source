/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkexport2/hkexport.h>

#include <hkexport2/exportworld.h>
#include <hkbase/hkBase.h>
#include <hkbase/class/hkClass.h>
#include <hkbase/class/hkTypeInfo.h>
#include <hkexport2/exportrigidbodycollection.h>
#include <hkexport2/exportrigidbody.h>
#include <hkcollide/dispatch/hkCollisionDispatcher.h>

// registered classes

#include <hkserialize/util/hkVtableClassRegistry.h>
#include <hkcollide/shape/capsule/hkCapsuleShape.h>
#include <hkcollide/filter/null/hkNullCollisionFilter.h>
#include <hkcollide/filter/list/hkCollisionFilterList.h>

extern const hkClass hkCapsuleShapeClass;
extern const hkClass hkNullCollisionFilterClass;
extern const hkClass hkCollisionFilterListClass;
extern const hkClass hkReferencedObjectClass;
extern const hkClass hkReferencedObjectClass;
extern const hkClass hkCollisionFilterClass;
extern const hkClass hkShapeClass;




using namespace NS_ExportStream;

int ExportWorld::priv_CLASS_SerialNumber=1;

ExportWorld::ExportWorld( hkWorld* myWorld, hkConvertCS::ConversionType conversionType) 
: ExportClass(), m_converter(conversionType)
{
	PROT_AutoName();

	m_pairwise_collision_filter = HK_NULL;
	m_group_collision_filter    = HK_NULL;

	priv_hk_world = myWorld;
	priv_hk_world->addReference();
	priv_world_scale=1.0f;
	priv_CLASS_SerialNumber++;

	priv_collapse_transforms = false;

	extern const hkTypeInfo hkNullCollisionFilterTypeInfo;
	extern const hkTypeInfo hkCapsuleShapeTypeInfo;
	extern const hkTypeInfo hkCollisionFilterListTypeInfo;

	priv_registry = new hkVtableClassRegistry;
	priv_registry->registerVtable( hkNullCollisionFilterTypeInfo.getVtable(), &hkNullCollisionFilterClass );
	priv_registry->registerVtable( hkCapsuleShapeTypeInfo.getVtable(), &hkCapsuleShapeClass );
	priv_registry->registerVtable( hkCollisionFilterListTypeInfo.getVtable(), &hkCollisionFilterListClass );
}

ExportWorld::~ExportWorld() 
{
	ExportSubspace_VectorIterator sub_it;
	for (sub_it=priv_subspaces_vector.begin(); sub_it<priv_subspaces_vector.end(); sub_it++)
		delete (*sub_it);

	ExportGeometry_VectorIterator geom_it;
	for (geom_it=priv_geometries_vector.begin(); geom_it<priv_geometries_vector.end(); geom_it++)
		delete (*geom_it);

	ExportMarkerList_VectorIterator mark_it;
	for (mark_it = priv_marker_lists_vector.begin(); mark_it<priv_marker_lists_vector.end(); mark_it++)
		delete (*mark_it);
	
	priv_hk_world->removeReference();
	priv_registry->removeReference();
}

void ExportWorld::Import(InExportStream &xtream, CheckLog &log)
{
	
	log.BeginLevel(LOC("WORLD"),prot_name);
	int dumb_int;
	// It empties the world first (so we can reuse) ------------------------------------------------
	ExportSubspace_VectorIterator sub_it;
	for (sub_it=priv_subspaces_vector.begin(); sub_it<priv_subspaces_vector.end(); sub_it++)
		delete (*sub_it);
	priv_subspaces_vector.clear();

	ExportGeometry_VectorIterator geom_it;
	for (geom_it=priv_geometries_vector.begin(); geom_it<priv_geometries_vector.end(); geom_it++)
		delete (*geom_it);
	priv_geometries_vector.clear();

	ExportMarkerList_VectorIterator ml_it;
	for (ml_it=priv_marker_lists_vector.begin(); ml_it<priv_marker_lists_vector.end(); ml_it++)
		delete (*ml_it);
	priv_marker_lists_vector.clear();

	// BEGIN_WORLD world_name
	xtream.TokenMustBe("BEGIN_WORLD");
	xtream>>prot_buffer; SetName(prot_buffer);
	
	// VERSION version
	xtream.TokenMustBe("VERSION");
	xtream>>dumb_int;
	xtream.SetVersion(dumb_int);

	// OLD HKEs need the HAVOK1 COLLISION GROUP FILTER
	if (xtream.GetVersion()<2000)
	{
		m_group_collision_filter = new hkGroupCollisionFilter();
		addFilterToWorld(m_group_collision_filter);
		m_group_collision_filter->removeReference();
	}

	xtream.TokenMustBe("WORLD_SCALE"); xtream>>priv_world_scale;
	xtream>>prot_token;

	HK_ASSERT2(0x32bc5e03, priv_hk_world->getCollisionDispatcher()->m_collisionAgentRegistered,
				"Error: You have to register all your collision agents before calling ExportWorld::Import");
	//hkAgentRegisterUtil::registerAllAgents( priv_hk_world->getCollisionDispatcher() );

	if (prot_token=="BEGIN_DISPLAY_WORLD")
	{
		log.AddError(LOC("Havok 2 hke import does not support display worlds. Display world ignored."));
		const Token my_token = "END_DISPLAY_WORLD";
		xtream.JumpToToken(my_token);
		xtream>>prot_token;
		// error checking to ensure a random number isn't choosen instead of the valid token 
		while (!((prot_token=="BEGIN_MARKER_LIST")|| (prot_token=="END_WORLD")|| (prot_token=="FAST_SUBSPACE")|| (prot_token=="BEGIN_SUBSPACE")|| (prot_token=="BEGIN_GEOMETRY")))
		{
			xtream.JumpToToken(my_token);
			xtream>>prot_token;
		}
	}

	// LIST OF SUBSPACES/GEOMETRIES
	while (prot_token!="END_WORLD")
	{
		hkBool parsed=false;

		//no fast_subspace so we ignore
		if (prot_token=="FAST_SUBSPACE")
		{
			hkBool use_fast_subspace = false;
			xtream>>use_fast_subspace;
			parsed=true;
		}
		
		if (prot_token=="BEGIN_SUBSPACE")
		{
			if (priv_subspaces_vector.getSize()>0)
			{
				PROT_WarningAndRecover( LOC("Havok 2 does not support multiple subspaces, extra subspaces ignored."), xtream, "END_SUBSPACE", log, false );
			}
			else
			{
				ExportSubspace *subspace=new ExportSubspace(this);
				AddSubspace(subspace);
				subspace->Import(xtream, log);
			}
			parsed=true;
		}
		
		if (prot_token=="BEGIN_GEOMETRY")
		{
			xtream>>prot_token;
			ExportGeometry *geometry = ExportGeometryRegistry::getInstance().CreateExportGeometry(prot_token,this);
			if (!geometry) // Unknown geometry, try to recover
			{
				char buffer[200];
				hkString::sprintf(buffer, LOC("Geometry type: %s unknown or unsupported in Havok 2."), prot_token.GetString());
				this->PROT_WarningAndRecover( buffer, xtream, "END_GEOMETRY", log );
			}
			else
			{
				AddGeometry(geometry);
				geometry->Import(xtream, log);
			}
			parsed=true;
		}
		
		if (prot_token=="BEGIN_MARKER_LIST")
		{
			ExportMarkerList *marker_list=new ExportMarkerList(this);
			AddMarkerList(marker_list);
			marker_list->Import(xtream, log);
			parsed=true;
		}

		if (!parsed) 
		{
			throw_exception("Unexpected token importing world");
			break;
		}
		xtream>>prot_token;
	}

//XXNAMES	// We set the name for this object for further serialization
//XXNAMES	priv_hk_world->setName(GetSerializableName().cString());

	log.EndLevel();
}

// ALWAYS: FIRST GEOMETRIES, THEN SUBSPACES (since objects in subspaces may depend on geometries)

ExportGeometry *ExportWorld::GetGeometry(const char* name) const
{
	ExportGeometry_VectorIterator geom_it;
	for (geom_it=priv_geometries_vector.begin(); geom_it<priv_geometries_vector.end(); geom_it++)
		if (!hkString::strCmp((*geom_it)->GetName(),name)) return (*geom_it);

	return HK_NULL;
}

// Marker Search : search through the marker_lists /collections
ExportMarker *ExportWorld::GetMarker(const char* name) const
{
	ExportMarker *result=HK_NULL;

	ExportMarkerList_VectorIterator ml_it;
	for (ml_it=priv_marker_lists_vector.begin(); ml_it<priv_marker_lists_vector.end(); ml_it++)
	{
		result=(*ml_it)->GetMarker(name);
		if (result) break;
	}
	return result;
}

ExportMarkerList *ExportWorld::GetMarkerList(const char* name) const
{
	ExportMarkerList_VectorIterator ml_it;
	for (ml_it=priv_marker_lists_vector.begin(); ml_it<priv_marker_lists_vector.end(); ml_it++)
		if (!hkString::strCmp((*ml_it)->GetName(),name)) return (*ml_it);

	return HK_NULL;
}

ExportSubspace* ExportWorld::GetSubspace(int index) const
{
	if (index >= priv_subspaces_vector.getSize())
	{
		return HK_NULL;
	}
	return priv_subspaces_vector[index];
}

// RigidBody Search : search through the subspaces /collections
int ExportWorld::GetNumRigidBodies() const
{
	int result = 0;
	ExportSubspace_VectorIterator s_it;
	for (s_it=priv_subspaces_vector.begin(); s_it<priv_subspaces_vector.end(); s_it++)
	{
		result += (*s_it)->GetNumRigidBodies();
	}
	return result;
}

ExportRigidBody* ExportWorld::GetRigidBody(const char* name) const
{
	ExportRigidBody *result=HK_NULL;

	ExportSubspace_VectorIterator s_it;
	for (s_it=priv_subspaces_vector.begin(); s_it<priv_subspaces_vector.end(); s_it++)
	{
		result=(*s_it)->GetRigidBody(name);
		if (result) break;
	}
	return result;
}

ExportRigidBody* ExportWorld::GetRigidBody(hkRigidBody *hk_rigid_body) const
{
	ExportRigidBody *result=HK_NULL;

	ExportSubspace_VectorIterator s_it;
	for (s_it=priv_subspaces_vector.begin(); s_it<priv_subspaces_vector.end(); s_it++)
	{
		result=(*s_it)->GetRigidBody(hk_rigid_body);
		if (result) break;
	}
	return result;
}

ExportRigidBody* ExportWorld::GetRigidBody( int index ) const
{
	int current_index = index;
	ExportRigidBody *result=HK_NULL;

	ExportSubspace_VectorIterator s_it;
	for (s_it=priv_subspaces_vector.begin(); s_it<priv_subspaces_vector.end(); s_it++)
	{
		if ( current_index < 0 )
		{
			break;
		}
		int numBodiesInCollection = (*s_it)->GetNumRigidBodies();
		if ( current_index < numBodiesInCollection )
		{
			result = (*s_it)->GetRigidBody( current_index );
			break;
		}
		current_index -= numBodiesInCollection;
	}
	return result;
}

// RigidBodyCollection Search : search through the subspaces
int ExportWorld::GetNumRigidBodyCollections() const
{
	int result = 0;
	ExportSubspace_VectorIterator s_it;
	for (s_it=priv_subspaces_vector.begin(); s_it<priv_subspaces_vector.end(); s_it++)
	{
		result += (*s_it)->GetNumRigidBodyCollections();
	}
	return result;
}

ExportRigidBodyCollection *ExportWorld::GetRigidBodyCollection(const char* name) const
{
	ExportRigidBodyCollection *result=HK_NULL;

	ExportSubspace_VectorIterator s_it;
	for (s_it=priv_subspaces_vector.begin(); s_it<priv_subspaces_vector.end(); s_it++)
	{
		result=(*s_it)->GetRigidBodyCollection( name );
		if (result) break;
	}
	return result;
}

ExportRigidBodyCollection *ExportWorld::GetRigidBodyCollection(int index) const
{
	int current_index = index;
	ExportRigidBodyCollection *result = HK_NULL;

	ExportSubspace_VectorIterator s_it;
	for (s_it=priv_subspaces_vector.begin(); s_it<priv_subspaces_vector.end(); s_it++)
	{
		if ( current_index < 0 )
		{
			break;
		}
		int numCollectionsInSubspace = (*s_it)->GetNumRigidBodyCollections();
		if ( current_index < numCollectionsInSubspace )
		{
			result = (*s_it)->GetRigidBodyCollection( current_index );
			break;
		}
		current_index -= numCollectionsInSubspace;
	}
	return result;
}


// Action Search : search through the subspaces /actions
int ExportWorld::GetNumActions() const
{
	int result = 0;
	ExportSubspace_VectorIterator s_it;
	for (s_it=priv_subspaces_vector.begin(); s_it<priv_subspaces_vector.end(); s_it++)
	{
		result += (*s_it)->GetNumActions();
	}
	return result;
}

ExportAction* ExportWorld::GetAction(const char* name) const
{
	ExportAction* result=HK_NULL;

	ExportSubspace_VectorIterator s_it;
	for (s_it=priv_subspaces_vector.begin(); s_it<priv_subspaces_vector.end(); s_it++)
	{
		result=(*s_it)->GetAction(name);
		if (result) break;
	}
	return result;
}

ExportAction *ExportWorld::GetAction(int index) const
{
	int current_index = index;
	ExportAction* result = HK_NULL;

	ExportSubspace_VectorIterator s_it;
	for (s_it=priv_subspaces_vector.begin(); s_it<priv_subspaces_vector.end(); s_it++)
	{
		if ( current_index < 0 )
		{
			break;
		}
		int numActionsInSubspace = (*s_it)->GetNumActions();
		if ( current_index < numActionsInSubspace )
		{
			result = (*s_it)->GetAction( current_index );
			break;
		}
		current_index -= numActionsInSubspace;
	}
	return result;
}

// Constraint Search : search through the subspaces /actions
int ExportWorld::GetNumConstraints() const
{
	int result = 0;
	ExportSubspace_VectorIterator s_it;
	for (s_it=priv_subspaces_vector.begin(); s_it<priv_subspaces_vector.end(); s_it++)
	{
		result += (*s_it)->GetNumConstraints();
	}
	return result;
}

ExportConstraint *ExportWorld::GetConstraint(const char* name) const
{
	ExportConstraint *result=HK_NULL;

	ExportSubspace_VectorIterator s_it;
	for (s_it=priv_subspaces_vector.begin(); s_it<priv_subspaces_vector.end(); s_it++)
	{
		result=(*s_it)->GetConstraint(name);
		if (result) break;
	}
	return result;
}

ExportConstraint *ExportWorld::GetConstraint(int index) const
{
	int current_index = index;
	ExportConstraint* result = HK_NULL;

	ExportSubspace_VectorIterator s_it;
	for (s_it=priv_subspaces_vector.begin(); s_it<priv_subspaces_vector.end(); s_it++)
	{
		if ( current_index < 0 )
		{
			break;
		}
		int numConstraintsInSubspace = (*s_it)->GetNumConstraints();
		if ( current_index < numConstraintsInSubspace )
		{
			result = (*s_it)->GetConstraint( current_index );
			break;
		}
		current_index -= numConstraintsInSubspace;
	}
	return result;
}

hkVector4 ExportWorld::GetWorldGravity() const
{
	hkVector4 default_gravity; default_gravity.set(0.0f,0.0f,-9.8f);
	ExportSubspace* exp_space = GetSubspace(0);
	if (exp_space)
	{
		default_gravity = exp_space->GetGravity();
	}
	return default_gravity;
}

float ExportWorld::GetCollisionTolerance() const
{
	float default_tolerance = 0.1f;
	ExportSubspace* exp_space = GetSubspace(0);
	if (exp_space)
	{
		default_tolerance = exp_space->GetTolerance();
	}
	return default_tolerance;
}

// This method adds a filter to the world, using a list filter if necessary (there was another filter already)
void ExportWorld::addFilterToWorld (hkCollisionFilter* filter)
{
	hkCollisionFilter* currentFilter = priv_hk_world->m_collisionFilter;

	const hkClass* derivClass = priv_registry->getClassFromVirtualInstance(filter);
	// Null filter or -> we just overwrite it
	if ( derivClass && (hkString::strCmp( derivClass->getName(), "hkNullCollisionFilter" )==0) )
	{
		priv_hk_world->setCollisionFilter(filter, true);
	}
	else
	{
		// List filter -> we add ourselves
		if ( derivClass && (hkString::strCmp(derivClass->getName(),"hkCollisionFilterList")==0) )
		{
			hkCollisionFilterList* listFilter = static_cast<hkCollisionFilterList*> (currentFilter);
			listFilter->addCollisionFilter(filter);
		}
		else
		{
			// Otherwise, we need to keep the old filter, and join it with this one in a list filter
			hkCollisionFilterList* listFilter = new hkCollisionFilterList();
			listFilter->addCollisionFilter(currentFilter);
			listFilter->addCollisionFilter(filter);
			priv_hk_world->setCollisionFilter(listFilter, true);
			listFilter->removeReference();
		}
	}
}


// Disables a collision pair - adds a pair collision filter if necessary
void ExportWorld::disableCollisionPair (hkRigidBody* rbodyA, hkRigidBody* rbodyB)
{
	if (!m_pairwise_collision_filter)
	{
		m_pairwise_collision_filter = new hkPairwiseCollisionFilter();

		addFilterToWorld(m_pairwise_collision_filter);
		m_pairwise_collision_filter->removeReference();
	}

	m_pairwise_collision_filter->disableCollisionPair(rbodyA, rbodyB);
}

// Support for H1 Collision groups - adds filter if necessary
void ExportWorld::enableCollisionGroups (unsigned int groupA, unsigned int groupB)
{
	if (!m_group_collision_filter)
	{
		m_group_collision_filter = new hkGroupCollisionFilter();

		addFilterToWorld(m_group_collision_filter);

		m_group_collision_filter->removeReference();
	}

	m_group_collision_filter->disableCollisionGroups(groupA, groupB);
}

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
