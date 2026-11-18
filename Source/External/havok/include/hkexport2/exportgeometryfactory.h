/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef INC_EXPORTGEOMETRYFACTORY
#define INC_EXPORTGEOMETRYFACTORY

#include <hkexport2/exportstream.h>

class ExportGeometry;
class ExportWorld;

class ExportGeometryFactory : public hkBaseObject
{
public:

	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_EXPORT)
	virtual ExportGeometry *CreateExportGeometry(NS_ExportStream::Token &token, ExportWorld *world)=0;
};

typedef hkArray<ExportGeometryFactory *> ExportGeometryFactory_Vector;
typedef ExportGeometryFactory_Vector::iterator ExportGeometryFactory_VectorIterator;

class ExportGeometryRegistry : public ExportSingleton<ExportGeometryRegistry>
{
public:
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_EXPORT)
	
	ExportGeometry *CreateExportGeometry(NS_ExportStream::Token &token, ExportWorld *world);
	void AddFactory(ExportGeometryFactory *new_factory) {priv_factories.pushBack(new_factory);}
	ExportGeometryRegistry(); //should really only be called once

private:
	ExportGeometryFactory_Vector priv_factories; //NOT OWNED, just referenced (they are not destroyed)
};

#endif //INC_EXPORTGEOMETRYFACTORY

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
