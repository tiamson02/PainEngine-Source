/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */



#include <hkexport2/hkexport.h>

#include <hkexport2/exportmarker.h>
#include <hkexport2/exportmarkerlist.h>

using namespace NS_ExportStream;


int ExportMarker::priv_CLASS_SerialNumber=1;

// Constructor
ExportMarker::ExportMarker (ExportMarkerList *marker_list) 
: priv_marker_list (marker_list)
{
	PROT_AutoName();
	priv_transform.setIdentity();
}

// Destructor
ExportMarker::~ExportMarker() 
{}

// Import
void ExportMarker::Import(InExportStream &xtream, CheckLog &log)
{
	hkVector4 translation;
	hkQuaternion rotation;

	xtream>>prot_buffer; SetName(prot_buffer);

	xtream.TokenMustBe("TRANSLATION"); xtream>>translation;
	xtream.TokenMustBe("ROTATION"); xtream>>rotation;

	priv_transform=hkTransform(rotation,translation);

	xtream.TokenMustBe("END_MARKER");
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
