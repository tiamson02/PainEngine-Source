/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkexport2/hkexport.h>



#include <hkexport2/exportinlinegeometry.h>
#include <hkexport2/exportgeometricprimitive.h>

using namespace NS_ExportStream;

// FACTORY ----------------------------------------------------------------------------------------------

EXPORT_SINGLETON_IMPLEMENTATION(ExportInlineGeometryFactory);

ExportGeometry* ExportInlineGeometryFactory::CreateExportGeometry(Token &token, ExportWorld *world)
{
	if (token=="Inline") return new ExportInlineGeometry(world);
	return HK_NULL;
}
// ------------------------------------------------------------------------------------------------------

ExportInlineGeometry::ExportInlineGeometry(ExportWorld *world) : ExportGeometry(world)
{
	
}
 
// Destructor
ExportInlineGeometry::~ExportInlineGeometry()
{
}

void ExportInlineGeometry::Import(InExportStream &xtream, CheckLog &log)
{
	// Name
	xtream>>prot_buffer; SetName(prot_buffer);

	int numVerts, numFaces;

	xtream>>numVerts;

	hkArray<hkVector4> verts (numVerts);
	
	for (int vert_it=0; vert_it<numVerts; vert_it++)
	{
		hkVector4 vert;
		xtream>>vert;
		verts[vert_it]=vert;
	}

	xtream>>numFaces;
	hkArray<int> indices (numFaces*3);

	for (int i = 0; i < numFaces * 3; i++)
	{
		int index;
		xtream>>index;
		indices[i] = index;
	}

	prot_hk_storageMeshShape = new hkStorageMeshShape (verts, indices);

	xtream.TokenMustBe("END_GEOMETRY");
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
