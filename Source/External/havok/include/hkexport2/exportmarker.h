/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef INC_EXPORT_MARKER
#define INC_EXPORT_MARKER

#include <hkexport2/exportclass.h>
//#include <hkmath2/transform.h>

class ExportMarkerList;

class ExportMarker: public ExportClass
{
public:
	// Constructor (empty)
	ExportMarker(ExportMarkerList *world);
	// Destructor
	~ExportMarker();

	// Overrides from ExportClass
	char *ClassName() {return "Marker";}
	int   SerialNumber() {return priv_CLASS_SerialNumber;}

	// set/get of the information (tansform)
	void SetTransform (const hkTransform &transform) {priv_transform=transform;}
	const hkTransform& GetTransform() const {return priv_transform;}

	// Import
	void Import(NS_ExportStream::InExportStream &xtream, CheckLog &log);

private:
	ExportMarkerList *priv_marker_list;		// Referenced
	hkTransform priv_transform;
	// Class (static) member
	static int priv_CLASS_SerialNumber;
};

typedef hkArray<ExportMarker *> ExportMarker_Vector;
typedef ExportMarker_Vector::const_iterator ExportMarker_VectorIterator;

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
