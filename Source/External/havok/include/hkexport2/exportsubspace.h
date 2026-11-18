/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
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
	char *ClassName() {return "Subspace";}
	int   SerialNumber() {return priv_CLASS_SerialNumber;}

	// Import
	void Import(NS_ExportStream::InExportStream &xtream, CheckLog &log);

	// EntityCollections management
	void AddRigidBodyCollection(ExportRigidBodyCollection *entity_collection)  {priv_rigid_body_collections_vector.pushBack(entity_collection);}
	int GetNumRigidBodyCollections() const {return priv_rigid_body_collections_vector.getSize();}
	ExportRigidBodyCollection *GetRigidBodyCollection(char *name) const;
	ExportRigidBodyCollection *GetRigidBodyCollection(int index) const {return priv_rigid_body_collections_vector[index];}

	// RigidBody Search : search through the collections
	int GetNumRigidBodies() const;
	ExportRigidBody *GetRigidBody(char *name) const;
	ExportRigidBody *GetRigidBody(hkRigidBody *hk_rigid_body) const;
	ExportRigidBody *GetRigidBody(int index) const;

	
	// Actions management
	void AddAction(ExportAction *action) {priv_actions_vector.pushBack(action);}
	int GetNumActions() const {return priv_actions_vector.getSize();}
	ExportAction *GetAction(char *name) const;
	ExportAction *GetAction(int index) const {return priv_actions_vector[index];}

	int GetNumConstraints() const;
	ExportConstraint *GetConstraint( char *name ) const;
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
