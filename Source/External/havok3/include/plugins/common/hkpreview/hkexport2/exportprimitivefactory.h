/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef INC_EXPORTPRIMITIVEFACTORY
#define INC_EXPORTPRIMITIVEFACTORY


#include <hkexport2/exportstream.h>

class ExportPrimitive;
class ExportRigidBody;

class ExportPrimitiveFactory : public hkReferencedObject
{
public:
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_EXPORT);

	virtual ExportPrimitive *CreateExportPrimitive(NS_ExportStream::Token &token, ExportRigidBody *rigid_body, CheckLog &log)=0;
};

typedef hkArray<ExportPrimitiveFactory *> ExportPrimitiveFactory_Vector;
typedef ExportPrimitiveFactory_Vector::iterator ExportPrimitiveFactory_VectorIterator;

class ExportPrimitiveRegistry : public ExportSingleton<ExportPrimitiveRegistry>
{
public:
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_EXPORT);
	ExportPrimitive *CreateExportPrimitive(NS_ExportStream::Token &token, ExportRigidBody *rigid_body, CheckLog &log);
	void AddFactory(ExportPrimitiveFactory *new_factory) {priv_factories.pushBack(new_factory);}
	ExportPrimitiveRegistry(); //should really only be called once

private:
	ExportPrimitiveFactory_Vector priv_factories; // NOT OWNED, just referenced (they are not destroyed)
};

//this is a dummy class to provide accurate warnings on import
class ExportUnsupportedPrimitiveFactory: public ExportSingleton<ExportUnsupportedPrimitiveFactory>, public ExportPrimitiveFactory
{
public: 
	HK_DECLARE_CLASS_ALLOCATOR_UNCHECKED(HK_MEMORY_CLASS_EXPORT);
	// OVERRIDE 
	ExportPrimitive *CreateExportPrimitive(NS_ExportStream::Token &token, ExportRigidBody *rigid_body, CheckLog &log);
	ExportUnsupportedPrimitiveFactory() { } //should really only be called once
};

#endif //INC_EXPORTPRIMITIVEFACTORY

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
