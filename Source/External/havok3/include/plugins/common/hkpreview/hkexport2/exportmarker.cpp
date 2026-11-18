/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */



#include <hkexport2/hkexport.h>

#include <hkexport2/exportmarker.h>
#include <hkexport2/exportmarkerlist.h>
#include <hkexport2/exportworld.h>

using namespace NS_ExportStream;


int ExportMarker::priv_CLASS_SerialNumber=1;

// Constructor
ExportMarker::ExportMarker (ExportMarkerList *marker_list) 
: priv_marker_list (marker_list)
{
	PROT_AutoName();
	priv_transform.setIdentity();
	m_hkMarker = HK_NULL;
}

// Destructor
ExportMarker::~ExportMarker() 
{
	if (m_hkMarker)
	{
		m_hkMarker->removeReference();
	}
}

// Import
void ExportMarker::Import(InExportStream &xtream, CheckLog &log)
{
	hkVector4 translation;
	hkQuaternion rotation;

	const hkConvertCS& converter = priv_marker_list->GetWorld()->getCSConverter();

	xtream>>prot_buffer; SetName(prot_buffer);

	xtream.TokenMustBe("TRANSLATION"); xtream>>translation;
	converter.convertVector(translation);

	xtream.TokenMustBe("ROTATION"); xtream>>rotation;
	converter.convertQuaternion(rotation);

	priv_transform=hkTransform(rotation,translation);

	xtream.TokenMustBe("END_MARKER");

	// Create the serializable marker
	m_hkMarker = new hkSerializedDisplayMarker();
	m_hkMarker->m_transform = priv_transform;
//XXNAMES	m_hkMarker->setName(GetSerializableName().cString());

}


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
