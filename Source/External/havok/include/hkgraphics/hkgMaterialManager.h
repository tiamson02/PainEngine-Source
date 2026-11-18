/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_GRAPHICS_MATERIAL_MANAGER
#define HK_GRAPHICS_MATERIAL_MANAGER

#include <hkbase/types/hkBaseTypes.h>
#include <hkgraphics/hkgMaterial.h>

/// The material manager is a container for all the materials used. An application does 
/// not have to use the manager, but it could help in texture management in future.
/// It is cross platform and has no platform specific parts at this time.
class hkgMaterialManager : public hkgReferencedObject
{
	public:
		
			/// Create a hkgMaterialManager. This will return the 
			/// same object on all platforms.
		static hkgMaterialManager* (*create)(hkgDisplayContext* context);

		static hkgMaterialManager* createMaterialManager(hkgDisplayContext* context)
		{
			return new hkgMaterialManager(context);
		}

			/// Is the i-th material textured (does it have a valid texture pointer)?
		inline bool materialIsTextured( int i ) const;
		
			/// Get the number of materials in this manager.
		inline int getNumMaterials() const;

			/// Get the i-th hkgMaterial in this manager. 'i' must be within range,
		inline hkgMaterial* getMaterial(int i);

			/// Get the const pointer to the i-th hkgMaterial in this manager. 'i' must be within range,
		inline const hkgMaterial* getMaterial(int i) const;

			/// It can be handy to just have some default, untextured, materials that
			/// get setup for use on random objects so at least they are not all the same
			/// color. By default there are 7 colors, which in ARGB format are:
			/// 	0xffff3333   
			/// 	0xff33ff33   
			/// 	0xff3333ff   
			/// 	0xffff33ff   
			/// 	0xffffff33
			/// 	0xff33ffff
			/// 	0xffffffff
		inline int getNumDefaultMaterials() const;

			/// Get the const pointer to the i-th default material. 'i' must be within range.
			/// See getNumDefaultMaterials() for that range and also the list of default 
			/// colors currently setup.
		inline const hkgMaterial* getDefaultMaterial(int i) const;

			/// Get the i-th default material. 'i' must be within range.
			/// See getNumDefaultMaterials() for that range and also the list of default 
			/// colors currently setup.	
		inline hkgMaterial* getDefaultMaterial(int i);

			/// Add a material to the manager. This will increment the reference count on the material.
			/// You can add the same material to any number of managers, although there is usual only 
			/// one manager.
		inline int addMaterial(hkgMaterial* m);

			/// Realize all textures in hardware. It will simply call realize() on all meterial textures
			/// it finds. See hkgTexture::realize() for more information.
		void realizeTextures(hkgDisplayContext* context); 

			/// Clear the list of materials (and release the reference to them).
		inline void clearMaterials();

			/// Print the material information to the given stream. It will assume the stream is text based.
		void print(hkOstream& stream) const;

	protected:

		inline void clearDefaultMaterials();
		void createDefaultMaterials(hkgDisplayContext* context);

		inline hkgMaterialManager(hkgDisplayContext* context);
		virtual ~hkgMaterialManager();

		hkArray<hkgMaterial*> m_materials;
		hkArray<hkgMaterial*> m_defMaterials;

};

#include <hkgraphics/hkgMaterialManager.inl>

#endif // HK_GRAPHICS_MATERIAL_MANAGER

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
