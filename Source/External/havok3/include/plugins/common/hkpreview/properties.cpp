/* 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkpreview/properties.h>
#include <hkbase/hkBase.h>
#include <string.h>
#include <malloc.h>


// Single instance of the property DB
static hkPropertyDB gPropertyDB;
hkPropertyDB& getPropertyDB() {return gPropertyDB;}


// Very simple implementation
struct hkPropertyDB::hkPropertyValue
{
	hkPropertyValue (const char* prop, int value)
	{
		m_property = _strdup(prop);
		m_value = value;
	}
	~hkPropertyValue ()
	{
		free (m_property);
	}
	
	char* m_property;
	int m_value;
};


hkPropertyDB::hkPropertyDB()
{
	m_numProperties = 0;
}

hkPropertyDB::~hkPropertyDB()
{
	for (int i=0; i< m_numProperties; i++)
	{
		hkPropertyValue* current = m_properties[i];
		delete current;
	}
}

// Looks for an existing property, returns NULL if not found;
 hkPropertyDB::hkPropertyValue* hkPropertyDB::findProperty (const char* prop)
{
	for (int i=0; i< m_numProperties; i++)
	{
		hkPropertyValue* current = m_properties[i];
		if (! strcmp(current->m_property, prop) )
		{
			return current;
		}
	}

	return 0;
}

// Sets a global property to the given value
bool hkPropertyDB::setProperty (const char* prop, int value)
{
	hkPropertyValue* existing = findProperty (prop);
	if (existing)
	{
		existing->m_value = value;
		return true;
	}
	else
	{
		HK_ASSERT2(0x397fd9f9, m_numProperties<MAXPROPERTIES, "Too many properties stored!");

		hkPropertyValue* newProp = new hkPropertyValue (prop, value);
		m_properties[m_numProperties++] = newProp;	
		return false;
	}
}

// Retrieves the value for a property
bool hkPropertyDB::getProperty (const char* prop, int &retvalue)
{
	hkPropertyValue* existing = findProperty (prop);
	if (existing)
	{
		retvalue = existing->m_value;
		return true;
	}
	else
	{
		return false;
	}
}

// Utility methods - they assert if property is undefined
bool hkPropertyDB::getPropertyBool (const char* prop)
{
	int intResult = 0;
	HK_ON_DEBUG( bool found = ) getProperty (prop, intResult);
	HK_ASSERT2(0x43947591, found, "Property Undefined");
	return intResult!=0;
}

int	 hkPropertyDB::getPropertyInt (const char* prop)
{
	int intResult = 0;
	HK_ON_DEBUG( bool found = ) getProperty (prop, intResult);
	HK_ASSERT2(0x43947591, found, "Property Undefined");
	return intResult;
}

// DLL METHODs, just forward to class instance
bool setGlobalProperty (const char* prop, int value)
{
	return getPropertyDB().setProperty(prop, value);
}

// DLL METHODs, just forward to class instance
bool getGlobalProperty (const char* prop, int &retvalue)
{
	return getPropertyDB().getProperty(prop, retvalue);
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
