/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
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
};

class ExportInlineGeometryFactory: public ExportGeometryFactory, public ExportSingleton<ExportInlineGeometryFactory>
{
public:
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_EXPORT)
	// OVERRIDE 
	ExportGeometry *CreateExportGeometry(NS_ExportStream::Token &token, ExportWorld *world);
	ExportInlineGeometryFactory() { } //should really only be called once
};

typedef hkArray<ExportInlineGeometry *> ExportInlineGeometry_Vector;
typedef ExportInlineGeometry_Vector::iterator ExportInlineGeometry_VectorIterator;

#endif //INC_EXPORTINLINEGEOMETRY

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
