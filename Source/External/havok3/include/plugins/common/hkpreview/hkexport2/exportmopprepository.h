/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef INC_EXPORT_MOPPREPOSITORY_H
#define INC_EXPORT_MOPPREPOSITORY_H

#include <hkexport2/exportclass.h>
#include <hkinternal/collide/mopp/code/hkMoppCode.h>

// exportGeometricPrimitive checks the repository for all primitives it tries to import in case of 
// pre created mopp code. If it finds a corresponding mopp code it uses it to make a MoppBVTreeShape
// and removes it from the repository. This removal is to avoid the possibility of unused memory being 
// stored in the repository until singletons are deleted. 

class ExportPreCreatedMoppRepository: public ExportSingleton<ExportPreCreatedMoppRepository>
{
public:
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_EXPORT);
	ExportPreCreatedMoppRepository(){}  //should only be called once
	~ExportPreCreatedMoppRepository();

	// adds the mopp code and name to the arrays and adds a reference to the mopp code
	// returns true if the name wasn't used before and if the mopp data was added
	hkBool addPreCreatedMoppCode( hkMoppCode* preCreatedMoppCode, const char* exportPrimitiveName );

	// retrieves the relevant stored mopp code or HK_NULL if not added or invalid index
	hkMoppCode* getPreCreatedMoppCode( const char* exportPrimitiveName ) const;
	hkMoppCode* getPreCreatedMoppCode( int index ) const;
	// retrieves the relevant name or HK_NULL if invalid index
	const char* getPreCreatedMoppCodeName( int index ) const;
	
	// removes the pre created mopp code and corresponding name from the repository 
	// and removes a reference from the mopp code returning false if an invalid index or name was passed
	hkBool removePreCreatedMoppCode( const char* name );

	int getNumStoredMoppCodes() const { return m_moppCodes.getSize();}

	// removes references from stored mopp codes and deletes stored names clearing the arrays
	void clearStoredElements();

private:
	hkArray<hkMoppCode*> m_moppCodes;
	hkArray<char*> m_correspondingExportPrimitiveNames;

	// searches the names for the given name returning the index if found and -1 if not
	int findName( const char* primitiveName ) const;
};

#endif //INC_EXPORT_MOPPREPOSITORY_H

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
