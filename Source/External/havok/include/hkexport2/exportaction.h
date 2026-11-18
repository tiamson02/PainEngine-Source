/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef INC_EXPORTACTION
#define INC_EXPORTACTION

#include <hkexport2/exportclass.h>

class ExportSubspace;
class hkAction;

//*** Abstract Class ***//
class ExportAction: public ExportClass
{
public:
	// Constructor
	ExportAction(ExportSubspace *subspace);
	// Destructor
	~ExportAction();

	virtual void Import(NS_ExportStream::InExportStream &xtream, CheckLog &log)=0;
	virtual hkAction* GetHKAction() const =0;

	// Owner retrieving
	ExportSubspace *GetSubspace() const {return prot_subspace;}
protected:
	ExportSubspace *prot_subspace;		// Referenced;
};

typedef hkArray<ExportAction *> ExportAction_Vector;
typedef ExportAction_Vector::const_iterator ExportAction_VectorIterator;

#endif // INC_EXPORTACTION

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
