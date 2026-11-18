/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef INC_EXPORTSUBSPACE
#define INC_EXPORTSUBSPACE

#include <hkexport2/exportclass.h>
#include <hkexport2/exportrigidbodycollection.h>
#include <hkexport2/exportentitycollfactory.h>
#include <hkexport2/exportaction.h>

class ExportWorld;
class ExportRigidBody;
class ExportConstraint;
class ExportSubspace: public ExportClass
{
public:
	// Constructor (empty)
	ExportSubspace(ExportWorld *world);
	// Destructor
	~ExportSubspace();

	// Overrides from ExportClass
	const char* ClassName() {return "Subspace";}
	int   SerialNumber() {return priv_CLASS_SerialNumber;}

	// Import
	void Import(NS_ExportStream::InExportStream &xtream, CheckLog &log);

	// EntityCollections management
	void AddRigidBodyCollection(ExportRigidBodyCollection *entity_collection)  {priv_rigid_body_collections_vector.pushBack(entity_collection);}
	int GetNumRigidBodyCollections() const {return priv_rigid_body_collections_vector.getSize();}
	ExportRigidBodyCollection *GetRigidBodyCollection(const char* name) const;
	ExportRigidBodyCollection *GetRigidBodyCollection(int index) const {return priv_rigid_body_collections_vector[index];}

	// RigidBody Search : search through the collections
	int GetNumRigidBodies() const;
	ExportRigidBody *GetRigidBody(const char* name) const;
	ExportRigidBody *GetRigidBody(hkRigidBody *hk_rigid_body) const;
	ExportRigidBody *GetRigidBody(int index) const;

	
	// Actions management
	void AddAction(ExportAction *action) {priv_actions_vector.pushBack(action);}
	int GetNumActions() const {return priv_actions_vector.getSize();}
	ExportAction *GetAction(const char* name) const;
	ExportAction *GetAction(int index) const {return priv_actions_vector[index];}

	int GetNumConstraints() const;
	ExportConstraint *GetConstraint( const char* name ) const;
	ExportConstraint *GetConstraint( int index ) const;

	const hkVector4& GetGravity() const {return priv_gravity;}
	float GetTolerance() const {return priv_tolerance;}

	// Owner retrieving
	ExportWorld *GetWorld() const {return priv_world;}

private:
	ExportWorld *priv_world;// Referenced
	hkVector4 priv_gravity;
	float priv_tolerance;

	ExportAction_Vector				priv_actions_vector;				// Owned
	ExportRigidBodyCollection_Vector	priv_rigid_body_collections_vector;		// Owned

	// Class (static) member
	static int priv_CLASS_SerialNumber;
};

typedef hkArray<ExportSubspace *> ExportSubspace_Vector;
typedef ExportSubspace_Vector::const_iterator ExportSubspace_VectorIterator;

#endif

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
