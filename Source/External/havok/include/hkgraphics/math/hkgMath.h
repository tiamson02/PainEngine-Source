/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_GRAPHICS_MATH_H
#define HK_GRAPHICS_MATH_H

#include <float.h>
	
#if defined(HK_PLATFORM_PS2)
#	include <fastmath.h>
#else 
#	include <math.h>
#endif


#define HKG_X 0
#define HKG_Y 1
#define HKG_Z 2
#define HKG_W 3

#define HKG_A 0
#define HKG_B 1
#define HKG_C 2
#define HKG_D 3


// FLT_DIG;	# of decimal digits of precision 
// FLT_EPSILON;	 smallest such that 1.0+FLT_EPSILON != 1.0 
// FLT_MANT_DIG;# of bits in mantissa 
// FLT_MAX;	 max value 
// FLT_MAX_10_EXP;	max decimal exponent 
// FLT_MAX_EXP;		max binary exponent 
// FLT_MIN;	 min positive value 
// FLT_MIN_10_EXP;	min decimal exponent 
// FLT_MIN_EXP;	min binary exponent 
// FLT_RADIX;  exponent radix 

const float HKG_PI = 3.14159265358979f;

// must use double intermediate since int may overflow float
#define HKG_RAND_01 ((float) (rand() / (double)RAND_MAX))
#define HKG_MIN(x,y) ((x)<(y)? (x) : (y))
#define HKG_MAX(x,y) ((x)>(y)? (x) : (y))


/************************************************************************/

#if defined(HK_PLATFORM_PS2)

	inline int   hkg_finite(float x)		{ return finitef(x); }
	inline float hkg_sqrt(float x)		{ __asm__ ("sqrt.s %0, %0" : "+f" (x) : ); return x; }
	inline float hkg_fabs(float x)		{ __asm__ ("abs.s  %0, %0" : "+f" (x) : ); return x; }
	inline float hkg_sin(float x)		{ return sinf(x); }
	inline float hkg_cos(float x)		{ return cosf(x); }
	inline float hkg_tan(float x)		{ return tanf(x); }
	inline float hkg_asin(float x)		{ return asinf(x); }
	inline float hkg_acos(float x)		{ return acosf(x); }
	inline float hkg_pow(float x,float y)	{ return powf(x,y); }
	inline float hkg_floor(float x)		{ return floorf(x); }
	inline float hkg_ceil(float x)		{ return ceilf(x); }

#elif defined (GEKKO)

	inline float hkg_sqrt(float x)		{ return sqrtf(x); }
	inline float hkg_fabs(float x)		{ return fabsf(x); }
	inline float hkg_sin(float x)		{ return sinf(x); }
	inline float hkg_cos(float x)		{ return cosf(x); }
	inline float hkg_tan(float x)		{ return tanf(x); }
	inline float hkg_asin(float x)		{ return asinf(x); }
	inline float hkg_acos(float x)		{ return acosf(x); }
	inline float hkg_pow(float x,float y) { return powf(x,y); }
	inline float hkg_floor(float x)		{ return floorf(x); }
	inline float hkg_ceil(float x)		{ return ceilf(x); }

#	if defined(__MWERKS__)
		inline float hkg_finite(float x)	{ return isfinite(x); }
#	else
		inline float hkg_finite(float x) { return finite(x); }
#	endif

#else

#	define hkg_sqrt(x)		float(sqrt((float)x))
#	define hkg_fabs(x)		float(fabs((float)x))
#	define hkg_sin(x)		float(sin((float)x))
#	define hkg_cos(x)		float(cos((float)x))
#	define hkg_tan(x)		float(tan((float)x))
#	define hkg_asin(x)		float(asin((float)x))
#	define hkg_acos(x)		float(acos((float)x))
#	define hkg_pow(x,y)		float(pow((float)x,y))
#	define hkg_floor(x)		float(floor((float)x))
#	define hkg_ceil(x)		float(ceil((float)x))
#	if ( defined(__POWERPC__) || defined(_M_PPC) ) && defined(__MWERKS__)
#		define hkg_finite(x) isfinite(x)
#	elif defined(_MSC_VER)
#		define hkg_finite(x) _finite(x)
#	else
#		define hkg_finite(x) finite(x)
#	endif
#endif


// Extra math ops

/// Return the absolute value of the integer x,
inline int hkg_abs(int x);

/// Return the clamped value of the float x, to the limits of l (lowest value) and h (highest value).
inline float hkg_clampf(float x, float l, float h);

/// Return the clamped value of the integer x, to the limits of l (lowest value) and h (highest value).
inline int hkg_clampi(int x, int l, int h);


// Vectors with 3 components

