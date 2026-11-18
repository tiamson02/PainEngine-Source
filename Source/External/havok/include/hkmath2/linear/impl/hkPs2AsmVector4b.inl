/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

inline void hkVector4::_setMul3( const hkMatrix3& r, const hkVector4& v )
{

	const hkQuadReal *p0 = reinterpret_cast<const hkQuadReal *>(&r);
	register hkQuadReal c0 = *p0;
	register hkQuadReal c1 = *(p0+1);
	register hkQuadReal c2 = *(p0+2);

	asm(
"		vmulax.xyz	ACC, %2, %1	# ACC =  c0 * v.x		\n"	
"		vmadday.xyz	ACC, %3, %1	# ACC += c1 * v.y		\n"
"		vmaddz.xyz	%0,  %4, %1	# v +=   c2 * v.z		"	
 : "=j" (m_quad)
 : "j"(v.m_quad), "j"(c0), "j"(c1), "j"(c2) );

}


inline void hkVector4::_setRotatedDir(const hkRotation& r, const hkVector4& v)
{
	_setMul3(r,v);
}

inline void hkVector4::_setRotatedInverseDir(const hkRotation& r, const hkVector4& v)
{

	const hkQuadReal *p0 = reinterpret_cast<const hkQuadReal *>(&r);
	hkQuadReal c0 = *p0;
	register hkQuadReal c1 = *(p0+1);
	register hkQuadReal c2 = *(p0+2);
	asm (
	"	vmul.xyz %0, %0, %3		\n"
	"	vmul.xyz %1, %1, %3		\n"		
	"	vmul.xyz %2, %2, %3		\n"
	"	vaddy.x  %0, %0, %0		\n"
	"	vaddx.y  %1, %1, %1		\n"
	"	vaddy.z  %2, %2, %2		\n"
	"	vaddz.x  %0, %0, %0		\n"
	"	vaddz.y  %0, %1, %1		\n"
	"	vaddx.z  %0, %2, %2		"
	: "+j"(c0), "+j"(c1), "+j"(c2) : "j"(v.m_quad) );
	m_quad = c0;
}

inline void hkVector4::_setTransformedPos(const hkTransform& t, const hkVector4& v)
{

	const hkQuadReal *p0 = reinterpret_cast<const hkQuadReal *>(&t);
	register hkQuadReal c0 = *p0;
	register hkQuadReal c1 = *(p0+1);
	register hkQuadReal c2 = *(p0+2);
	register hkQuadReal c3 = *(p0+3);

	asm (
"		vmulax.xyz	ACC, %2, %1		# ACC =  c0 * v.x		\n"	
"		vmadday.xyz	ACC, %3, %1		# ACC += c1 * v.y		\n"
"		vmaddaz.xyz ACC, %4, %1		# ACC += c2 * v.z		\n"
"		vmaddw.xyz  %0,  %5, vf00	# result = ACC + p		\n"
 : "=j" (m_quad)
 : "j"(v.m_quad), "j"(c0), "j"(c1), "j"(c2), "j"(c3) );

}


inline void hkVector4::_setTransformedInversePos(const hkTransform& t, const hkVector4& v)
{
	hkVector4 trans;
	trans.setSub4( v, t.getTranslation() );
	_setRotatedInverseDir( t.getRotation(), trans );
}


/* matrix3, rotation, quaternion, transform */

inline void hkVector4::setRotatedDir(const hkQuaternion& q, const hkVector4& v)
{

	//!me
#if 0
	// (2*qr*qr-1) * v  +  (2*qi.v) * qi  +  (2*qr) * (qi x v)
	hkReal scale[3];
	scale[0] = (2.0f * q.m_vec(3) * q.m_vec(3)) - 1.0f;
	scale[1] = 2.0f * ( q.m_vec(0)*v(0) + q.m_vec(1)*v(1) + q.m_vec(2)*v(2) );
	scale[2] = 2.0f * q.m_vec(3);

	hkVector4 imagCrossV;
	imagCrossV.setCross( q.getImag(), v);

	setMul4( scale[0], v);
	addMul4( scale[1], q.getImag() );
	addMul4( scale[2], imagCrossV  );
#else 

	hkQuadReal t0;
	register hkQuadReal t1, t2, t3;

	// p in vf1, q in vf2, result in vf3

	asm (
"		vmul 			%0, %4, %5			\n"
"		vopmula.xyz 	ACC, %5, %4			\n"
"		vopmsub.xyz 	%1, %4, %5			\n"
"		vmul.w 			%2w, %5w, %5w		\n"
"		vadd.w 			%3w, %5w, %5w 		\n"
"		vmulax.w 		ACC, vf0w, %0x		\n"
"		vmadday.w 		ACC, vf0w, %0y		\n"
"		vmaddz.w 		%0w, vf0w, %0z		\n"
"		vopmula.xyz 	ACC, %5, %1			\n"
"		vmaddaw.xyz 	ACC, %1, %3w		\n"
"		vmaddaw.xyz 	ACC, %4, %2w		\n"
"		vmaddaw.xyz 	ACC, %5, %0w		\n"
"		vopmsub.xyz 	%0, %1, %5			"
	: "=&j"(t0), "=&j"(t1), "=&j"(t2), "=&j"(t3) 
	 : "j"(v.m_quad), "j"(q.m_vec.m_quad) );

	m_quad = t0;

#endif
}


inline void hkVector4::setRotatedInverseDir(const hkQuaternion& q, const hkVector4& v)
{

//!me<TODO> optimize this one too
#if 0
	// as above with q.imag = -q.imag, and much cancelling
	hkReal scale[3];
	scale[0] = (2.0f * q.m_vec(3) * q.m_vec(3)) - 1.0f;
	scale[1] = 2.0f * ( q.m_vec(0)*v(0) + q.m_vec(1)*v(1) + q.m_vec(2)*v(2) );
	scale[2] = 2.0f * -q.m_vec(3);

	hkVector4 imagCrossV;
	imagCrossV.setCross( q.getImag(), v);

	setMul4( scale[0], v);
	addMul4( scale[1], q.getImag() );
	addMul4( scale[2], imagCrossV  );
#else
	hkQuaternion qInv;
	qInv.setInverse( q );

	setRotatedDir( qInv, v );

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
