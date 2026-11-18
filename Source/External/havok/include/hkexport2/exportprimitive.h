/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef INC_EXPORTPRIMITIVE
#define INC_EXPORTPRIMITIVE

class ExportRigidBody;
class hkShape;
class hkStorageMeshShape;

// THIS CLASS IS ABSTRACT
class ExportPrimitive: public ExportClass
{
public:
	// Constructor
	ExportPrimitive(ExportRigidBody *rigid_body);
	// Destructor
	~ExportPrimitive();

	virtual void Import (NS_ExportStream::InExportStream &xtream, CheckLog &log)=0;

	// Common import/export
	void CommonPreImport(NS_ExportStream::InExportStream &xtream, CheckLog &log);
	//currently deals with some collision disabling and createstop level transform shape if required
	void CommonPostImport(NS_ExportStream::InExportStream &xtream, CheckLog &log);
	float GetMass() const {return prot_mass;}
	const hkTransform& GetTransform() const {return prot_transform;}

	//the top level is always a transform shape
	virtual hkShape* GetHKShape() const {return prot_shape;} //owned (we remove a reference on deletion)
	//the collision mask for Havok 1 was per primitive, in Havok 2 collision masks are rigidbody properties
	//the primitive masks are OR'd together to give the rigidbody mask so if a rigidbody had a primitive in 
	//a particular group in Havok 1 then the body is in that group in Havok 2
	unsigned int GetCollisionMask() const { return prot_collision_mask;	}

	hkStorageMeshShape* GetHKSourceShape( void ) const { return source_shape; };

	// Owner Retrieving
	ExportRigidBody *GetRigidBody() const {return prot_rigid_body;}

protected:
	ExportRigidBody *prot_rigid_body;		// Referenced
	// Common data
	float			prot_mass;
	hkTransform	prot_transform;
	unsigned int prot_collision_mask;
	bool prot_collisions_disabled;
	hkShape* prot_shape;

	hkStorageMeshShape* source_shape;
};

typedef hkArray<ExportPrimitive *> ExportPrimitive_Vector;
typedef ExportPrimitive_Vector::const_iterator ExportPrimitive_VectorIterator;

#endif // INC_EXPORTPRIMITIVE

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
