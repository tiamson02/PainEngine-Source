/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


inline void HK_CALL hkVector4Util::mulSigns4( hkVector4& inout, const hkVector4& signs)
{
	static const hkUint32 HK_ALIGNED_VARIABLE(signmask[4], 16) =
		{ 0x80000000, 0x80000000, 0x80000000, 0x80000000 };
	inout.quad() = _mm_xor_ps( inout.quad(),
								_mm_and_ps(signs.quad(), *(hkQuadReal*)&signmask));
}


inline void HK_CALL hkVector4Util::convertToUint16( hkIntUnion64& out, const hkVector4& in, const hkVector4& offset, const hkVector4& scale)
{

	static const float HK_ALIGNED_VARIABLE(magic[], 16 ) = { 65536.0f, 65536.0f, 65536.0f, 65536.0f };

	hkVector4 x;
	x.setAdd4(in,offset);
	x.mul4(scale);
	x.add4( reinterpret_cast<const hkVector4&>(magic) );

	union 
	{
		hkQuadReal q;
		hkUint32 i[4];
	} u;

	u.q = x.quad();
	// note implictly throw away top 16 bits
	out.i16[0] = hkInt16(u.i[0] >> 7);
	out.i16[1] = hkInt16(u.i[1] >> 7);
	out.i16[2] = hkInt16(u.i[2] >> 7);
	out.i16[3] = hkInt16(u.i[3] >> 7);
}

inline void HK_CALL hkVector4Util::convertToUint16WithClip( hkIntUnion64& out, const hkVector4& in, const hkVector4& offset, const hkVector4& scale, const hkVector4& min, const hkVector4& max)
{

	static const float HK_ALIGNED_VARIABLE(magic[], 16 ) = { 65536.0f, 65536.0f, 65536.0f, 65536.0f };

	hkVector4 x;
	x.setAdd4(in,offset);
	x.mul4(scale);
	x.setMin4(x,max);
	x.setMax4(x,min);
	x.add4( reinterpret_cast<const hkVector4&>(magic) );
	
	union 
	{
		hkQuadReal q;
		hkUint32 i[4];
	} u;

	u.q = x.quad();
	// note implictly throw away top 16 bits
	out.i16[0] = hkInt16(u.i[0] >> 7);
	out.i16[1] = hkInt16(u.i[1] >> 7);
	out.i16[2] = hkInt16(u.i[2] >> 7);
	out.i16[3] = hkInt16(u.i[3] >> 7);
}


inline void HK_CALL hkVector4Util::exitMmx()
{
	_mm_empty();
}

	/// transform an array of points
inline void	HK_CALL hkVector4Util::transformPoints( const hkTransform& t, const hkVector4* vectorsIn, int numVectors, hkVector4* vectorsOut )
{
	checkRegisters( 0xf0000000 );
	while ( --numVectors >= 0 )
	{
		vectorsOut->_setTransformedPos( t, *vectorsIn );
		vectorsOut++;
		vectorsIn++;
	}
}
	/// transform an array of points
inline void	HK_CALL hkVector4Util::rotatePoints( const hkMatrix3& t, const hkVector4* vectorsIn, int numVectors, hkVector4* vectorsOut )
{
	checkRegisters( 0x70000000 );		

	while ( --numVectors >= 0 )
	{
		vectorsOut->_setMul3( t, *vectorsIn );
		vectorsOut++;
		vectorsIn++;
	}
}

	/// transform an array of points
inline void	HK_CALL hkVector4Util::rotateInversePoints( const hkRotation& r, const hkVector4* vectorsIn, int numVectors, hkVector4* vectorsOut )
{
	checkRegisters( 0x70000000 );		

	hkQuadReal c0 = r.getColumn(0).quad();
	hkQuadReal c1 = r.getColumn(1).quad();
	hkQuadReal c2 = r.getColumn(2).quad();
	hkQuadReal c3 = r.getColumn(2).quad(); // c3 doesn't exist, fake it
	_MM_TRANSPOSE4_PS(c0,c1,c2,c3);

	while ( --numVectors >= 0 )
	{
		const hkVector4& b= *vectorsIn;
		const hkQuadReal b0 = _mm_shuffle_ps( b.quad(), b.quad(), _MM_SHUFFLE(0,0,0,0));
		const hkQuadReal r0 = _mm_mul_ps( c0, b0 );
		const hkQuadReal b1 = _mm_shuffle_ps( b.quad(), b.quad(), _MM_SHUFFLE(1,1,1,1));
		const hkQuadReal r1 = _mm_mul_ps( c1, b1 );
		const hkQuadReal b2 = _mm_shuffle_ps( b.quad(), b.quad(), _MM_SHUFFLE(2,2,2,2));
		const hkQuadReal r2 = _mm_mul_ps( c2, b2 );
		vectorsOut->quad() = _mm_add_ps( _mm_add_ps(r0, r1), r2 );
		vectorsOut++;
		vectorsIn++;
	}
}


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
