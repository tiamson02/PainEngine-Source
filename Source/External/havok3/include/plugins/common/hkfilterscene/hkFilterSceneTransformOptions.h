/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HKFILTERSCENE_HKFILTERSCENETRANSFORMOPTIONS_HKCLASS_H
#define HKFILTERSCENE_HKFILTERSCENETRANSFORMOPTIONS_HKCLASS_H

/// hkFilterSceneTransformOptions meta information
extern const class hkClass hkFilterSceneTransformOptionsClass;

/// Describes delta compression settings
class hkFilterSceneTransformOptions
{
	public:
	
		HK_DECLARE_REFLECTION();

			/// 
		enum Preset
		{
				/// 
			IDENTITY,
				/// 
			MIRROR_X,
				/// 
			MIRROR_Y,
				/// 
			MIRROR_Z,
				/// 
			SCALE_FEET_TO_METERS,
				/// 
			SCALE_INCHES_TO_METERS,
				/// 
			SCALE_CMS_TO_METERS,
				/// 
			CUSTOM,
				/// 
			PRESET_MAX_ID
		};
		
			/// Default constructor
		hkFilterSceneTransformOptions() { }
		
		//
		// Members
		//
	public:
		
			/// Chosen transformation preset
		hkEnum<enum Preset, hkInt8> m_preset; //+default(hkFilterSceneTransformOptions::IDENTITY)
		
			/// Should we apply this transformation to scene graph nodes
		hkBool m_applyToNodes; //+default(true)
		
			/// Should we apply this transformation to vertex buffers
		hkBool m_applyToBuffers; //+default(true)
		
			/// Should we apply this transformation to lights
		hkBool m_applyToLights; //+default(true)
		
			/// Should we apply this transformation to cameras
		hkBool m_applyToCameras; //+default(true)
		
			/// Should we flip index buffer winding
		hkBool m_flipWinding; //+default(false)
		
			/// The transform to apply
		hkMatrix4 m_matrix;
};

#endif // HKFILTERSCENE_HKFILTERSCENETRANSFORMOPTIONS_HKCLASS_H

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
