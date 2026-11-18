/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef INC_EXPORTGEOMETRY
#define INC_EXPORTGEOMETRY

#include <hkexport2/exportclass.h>
#include <hkgeometry2/hkGeometry.h>
#include <hkgeometry2/hkTriangleIndices.h>
#include <hkcollide2/shape/hkStorageMeshShape.h>

class ExportWorld;

//*** Abstract Class ***//
class ExportGeometry: public ExportClass
{
public:
	// Constructor
	ExportGeometry(ExportWorld *world);
	// Destructor
	~ExportGeometry();

	// From ExportClass
	char *ClassName() {return "Geometry";}
	int  SerialNumber() {return priv_CLASS_SerialNumber;}

	virtual void Import(NS_ExportStream::InExportStream &xtream, CheckLog &log)=0;

	hkStorageMeshShape *GetHKStorageMeshShape() const {return prot_hk_storageMeshShape;}

	// Owner retrieving
	ExportWorld *GetWorld() const {return prot_world;}

protected:
	ExportWorld *prot_world;		// Referenced
	hkStorageMeshShape* prot_hk_storageMeshShape; // Referenced
	
private:
	// Class (static) member
	static int priv_CLASS_SerialNumber;
};
 
typedef hkArray<ExportGeometry *> ExportGeometry_Vector;
typedef ExportGeometry_Vector::const_iterator ExportGeometry_VectorIterator;

#endif // INC_EXPORTGEOMETRY

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
