/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

// For internal use. Define this to check that register allocations
// between functions do not clash. Disabled by default
// because it does not work in a multithreaded environment.
//#	define HK_CHECK_REG_FOR_PS2


	/// reserve registers for computers with lots of registers (e.g. PS2)
HK_FORCE_INLINE void HK_CALL hkVector4Util::reserveRegisters( hkUint32 registerMask )
{
#ifdef HK_CHECK_REG_FOR_PS2
	HK_ASSERT2( (registerMask & m_reservedRegisters) == 0, "Register clash");
	m_reservedRegisters |= registerMask;
#endif
}

	/// reserve registers for computers with lots of registers (e.g. PS2)
HK_FORCE_INLINE void HK_CALL hkVector4Util::checkRegisters( hkUint32 registerMask )
{
#ifdef HK_CHECK_REG_FOR_PS2
	HK_ASSERT2( (registerMask & m_reservedRegisters) == 0, "Register clash");
#endif
}

	/// release these registers
HK_FORCE_INLINE void HK_CALL hkVector4Util::releaseRegisters( hkUint32 registerMask )
{
#ifdef HK_CHECK_REG_FOR_PS2
	m_reservedRegisters &= ~registerMask;
#endif
}


HK_FORCE_INLINE void HK_CALL hkVector4Util::assignSubDot(hkReal& result, const hkVector4& a, const hkVector4& b, const hkVector4& c)
{
	hkVector4 ab;
	ab.setSub4( a, b );
	result = ab.dot3(c);
}

// Formula: (esoteric function)
// result0 = (a-b0).dot(c);
// result1 = (a-b1).dot(c);
HK_FORCE_INLINE void HK_CALL hkVector4Util::pairAssignSubDots(hkReal& result0, hkReal& result1, const hkVector4& a, const hkVector4& b0, const hkVector4& b1, const hkVector4& c)
{
	assignSubDot( result0, a, b0, c );
	assignSubDot( result1, a, b1, c );
}

// Formula: (esoteric function)
// result0 = (a-b0).dot(a);
// result1 = (a-b1).dot(a);
// result2 = (a-b2).dot(a);

HK_FORCE_INLINE void HK_CALL hkVector4Util::tripleAssignSubDots(hkReal& result0, hkReal& result1, hkReal& result2, const hkVector4& a, const hkVector4& b0, const hkVector4& b1, const hkVector4& b2)
{
	assignSubDot( result0, a, b0, a );
	assignSubDot( result1, a, b1, a );
	assignSubDot( result2, a, b2, a );


	/*	experimental code, should produce good results if setRotatedInverseDir is inlined
	hkRotation mat;
	mat.getColumn(0).setSub4( a, b0 );
	mat.getColumn(1).setSub4( a, b1 );
	mat.getColumn(2).setSub4( a, b2 );

	hkVector4 result; result.setRotatedInverseDir( mat, a );
	result0 = result(0);
	result1 = result(1);
	result2 = result(2);

	*(int*)0 = 0;
	*/
}


/// Sets the calling vector to be the normal to the 2 vectors (b-a) and (c-a).<br>
/// NOTE: The calculated result is not explicitly normalized.
/// This function is particularly fast on PS2.
//!me <TODO> make particularly fast on PS2
HK_FORCE_INLINE void HK_CALL hkVector4Util::setNormalOfTriangle(hkVector4 &result, const hkVector4 &a, const hkVector4& b,const hkVector4& c)
{
	hkVector4 ba,ca;
	ba.setSub4( b, a );
	ca.setSub4( c, a );
	result.setCross( ba , ca);
}


/// Find a vector that is perpendicular to a vector
///
/// Achieved by constructint a vector from the segment vector with the following properties:
/// (Segment vector is any vector parallel to the line segment.)
/// 1) component with the smallest index is set to 0
/// 2) the remaing two component are copied into the new vector but are swapped in position
/// 3) one of the copied components is multiplied by -1.0 (has it's sign flipped!)
///
/// leaving: (for example)
/// segmentVector = (x, y, z), with let's say y as the smallest component
/// segmentNormal = (-z, 0 , x) or (z, 0, -x), either will do.
///
/// This will in fact be orthogonal as (in the example) the dot product will be zero.
/// i.e. x*-z + y*0 + z*x = 0 
///
HK_FORCE_INLINE void HK_CALL hkVector4Util::calculatePerpendicularVector(const hkVector4& vectorIn, hkVector4& biVectorOut)
{
	// find the indicies of (one of) the smallest component(s) and the indicies of
	// the remaing two components in the vector
	int min = 0;
	int ok1 = 1;
	int ok2 = 2;

	hkReal a0 = hkMath::fabs(vectorIn(0));
	hkReal a1 = hkMath::fabs(vectorIn(1));
	hkReal a2 = hkMath::fabs(vectorIn(2));

	// sort the indizes to make min index point to the smallest
	if( a1 < a0 )
	{
		ok1 = 0;
		min = 1;
		a0 = a1;
	}

	if(a2 < a0)
	{
		ok2 = min;
		min = 2;
	}

	biVectorOut(min) = 0;
	biVectorOut(ok1) = vectorIn(ok2);
	biVectorOut(ok2) = -vectorIn(ok1);
}


// return the squared distance from p to the line segment ab
HK_FORCE_INLINE hkSimdReal HK_CALL hkVector4Util::distToLineSquared( const hkVector4& a, const hkVector4& b, const hkVector4& p )
{

	hkVector4 ab, ap;
	ab.setSub4( b, a );
	ap.setSub4( p, a );

	const hkSimdReal ab_ = ab.lengthSquared3();

	const hkReal projectionAsPercentOfSegment = ap.dot3(ab)/ab_;

	ap.addMul4( -projectionAsPercentOfSegment, ab );

	return ap.lengthSquared3();

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
