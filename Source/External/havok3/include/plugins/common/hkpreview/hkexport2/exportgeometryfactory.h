/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef INC_EXPORTGEOMETRYFACTORY
#define INC_EXPORTGEOMETRYFACTORY

#include <hkexport2/exportstream.h>

class ExportGeometry;
class ExportWorld;

class ExportGeometryFactory : public hkReferencedObject
{
public:

	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_EXPORT);
	virtual ExportGeometry *CreateExportGeometry(NS_ExportStream::Token &token, ExportWorld *world)=0;
};

typedef hkArray<ExportGeometryFactory *> ExportGeometryFactory_Vector;
typedef ExportGeometryFactory_Vector::iterator ExportGeometryFactory_VectorIterator;

class ExportGeometryRegistry : public ExportSingleton<ExportGeometryRegistry>
{
public:
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_EXPORT);
	
	ExportGeometry *CreateExportGeometry(NS_ExportStream::Token &token, ExportWorld *world);
	void AddFactory(ExportGeometryFactory *new_factory) {priv_factories.pushBack(new_factory);}
	ExportGeometryRegistry(); //should really only be called once

private:
	ExportGeometryFactory_Vector priv_factories; //NOT OWNED, just referenced (they are not destroyed)
};

#endif //INC_EXPORTGEOMETRYFACTORY

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
