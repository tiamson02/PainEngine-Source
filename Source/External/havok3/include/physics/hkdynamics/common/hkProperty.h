/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_PROPERTY_H
#define HK_DYNAMICS2_PROPERTY_H

extern const hkClass hkPropertyValueClass;
extern const hkClass hkPropertyClass;

/// A union of an int and a hkReal, used for the value field of a property
struct hkPropertyValue
{
	HK_DECLARE_REFLECTION();

	hkUint64 m_data;

	inline hkPropertyValue( const int i );
	inline hkPropertyValue( const hkReal f );
	inline hkPropertyValue( void* p );
	inline hkPropertyValue( ) { }

	inline void setReal(const hkReal r);
	inline void setInt(const int i);
	inline void setPtr(void* p);

	inline hkReal getReal() const;
	inline int getInt() const;
	inline void* getPtr() const;
};

/// A property for an hkWorldObject. An hkProperty has a type and a value.
/// You can use properties to add additional information to an entity - for instance, 
/// for using your own collision filters or flagging certain types of game objects.
class hkProperty
{
	public:
		
		HK_DECLARE_REFLECTION();

	public:
			/// Default constructor - does nothing
		inline hkProperty();

			/// Create a property with a key and a hkUint32 value
		inline hkProperty( hkUint32 key, hkInt32 value );

			/// Create a property with a key and value
		inline hkProperty( hkUint32 key, hkPropertyValue value );

	public:

			///The property key.
		hkUint32 m_key;

			// Ensure m_value starts at 8 byte offset.
		hkUint32 m_alignmentPadding;

			///The property's value.
		struct hkPropertyValue m_value;

	public:

		hkProperty( class hkFinishLoadedObjectFlag flag ) { }

};

#include <hkdynamics/common/hkProperty.inl>

/// Properties between these values are used by havok demos and sample code.
#define HK_HAVOK_PROPERTIES_START 0x1000
#define HK_HAVOK_PROPERTIES_END 0x2000

// Here is a list of all the properties currently used by the demos and sample code.

// Multiple Worlds
#define HK_PROPERTY_ENTITY_REP_PHANTOM 0x1111

	// graphics bridge
#define HK_PROPERTY_DEBUG_DISPLAY_COLOR 0x1130

	// the geometry which overrides the default shape view.
	// Once the graphics engine has created the object,
	// the geometry is deleted and the property removed
#define HK_PROPERTY_OVERRIDE_DEBUG_DISPLAY_GEOMETRY 0x1131

	// The volume of a rigid body. This is only used by the fracture demo
#define HK_PROPERTY_RIGID_BODY_VOLUME 0x1132

	// The isFracturable property. Attached to bodies which are meant to be fractured.
	// This is only used by the fracture demo.
#define HK_PROPERTY_RIGID_BODY_IS_FRACTURABLE 0x1133

	// the id used for the graphics bridge. If this property is not preset, the address
	// of the collidable will used as an id
#define HK_PROPERTY_DEBUG_DISPLAY_ID 0x1134


// Half Stepping Utility
#define HK_PROPERTY_HALF_STEPPER_INDEX 0x1200

// Character controller
#define HK_PROPERTY_CHARACTER_PROXY 0x1300

#endif // HK_DYNAMICS2_PROPERTY_H

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
