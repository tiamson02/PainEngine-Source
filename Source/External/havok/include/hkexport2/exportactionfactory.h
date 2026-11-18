/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef INC_EXPORTACTIONFACTORY
#define INC_EXPORTACTIONFACTORY

#include <hkexport2/exportstream.h>

class ExportAction;
class ExportSubspace;

class ExportActionFactory : public hkBaseObject
{
public:
	
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_EXPORT)
	virtual ExportAction *CreateExportAction(NS_ExportStream::Token &token, ExportSubspace *subspace, CheckLog& log)=0;
};

typedef hkArray<ExportActionFactory *> ExportActionFactory_Vector;
typedef ExportActionFactory_Vector::const_iterator ExportActionFactory_VectorIterator;

class ExportActionRegistry : public ExportSingleton<ExportActionRegistry>
{
public:	
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_EXPORT)
	ExportAction *CreateExportAction(NS_ExportStream::Token &token, ExportSubspace *subspace, CheckLog& log);
	void AddFactory(ExportActionFactory *new_factory) {priv_factories.pushBack(new_factory);}
	ExportActionRegistry(); //this should only be called once

private:
	ExportActionFactory_Vector priv_factories; // referenced (they are not destroyed and are usually singletons)
};

//dummy factory that provides accurate warnings on import for unsupported actions
class ExportUnsupportedActionFactory: public ExportSingleton<ExportUnsupportedActionFactory>, public ExportActionFactory
{
public: 
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_EXPORT)
	// OVERRIDE 
	ExportAction *CreateExportAction(NS_ExportStream::Token &token, ExportSubspace *subspace, CheckLog& log);
	ExportUnsupportedActionFactory() { }
};

#endif //INC_EXPORTACTIONFACTORY

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
