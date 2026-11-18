/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_PROPERTY_H
#define HK_DYNAMICS2_PROPERTY_H


union hkPropertyValue
{
	void* asPtr;
	int asInt;
	hkReal asReal;
};

/// A property for an hkEntity. An hkProperty has a type and a value.
/// You can use properties to add additional information to an entity - for instance, 
/// for using your own collision filters or backlinks into your game.
class hkProperty
{
	public:

			///The property key.
		hkUint32 m_key;

			///The property's value.
		hkPropertyValue m_value;
};

/// Properties between these values are used by havok demos and sample code.
#define HK_HAVOK_PROPERTIES_START 0x1000
#define HK_HAVOK_PROPERTIES_END 0x2000

// Here is a list of all the properties currently used by the demos and sample code.

// Multiple Worlds
#define HK_PROPERTY_ENTITY_REP_PHANTOM 0x1111

// Half Stepping Utility
#define HK_PROPERTY_HALF_STEPPER_INDEX 0x1200

// Character controller
#define HK_PROPERTY_HAS_JUMP_PAD 0x1666
#define HK_PROPERTY_TARGET_LOCATION_X 0x1667
#define HK_PROPERTY_TARGET_LOCATION_Y 0x1668
#define HK_PROPERTY_TARGET_LOCATION_Z 0x1669


#endif // HK_DYNAMICS2_PROPERTY_H

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
