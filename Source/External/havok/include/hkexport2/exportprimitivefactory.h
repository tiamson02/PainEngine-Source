/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef INC_EXPORTPRIMITIVEFACTORY
#define INC_EXPORTPRIMITIVEFACTORY


#include <hkexport2/exportstream.h>

class ExportPrimitive;
class ExportRigidBody;

class ExportPrimitiveFactory : public hkBaseObject
{
public:
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_EXPORT)

	virtual ExportPrimitive *CreateExportPrimitive(NS_ExportStream::Token &token, ExportRigidBody *rigid_body, CheckLog &log)=0;
};

typedef hkArray<ExportPrimitiveFactory *> ExportPrimitiveFactory_Vector;
typedef ExportPrimitiveFactory_Vector::iterator ExportPrimitiveFactory_VectorIterator;

class ExportPrimitiveRegistry : public ExportSingleton<ExportPrimitiveRegistry>
{
public:
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_EXPORT)
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
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_EXPORT)
	// OVERRIDE 
	ExportPrimitive *CreateExportPrimitive(NS_ExportStream::Token &token, ExportRigidBody *rigid_body, CheckLog &log);
	ExportUnsupportedPrimitiveFactory() { } //should really only be called once
};

#endif //INC_EXPORTPRIMITIVEFACTORY

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
