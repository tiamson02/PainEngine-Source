/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_GRAPHICS_FACE_SET_H
#define HK_GRAPHICS_FACE_SET_H

#include <hkbase/types/hkBaseTypes.h>
#include <hkgraphics/hkgObject.h>
#include <hkgraphics/hkgFaceSetPrimitive.h>

class hkgVertexSet;
class hkgRay;
struct hkgFaceSetPickData;

typedef int HKG_FACE_SET_STATUS_FLAGS;
#define HKG_FACE_SET_UNOPTIMIZED	0
#define HKG_FACE_SET_STRIPPED		1<<1
#define HKG_FACE_SET_REORDERED		1<<2
#define HKG_FACE_SET_PLATFORM_OPT	1<<3

typedef unsigned short HKG_FACE_SET_STRIP_OPTIONS;
#define HKG_FACE_SET_STRIP_DEFAULT 0 // normal, no post T&L cache optimization
#define	HKG_FACE_SET_STRIP_CACHE   1 // strip, assuming there is a post T&L cache
#define	HKG_FACE_SET_STRIP_CACHE24 3 // strip, assuming there is a 24 entry post T&L cache (GeForce3 etc)

/// The face set is a object that holds a set of vertices and a corresponding collection of primitives
/// that use those vertices. A primitive is a single rendering primitive, such as a triangle strip or list.
/// The vertices get placed in a vertex array on most plaforms and the primitives index into that array.
/// The face set exposes methods to optimize (such as NvTriStrip) the underlying triangle data.
class hkgFaceSet : public hkgReferencedObject
{
	public:
			/// Create a platform specific holder for the given context. The context is
			/// needed as some platforms require the context to create the internal arrays. 
		static hkgFaceSet* (*create)(hkgDisplayContext* state);

		static hkgFaceSet* createFaceSet(hkgDisplayContext* dc) { return new hkgFaceSet(dc); }
		
			/// Get the const pointer to the vertex set. The vertex set can be though of as a vertex array.
		inline const hkgVertexSet* getVertices() const;
		
			/// Get the non-const pointer to the vertex set. The vertex set can be though of as a vertex array.
		inline hkgVertexSet* getVertices();

			/// Add a new primitive to the face set. The face set will claim owner ship of the primitive and 
			/// so you should not delete the primitive explicitly yourself. This will be changed in future to 
			/// be referenced like most of the other parts of the graphics library, It also means that you must 
			/// create a new primitive object for every primitive you add.
			/// A primitive repesents a triangle connectivity information for a single renderable list or strip.
		inline void addPrimitive(hkgFaceSetPrimitive* p); 

			/// Get the number of primitives in the set.
		inline int getNumPrimitives() const;

			/// Get the const pointer to the i-th primitive in the set.
		inline const hkgFaceSetPrimitive* getPrimitive(int i) const;

			/// This strip method will recreate new primitives after computing stripping 
			/// information. Note that this will only split Tri List prims into one or 
			/// more strips and possibly leave a tri list with
			/// leftovers. It will leave stripped prims as is. This will alter the 
			/// order and number of primitives. Will use the Nvidia stripper by 
			/// default, so the options reflect the nvidia cache optimizations for 16 or
			/// 24 line caches (or none).
		void strip(HKG_FACE_SET_STRIP_OPTIONS opts, bool stitch = true, int minStripLen = 0);
		
			/// Reodering the vertex set will mean better locality for the indices
			/// It will depends that the current prim order and will alter the index 
			/// arrays in each prim to match the new order.
		void reorderVertexSet();

			/// Tesselate the triangles 'iteration' times. Each iteration will add 4 triangles per face. 
			/// Not implimented in this release.
		void tesselate( unsigned int iterations );

			/// Ray cast intersection with the object. Returns the number of triangles hit (will be zero if no hot).
			/// Only check the data if the hits are greater than one. The ray should be in the same space as the face vertices.
			/// See the hkgFaceSetPickData struct for more information on the hit data.
		unsigned int intersect( const hkgRay& ray, bool ignoreBackfaces, hkgFaceSetPickData& data) const;
	
