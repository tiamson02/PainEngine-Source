/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
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
	const char* ClassName() {return "EnabledCollisionGroups";}
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
