/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_GRAPHICS_PICK_DATA
#define HK_GRAPHICS_PICK_DATA

// forward decl.
class hkgDisplayWorld;

/// hkgFaceSetPickData, returned by hkgFaceSet::intersect().
struct hkgFaceSetPickData
{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_DISPLAY, hkgFaceSetPickData );

		/// The distance along ray of triangle hit point. 
		/// Use  (dir*m_dist + from)  for world point
		float m_dist; 

		/// The index of the primitive in the hkgFaceSet in which traingle presides that was hit.
		int m_primitiveIndex; 
		
		/// The index in the primitive of the triangle (i-th triangle, not i-th connectivity value)
		int m_primitiveTriIndex; 

		/// The 3 indices that make up the triangle (so that you don't have to walk the above information
		/// if you don't want to). Index into the vertex set with these values to get the vertex positions.
		int m_verts[3]; 
};


/// hkgGeometryPickData, returned by hkgGeometry::intersect().
struct hkgGeometryPickData
{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_DISPLAY, hkgGeometryPickData );

		/// The hkgFaceSetPickData	that came from the sucessfull hkgFaceSet::intersect call.
		hkgFaceSetPickData	m_facePickData;

		/// The index of the material face set that held the hkgFaceSet that returned the closest hit.
		int		m_matFaceSetIndex;		
	
		/// The index in the material facet set of the actual hkgFaceSet, used in conjucttion with the above index.
		int		m_matFaceSetSubIndex;	
};


/// hkgDisplayObjectPickData, returned by hkgDisplayObject::intersect().
struct hkgDisplayObjectPickData
{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_DISPLAY, hkgDisplayObjectPickData );

		/// The hkgGeometryPickData  that came from the successful hkgGeometry::intersect call.
		hkgGeometryPickData	m_geomPickData;	
		
		/// The index of the geometry that returned the closest hit.
		int		m_geomIndex;
};

// hkgViewportPickData, returned from hkgViewport::pick().
struct hkgViewportPickData
{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_DISPLAY, hkgViewportPickData );

		/// The hkgDisplayObjectPickData  that came from the hkgDisplayObject::intersect() for the 
		/// hkgDisplayObject that was closest hit by the ray in the hkgDisplayWorld given to the pick()
		/// method. The display world is stored below for reference too.
		hkgDisplayObjectPickData	m_objectPickData;	

		/// The index into the hkgDisplayWorld of the sucessful hit object.
		int							m_objectIndex;	

		/// The world position of the hit object. This is hit point from the hkgFaceSet transformed
		/// by the hkgDisplayObject.m_transform if one.
		float						m_worldPos[3];

		/// The pointer to the world that was used to query for intersection. Stored here for
		/// convience.
		hkgDisplayWorld*			m_world;

		/// The total number of triangles that where sucessfully hit during the test for the closest hit with the ray.
		int							m_trianglesHit; 
};

#endif // HK_GRAPHICS_PICK_DATA

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
