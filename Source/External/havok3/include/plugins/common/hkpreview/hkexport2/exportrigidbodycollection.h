/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef INC_EXPORTRIGIDBODYCOLLECTION
#define INC_EXPORTRIGIDBODYCOLLECTION

#include <hkexport2/exportentitycollection.h>
#include <hkexport2/exportentitycollfactory.h>
#include <hkexport2/exportrigidbody.h>

class ExportRigidBodyCollection: public ExportEntityCollection
{
public:
	// Constructor
	ExportRigidBodyCollection(ExportSubspace *subspace);
	// Destructor
	~ExportRigidBodyCollection();

	// Override from ExportClass
	const char* ClassName() {return "RigidBodyCollection";}
	int  SerialNumber() {return priv_CLASS_SerialNumber;}

	// Import
	void Import (NS_ExportStream::InExportStream &xtream, CheckLog &log); // OVERRIDE from ExportEntityCollection

	// Management of the rigid body list
	void AddRigidBody (ExportRigidBody *rigid_body) {priv_rigid_bodies_vector.pushBack(rigid_body);}
	int GetNumRigidBodies() const {return priv_rigid_bodies_vector.getSize();}
	ExportRigidBody *GetRigidBody(const char* name) const;
	ExportRigidBody *GetRigidBody(hkRigidBody *hk_rigid_body) const; 
	ExportRigidBody *GetRigidBody(int index) const {return priv_rigid_bodies_vector[index];}

private:
	ExportRigidBody_Vector priv_rigid_bodies_vector;	// Owned

	// Class (static) member
	static int priv_CLASS_SerialNumber;
};

class ExportRigidBodyCollectionFactory: public ExportEntityCollectionFactory, public ExportSingleton<ExportRigidBodyCollectionFactory>
{
public:
	HK_DECLARE_CLASS_ALLOCATOR_UNCHECKED(HK_MEMORY_CLASS_EXPORT);
	// OVERRIDE 
	ExportEntityCollection *CreateExportEntityCollection(NS_ExportStream::Token &token, ExportSubspace *subspace);
	ExportRigidBodyCollectionFactory() { } //should really only be called once
};

typedef hkArray<ExportRigidBodyCollection *> ExportRigidBodyCollection_Vector;
typedef ExportRigidBodyCollection_Vector::const_iterator ExportRigidBodyCollection_VectorIterator;

#endif //INC_EXPORTRIGIDBODYCOLLECTION

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
