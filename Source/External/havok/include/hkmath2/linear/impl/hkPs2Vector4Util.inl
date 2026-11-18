/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

inline void HK_CALL hkVector4Util::mulSigns4( hkVector4& inout, const hkVector4& signs)
{
	static const __m128 signmask = 0x80000000800000008000000080000000; // top bit ones
	__m128 itmp;
	__m128 stmp;

	asm("lq %2, %3\n"
		"lq %1, %0\n"
		"pand %2, %2, %4\n"
		"pxor %1, %1, %2\n"
		"sq %1, %0\n"
			:	"+m" (inout), //0
				"=&r" (itmp), //1
				"=&r" (stmp) //2
			:	"m" (signs.quad()), //3
				"r" (signmask)); //4
}


inline void HK_CALL hkVector4Util::convertToUint16( hkIntUnion64& out, const hkVector4& in, const hkVector4& offset, const hkVector4& scale)
{
	hkQuadReal tmp_f;
	__m128 tmp_r;

	asm("vadd.xyzw %2, %3, %4\n"
		"vmul.xyzw %2, %2, %5\n"
		"vftoi0.xyzw %2, %2\n"
		"qmfc2 %1, %2\n"
		"nop\n"
		"ppach %1, %1, %1\n"
		"sd %1, %0\n"
			:	"=m" (out), //0
				"=&r" (tmp_r), //1
				"=&j" (tmp_f) //2
			:	"j" (in.quad()), //3
				"j" (offset.quad()), //4
				"j" (scale.quad())); //5
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
	out.i16[0] = u.i[0] >> 7;
	out.i16[1] = u.i[1] >> 7;
	out.i16[2] = u.i[2] >> 7;
	out.i16[3] = u.i[3] >> 7;
	
	/*
	hkQuadReal tmp_f;
    __m128 tmp_r;

    asm("vadd.xyzw %2, %3, %4\n"
        "vmul.xyzw %2, %2, %5\n"
        "vmini.xyzw %2, %2, %7\n"
        "vmax.xyzw %2, %2, %6\n"
        "vftoi0.xyzw %2, %2\n"
        "qmfc2 %1, %2\n"
        "nop\n"
        "ppach %1, %1, %1\n"
        "sd %1, %0\n"
            :   "=m" (out), //0
                "=&r" (tmp_r), //1
                "=&j" (tmp_f) //2
            :   "j" (in.quad()), //3
                "j" (offset.quad()), //4
                "j" (scale.quad()), //5
                "j" (min.quad()), //6
                "j" (max.quad()) ); //7
				*/
}


inline void HK_CALL hkVector4Util::exitMmx()
{
}

inline void	HK_CALL hkVector4Util::transformPoints( const hkTransform& t, const hkVector4* vectorsIn, int numVectors, hkVector4* vectorsOut )
{
	for(int i = 0; i < numVectors; ++i)
	{
		vectorsOut[i]._setTransformedPos(t, vectorsIn[i]);
	}
}

inline void	HK_CALL hkVector4Util::rotatePoints( const hkMatrix3& r, const hkVector4* vectorsIn, int numVectors, hkVector4* vectorsOut )
{
	for(int i = 0; i < numVectors; ++i)
	{
		vectorsOut[i]._setMul3(r, vectorsIn[i]);
	}
}

inline void	HK_CALL hkVector4Util::rotateInversePoints( const hkRotation& r, const hkVector4* vectorsIn, int numVectors, hkVector4* vectorsOut )
{
	for(int i = 0; i < numVectors; ++i)
	{
		vectorsOut[i]._setRotatedInverseDir(r, vectorsIn[i]);
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
