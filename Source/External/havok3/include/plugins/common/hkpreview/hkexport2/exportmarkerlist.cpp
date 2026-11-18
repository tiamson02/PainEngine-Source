/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */



#include <hkexport2/hkexport.h>

#include <hkexport2/exportmarkerlist.h>
#include <hkexport2/exportworld.h>

using namespace NS_ExportStream;

int	ExportMarkerList::priv_CLASS_SerialNumber=1;

ExportMarkerList::ExportMarkerList(ExportWorld *world) : ExportClass()
{
	PROT_AutoName();
	priv_world=world;
	priv_CLASS_SerialNumber++;
	m_hkMarkerList = HK_NULL;
}

ExportMarkerList::~ExportMarkerList()
{

	ExportMarker_VectorIterator m_it;
	for (m_it=priv_markers_vector.begin(); m_it<priv_markers_vector.end(); m_it++)
		delete (*m_it);

	if (m_hkMarkerList)
	{
		m_hkMarkerList->removeReference();
	}
}

void ExportMarkerList::Import(InExportStream &xtream, CheckLog &log)
{
	// BEGIN_MARKER_LIST is already read -->get name
	xtream>>prot_buffer; SetName(prot_buffer);

	m_hkMarkerList = new hkSerializedDisplayMarkerList();
	while (xtream>>prot_token, prot_token=="BEGIN_MARKER")
	{
		ExportMarker *marker=new ExportMarker(this);
		marker->Import(xtream, log);
		AddMarker(marker);
		m_hkMarkerList->addMarker( marker->getHKMarker() );
	}

//XXNAMES	m_hkMarkerList->setName(GetSerializableName().cString());

	if (prot_token!="END_MARKER_LIST") throw_exception("Unexpected token. END_MARKER_LIST was expected");

}

ExportMarker *ExportMarkerList::GetMarker (const char* name) const
{
	ExportMarker_VectorIterator m_it;
	for (m_it=priv_markers_vector.begin(); m_it<priv_markers_vector.end(); m_it++)
		if(!hkString::strCmp((*m_it)->GetName(),name)) return (*m_it);

	return HK_NULL;
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
