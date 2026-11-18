/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef INC_EXPORTGEOMETRY
#define INC_EXPORTGEOMETRY

#include <hkexport2/exportclass.h>

class ExportWorld;

class hkMoppBvTreeShape;
class hkConvexVerticesShape;
class hkBoxShape;
class hkCapsuleShape;
class hkCylinderShape;
class hkSimpleMeshShape;

// Geometry types
enum ExpGeometryType
{
	GT_MESH,
	GT_HULL,
	GT_BOX,
	GT_CAPSULE,
	GT_CYLINDER,
};


//*** Abstract Class ***//
class ExportGeometry: public ExportClass
{
public:

	// Constructor
	ExportGeometry(ExportWorld *world);
	// Destructor
	~ExportGeometry();

	// From ExportClass
	const char* ClassName() {return "Geometry";}
	int  SerialNumber() {return priv_CLASS_SerialNumber;}

	// We override the naming to avoid conflict with rigid body names.
	// Geometries (shapes) return the name plus a "_SHAPE" suffix.
	hkString GetSerializableName();

	virtual void Import(NS_ExportStream::InExportStream &xtream, CheckLog &log)=0;

	// Multiple shapes associated with a single geometry
	hkSimpleMeshShape *getStorageMeshShape() const {return m_storageMeshShape;}
	hkMoppBvTreeShape* getConcaveMeshShape() const {return m_moppBvTreeShape;}
	hkConvexVerticesShape* getConvexHullShape() const {return m_convexVerticesShape;}
	hkBoxShape* getBoxShape() const {return m_boxShape;}
	hkCapsuleShape* getCapsuleShape() const {return m_capsuleShape;}
	hkCylinderShape* getCylinderShape() const {return m_cylinderShape;}

	// The creation of box and capsule shapes from geometries may imply an extra transform to place the shape
	// to match the original geometry. This is returned here
	const hkTransform& getShapeTransform() const {return m_shapeTransform;}


	ExpGeometryType getGeometryType() {return m_geometryType;}

	// Owner retrieving
	ExportWorld *GetWorld() const {return prot_world;}

protected:
	ExportWorld *prot_world;		// Referenced

	hkSimpleMeshShape* m_storageMeshShape; // Referenced
	hkMoppBvTreeShape* m_moppBvTreeShape;
	hkConvexVerticesShape* m_convexVerticesShape;
	hkBoxShape* m_boxShape;
	hkCapsuleShape* m_capsuleShape;
	hkCylinderShape* m_cylinderShape;
	hkTransform m_shapeTransform;

	ExpGeometryType m_geometryType;
	
private:
	// Class (static) member
	static int priv_CLASS_SerialNumber;
};
 
typedef hkArray<ExportGeometry *> ExportGeometry_Vector;
typedef ExportGeometry_Vector::const_iterator ExportGeometry_VectorIterator;

#endif // INC_EXPORTGEOMETRY

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
