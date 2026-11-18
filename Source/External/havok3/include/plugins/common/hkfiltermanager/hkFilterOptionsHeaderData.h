/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HKFILTERMANAGER_HKFILTEROPTIONSHEADERDATA_HKCLASS_H
#define HKFILTERMANAGER_HKFILTEROPTIONSHEADERDATA_HKCLASS_H

/// hkFilterData meta information.
extern const class hkClass hkFilterDataClass;
/// hkConfigurationData meta information.
extern const class hkClass hkConfigurationDataClass;
/// hkConfigurationSetData meta information.
extern const class hkClass hkConfigurationSetDataClass;

/// Describes a filter.
class hkFilterData
{
public:

	HK_DECLARE_REFLECTION();

	// Struct holding default values

	struct DefaultStruct;

		/// Default constructor
	hkFilterData() { }

	//
	// Members
	//
public:
		/// The filter's unique id.
	unsigned m_id;
		/// The version of the filter.
	unsigned m_ver;
		/// Whether the filter has options or not.
	hkBool m_hasOptions;
};

/// Describes a configuration.
class hkConfigurationData
{
public:

	HK_DECLARE_REFLECTION();

	// Struct holding default values

	struct DefaultStruct;

		/// Default constructor
	hkConfigurationData() { }

	//
	// Members
	//
public:
		/// The name of the configuration.
	const char* m_configurationName;

		/// The number of filters in the configuration.
	int m_numFilters;
};

/// Describes a configuration set.
class hkConfigurationSetData
{
public:

	HK_DECLARE_REFLECTION();

	// Struct holding default values

	struct DefaultStruct;

		/// Default constructor
	hkConfigurationSetData() { }

	//
	// Members
	//
public:
		/// The filter manager version used to create the configuration set.
	unsigned m_filterManagerVersion;
		/// The configuration to display in the filter manager when the set is loaded.
	int m_activeConfiguration;
};

#endif // HKFILTERMANAGER_HKFILTEROPTIONSHEADERDATA_HKCLASS_H

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
