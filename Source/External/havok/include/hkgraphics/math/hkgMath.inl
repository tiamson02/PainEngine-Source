/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

inline int hkg_abs(int x)
{
	return (x < 0) ? -x : x;
}

/// Return the clamped value of the float x, to the limits of l (lowest value) and h (highest value).
inline float hkg_clampf(float x, float l, float h)
{
	return ( (x < l) ? l : (x > h ? h : x) );
}

/// Return the clamped value of the integer x, to the limits of l (lowest value) and h (highest value).
inline int hkg_clampi(int x, int l, int h)
{
	return ( (x < l) ? l : (x > h ? h : x) );
}

inline float hkgVec3Dot( const float a[3], const float b[3])
{
	//(ax*bx) + (ay*ay) + (az*bz)
	return (a[HKG_X] * b[HKG_X]) + (a[HKG_Y] * b[HKG_Y]) + (a[HKG_Z] * b[HKG_Z]);
}

inline float hkgVec3Length( const float v[3] )
{
	return hkg_sqrt( hkgVec3Dot(v,v) );
}

inline float hkgVec3LengthSqrd( const float v[3] )
{
	return hkgVec3Dot(v,v);
}

inline void hkgVec3Div( float out[3], const float a[3], const float scale )
{
	out[HKG_X] = a[HKG_X] / scale;
	out[HKG_Y] = a[HKG_Y] / scale;
	out[HKG_Z] = a[HKG_Z] / scale;
}

inline void hkgVec3Div( float v[3], const float scale )
{
	v[HKG_X] /= scale;
	v[HKG_Y] /= scale;
	v[HKG_Z] /= scale;
}

inline float hkgVec3Normalize( float v[3] )
{
	float l = hkgVec3Length( v );
	hkgVec3Div( v,  l ) ;
	return l;
}


inline void hkgVec3Mult( float out[3], const float a[3], const float scale )
{
	out[HKG_X] = a[HKG_X] * scale;
	out[HKG_Y] = a[HKG_Y] * scale;
	out[HKG_Z] = a[HKG_Z] * scale;
}

inline void hkgVec3Scale( float v[3], const float scale )
{
	v[HKG_X] *= scale;
	v[HKG_Y] *= scale;
	v[HKG_Z] *= scale;
}

inline void hkgVec3MultAdd( float out[3], const float a[3], const float scale)
{
	out[HKG_X] += a[HKG_X] * scale;
	out[HKG_Y] += a[HKG_Y] * scale;
	out[HKG_Z] += a[HKG_Z] * scale;
}

inline void  hkgVec3Sub( float out[3], const float a[3], const float b[3] )
{
	out[HKG_X] = a[HKG_X] - b[HKG_X];
	out[HKG_Y] = a[HKG_Y] - b[HKG_Y];
	out[HKG_Z] = a[HKG_Z] - b[HKG_Z];
}

inline void  hkgVec3Add( float out[3], const float a[3], const float b[3] )
{
	out[HKG_X] = a[HKG_X] + b[HKG_X];
	out[HKG_Y] = a[HKG_Y] + b[HKG_Y];
	out[HKG_Z] = a[HKG_Z] + b[HKG_Z];
}

inline void  hkgVec3Sub( float inout[3], const float a[3])
{
	inout[HKG_X] -= a[HKG_X];
	inout[HKG_Y] -= a[HKG_Y];
	inout[HKG_Z] -= a[HKG_Z];
}

inline void  hkgVec3Add( float inout[3], const float a[3])
{
	inout[HKG_X] += a[HKG_X];
	inout[HKG_Y] += a[HKG_Y];
	inout[HKG_Z] += a[HKG_Z];
}

inline void hkgVec3Cross( float out[3], const float a[3], const float b[3])
{
	//(y*v.z-z*v.y, z*v.x-x*v.z, x*v.y-y*v.x);
	//(ay * bz) - (az * by)
	//(az * bx) - (ax * bz)
	//(ax * by) - (ay * bx)

	out[HKG_X] = (a[HKG_Y] * b[HKG_Z]) - (a[HKG_Z] * b[HKG_Y]);
	out[HKG_Y] = (a[HKG_Z] * b[HKG_X]) - (a[HKG_X] * b[HKG_Z]);
	out[HKG_Z] = (a[HKG_X] * b[HKG_Y]) - (a[HKG_Y] * b[HKG_X]);
}

inline void hkgVec3Copy( float out[3], const float a[3] )
{
	out[0] = a[0];
	out[1] = a[1];
	out[2] = a[2];
}

inline void hkgVec3Zero( float v[3] )
{
	v[0] = 0;
	v[1] = 0;
	v[2] = 0;
}

