/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkmath2/hkMath.h>
#include <hkmath2/linear/hkVector4Util.h>
/// \class hkTransform
/// The following conventions are adopted:<br>
/// Internal representation of rotation is column-major<br>
/// Applying matrix multiplication to a vector is done by premultiplying
/// by the matrix<p>	
///
/// IMPORTANT NOTE ON SCALING: <br>
/// This class is not designed to represent a full transformation including
/// skewing and scaling components. The assumption is that skewing will not be
/// used and that scaling is not meaningful in a simulation of rigid bodies.
/// that only change their position and orientation.<p>
/// Do not attempt to put scaling into the rows or columns of the
/// rotation matrix. it must remain orthogonal at all times.<br>


void hkTransform::get4x4ColumnMajor(hkReal* d) const
{
	const hkReal* p = &m_rotation(0,0);
	for (int i = 0; i<4; i++)
	{
		d[0] = p[0];
		d[1] = p[1];
		d[2] = p[2];
		d[3] = 0.0f;
		d+= 4;
		p+= 4;
	}
	d[-1] = 1.0f;
}

void hkTransform::set4x4ColumnMajor(const hkReal* p)
{
	hkReal* d = &m_rotation(0,0);
	for (int i = 0; i<4; i++)
	{
		d[0] = p[0];
		d[1] = p[1];
		d[2] = p[2];
		d[3] = 0.0f;
		d+= 4;
		p+= 4;
	}
	d[-1] = 1.0f;
}

void hkTransform::setInverse( const hkTransform& t )
{
	m_rotation.setTranspose( t.getRotation() ); 

	hkVector4 tr;	tr.setNeg4(t.getTranslation()); 
	m_translation._setRotatedDir( m_rotation, tr);
}


/// aTc = aTb * bTc
void hkTransform::setMul( const hkTransform& aTb, const hkTransform& bTc )
{
	HK_ASSERT( this != &aTb );

	hkVector4Util::rotatePoints( aTb.getRotation(), &bTc.getRotation().getColumn(0), 4, &this->getRotation().getColumn(0) );
	m_translation.add4( aTb.m_translation);
}

void hkTransform::setMulInverseMul( const hkTransform& aTb, const hkTransform &bTc )
{
	hkVector4Util::rotateInversePoints( aTb.getRotation(), &bTc.getRotation().getColumn(0), 4, &this->getRotation().getColumn(0) );
	hkVector4 tr; tr._setRotatedInverseDir( aTb.m_rotation, aTb.m_translation);
	m_translation.sub4( tr );
}


void hkTransform::setMulEq( const hkTransform& b )
{
	hkTransform at = *this;
	setMul( at, b );
}

bool hkTransform::isOk() const
{
	const hkReal* r = reinterpret_cast<const hkReal*>(this);
	for(int i=0; i<16; ++i)
	{
		if( hkMath::isFinite(r[i]) == false )
		{
			return false;
		}
	}
	return true;
}


void hkTransform::setMulMulInverse( const hkTransform &wTa, const hkTransform &wTb )
{

	hkTransform h;
	h.setInverse( wTb );
	this->setMul( wTa, h );	
}

hkReal HK_ALIGNED_VARIABLE(hkTransformIdentity[],16) =
{
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
};


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