			/// MinE and MaxE will be assumed to be initialized to values already
			/// to optimize group AABB computation. If this is for a single
			/// query, minE should be FLT_MAX, and maxE should be -FLT_MAX
		void computeAABB(float minE[3], float maxE[3]) const; 

			/// Initialize all internal data required after setting up the vertex information and primitives. 
			/// On some platforms this may convert the underling vertex representation to get rid of indexing for 
			/// example in the case of PS2GL that does not support indexed vertex arrays for example. So do not 
			/// expect the data to be exactly the same after initialization.
		virtual void initialize();

			/// Render the primitives to the current viewport in the context. Will set the vertex arrays
			/// to be current and render all primitive parts.
		virtual void render(hkgDisplayContext* context);

			/// Construct a 2D grid. It will assume an empty face set.
			/// A grid is a planar collection of triangles, represented as one large triangle strip. It will
			/// extend from the 'corner' with 'sideA' as one vector and 'sideB' as the other vector representing
			/// the two edges that start at 'corner'. The tesselation is specified as 'resA' along A and 'resB' along B. 
			/// If you want vertex colors or texture coordinates in the arrays, specifiy as such in the last two args.
		void makeGrid( const float corner[3], const float sideA[3], const float sideB[3], int resA = 1, int resB = 1, bool uv = true, bool vcolor = false);

			/// Construct a 3D box. It will assume an empty face set.
			/// A box will be made with one corner as 'corner', with each of the three 'side' vectors as the three
			/// edges that connect that point. The winding will be affected by the order of the 3 sides. The tesselation
			/// along the three edges is specified with the three 'res' arguments. If you want vertex colors or texture
			/// coordinates, specify so with the last 2 args.
		void makeBox( const float corner[3], const float sideA[3], const float sideB[3], const float sideC[3], int resA = 1, int resB = 1, int resC = 1, bool uv = true, bool vcolor = false);
		
			/// Construct a Sphere. It will assume an empty face set.	
			/// A sphere centerd at 'center' with given 'radius', and tesselation in along the major and minor directions are specified.
			/// If you want vertex colors or texture coordinates, specify so with the last 2 args.
		void makeSphere( const float center[3], float radius, int numMajor = 8, int numMinor = 8, bool uv = true, bool vcolor = false);

			/// Construct a Cone (or Cylinder if the radius both ends). It will assume an empty face set.
			/// Starting at 'from', the vector to 'to' specifies the axis of the shape, whoch will have 'radius0' radius at the 'from' end
			/// and 'radius1' radius at the to end. Radius can be negative. The major and minor tesselation parameters are specified next and also 
			/// whether to build caps at each end (if radius is not zero of course). If you want vertex colors or texture coordinates, specify so 
			/// with the last 2 args. The main body of the cone/cylinder is one triangle strip and the caps (if any) are tri lists.
		void makeCone( const float from[3], const float to[3], float radius0, float radius1, int numMajor = 8, int numMinor = 8, bool cap = true, bool uv = true, bool vcolor = false);

			/// Print general information about this face set to the give stream. It will assume
			/// that the stream is text based.
		void print(hkOstream& stream) const;

			/// Collapse will reduce all primitives with the same type to a single primitive.
			/// After this call you will have at most two primituives: one strip and one tri list.
		void collapse();
		
			/// Transform all the vertices held in this face set by a 4x4 matrix
			/// It will lock the verts, transform, and then unlock. This is used to bake transforms
			/// down to static face sets so that they do not need to be transformed the same 
			/// each frame.
		void transform( const float t[16] );

	protected:

		hkgFaceSet(hkgDisplayContext* state);
		virtual ~hkgFaceSet();

		void removeIndexArrays();

		// vertices
		hkgVertexSet*	m_vertices;

		// primitives
		hkArray<hkgFaceSetPrimitive*> m_primitives;

		HKG_FACE_SET_STATUS_FLAGS    m_flags;
		
};

#include <hkgraphics/hkgFaceSet.inl>

#endif //HK_GRAPHICS_FACE_SET_H

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