inline void  hkgVec3Transform( float out[3], const float in[3], const float t[16] )
{
	out[0] = in[HKG_X]*t[0] + in[HKG_Y]*t[4] + in[HKG_Z]*t[8] + t[12];
	out[1] = in[HKG_X]*t[1] + in[HKG_Y]*t[5] + in[HKG_Z]*t[9] + t[13];
	out[2] = in[HKG_X]*t[2] + in[HKG_Y]*t[6] + in[HKG_Z]*t[10] + t[14];

	float w = in[HKG_X]*t[3] + in[HKG_Y]*t[7] + in[HKG_Z]*t[11] + t[15];
	out[0] /= w;
	out[1] /= w;
	out[2] /= w;
}

inline void  hkgVec3Clamp( float v[3], float lower, float upper)
{
	v[HKG_X] = HKG_MAX( v[HKG_X], lower );
	v[HKG_X] = HKG_MIN( v[HKG_X], upper );
	v[HKG_Y] = HKG_MAX( v[HKG_Y], lower );
	v[HKG_Y] = HKG_MIN( v[HKG_Y], upper );
	v[HKG_Z] = HKG_MAX( v[HKG_Z], lower );
	v[HKG_Z] = HKG_MIN( v[HKG_Z], upper );

}

inline void hkgVec4Transform( float out[4], const float in[4], const float t[16] )
{
	out[0] = in[HKG_X]*t[0] + in[HKG_Y]*t[4] + in[HKG_Z]*t[8]  + in[HKG_W]*t[12];
	out[1] = in[HKG_X]*t[1] + in[HKG_Y]*t[5] + in[HKG_Z]*t[9]  + in[HKG_W]*t[13];
	out[2] = in[HKG_X]*t[2] + in[HKG_Y]*t[6] + in[HKG_Z]*t[10] + in[HKG_W]*t[14];
	out[3] = in[HKG_X]*t[3] + in[HKG_Y]*t[7] + in[HKG_Z]*t[11] + in[HKG_W]*t[15];
}

inline void hkgVec4TransformD( double out[4], const double in[4], const double t[16] )
{
	out[0] = in[HKG_X]*t[0] + in[HKG_Y]*t[4] + in[HKG_Z]*t[8]  + in[HKG_W]*t[12];
	out[1] = in[HKG_X]*t[1] + in[HKG_Y]*t[5] + in[HKG_Z]*t[9]  + in[HKG_W]*t[13];
	out[2] = in[HKG_X]*t[2] + in[HKG_Y]*t[6] + in[HKG_Z]*t[10] + in[HKG_W]*t[14];
	out[3] = in[HKG_X]*t[3] + in[HKG_Y]*t[7] + in[HKG_Z]*t[11] + in[HKG_W]*t[15];
}

inline void  hkgVec4Zero( float inout[4] )
{
	hkString::memSet( inout, 0, sizeof(float)*4);
}

inline void  hkgVec4Copy( float out[4], const float in[4] )
{
	hkString::memCpy( out, in, sizeof(float)*4);
}

inline bool hkgVec3Equals( const float a[3], const float b[3], float tolerance)
{
	return (hkg_fabs(a[HKG_X] - b[HKG_X]) < tolerance) && 
		(hkg_fabs(a[HKG_Y] - b[HKG_Y]) < tolerance) && 
		(hkg_fabs(a[HKG_Z] - b[HKG_Z]) < tolerance);
}


inline void hkgQuatIdentity( float q[4] )
{
	q[HKG_X] = 0.0f;
	q[HKG_Y] = 1.0f;
	q[HKG_Z] = 0.0f;
	q[HKG_W] = 0.0f;
}

inline void  hkgQuatNormalize( float q[4] )
{
 	float len = hkg_sqrt( (q[HKG_W] * q[HKG_W]) + hkgVec3Dot( q, q ) );
	q[HKG_W] /= len;
	hkgVec3Div( q, len );
}

inline void hkgQuatFromAngleAxis(  float q[4], const float angle, const float axis[3] )
{
	q[HKG_W] = hkg_cos(0.5f*angle);
	hkgVec3Copy( q, axis  );
	hkgVec3Scale( q, hkg_sin(0.5f*angle) );

	hkgQuatNormalize( q );
}

