/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_COLLIDE2_COLLIDE_TRIANGLEUTIL_H
#define HK_COLLIDE2_COLLIDE_TRIANGLEUTIL_H

/// Triangle utilities, including ray intersection, normal calculation, and point-in-triangle tests. 
class hkCollideTriangleUtil
{
	public:

		static void HK_CALL calcBarycentricCoordinates(const hkVector4& pos, const hkVector4& t0, const hkVector4& t1, const hkVector4& t2, hkReal result[3]);




		/*
		**	EDGE EDGE
		*/

		enum {		CLSLS_POINTA_START = 1,		CLSLS_POINTA_END   = 2,		CLSLS_POINTB_START = 4,		CLSLS_POINTB_END   = 8		};

		struct ClosestLineSegLineSegResult
		{
			hkReal	  m_distance;
		};

		/// returns (see enum): if bit is set, point can be deleted
		static int HK_CALL closestLineSegLineSeg( const hkVector4& A, const hkVector4& dA, const hkVector4& B, const hkVector4& dB, ClosestLineSegLineSegResult* optResult );





		/*
		**	POINT EDGE
		*/

		struct ClosestPointLineSegResult
		{
			hkVector4 m_pointOnEdge;
			hkReal	  m_distance;
		};

		static int HK_CALL closestPointLineSeg( const hkVector4& A, const hkVector4& B0, const hkVector4& B1, ClosestPointLineSegResult& result );




		/*
		**	POINT TRIANGLE 
		*/

		/// Fast way to repeatedly get the closest point between a triangle and a vertex
		struct ClosestPointTriangleCache
		{
			hkReal m_invDet;
			hkReal m_QQ;
			hkReal m_RR;
			hkReal m_QR;
		};

		struct ClosestPointTriangleResult
		{
			hkVector4 closestPoint;
			//int		   pointsUsed;	// MASK_X means point 0 is not used
		};


		static void HK_CALL setupClosestPointTriangleCache( const hkVector4* triangle, ClosestPointTriangleCache& cache );

		enum ClosestPointTriangleStatus
		{
			HIT_TRIANGLE_FACE = 0,
			HIT_TRIANGLE_EDGE
		};

		static ClosestPointTriangleStatus HK_CALL closestPointTriangle( const hkVector4& position, const hkVector4* tri, const ClosestPointTriangleCache& cache, ClosestPointTriangleResult& result );


		/*
		**	MASKS
		*/

		/// returns the index of the bit set if passed in a compare mask or a index-8 of the only bit not set, works only on XYZ
		/// returns -1 for invalid inpu
		static const char maskToIndex[];

		static const char vertexToEdgeLut[];

		static inline int HK_CALL getNextModulo3( int i ) { return vertexToEdgeLut[i+2]; }
		static inline int HK_CALL getPrevModulo3( int i ) { return vertexToEdgeLut[i]; }
	protected:

		hkCollideTriangleUtil();

};

//#include <hkcollide2/algorithms/util/hkCollideTriangleUtil.inl>

#endif // HK_GEOMETRY2_TRIANGLEUTIL_H

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
