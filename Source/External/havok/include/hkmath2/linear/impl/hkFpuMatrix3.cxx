/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

delete this file 

bool hkMatrix3::isIdentity(hkReal zero) const
{
	const hkReal one  = 1 - zero;
	const hkMatrix3& m = *this;
	return	   hkMath::fabs(m(0,0)) >= one
			&& hkMath::fabs(m(0,1)) <= zero
			&& hkMath::fabs(m(0,2)) <= zero
			&& hkMath::fabs(m(1,0)) <= zero
			&& hkMath::fabs(m(1,1)) >= one
			&& hkMath::fabs(m(1,2)) <= zero
			&& hkMath::fabs(m(2,0)) <= zero
			&& hkMath::fabs(m(2,1)) <= zero
			&& hkMath::fabs(m(2,2)) >= one;
}


void hkMatrix3::setCrossSkewSymmetric( const hkVector4& r )
{
	getColumn(0).set(  0   ,  r(2), -r(1) );
	getColumn(1).set( -r(2),     0, +r(0) );
	getColumn(2).set(  r(1), -r(0),     0 );
}

void hkMatrix3::rotate( hkCanonicalAxis axis, hkReal angle )
{
	hkMatrix3 rotation;
	rotation.setZero();

	const int x = axis;
	const int y = (x != 2) ? x+1 : 0;
	const int z = (y != 2) ? y+1 : 0;

	hkReal cosalpha = hkMath::cos(angle);
	hkReal sinalpha = hkMath::sin(angle);

	rotation.getColumn( y )( y ) = cosalpha;
	rotation.getColumn( z )( z ) = cosalpha;

	rotation.getColumn( y )( z ) = sinalpha;
	rotation.getColumn( z )( y ) = -sinalpha;

	rotation.getColumn( x )( x ) = 1.0f;

	hkMatrix3 savethis(*this);
	this->setMul( savethis, rotation );
}

hkResult hkMatrix3::invert(hkReal epsilon)
{
	hkVector4 r0; r0.setCross( getColumn(1), getColumn(2) );
    hkVector4 r1; r1.setCross( getColumn(2), getColumn(0) );
    hkVector4 r2; r2.setCross( getColumn(0), getColumn(1) );

    hkSimdReal determinant = getColumn(0).dot3(r0);
    
    if( hkMath::fabs(determinant) > epsilon * epsilon * epsilon)
	{
		hkSimdReal dinv = 1.0f / determinant;
		r0.mul4( dinv );
		r1.mul4( dinv );
		r2.mul4( dinv );

		setRows( r0, r1, r2 );
		return HK_SUCCESS;	
    }
	else
	{
		return HK_FAILURE;
	}    
}

/*
void hkMatrix3::setRotatedDiagonalMatrix( const hkRotation& r, const hkVector4& diagonalMatrix3 )
{
	HK_BREAKPOINT();
}
*/

void hkMatrix3::transpose()
{
	hkMatrix3& m = *this;
	hkAlgorithm::swap( m(0,1), m(1,0) );
	hkAlgorithm::swap( m(0,2), m(2,0) );
	hkAlgorithm::swap( m(1,2), m(2,1) );
}

void hkMatrix3::setMul( const hkMatrix3& a, const hkMatrix3& b )
{
	for( int i=0; i<3; ++i)
	{
		hkVector4 x;
		hkSimdReal b0 = b(0,i);
		x.setMul4( b0, a.getColumn(0) );
		hkSimdReal b1 = b(1,i);
		x.addMul4( b1, a.getColumn(1) );
		hkSimdReal b2 = b(2,i);
		x.addMul4( b2, a.getColumn(2) );
		getColumn(i) = x;
	}
}


void hkMatrix3::setMulInverse( const hkMatrix3& a, const hkRotation& b )
{
	hkRotation HK_ALIGNED_VARIABLE(binverse, 16);
	binverse.setTranspose(b);
	setMul(a, binverse);
}

/*
void hkMatrix3::setMulInverse( const hkRotation& a, const hkMatrix3& b )
{
	*(int*)0 = 0;
}
*/

void hkMatrix3::add( const hkMatrix3& a )
{
	getColumn(0).add4( a.getColumn(0) );
	getColumn(1).add4( a.getColumn(1) );
	getColumn(2).add4( a.getColumn(2) );
}

void hkMatrix3::sub( const hkMatrix3& a )
{
	getColumn(0).sub4( a.getColumn(0) );
	getColumn(1).sub4( a.getColumn(1) );
	getColumn(2).sub4( a.getColumn(2) );
}

void hkMatrix3::changeBasis(const hkRotation& r)
{

	hkRotation HK_ALIGNED_VARIABLE(temp, 16);
	temp.setMulInverse(*this, r);
	this->setMul(r, temp);
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
