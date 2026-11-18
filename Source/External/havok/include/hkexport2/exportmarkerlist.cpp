/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
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
}

ExportMarkerList::~ExportMarkerList()
{

	ExportMarker_VectorIterator m_it;
	for (m_it=priv_markers_vector.begin(); m_it<priv_markers_vector.end(); m_it++)
		delete (*m_it);

}

void ExportMarkerList::Import(InExportStream &xtream, CheckLog &log)
{
	// BEGIN_MARKER_LIST is already read -->get name
	xtream>>prot_buffer; SetName(prot_buffer);

	while (xtream>>prot_token, prot_token=="BEGIN_MARKER")
	{
		ExportMarker *marker=new ExportMarker(this);
		marker->Import(xtream, log);
		AddMarker(marker);
	}
	if (prot_token!="END_MARKER_LIST") throw_exception("Unexpected token. END_MARKER_LIST was expected");
}

ExportMarker *ExportMarkerList::GetMarker (char *name) const
{
	ExportMarker_VectorIterator m_it;
	for (m_it=priv_markers_vector.begin(); m_it<priv_markers_vector.end(); m_it++)
		if(!hkString::strCmp((*m_it)->GetName(),name)) return (*m_it);

	return HK_NULL;
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
