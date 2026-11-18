/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef INC_EXPORTGEOMETRICPRIMITIVE
#define INC_EXPORTGEOMETRICPRIMITIVE

#include <hkexport2/exportprimitive.h>
#include <hkexport2/exportprimitivefactory.h>
#include <hkexport2/exportgeometry.h>

class ExportRigidBody;
class hkShape;
 
class ExportGeometricPrimitive: public ExportPrimitive
{
public:
	// Constructor
	ExportGeometricPrimitive (ExportRigidBody *rigid_body);
	// Destructor
	~ExportGeometricPrimitive();

	// Overrides from ExportClass
	char *ClassName() {return "GeometricPrimitive";}
	int  SerialNumber() {return priv_CLASS_SerialNumber;}

	// Import&Export
	void Import (NS_ExportStream::InExportStream &xtream, CheckLog &log); //OVERRIDE from ExportPrimitive

private:
	// Class (static) members
	static int priv_CLASS_SerialNumber;

	//this takes and arbitrary set of vertices and finds out it it's a box (I hope :) )
	bool getIsBoxGeometry( const hkStorageMeshShape* geom, hkVector4& extents, hkTransform& boxToGeom, const hkReal epsilon = 1e-2f );
};

class ExportGeometricPrimitiveFactory: public ExportSingleton<ExportGeometricPrimitiveFactory>, public ExportPrimitiveFactory
{
public: 
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_EXPORT)
	// OVERRIDE 
	ExportPrimitive *CreateExportPrimitive(NS_ExportStream::Token &token, ExportRigidBody *rigid_body, CheckLog &log);
	ExportGeometricPrimitiveFactory() { } //should really only be called once
};

typedef hkArray<ExportGeometricPrimitive *> ExportGeometricPrimitive_Vector;
typedef ExportGeometricPrimitive_Vector::iterator ExportGeometricPrimitive_VectorIterator;

#endif //INC_EXPORTGEOMETRICPRIMITIVE

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
