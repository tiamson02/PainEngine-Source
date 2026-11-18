/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkmath/hkMath.h>
#include <hkmath/linear/hkMathUtil.h>

void hkQsTransform::get4x4ColumnMajor(hkReal* d) const
{

	// Calculate the 3x3 matrices for rotation and scale
	hkRotation rotMatrix; rotMatrix.set (m_rotation);
	hkRotation scaMatrix;
	scaMatrix.setZero();
	scaMatrix.setDiagonal(m_scale(0), m_scale(1), m_scale(2));

	// Calculate R*S
	hkRotation rotSca;
	rotSca.setMul(rotMatrix, scaMatrix);

	// Construct hkTransform
	hkTransform temp(rotSca, m_translation);

	// Use hkTransform implementation
	temp.get4x4ColumnMajor(d);

}

hkBool hkQsTransform::set4x4ColumnMajor(const hkReal* p)
{
	hkMathUtil::Decomposition decomposition;
	hkMathUtil::decompose4x4ColTransform(p, decomposition);

	set(decomposition.m_translation, decomposition.m_rotation, decomposition.m_scale);

	return !decomposition.m_hasSkew;

}

void hkQsTransform::setInverse( const hkQsTransform& t )
{
	// We are not "reference-safe" because quaternion inversion is not reference-safe
	HK_ASSERT2(0x1acceb8d, (&t != this) , "Using unsafe references in math operation");

	m_translation.setRotatedInverseDir(t.m_rotation, t.m_translation);
	m_translation.mul4(-1.0f);
	m_rotation.setInverse(t.m_rotation);

	hkVector4 invScale; invScale.setReciprocal4( t.m_scale );
	m_scale.setXYZ0( invScale );
}

void hkQsTransform::setFromTransformNoScale (const hkTransform& transform)
{
	m_rotation.set(transform.getRotation());
	m_translation = transform.getTranslation();
	m_scale.setAll(1.0f);
}

void hkQsTransform::copyToTransformNoScale (hkTransform& transformOut) const
{
	transformOut.set(m_rotation, m_translation);
}

void hkQsTransform::setFromTransform (const hkTransform& transform)
{
	hkReal buffer[16];
	transform.get4x4ColumnMajor(buffer);
	set4x4ColumnMajor(buffer);
}

void hkQsTransform::copyToTransform (hkTransform& transformOut) const
{
	hkReal buffer[16];
	get4x4ColumnMajor(buffer);
	transformOut.set4x4ColumnMajor(buffer);
}


// aTc = aTb * bTc
void hkQsTransform::setMul( const hkQsTransform& aTb, const hkQsTransform& bTc )
{

	// Rotation and position go together
	hkVector4 extraTrans;	
	extraTrans.setRotatedDir(aTb.m_rotation, bTc.m_translation);
	m_translation.setAdd4(aTb.m_translation, extraTrans);
	m_rotation.setMul(aTb.m_rotation, bTc.m_rotation);

	// Scale goes apart
	m_scale.setMul4(aTb.m_scale, bTc.m_scale);
}

void hkQsTransform::setMulInverseMul( const hkQsTransform& a, const hkQsTransform &b )
{
	hkQsTransform h;
	h.setInverse( a );
	setMul(h, b);
}

void hkQsTransform::setMulMulInverse( const hkQsTransform &a, const hkQsTransform &b )
{
	hkQsTransform h;
	h.setInverse( b );
	setMul( a, h);	
}

void hkQsTransform::setMulEq( const hkQsTransform& b )
{
	// We are not "reference-safe" because quaternion multiplication is not reference-safe
	HK_ASSERT2(0x1acceb8d, (&b!=this) , "Using unsafe references in math operation");

	// Rotation and position go together
	hkVector4 extraTrans;
	extraTrans.setRotatedDir(m_rotation, b.m_translation);
	m_translation.add4(extraTrans);
	m_rotation.mul(b.m_rotation);

	// Scale goes apart
	m_scale.mul4(b.m_scale);

}

hkBool hkQsTransform::isOk() const
{
	const hkReal* r = reinterpret_cast<const hkReal*>(this);
	for(int i=0; i<12; ++i)
	{
		// Do not care about the w components of translation and scale
		if ((i==3) || (i==11)) continue;

		if( hkMath::isFinite(r[i]) == false )
		{
			return false;
		}
	}
	return true;
}

hkBool hkQsTransform::isApproximatelyEqual( const hkQsTransform& other, hkReal epsilon ) const
{

	if ( ! m_translation.equals3( other.m_translation, epsilon ) ) 
	{
		return false;
	}

	if ( ! m_scale.equals3(other.m_scale, epsilon) )
	{
		return false;
	}

	// Quaternions are sligtly trickier to compare due to the duplicity (-axis,angle) <=> (axis,-angle)

	hkVector4 toCompareThis = m_rotation.m_vec;
	hkVector4 toCompareOther = other.m_rotation.m_vec;

	// Make sure they both have a positive w component

	if (toCompareThis(3) * toCompareOther(3) < 0)
	{
		toCompareThis.mul4(-1.0f);
	}

	return toCompareThis.equals4(toCompareOther, epsilon);

}



HK_ALIGN16( hkReal hkQsTransformIdentity[] ) =
{
		0,0,0,0, // position
		0,0,0,1, // rotation
		1,1,1,0, // scale
};


/*
* Havok SDK - PUBLIC RELEASE, BUILD(#20060114)
*
* Confidential Information of Havok.  (C) Copyright 1999-2006 
* Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
* Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
* rights, and intellectual property rights in the Havok software remain in
* Havok and/or its suppliers.
*
* Use of this software for evaluation purposes is subject to and indicates 
* acceptance of the End User licence Agreement for this product. A copy of 
* the license is included with this software and is also available from salesteam@havok.com.
*
*/
