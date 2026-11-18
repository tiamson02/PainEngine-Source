/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


#include <hkcollide2/algorithms/boundingvolume/hkAabb.h>

void HK_CALL hkAabbUtil::calcAabb( const hkTransform& localToWorld, const hkVector4& halfExtents, float extraRadius, hkAabb& aabbOut )
{
#if defined (HK_PS2) && defined (HK_COMPILER_GCC)
	asm (
"		lqc2		vf8, 0x00(%3)	#	column(0)			\n"		\
"		qmtc2			%4, vf12	# extra radius \n"				\
"		lqc2		vf9, 0x10(%3)	#	column(1)			\n"		\
"		lqc2		vf10, 0x20(%3)	#	column(2)			\n"		\
"		lqc2		vf11, 0x30(%3)	#	column(3)			\n"		\
"		vaddx.xyzw	vf12, vf0, vf12 #   broadcast extra radius \n"	\
"		vmulx.xyz	vf8, vf8,  %2		# transformedX.setMul4( halfExtents(0), localToWorld.getRotation().getColumn(0))		\n"		\
"		vmuly.xyz	vf9, vf9,  %2		# transformedY.setMul4( halfExtents(1), localToWorld.getRotation().getColumn(0))		\n"		\
"		vmulz.xyz	vf10,vf10, %2		# transformedZ.setMul4( halfExtents(2), localToWorld.getRotation().getColumn(0))		\n"		\
"		vabs.xyz    vf8, vf8			# abs x \n"		\
"		vabs.xyz    vf9, vf9			# abs y \n"		\
"		vabs.xyz    vf10, vf10			# abs z \n"		\
"		vmulaw.xyzw	     ACC, vf8, vf0	# x+y+z\n"		\
"		vmaddaw.xyzw	 ACC, vf9, vf0	# \n"			\
"		vmaddaw.xyzw	 ACC, vf12,vf0	# extra radius \n"			\
"		vmaddw.xyzw		%1, vf10, vf0	# extends = all together\n"		\
"		vsub.xyz	%0, vf11, %1		# min = offset - extends  \n"	\
"		vadd.xyz	%1, vf11, %1		# max += offset	\n"
 : "=j" (aabbOut.m_min.quad() ), "=j" (aabbOut.m_max.quad() )
 :  "j"(halfExtents.quad()), "r"( &localToWorld.getRotation().getColumn(0) ), "r" ( extraRadius )
 : "vf8", "vf9", "vf10", "vf11", "vf12", "memory"	);
#else
	hkVector4 transformedX; transformedX.setMul4( halfExtents(0), localToWorld.getRotation().getColumn(0));
	hkVector4 transformedY; transformedY.setMul4( halfExtents(1), localToWorld.getRotation().getColumn(1));
	hkVector4 transformedZ; transformedZ.setMul4( halfExtents(2), localToWorld.getRotation().getColumn(2));
	
	transformedX.setAbs4( transformedX );
	transformedY.setAbs4( transformedY );
	transformedZ.setAbs4( transformedZ );

	hkVector4& max = aabbOut.m_max;
	hkVector4& min = aabbOut.m_min;

	hkVector4 extra; extra.setAll3( extraRadius );
	transformedZ.add4( extra );
	max.setAdd4( transformedX, transformedY );
	max.add4( transformedZ );

	min.setNeg4(max);
	max.add4( localToWorld.getTranslation() );
	min.add4( localToWorld.getTranslation()  );
#endif
}


void HK_CALL hkAabbUtil::calcAabb( const hkTransform& localToWorld, const hkVector4& halfExtents, const hkVector4& center, float extraRadius, hkAabb& aabbOut )
{
#if defined HK_PS2 && defined (HK_COMPILER_GCC)
	asm (
"		lqc2		vf8, 0x00(%3)	#	column(0)			\n"		\
"		qmtc2			%4, vf12	# extra radius \n"				\
"		lqc2		vf9, 0x10(%3)	#	column(1)			\n"		\
"		lqc2		vf10, 0x20(%3)	#	column(2)			\n"		\
"		lqc2		vf11, 0x30(%3)	#	column(3)			\n"		\
"		vaddx.xyzw	vf12, vf0, vf12 #   broadcast extra radius \n"	\
"		vmulax.xyz	ACC,  vf8, %5	#   vf13.setTransformedPos(center)	\n" \
"		vmadday.xyz	ACC,  vf9, %5	#   vf13.setTransformedPos(center)	\n" \
"		vmaddaz.xyz	ACC,  vf10, %5	#   vf13.setTransformedPos(center)	\n" \
"		vmaddw.xyz	vf11, vf11, vf0	#   vf13.setTransformedPos(center)	\n" \
"		vmulx.xyz	vf8, vf8,  %2		# transformedX.setMul4( halfExtents(0), localToWorld.getRotation().getColumn(0))		\n"		\
"		vmuly.xyz	vf9, vf9,  %2		# transformedY.setMul4( halfExtents(1), localToWorld.getRotation().getColumn(0))		\n"		\
"		vmulz.xyz	vf10,vf10, %2		# transformedZ.setMul4( halfExtents(2), localToWorld.getRotation().getColumn(0))		\n"		\
"		vabs.xyz    vf8, vf8			# abs x \n"		\
"		vabs.xyz    vf9, vf9			# abs y \n"		\
"		vabs.xyz    vf10, vf10			# abs z \n"		\
"		vmulaw.xyzw	     ACC, vf8, vf0	# x+y+z\n"		\
"		vmaddaw.xyzw	 ACC, vf9, vf0	# \n"			\
"		vmaddaw.xyzw	 ACC, vf10,vf0	# extra radius \n"			\
"		vmaddw.xyzw		%1, vf12, vf0	# extends = all together\n"		\
"		vsub.xyz	%0, vf11, %1		# min = offset - extends  \n"	\
"		vadd.xyz	%1, vf11, %1		# max += offset	\n"
 : "=j" (aabbOut.m_min.quad() ), "=j" (aabbOut.m_max.quad() )
 :  "j"(halfExtents.quad()), "r"( &localToWorld.getRotation().getColumn(0) ), "r" ( extraRadius ), "j" (center.quad())
 : "vf8", "vf9", "vf10", "vf11", "vf12", "memory"	);
#else
	//!me TODO simd optimize this
	hkVector4 transformedX; transformedX.setMul4( halfExtents(0), localToWorld.getRotation().getColumn(0));
	hkVector4 transformedY; transformedY.setMul4( halfExtents(1), localToWorld.getRotation().getColumn(1));
	hkVector4 transformedZ; transformedZ.setMul4( halfExtents(2), localToWorld.getRotation().getColumn(2));
	
	transformedX.setAbs4( transformedX );
	transformedY.setAbs4( transformedY );
	transformedZ.setAbs4( transformedZ );

	hkVector4& max = aabbOut.m_max;
	hkVector4& min = aabbOut.m_min;

	hkVector4 extra; extra.setAll3( extraRadius );
	transformedZ.add4( extra );

	max.setAdd4( transformedX, transformedY );
	max.add4( transformedZ );

	min.setNeg4(max);

	hkVector4 temp;
	temp._setTransformedPos(localToWorld, center);

	max.add4( temp );
	min.add4( temp );
#endif
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