inline void hkgQuatRotateVec3( float out[3], const float q[4], const float v[3] )
{
	float result[3];

	float vs = ( (2*q[HKG_W]*q[HKG_W]) -1.0f);
	hkgVec3Mult( result, v, vs  ); // result=v*(2*m_real*m_real-1.0f);

	float idv = hkgVec3Dot( q, v ); // m_imag.dot(v)

	float idvV[3];
	hkgVec3Mult( idvV, q, idv ); //m_imag.dot(v)*m_imag

	float icV[3];
	hkgVec3Cross( icV, q, v );    //m_imag.cross(v)
	hkgVec3Scale( icV, q[HKG_W]);     // m_real*m_imag.cross(v)

	hkgVec3Add( icV, idvV, icV ); // m_imag.dot(v)*m_imag + m_real*m_imag.cross(v);
	hkgVec3MultAdd( result, icV, 2.0f ); //result += 2.0f*(m_imag.dot(v)*m_imag+m_real*m_imag.cross(v));

	hkgVec3Copy( out, result );
}

inline void  hkgQuatMultiply( float out[4], const float q[4], const float r[4])
{
	/*
	m_vec.setCross(q0.getImag(), q1.getImag());
	m_vec.addMul4(q0.getReal(), q1.getImag());
	m_vec.addMul4(q1.getReal(), q0.getImag());
	m_vec(3) = q0.getReal() * q1.getReal() - hkReal( q0.m_vec.dot3( q1.m_vec ) );
	*/

	float temp[3];
	hkgVec3Cross( out, q, r);
	hkgVec3Mult( temp, r ,q[HKG_W] );
	hkgVec3Add( out, temp );
	hkgVec3Mult( temp, q, r[HKG_W] );
	hkgVec3Add( out, temp );
	out[HKG_W] = q[HKG_W] * r[HKG_W] - hkgVec3Dot(q, r);
}

inline void  hkgPlaneFrom3Points( float plane[4], const float a[3], const float b[3], const float c[3] )
{
	float ab[3];
	float ac[3];

	hkgVec3Sub( ab, b, a);
	hkgVec3Sub( ac, c, a);

	hkgVec3Cross( plane, ab, ac ); // normal to the plane, N
	hkgVec3Normalize( plane ); // unit length N/|N|

	// perp distance to origin
	plane[HKG_D] = hkgVec3Dot( a, plane );

/*
// wrong expansion (derived from Cramer's rule and with expanded determinants..)
	plane[HKG_A] = a[HKG_Y]*( b[HKG_Z] - c[HKG_Z] ) + b[HKG_Y]*( c[HKG_Z] - a[HKG_Z] ) + c[HKG_Y]*( a[HKG_Z] - b[HKG_Z] );
	plane[HKG_B] = a[HKG_Z]*( b[HKG_X] - c[HKG_X] ) + b[HKG_Z]*( c[HKG_X] - a[HKG_X] ) + c[HKG_Z]*( a[HKG_X] - b[HKG_X] );
	plane[HKG_C] = a[HKG_X]*( b[HKG_Y] - c[HKG_Y] ) + b[HKG_X]*( c[HKG_Y] - a[HKG_Y] ) + c[HKG_X]*( a[HKG_Y] - b[HKG_Y] );
	plane[HKG_D] = -a[HKG_X]*((b[HKG_Y] * c[HKG_Z]) - (c[HKG_Y] * b[HKG_Z]))
		           -b[HKG_X]*((c[HKG_Y] * a[HKG_Z]) - (a[HKG_Y] * c[HKG_Z]))
				   -c[HKG_X]*((a[HKG_Y] * b[HKG_Z]) - (b[HKG_Y] * a[HKG_Z]));
*/
}


inline void hkgMat4Zero( float m[16] )
{
	hkString::memSet(m, 0, sizeof(float)*16);
}
	
inline void hkgMat4Identity( float m[16] )
{
	hkgMat4Zero( m );
	m[0] = 1.0f;
	m[5] = 1.0f;
	m[10] = 1.0f;
	m[15] = 1.0f;
}

inline void hkgMat4ZeroD( double m[16] )
{
	hkString::memSet(m, 0, sizeof(double)*16);
}
	
inline void hkgMat4IdentityD( double m[16] )
{
	hkgMat4ZeroD( m );
	m[0] = 1.0f;
	m[5] = 1.0f;
	m[10] = 1.0f;
	m[15] = 1.0f;
}

inline void hkgMat4RotateAndAdd( float inout[16], const float offset[3] )
{
	// rotate the offset and add to the 
	inout[12] += (inout[0]*offset[0]) + (inout[4]*offset[1]) + (inout[8] *offset[2]);
	inout[13] += (inout[1]*offset[0]) + (inout[5]*offset[1]) + (inout[9] *offset[2]);
	inout[14] += (inout[2]*offset[0]) + (inout[6]*offset[1]) + (inout[10]*offset[2]);
}

