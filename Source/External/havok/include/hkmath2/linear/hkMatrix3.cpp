/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkmath2/hkMath.h>
#include <hkmath2/linear/hkVector4Util.h>


/// \class hkMatrix3
/// Due to alignment issues internal storage is 12 hkReals in a 4x3 matrix.
/// Elements are stored in column major format.<br>
/// i.e. contiguous memory locations are (x00, x10, x20, pad), (x10, x11,...) 
/// where x10 means row 1, column 0 for example.

bool hkMatrix3::isOk() const
{
	const hkReal* f = reinterpret_cast<const hkReal*>(this);
	for(int i=0; i<12; ++i)
	{
		if( hkMath::isFinite(f[i]) == false )
		{
			return false;
		}
	}
	return true;
}


void hkMatrix3::transpose()
{
	hkMatrix3& m = *this;
	hkAlgorithm::swap( m(0,1), m(1,0) );
	hkAlgorithm::swap( m(0,2), m(2,0) );
	hkAlgorithm::swap( m(1,2), m(2,1) );
}

	/// copy and transpose 
void hkMatrix3::setTranspose( const hkMatrix3& s )
{
	hkMatrix3& d = *this;
	d(0,0) = s(0,0);
	d(1,1) = s(1,1);
	d(2,2) = s(2,2);
	d(3,0) = 0.0f;
	d(3,1) = 0.0f;
	d(3,2) = 0.0f;
	d(1,0) = s(0,1);	
	d(0,1) = s(1,0);	
	d(2,0) = s(0,2);	
	d(0,2) = s(2,0);	
	d(2,1) = s(1,2);	
	d(1,2) = s(2,1);	
}

/// aTc = aTb * bTc
void hkMatrix3::setMul( const hkMatrix3& aTb, const hkMatrix3& bTc )
{
	HK_ASSERT( this != &aTb );
	HK_ASSERT( this != &bTc );
	hkVector4Util::rotatePoints( aTb, &bTc.getColumn(0), 3, &this->getColumn(0) );
}

void hkMatrix3::setMul( hkSimdRealParameter scale, const hkMatrix3& a)
{
	HK_ASSERT( this != &a);
	getColumn(0).setMul4(scale, a.getColumn(0));
	getColumn(1).setMul4(scale, a.getColumn(1));
	getColumn(2).setMul4(scale, a.getColumn(2));
}

void hkMatrix3::setMulInverse( const hkMatrix3& a, const hkRotation& b )
{
	hkRotation HK_ALIGNED_VARIABLE(binverse, 16) = b;
	binverse.transpose();
	setMul(a, binverse);
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


bool hkMatrix3::isApproximatelyEqual( const hkMatrix3& m, hkReal zero) const
{
	return	   m_cols[0].equals3( m.getColumn(0), zero )
			&& m_cols[1].equals3( m.getColumn(1), zero )
			&& m_cols[2].equals3( m.getColumn(2), zero );
}

void hkMatrix3::setCrossSkewSymmetric( const hkVector4& r )
{
	getColumn(0).set(  0   ,  r(2), -r(1) );
	getColumn(1).set( -r(2),     0, +r(0) );
	getColumn(2).set(  r(1), -r(0),     0 );
}


hkResult hkMatrix3::invert(hkReal epsilon)
{
	hkVector4 r0; r0.setCross( getColumn(1), getColumn(2) );
    hkVector4 r1; r1.setCross( getColumn(2), getColumn(0) );
    hkVector4 r2; r2.setCross( getColumn(0), getColumn(1) );

    hkSimdReal determinant = getColumn(0).dot3(r0);
    
    if( hkMath::fabs(determinant) > epsilon * epsilon * epsilon)
	{
		hkSimdReal dinv = hkSimdReal(1.0f) / determinant;
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

void hkMatrix3::mul( const hkMatrix3& a )
{

	hkMatrix3 temp;
	temp.setMul( *this, a );
	*this = temp;
}

void hkMatrix3::mul( hkSimdRealParameter scale)
{
	getColumn(0).mul4(scale);
	getColumn(1).mul4(scale);
	getColumn(2).mul4(scale);
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
