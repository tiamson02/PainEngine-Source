/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

inline void HK_CALL hkVector4Util::mulSigns4( hkVector4& inout, const hkVector4& signv)
{
	hkUint32* intp = reinterpret_cast<hkUint32*>(&inout);
	const hkUint32* sign = reinterpret_cast<const hkUint32*>(&signv);
	intp[0] ^= (sign[0] & 0x80000000);
	intp[1] ^= (sign[1] & 0x80000000);
	intp[2] ^= (sign[2] & 0x80000000);
	intp[3] ^= (sign[3] & 0x80000000);
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
}

	/// transform an array of points
inline void	HK_CALL hkVector4Util::transformPoints( const hkTransform& t, const hkVector4* vectorsIn, int numVectors, hkVector4* vectorsOut )
{
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
	while ( --numVectors >= 0 )
	{
		vectorsOut->_setMul3( t, *vectorsIn );
		vectorsOut++;
		vectorsIn++;
	}
}

	/// transform an array of points
inline void	HK_CALL hkVector4Util::rotateInversePoints( const hkRotation& t, const hkVector4* vectorsIn, int numVectors, hkVector4* vectorsOut )
{
	while ( --numVectors >= 0 )
	{
		vectorsOut->_setRotatedInverseDir( t, *vectorsIn );
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
