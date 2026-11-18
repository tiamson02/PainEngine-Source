/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_GRAPHICS_MATERIAL_H
#define HK_GRAPHICS_MATERIAL_H

#include <hkbase/types/hkBaseTypes.h>
#include <hkgraphics/hkgTexture.h>

typedef unsigned int HKG_MATERIAL_TRANS_SOURCE;
#define HKG_MATERIAL_TRANS_SOURCE_NONE    0  
#define HKG_MATERIAL_TRANS_SOURCE_COLOR   1
#define HKG_MATERIAL_TRANS_SOURCE_TEXTURE 2
#define HKG_MATERIAL_TRANS_SOURCE_BOTH    ( HKG_MATERIAL_TRANS_SOURCE_TEXTURE | HKG_MATERIAL_TRANS_SOURCE_COLOR ) 

/// A materail has both color (diffuse and specular, although specular is not used at the moment) and 
/// texture (which may be null).
class hkgMaterial : public hkgReferencedObject
{
	public:
			/// Create a platform specific material.
		static hkgMaterial* (*create)();

			/// Set the texture for the material. Will reference count the texture. This may be set to HK_NULL.
		inline void setTexture(hkgTexture* t); 

			/// Get the const pointer to the texture for the material. This may be HK_NULL.
		inline const hkgTexture* getTexture() const;

			/// Get the pointer to the texture for the material. This may be HK_NULL.
		inline hkgTexture* getTexture(); 

			/// Set the diffuse color for material. The values are in the range 0-->1 and the alpha
			/// value defaults to 1.0, setting it to less than that will cause the material
			/// to be translucent.
		inline void setDiffuseColor( float r, float g, float b, float a = 1.0f );
		
			/// Get the 4 color values (RGBA) for the material.
		inline void getDiffuseColor( float& r, float& g, float& b, float &a ) const;

			/// Set the specular color components for the material. Alpha is not a specular part. 
		inline void setSpecularColor( float r, float g, float b );

			/// Get the specular color components for the material. Alpha is not a specular part.
		inline void getSpecularColor( float& r, float& g, float& b ) const;

			/// Get the void* user data, which can be used for tagging (with a source material pointer for instance).
		inline void* getUserData() const;

			/// Set the void* user data, which can be used for tagging (with a source material pointer for instance).
		inline void setUserData(void* t);

			/// Bind this material as the current material (including texture) in the given context. This may change state.
		virtual void bind(hkgDisplayContext* context) const; 

			/// Unbind the current material. Notice that this is static as it is just a state change and
			/// will set the material back to a null state (ie, texture stage 0 to NULL, disable textures)
		static void unbind(hkgDisplayContext* context); 

			/// Is the material transparent? A material is transpaarent if the texture is transparent
			/// or the diffuse color has non 1.0 alpha.
		inline bool isTransparent() const;

			/// Get the source for the material's transparency so that we can figure out such things as when 
			/// the textures are disabled, is the material still transparent: ie, is the transparent source
			/// the diffuse color?
		inline HKG_MATERIAL_TRANS_SOURCE getTransparentSource() const;

			/// Print trhe material information to the given stream. It will assume the stream is text based.
		void print(hkOstream& stream) const;

			/// Is this material the same as the given other material? It will check for colors with tolerance 
			/// of each other and texture pointers being the same.
		inline bool operator==( const hkgMaterial& other ) const;

	protected:

		inline hkgMaterial();
		virtual ~hkgMaterial();

		hkgTexture*  m_texture;
		
		bool	m_isTransparent; // either the diffuse is transparent or the texture has alpha.
		float	m_diffuse[4];
		float	m_specular[3];
		
		void*   m_userData;

};

#include <hkgraphics/hkgMaterial.inl>

#endif //  HK_GRAPHICS_MATERIAL_H

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
