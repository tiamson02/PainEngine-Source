/* 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef INC_H2DLL_PROPERTIES
#define INC_H2DLL_PROPERTIES

class hkPropertyDB
{
public:

	// Constructor, prepares the DB
	hkPropertyDB();

	// Destructor, destroys all entries
	~hkPropertyDB();

	// Sets a property 
	bool setProperty (const char* prop, int value);
	// Gets a property
	bool getProperty (const char* prop, int &retvalue);
	
	// Utility methods - they assert if property is undefined
	bool getPropertyBool (const char* prop);
	int	 getPropertyInt (const char* prop);

private:

	struct hkPropertyValue;

		// Looks for an existing property, returns NULL if not found;
	hkPropertyValue* findProperty (const char* prop);


	#define MAXPROPERTIES 50
	int m_numProperties;
	hkPropertyValue* m_properties[MAXPROPERTIES];
};

hkPropertyDB& getPropertyDB();

#endif // INC_H2DLL_PROPERTIES

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