/// Returns the length of 3D vector 'v'. Uses hkg_sqrt().
inline float hkgVec3Length( const float v[3] );

/// Returns the length squared of 3D vector 'v'. 
inline float hkgVec3LengthSqrd( const float v[3] );

/// Normalize a 3D vector 'v'. Returns the length that was divided in, and alters 'v'.
inline float hkgVec3Normalize( float v[3] );

/// Mutiply a 3D vector 'a' by a scaler 'scale' and place the result in the 3D vector 'out'.
inline void  hkgVec3Mult( float out[3], const float a[3], float scale ); 

/// Divide a 3D vector 'a' by a scaler 'scale' and place the result in the 3D vector 'out'.
inline void  hkgVec3Div( float out[3], const float a[3], float scale ); 

/// Inplace scale the 3D vector 'a' by the scalar 'scale'.
inline void  hkgVec3Scale( float a[3], float scale ); 

/// Inplace divide the 3D vector 'a' by the scalar 'scale'.
inline void  hkgVec3Div( float a[3], float scale ); 

/// Subtract the 3D vector 'b' from the vector 'a' and place the result in 'out'.
inline void  hkgVec3Sub( float out[3], const float a[3], const float b[3]); 

/// Add the 3D vector 'a' to the vector 'a' and place the result in 'out'.
inline void  hkgVec3Add( float out[3], const float a[3], const float b[3]);

/// Inplace subtract the 3D vector 'a' from 'inout'.
inline void  hkgVec3Sub( float inout[3], const float a[3]); 

/// Inplace add the 3D vector 'a' to 'inout'.
inline void  hkgVec3Add( float inout[3], const float a[3]);

/// Test for the 3 components of vector 'a' being within tolerance of the corresponding components of 'b'.
inline bool  hkgVec3Equals( const float a[3], const float b[3], float tolerance = FLT_MIN);

/// Compute the Cross Product of vector 'a' cross 'b' and place the result in 'out'. Note that 'out' must not be either 'a' or 'b'.
inline void  hkgVec3Cross( float out[3], const float a[3], const float b[3]); // out != a or b

/// Return the Dot Product of the 3D vector 'a' dot 'b'.
inline float hkgVec3Dot( const float a[3], const float b[3]); 

/// Copy the 3D vector 'a' into the vector 'out'. Uses hkString::memCpy().
inline void  hkgVec3Copy( float out[3], const float a[3] ); 

/// Inplace rotate the 3D vector (3 component) 'inout' by the upper 3x3 rotational portion of the 4x4 transform 'r'. 
/// Will create a local temp variable to store the result during the mutiplication.
inline void  hkgVec3Rotate( float inout[3], const float r[16] );

/// Transform the 3D vector (3 component) 'in' by the 4x4 transform 't'.
inline void  hkgVec3Transform( float out[3], const float in[3], const float t[16] );

/// Zero the 3 components of the 3D vector 'v'. Uses hkString::memSet().
inline void  hkgVec3Zero( float v[3] ); 

/// Clamp the 3 float values in the 3D vector 'v' to the range [lower,upper] inclusive.
inline void  hkgVec3Clamp( float v[3], float lower, float upper); 


// Vectors with 4 components

/// Zero all 4 components (so includes w) of the homegenous 3D vector 'v'. Uses hkString::memSet().
inline void  hkgVec4Zero( float inout[4] );

/// Copy all 4 comppnerts (so includes w) of the homegenous 3D vector 'in' into the vector 'out'. Uses hkString::memCpy().
inline void  hkgVec4Copy( float out[4], const float in[4] );

/// Transform the 3D vector (4 component) 'in' by the 4x4 transform 't'.
inline void  hkgVec4Transform( float out[4], const float in[4], const float t[16] ); // out != in

/// Transform the 3D vector (4 component) 'in' by the 4x4 transform 't', where t,in, and out, are double precission floats.
inline void  hkgVec4TransformD( double out[4], const double in[4], const double t[16] ); // out != in


// Quaternions

/// Make the quaternion 'q' the identity. In this case it will set the real part w( which is q[3], or q[HKG_W] ) to 0, and
/// the (X,Y,Z) portion to (0,1,0) (which is q[0],q[1],q[2]).
inline void  hkgQuatIdentity( float q[4] ); 

/// Construct the quaternion 'q' from the given 'angle' (in radians) about 'axis'.
inline void  hkgQuatFromAngleAxis( float q[4], const float angle, const float axis[3] ); 

/// Rotate the 3D vector 'v' (3 components) by the unit quaternion 'q', and place the result in the vector 'out'.
inline void  hkgQuatRotateVec3( float out[3], const float q[4], const float v[3] ); 

/// Inplace normalize the quaternion 'q' so that it is of unit length.
inline void  hkgQuatNormalize( float q[4] );

