/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkexport2/hkexport.h>
#include <hkexport2/exportgeometry.h>

int ExportGeometry::priv_CLASS_SerialNumber=1;
 
ExportGeometry::ExportGeometry(ExportWorld *world) : ExportClass()
{
	PROT_AutoName();

	prot_world=world;
	prot_hk_storageMeshShape = HK_NULL;
	priv_CLASS_SerialNumber++;
}
	// Destructor
ExportGeometry::~ExportGeometry()
{
	if (prot_hk_storageMeshShape)
	{
		prot_hk_storageMeshShape->removeReference();
		prot_hk_storageMeshShape = HK_NULL;
	}
}



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