inline void hkgMat4QuickInvert( float out[16], const float in[16] )
{
	// Transpose of the ORTHONORMAL rotational part

	out[0] = in[0];
	out[1] = in[4];
	out[2] = in[8];
	out[3] = 0;

	out[4] = in[1];
	out[5] = in[5];
	out[6] = in[9];
	out[7] = 0;

	out[8] = in[2];
	out[9] = in[6];
	out[10] = in[10];
	out[11] = 0;

	// The inverse translation rotated by the basis (works out as the same as a dot product of the cols and translation)
	out[12] = -hkgVec3Dot( &(in[0]), &(in[12]) );
	out[13] = -hkgVec3Dot( &(in[4]), &(in[12]) );
	out[14] = -hkgVec3Dot( &(in[8]), &(in[12]) );
	out[15] = 1.0f;


}

inline void hkgVec3Rotate( float inout[3], const float mat[16])
{
	float temp[3];
	
	temp[0] = inout[HKG_X]*mat[0] + inout[HKG_Y]*mat[4] + inout[HKG_Z]*mat[8];
	temp[1] = inout[HKG_X]*mat[1] + inout[HKG_Y]*mat[5] + inout[HKG_Z]*mat[9];
	temp[2] = inout[HKG_X]*mat[2] + inout[HKG_Y]*mat[6] + inout[HKG_Z]*mat[10];
	
	hkgVec3Copy( inout, temp );
}

inline void hkgMat4Transpose( float out[16], const float in[16] )
{
	out[0] = in[0];
	out[1] = in[4];
	out[2] = in[8];
	out[3] = in[12];

	out[4] = in[1];
	out[5] = in[5];
	out[6] = in[9];
	out[7] = in[13];

	out[8] = in[2];
	out[9] = in[6];
	out[10] = in[10];
	out[11] = in[14];

	out[12] = in[3];
	out[13] = in[7];
	out[14] = in[11];
	out[15] = in[15];
}

inline void hkgMat4FloatToDouble( double out[16], const float in[16])
{
	for (int i = 0; i < 16; ++i)
		out[i] = (double)in[i];
}

inline void hkgMat4DoubleToFloat( float out[16], const double in[16])
{
	for (int i = 0; i < 16; ++i)
		out[i] = (float)in[i];
}

inline void hkgMat4Copy( float out[16], const float in[16] )
{
	hkString::memCpy( out, in, sizeof(float)*16 );
}

inline void  hkgMat4Mult( float out[16], const float a[16], const float b[16])
{
	for( int c = 0; c < 4; ++c)
	{
		const int col = c*4;
		for( int r = 0; r < 4; ++r)
		{	
			const float ab0 = a[r]*b[col];
			const float ab1 = a[r+4]*b[col + 1]; 
			const float ab2 = a[r+8]*b[col + 2];
			const float ab3 = a[r+12]*b[col + 3];
			out[col + r] = ab0 + ab1 + ab2 + ab3;
		}
	}
}

inline void  hkgMat4MultD( double out[16], const double a[16], const double b[16])
{
	for( int c = 0; c < 4; ++c)
	{
		const int col = c*4;
		for( int r = 0; r < 4; ++r)
		{
			const double ab0 = a[r]*b[col];
			const double ab1 = a[r+4]*b[col + 1]; 
			const double ab2 = a[r+8]*b[col + 2];
			const double ab3 = a[r+12]*b[col + 3];
			out[col + r] = ab0 + ab1 + ab2 + ab3;
		}
	}
}

//
// Color
//
inline float hkgColor4GetRed( unsigned int argb )
{
	return ((argb & 0x00ff0000) >> 16) / 255.0f;
}

inline float hkgColor4GetGreen( unsigned int argb )
{
	return ((argb & 0x0000ff00) >> 8) / 255.0f;
}

inline float hkgColor4GetBlue( unsigned int argb )
{
	return (argb & 0x000000ff) / 255.0f;
}

inline float hkgColor4GetAlpha( unsigned int argb )
{
	return ((argb & 0xff000000) >> 24) / 255.0f;
}

inline unsigned int hkgColor4FromVec4( const float c[4] )
{
	return  (((unsigned int)(c[3] * 255.0f)) << 24 ) |
			(((unsigned int)(c[0] * 255.0f)) << 16 ) |
			(((unsigned int)(c[1] * 255.0f)) << 8  ) |
			 ((unsigned int)(c[2] * 255.0f));
}

inline unsigned int hkgColor4FromVec3( const float c[3] )
{
	return    ( 0xff000000 ) |
			(((unsigned int)(c[0] * 255.0f)) << 16 ) |
			(((unsigned int)(c[1] * 255.0f)) << 8  ) |
			 ((unsigned int)(c[2] * 255.0f));
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