/// Concat two quaternions together
inline void  hkgQuatMultiply( float out[4], const float q[4], const float r[4]);

// Planes

/// Construct a plane of the form [Ax + By + Cz + D = 0], stored in 'plane' in that order (A == [0] etc) from
/// the set of 3 points 'a', 'b', and 'c' so that all 3 are on the plane.
inline void  hkgPlaneFrom3Points( float plane[4], const float a[3], const float b[3], const float c[3] ); 
		
/// Construct the 4x4 matrix 'm' so that it is the identity matrix. Matrices are column major, of an OpenGL convention. Uses hkgMat4Zero() first.
inline void  hkgMat4Identity( float m[16] );

/// Construct the 4x4 matrix 'm' so that it is the identity matrix, where 'm' is double precission. Uses hkgMat4Zero() first.
inline void  hkgMat4IdentityD( double m[16] );

/// Construct the 4x4 matrix 'm' so that it is the zero matrix (uses hkString::memSet()).
inline void  hkgMat4Zero( float m[16] );

/// Construct the 4x4 matrix 'm' so that it is the zero matrix (uses hkString::memSet()), where 'm' is double prescission.
inline void  hkgMat4ZeroD( double m[16] );

/// Transpose the 4x4 matrix 'in' and place the result into 'out'. The two matrices must not be the same variable.
inline void  hkgMat4Transpose( float out[16], const float in[16] );

/// Copy the 4x4 matrix 'in' into the matrix 'out'. Uses hkString::memCpy().
inline void  hkgMat4Copy( float out[16], const float in[16] );

/// Rotate the 3D vector 'offset' by the rotational part of 'inout' and then add the 
/// resultant vector to the translational portion of 'inout'. This can be thought of
/// as the effect of a local offset on a body transform for instance.
inline void  hkgMat4RotateAndAdd( float inout[16], const float offset[3] );

/// Multiply the 4x4 matrix 'a' by the matrix 'b' and place the result in 'out'. Note that a and/or b can't be the same matrix pointer as 'out' (but a can be b if desired).
inline void  hkgMat4Mult( float out[16], const float a[16], const float b[16]); 

/// Multiply the 4x4 matrix 'a' by the matrix 'b' and place the result in 'out', where the 3 matrices are double precission. Note that a and/or b can't be the same matrix pointer as 'out' (but a can be b if desired).
inline void  hkgMat4MultD( double out[16], const double a[16], const double b[16]); 

/// Assuming that the 4x4 matrix 'in' is quick interable (ie: that it is a rigid transform
/// such that the transpose of the upper 3x3 rotation is its inverse rotation and that the 
/// only other members that have non identity parts are the translational elements).
inline void  hkgMat4QuickInvert( float out[16], const float in[16]); 

/// Copy the 4x4 float matrix 'in' into the double precission matrix 'out'
inline void  hkgMat4FloatToDouble( double out[16], const float in[16]); 

/// Copy the 4x4 double precission matrix 'in' into the float matrix 'out'
inline void  hkgMat4DoubleToFloat( float out[16], const double in[16]); 

/// Get the red component as a value from 0.0f to 1.0f from the packed ARGB color 'argb'
inline float hkgColor4GetRed( unsigned int argb );

/// Get the green component as a value from 0.0f to 1.0f from the packed ARGB color 'argb'
inline float hkgColor4GetGreen( unsigned int argb );

/// Get the blue component as a value from 0.0f to 1.0f from the packed ARGB color 'argb'
inline float hkgColor4GetBlue( unsigned int argb );

/// Get the alpha component as a value from 0.0f to 1.0f from the packed ARGB color 'argb'
inline float hkgColor4GetAlpha( unsigned int argb );

/// Pack a 4 component color vector of floats in the range 0.0f to 1.0f into a 32 bit ARGB value.
inline unsigned int hkgColor4FromVec4( const float c[4] ); 

/// Pack a 3 component color vector of floats in the range 0.0f to 1.0f into a 32 bit ARGB value, assuming alpha is 1.0f.
inline unsigned int hkgColor4FromVec3( const float c[3] ); 

// Handy constants

extern const float HKG_VEC3_ZERO[3];
extern const float HKG_VEC3_ONE[3];

extern const float HKG_VEC4_ZERO[4];
extern const float HKG_VEC4_ONE[4];

extern const float HKG_VEC3_X[3];
extern const float HKG_VEC3_Y[3];
extern const float HKG_VEC3_Z[3];

extern const float HKG_VEC4_X[4];
extern const float HKG_VEC4_Y[4];
extern const float HKG_VEC4_Z[4];
extern const float HKG_VEC4_W[4];

#include <hkgraphics/math/hkgMath.inl>

#endif


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
