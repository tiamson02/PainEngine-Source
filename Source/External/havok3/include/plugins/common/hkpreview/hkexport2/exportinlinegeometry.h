/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef INC_EXPORTINLINEGEOMETRY
#define INC_EXPORTINLINEGEOMETRY

#include <hkexport2/exportgeometry.h>
#include <hkexport2/exportgeometryfactory.h>

class ExportInlineGeometry: public ExportGeometry
{
public:
	// Constructor
	ExportInlineGeometry(ExportWorld *world);
	// Destructor
	~ExportInlineGeometry();

	void Import(NS_ExportStream::InExportStream &xtream, CheckLog &log); // OVERRIDE from ExportGeometry

private:

	//this takes and arbitrary set of vertices and finds out if it's a box
	static hkBool getIsBoxGeometry( const hkArray<hkVector4>& vertices, hkVector4& halfExtents, hkTransform& boxToGeom, const hkReal epsilon = 1e-2f );

	//will call the above, but if it fails, it will create a box anyway
	static hkBool getOrOtherwiseForceBoxGeometry (const hkArray<hkVector4>& vertices, hkVector4& halfExtents, hkTransform& boxToGeom, const hkReal epsilon = 1e-2f );

	//will find a fitting cylinder for the geometry
	static void findBestCylinder (const hkArray<hkVector4>& vertices, hkVector4& pointAout, hkVector4& pointBout, hkReal& radiusOut );
};

class ExportInlineGeometryFactory: public ExportGeometryFactory, public ExportSingleton<ExportInlineGeometryFactory>
{
public:
	HK_DECLARE_CLASS_ALLOCATOR_UNCHECKED(HK_MEMORY_CLASS_EXPORT);
	// OVERRIDE 
	ExportGeometry *CreateExportGeometry(NS_ExportStream::Token &token, ExportWorld *world);
	ExportInlineGeometryFactory() { } //should really only be called once
};

typedef hkArray<ExportInlineGeometry *> ExportInlineGeometry_Vector;
typedef ExportInlineGeometry_Vector::iterator ExportInlineGeometry_VectorIterator;

#endif //INC_EXPORTINLINEGEOMETRY

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
