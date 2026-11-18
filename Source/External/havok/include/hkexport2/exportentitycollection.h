/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef INC_EXPORTENTITYCOLLECTION
#define INC_EXPORTENTITYCOLLECTION

#include <hkexport2/exportclass.h>

class ExportSubspace;

class ExportRigidBody;
class ExportCollidable;

// THIS CLASS IS ABSTRACT
class ExportEntityCollection : public ExportClass
{
public:
	// Constructor (empty)
	ExportEntityCollection (ExportSubspace *subspace);
	// Destructor
	~ExportEntityCollection();

	// VIRTUAL methods
	virtual void Import(NS_ExportStream::InExportStream &xtream, CheckLog &log)=0;

	// Owner retrieving
	ExportSubspace *GetSubspace() const {return prot_subspace;}

protected:
	ExportSubspace *prot_subspace;// Referenced
};

typedef hkArray<ExportEntityCollection *> ExportEntityCollection_Vector;
typedef ExportEntityCollection_Vector::const_iterator ExportEntityCollection_VectorIterator;

#endif  //INC_EXPORTENTITYCOLLECTION

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
