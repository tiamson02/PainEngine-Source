/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_GRAPHICS_MAT_FACE_SET_H
#define HK_GRAPHICS_MAT_FACE_SET_H

#include <hkbase/types/hkBaseTypes.h>
#include <hkgraphics/hkgFaceSet.h>
#include <hkgraphics/hkgMaterial.h>

/// A Material FaceSet is a collection of FaceSets that share the same material
/// and is just a simple container class with no platform specific parts.
class hkgMaterialFaceSet : public hkgReferencedObject
{
	public:

			/// Create a hkgMaterialFaceSet. This will be the same object on all platforms.
		static hkgMaterialFaceSet* (*create)();

		static hkgMaterialFaceSet* createMaterialFaceSet() { return new hkgMaterialFaceSet(); }

			/// Add a face set to this collection. It will reference count the face set.
			/// A face set canbe added to multiple different material face sets.
		inline void addFaceSet(hkgFaceSet* faceSet); 
		
			/// Reserve enough array space for the given estimate. It will set the capacity 
			/// of the internal array to be this value.
		inline void reserve( int estimateNum );

			/// Get the number of face sets in this object. 
		inline int getNumFaceSets() const;

			/// Get the const pointer to the i-th face set.
		inline const hkgFaceSet* getFaceSet(int i) const;

			/// Get the pointer to the i-th face set.
		inline hkgFaceSet* getFaceSet(int i);

			/// Get the const pointer to the material for this collection of face sets. Can be HK_NULL.
		inline const hkgMaterial* getMaterial() const;

			/// Get the material for this collection of face sets. Can be HK_NULL.
		inline hkgMaterial* getMaterial();

			/// Set the material for this collection of face sets. Can be HK_NULL. It will be reference counted.
		inline void setMaterial( hkgMaterial* m );

			/// Optimize all the face sets in this collection. At the moment this will call strip() on
			/// each face set and then call reorderVertexSet to match the strip order. It may do more 
			/// optimizations in future.
		void optimize();  

			/// Set the material as current and render all face sets in order to the current viewport in the
			/// given context.
		void render(hkgDisplayContext* context) const;

			/// Print the material fact set information to the given stream, It will assume the stream is text based.
			/// It will recurse down to all contained face sets.
		void print(hkOstream& stream) const;

			/// Bake the face sets together. Will effectivly bake all vertex sets that have the same vertex format (color,uv)
			/// into a single face sets, so depending on how alike the contained vertex information is this 
			/// call could result in just on face set with all the triangles in it.
		void bake(hkgDisplayContext* context);

	protected:

		inline hkgMaterialFaceSet();
		virtual ~hkgMaterialFaceSet();
		
		hkgMaterial*		  m_material;
		hkArray<hkgFaceSet*>  m_faceSets;
};

#include <hkgraphics/hkgMaterialFaceSet.inl>

#endif // HK_GRAPHICS_MAT_FACE_SET_H

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
