/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkexport2/hkexport.h>
#include <hkexport2/exportmopprepository.h>

EXPORT_SINGLETON_IMPLEMENTATION( ExportPreCreatedMoppRepository );

ExportPreCreatedMoppRepository::~ExportPreCreatedMoppRepository()
{
	// Destruct
	clearStoredElements();
}

bool ExportPreCreatedMoppRepository::addPreCreatedMoppCode(hkMoppCode* preCreatedMoppCode,const char* exportPrimitiveName)
{
	int index = findName( exportPrimitiveName );
	if ( index != -1 )
	{
		return false;
	}

	char* newName = hkString::strDup( exportPrimitiveName );
	m_correspondingExportPrimitiveNames.pushBack( newName );
	m_moppCodes.pushBack( preCreatedMoppCode );
	preCreatedMoppCode->addReference();
	return true;
}

hkMoppCode* ExportPreCreatedMoppRepository::getPreCreatedMoppCode(const char* exportPrimitiveName) const
{
	int index = findName( exportPrimitiveName );
	if ( index == -1 )
	{
		return HK_NULL;
	}
	return m_moppCodes[ index ];
}

hkMoppCode* ExportPreCreatedMoppRepository::getPreCreatedMoppCode(int index) const
{
	if( ( index < 0 ) || ( index >= m_moppCodes.getSize() ) )
	{
		return HK_NULL;
	}
	return m_moppCodes[ index ];
}

const char* ExportPreCreatedMoppRepository::getPreCreatedMoppCodeName(int index) const
{
	if( ( index < 0 ) || ( index >= m_moppCodes.getSize() ) )
	{
		return HK_NULL;
	}
	char* theName = m_correspondingExportPrimitiveNames[ index ];
	return theName;
}

bool ExportPreCreatedMoppRepository::removePreCreatedMoppCode(const char* name)
{
	int index = findName( name );
	if ( index == -1 )
	{
		return false;
	}
	hkDeallocate( m_correspondingExportPrimitiveNames[index] );
	m_moppCodes[index]->removeReference();
	m_correspondingExportPrimitiveNames.removeAt( index );
	m_moppCodes.removeAt( index ); 
	return true;
}

int ExportPreCreatedMoppRepository::findName(const char* primitiveName) const
{
	int index = 0;
	for ( ; index < m_correspondingExportPrimitiveNames.getSize(); index++ )
	{
		if ( !hkString::strCmp( m_correspondingExportPrimitiveNames[index], primitiveName ) )
		{
			// name found
			return index;
		}
	}
	return -1;
}

void ExportPreCreatedMoppRepository::clearStoredElements()
{
	int numElements = m_moppCodes.getSize();
	for ( int it = 0; it < numElements; it ++ )
	{
		hkDeallocate( m_correspondingExportPrimitiveNames[it] );
		m_moppCodes[it]->removeReference();
	}
	m_correspondingExportPrimitiveNames.clearAndDeallocate();
	m_moppCodes.clearAndDeallocate();
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
