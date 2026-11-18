/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef INC_EXPORTCOLLISIONGROUPS
#define INC_EXPORTCOLLISIONGROUPS

#include <hkexport2/exportclass.h>

class ExportSubspace;

class ExportCollisionGroups: public ExportClass
{
public:
	// Constructor (empty)
	ExportCollisionGroups(ExportSubspace *subspace);
	// Destructor
	~ExportCollisionGroups();

	// Overrides from ExportClass
	char *ClassName() {return "EnabledCollisionGroups";}
	int   SerialNumber() {return priv_CLASS_SerialNumber;}

	// Import
	void Import(NS_ExportStream::InExportStream &xtream, CheckLog& log);

	// Owner retrieving
	ExportSubspace *GetSubspace() {return priv_subspace;}

private:
	ExportSubspace *priv_subspace;										// Referenced
	// Class (static) member
	static int priv_CLASS_SerialNumber;

};



#endif //INC_EXPORTCOLLISIONGROUPS

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
