/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

extern const hkUint128 hkQuadSignMask;

inline void HK_CALL hkVector4Util::mulSigns4( hkVector4& inout, const hkVector4& signs)
{
	
	hkQuadReal itmp = inout.quad();
	register hkQuadReal stmp = signs.quad();

	asm(
		"pand %1, %1, %2\n"
		"pxor %0, %0, %1\n"
			:	"+r" (itmp), //0
				"+r" (stmp)  //1
			:	"r" (hkQuadSignMask)); //2
		
	inout.quad() = itmp;

}

extern const float hkConvertMagicVector[];

inline void HK_CALL hkVector4Util::convertToUint16( hkIntUnion64& out, const hkVector4& in, const hkVector4& offset, const hkVector4& scale)
{
	hkQuadReal tmp_f;
	hkQuadReal tmp_r;

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

	hkVector4 x;
	x.setAdd4(in,offset);
	x.mul4(scale);
	x.setMin4(x,max);
	x.setMax4(x,min);
	x.add4( reinterpret_cast<const hkVector4&>(hkConvertMagicVector) );
	
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



	/// transorm an array of points
inline void	HK_CALL hkVector4Util::transformPoints( const hkTransform& t, const hkVector4* vectorsIn, int numVectors, hkVector4* vectorsOut )
{
	checkRegisters( 0xf0000000 );
	__asm__ __volatile__ (
	"		.set noreorder\n"	\
	"		lqc2	vf28, 0x00(%0)	\n"	\
	"		lqc2	vf29, 0x10(%0)	\n"	\
	"		lqc2	vf30, 0x20(%0)	\n"	\
	"		lqc2	vf31, 0x30(%0)	\n" \
	"	.set reorder\n"
	: : "r"(&t.getRotation().getColumn(0) ): "vf28", "vf29", "vf30", "vf31", "memory" );

	while ( --numVectors >= 0 )
	{
		__asm__ __volatile__ (
		"		.set noreorder\n"	\
		"		lqc2	vf8, 0x00(%0)	\n"	\
		"		vmulax.xyzw		ACC, vf28, vf8 \n"	\
		"		vmadday.xyzw	ACC, vf29, vf8	\n"	\
		"		vmaddaz.xyzw	ACC, vf30, vf8	\n"	\
		"		vmaddw.xyzw		vf8, vf31, vf0	\n"	\
		"	.set reorder\n"
		:  : "r"(vectorsIn ): "vf8", "vf28", "vf29", "vf30", "vf31", "memory" );
		vectorsIn++;
		__asm__ __volatile__ (
		"		.set noreorder\n"	\
		"		sqc2	vf8, 0x0(%0)	\n"	\
		"	.set reorder\n"
		:  : "r"(vectorsOut ): "vf8", "memory" );
		vectorsOut++;
	}
}



	/// transorm an array of points
inline void	HK_CALL hkVector4Util::rotatePoints( const hkMatrix3& t, const hkVector4* vectorsIn, int numVectors, hkVector4* vectorsOut )
{
	checkRegisters( 0xf0000000 );		
	__asm__ __volatile__ (
	"		.set noreorder\n"	\
	"		lqc2	vf28, 0x00(%0)	\n"	\
	"		lqc2	vf29, 0x10(%0)	\n"	\
	"		lqc2	vf30, 0x20(%0)	\n"	\
	"	.set reorder\n"
	: : "r"(&t.getColumn(0) ): "vf28", "vf29", "vf30", "vf31", "memory" );

	while ( --numVectors >= 0 )
	{
		__asm__ __volatile__ (
		"		.set noreorder\n"	\
		"		lqc2	vf31, 0x00(%0)	\n"	\
		"		vmulax.xyzw		ACC, vf28, vf31 \n"	\
		"		vmadday.xyzw	ACC, vf29, vf31	\n"	\
		"		vmaddz.xyzw	   vf31, vf30, vf31	\n"	\
		"	.set reorder\n"
		: : "r"(vectorsIn ): "vf28", "vf29", "vf30", "vf31", "memory" );
		vectorsIn++;
		__asm__ __volatile__ (
		"		.set noreorder\n"	\
		"		sqc2	vf31, 0x0(%0)	\n"	\
		"	.set reorder\n"
		:  : "r"(vectorsOut ): "vf31", "memory" );
		vectorsOut++;
	}
}



	/// inverse rotate and array of points
inline void	HK_CALL hkVector4Util::rotateInversePoints( const hkRotation& t, const hkVector4* vectorsIn, int numVectors, hkVector4* vectorsOut )
{
	checkRegisters( 0xf0000000 );
	__asm__ __volatile__ (
	"		.set noreorder\n"	\
	"		lqc2	vf28, 0x00(%0)	\n"	\
	"		lqc2	vf29, 0x10(%0)	\n"	\
	"		lqc2	vf30, 0x20(%0)	\n"	\
	"	.set reorder\n"
	:  : "r"(&t.getColumn(0) ): "vf28", "vf29", "vf30", "vf31", "memory" );

	while ( --numVectors >= 0 )
	{
		__asm__ __volatile__ (
		"		.set noreorder\n"	\
		"	lqc2	vf31, 0x00(%0)	\n"	\
		"	vmul.xyz vf12,vf29,vf31		\n"	\
		"	vmul.xyz vf13,vf30,vf31		\n"	\
		"	vmul.xyz vf31,vf28,vf31		\n"	\
		"	vaddx.y vf12,vf12,vf12		\n"	\
		"	vaddy.z vf13,vf13,vf13		\n"	\
		"	vaddy.x vf31,vf31,vf31		\n"	\
		"	vaddz.x vf31,vf31,vf31		\n"	\
		"	vaddz.y vf31,vf12,vf12		\n"	\
		"	vaddx.z vf31,vf13,vf13		\n"	\
		"	.set reorder\n"
		:  : "r"(vectorsIn ): "vf12", "vf13", "vf31", "vf28", "vf29", "vf30", "vf31", "memory" );
		vectorsIn++;
		__asm__ __volatile__ (
		"		.set noreorder\n"	\
		"		sqc2	vf31, 0x0(%0)	\n"	\
		"	.set reorder \n"
		:  : "r"(vectorsOut ): "memory" );
		vectorsOut++;
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
