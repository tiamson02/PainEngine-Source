/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef INC_EXPORTPRIMITIVE
#define INC_EXPORTPRIMITIVE

class ExportRigidBody;
class hkShape;

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

	// We override the naming to avoid conflict with rigid body names.
	// Primitives (shapes) return the name plus a "_SHAPE" suffix.
	hkString GetSerializableName();

	//the top level is always a transform shape
	virtual hkShape* GetHKShape() const {return prot_shape;} //owned (we remove a reference on deletion)
	//the collision mask for Havok 1 was per primitive, in Havok 2 collision masks are rigidbody properties
	//the primitive masks are OR'd together to give the rigidbody mask so if a rigidbody had a primitive in 
	//a particular group in Havok 1 then the body is in that group in Havok 2
	unsigned int GetCollisionMask() const { return prot_collision_mask;	}

	// Owner Retrieving
	ExportRigidBody *GetRigidBody() const {return prot_rigid_body;}

protected:
	ExportRigidBody *prot_rigid_body;		// Referenced
	// Common data
	float			prot_mass;
	hkTransform	prot_transform;
	unsigned int prot_collision_mask;
	hkBool prot_collisions_disabled;
	hkShape* prot_shape;
	

};

typedef hkArray<ExportPrimitive *> ExportPrimitive_Vector;
typedef ExportPrimitive_Vector::const_iterator ExportPrimitive_VectorIterator;

#endif // INC_EXPORTPRIMITIVE

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
