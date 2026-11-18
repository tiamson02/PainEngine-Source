/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
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
