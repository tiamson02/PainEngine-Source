/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef INC_EXPORT_MARKER
#define INC_EXPORT_MARKER

#include <hkexport2/exportclass.h>
//#include <hkmath/transform.h>
#include <hkutilities/serialize/display/hkSerializedDisplayMarker.h>

class ExportMarkerList;

class ExportMarker: public ExportClass
{
public:
	// Constructor (empty)
	ExportMarker(ExportMarkerList *world);
	// Destructor
	~ExportMarker();

	// Overrides from ExportClass
	const char* ClassName() {return "Marker";}
	int   SerialNumber() {return priv_CLASS_SerialNumber;}

	// set/get of the information (tansform)
	void SetTransform (const hkTransform &transform) {priv_transform=transform;}
	const hkTransform& GetTransform() const {return priv_transform;}

	// Import
	void Import(NS_ExportStream::InExportStream &xtream, CheckLog &log);

	hkSerializedDisplayMarker* getHKMarker() const {return m_hkMarker;} 

private:

	ExportMarkerList *priv_marker_list;		// Referenced
	hkTransform priv_transform;
	// Class (static) member
	static int priv_CLASS_SerialNumber;

	hkSerializedDisplayMarker* m_hkMarker;
};

typedef hkArray<ExportMarker *> ExportMarker_Vector;
typedef ExportMarker_Vector::const_iterator ExportMarker_VectorIterator;

#endif

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
