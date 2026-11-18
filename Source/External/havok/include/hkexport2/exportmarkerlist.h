/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef INC_EXPORTMARKERLIST
#define INC_EXPORTMARKERLIST

#include <hkexport2/exportclass.h>
#include <hkexport2/exportmarker.h>

class ExportWorld;

class ExportMarkerList: public ExportClass
{
public:
	// Constructor (empty)
	ExportMarkerList(ExportWorld *world);
	// Destructor
	~ExportMarkerList();

	// Overrides from ExportClass
	char *ClassName() {return "MarkerList";}
	int   SerialNumber() {return priv_CLASS_SerialNumber;}

	// Import
	void Import(NS_ExportStream::InExportStream &xtream, CheckLog &log);

	// Markers management
	void AddMarker(ExportMarker *marker) {priv_markers_vector.pushBack(marker);}
	int GetNumMarkers() const {return priv_markers_vector.getSize();}
	ExportMarker *GetMarker (char *name) const;
	ExportMarker *GetMarker(int index) const {return priv_markers_vector[index];}

	// Owner retrieving
	ExportWorld *GetWorld() {return priv_world;}

private:
	ExportWorld *priv_world;										// Referenced
	ExportMarker_Vector			  priv_markers_vector;				// Owned
	// Class (static) member
	static int priv_CLASS_SerialNumber;

};

typedef hkArray<ExportMarkerList *> ExportMarkerList_Vector;
typedef ExportMarkerList_Vector::const_iterator ExportMarkerList_VectorIterator;

#endif

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
