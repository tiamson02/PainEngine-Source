/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_GRAPHICS_VERTEX_SET
#define HK_GRAPHICS_VERTEX_SET

#include <hkbase/types/hkBaseTypes.h>
#include <hkgraphics/hkgDisplayContext.h>

typedef unsigned short HKG_VERTEXSET_FORMAT;
#define HKG_VERTEXSET_NONE   0
#define HKG_VERTEXSET_POS    1 // has pos
#define HKG_VERTEXSET_COLOR  2 // has color
#define HKG_VERTEXSET_UV     4 // has tex coords
#define HKG_VERTEXSET_NORMAL 8 // has normal

class hkgFaceSet;


/// A vertex set is basically a wrapper for a pure float vertex array. It may split internally into multiple
/// arrays (one for each component) or have a strided, interleaved, internal format. A vertex
/// set is not reference counted as it is assumed that a face set is the only user.
class hkgVertexSet
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_DISPLAY, hkgVertexSet );

			/// Create a platform specific vertex set for the given face set owner.
		static hkgVertexSet* (*create)(hkgFaceSet* owner); 

			/// Delete the vertex set. This will release all held resources.
		virtual ~hkgVertexSet(); 

			/// Set the number of vertices and which optional components to have. 
			/// Normals are assumed, but this will be changed to be an option in future too.
			/// This will reallocate any held buffers and does not preserve data. If you set the 
			/// numVerts to 0 then it will deallocate all held resources.
		virtual void setNumVerts(int nv, bool uvs = true, bool colors = true) = 0;

			/// Get the number of vertices represented by the vertex set.
		inline int getNumVerts() const;

			/// Lock the vertex set so that you can alter its values to read
			/// consistent data from the buffers. It locks from 'offset' len vertices.
			/// If len is -1, the lock will be to the end of the buffers and negative offset is
			/// not allowed.
		virtual unsigned char* lock(int offset = 0, int len = -1) = 0;

			/// Unlock the given range, from 'offset' of 'len' vertices. This should
			/// match what you previously locked. Call this once you no longer require
			/// write access or consistent read access.
		virtual void unlock(int offset = 0, int len = -1) = 0; 

			/// Get the distance, or stride, between each vertex component. There are
			/// 4 bytes in a float, so if stride is 12 then the vertex array is 
			/// not interleaved, but all components are split internally so lock()
			/// returns the pure positional vertex array. In DirectX for example, a full format such as:
			/// 
			/// vert{
			///    float x,y,z;
			///    float nx,ny,nz;
			///	   float u,v;
			///    unsigned color;
			/// }
			///    which would have a stride of 8*float + 4 = 36 bytes.
			/// In this case, lock() returns the interleaved buffer of all components
			/// such that buf[i*stride] is the ith vert as above (take out u.v / color as required).
		inline int getStrideInBytes();

			/// Set the client pointers / stream pointers to this data. Passed the current 
			/// enabled streams so that it can match what it needs. This will effectivly get the
			/// hardware to use this vertex set as the current vertex buffers.
		virtual void setStreams(hkgDisplayContext* state) = 0; 
		
			/// Add 'numPos' vert positions (3 floats each) from the given posInfo. Do not add more positions than
			/// the vertex set has vertices.
		virtual void addPosInfo(const float* posInfo, int numPos) = 0;

			/// Add 'numNormals' vert normals (3 floats each) from the given normalInfo. Do not add more normals than
			/// the vertex set has vertices.
		virtual void addNormalInfo(const float* normalInfo, int numNormals) = 0;

			/// Add 'numUVs' vert texture coords (2 floats) from the given uvInfo. Do not add more coord pairs than
			/// the vertex set has vertices.
		virtual void addUVInfo(const float* uvInfo, int numUVs) = 0; 

			/// Add 'numColors' vert colors (3 floats) from the given colorInfo. Do not add more colors than
			/// the vertex set has vertices. This may be converted internally to a packed format.
		virtual void addColorInfo(const float* colorInfo, int numColors) = 0;
		
			/// Get the i-th vertex postion. You must lock() the buffers before
			/// calling this, and using the returned pointer to access other
			/// the 3 floats returned is unsafe.
		virtual float* getPosInfo( int i ) = 0;

			/// Get the i-th vertex normal. You must lock() the buffers before
			/// calling this, and using the returned pointer to access other
			/// the 3 floats returned is unsafe.
		virtual float* getNormalInfo( int i ) = 0;

			/// Get the i-th vertex uv coord. You must lock() the buffers before
			/// calling this, and using the returned pointer to access other
			/// the 2 floats returned is unsafe, and it will return HK_NULL if there
			/// are no vertex texture coordinates enabled in this set.
		virtual float* getUVInfo( int i ) = 0;

			/// Get the i-th vertex color. You must lock() the buffers before
			/// calling this, and using the returned pointer to access other
			/// the 3 floats returned is unsafe, and it will return HK_NULL if there
			/// are no vertex colors are enabled in this set. This is not implimented
			/// correctly on all platforms yet.
		virtual float* getColorInfo( int i ) = 0;

			/// Internal use. Do not call unless you know what you are doing!
		inline void setPosAddCounter(int c);
		
			/// Internal use. Do not call unless you know what you are doing!
		inline void setUVAddCounter(int c);
		
			/// Internal use. Do not call unless you know what you are doing!
		inline void setColorAddCounter(int c);
		
			/// Internal use. Do not call unless you know what you are doing!
		inline void setNormalAddCounter(int c);

			/// Internal use. Do not call unless you know what you are doing!
		inline int getPosAddCounter();

			/// Internal use. Do not call unless you know what you are doing!
		inline int getUVAddCounter();
			
			/// Internal use. Do not call unless you know what you are doing!
		inline int getColorAddCounter();
		
			/// Internal use. Do not call unless you know what you are doing!
		inline int getNormalAddCounter();

			/// Internal use. Do not call unless you know what you are doing!
		inline void resetCounters();

			/// Get vertex format for the vertex set. The return type is
			/// a bit field of enabled components.
			/// The components are:
			///
			///   HKG_VERTEXSET_POS    
			///   HKG_VERTEXSET_COLOR  
			///   HKG_VERTEXSET_UV     
			///   HKG_VERTEXSET_NORMAL 
		inline HKG_VERTEXSET_FORMAT getVertexFormat() const;

	protected:

		inline hkgVertexSet(hkgFaceSet* owner);
		
		hkgFaceSet*		m_owner;

		// counters
		int	m_numVerts;
		int	m_stride; 
		HKG_VERTEXSET_FORMAT	m_vertexFormat;

		unsigned char*	m_lockedBuf;

		int	m_lastAddedPosIndex;
		int	m_lastAddedUVIndex;
		int	m_lastAddedColorIndex;
		int	m_lastAddedNormalIndex;
};

#include <hkgraphics/hkgVertexSet.inl>

#endif //HK_GRAPHICS_VERTEX_SET

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
