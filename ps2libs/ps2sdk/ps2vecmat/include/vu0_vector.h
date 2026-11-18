/*           Copyright (C) 2003 Sony Computer Entertainment America               
                             All Rights Reserved                                  
                               SCEA Confidential                                */

#ifndef vu0_vector_h
#define vu0_vector_h

#include <stdio.h>
#include <math.h>

#ifdef __GNUC__
#include <vu0intrin.h>
#include <mmintrin.h>
#else
#include <vu0_mmi_types.h>
#endif

/********************************************
 * 128-bit integer:
 * used to cast ints to 128-bit data for multimedia intrinsics.
 */
 
typedef unsigned int u_int128 __attribute__ ((mode (TI)));

/********************************************
 * Forward Declarations
 */

class vu0_vec_xyz;
class vu0_vec_xyzw;
class vu0_vector_t;
class vu0_point_t;
class vu0_vec_x;
class vu0_vec_y;
class vu0_vec_z;
class vu0_vec_w;
class core_vec_xyz;
class core_vec_xyzw;
class vu0_mat_33;

/********************************************
 * Vector Class Declarations
 */

class vu0_vec_xyz
{
   public:
      __v4sf vec128;

      inline vu0_vec_xyz( ) { }
      inline vu0_vec_xyz( __v4sf rhs );
      inline vu0_vec_xyz( float rhs );
      inline vu0_vec_xyz( float x, float y, float z );

      explicit inline vu0_vec_xyz( const core_vec_xyz& vec );
      explicit inline vu0_vec_xyz( vu0_vec_x rhs );
      explicit inline vu0_vec_xyz( vu0_vec_y rhs );
      explicit inline vu0_vec_xyz( vu0_vec_z rhs );
      explicit inline vu0_vec_xyz( vu0_vec_w rhs );
      explicit inline vu0_vec_xyz( vu0_vec_xyzw rhs );
      explicit inline vu0_vec_xyz( vu0_vector_t rhs );
      explicit inline vu0_vec_xyz( vu0_point_t rhs );

      // get and set

      inline __v4sf get128() const;

      inline void set( __v4sf rhs );
      inline void set( vu0_vec_xyz rhs );
      inline void set( vu0_vec_x rhs );
      inline void set( vu0_vec_y rhs );
      inline void set( vu0_vec_z rhs );
      inline void set( vu0_vec_w rhs );
      inline void set( vu0_vec_xyzw rhs );
      inline void set( vu0_vector_t rhs );
      inline void set( vu0_point_t rhs );

      // set all elements to float

      inline void set( float rhs );

      // set from floats

      inline void set( float x, float y, float z );

      // set elements to zero

      inline void set_zero();

      // get and set elements:
      // can get elements as floats because of implicit conversions.

      inline vu0_vec_x get_x() const;
      inline vu0_vec_y get_y() const;
      inline vu0_vec_z get_z() const;
      inline void set_x( vu0_vec_x rhs );
      inline void set_x( vu0_vec_y rhs );
      inline void set_x( vu0_vec_z rhs );
      inline void set_x( vu0_vec_w rhs );
      inline void set_x( float rhs );
      inline void set_y( vu0_vec_x rhs );
      inline void set_y( vu0_vec_y rhs );
      inline void set_y( vu0_vec_z rhs );
      inline void set_y( vu0_vec_w rhs );
      inline void set_y( float rhs );
      inline void set_z( vu0_vec_x rhs );
      inline void set_z( vu0_vec_y rhs );
      inline void set_z( vu0_vec_z rhs );
      inline void set_z( vu0_vec_w rhs );
      inline void set_z( float rhs );

      inline vu0_vec_xyz& operator = ( vu0_vec_xyz rhs );
      inline vu0_vec_xyz& operator = ( vu0_vec_x rhs );
      inline vu0_vec_xyz& operator = ( vu0_vec_y rhs );
      inline vu0_vec_xyz& operator = ( vu0_vec_z rhs );

      // operations with single element types (and floats) are broadcast operations

      inline vu0_vec_xyz operator + ( vu0_vec_xyz rhs ) const;
      inline vu0_vec_xyz operator + ( vu0_vec_x rhs ) const;
      inline vu0_vec_xyz operator + ( vu0_vec_y rhs ) const;
      inline vu0_vec_xyz operator + ( vu0_vec_z rhs ) const;
      inline vu0_vec_xyz operator + ( vu0_vec_w rhs ) const;
      inline vu0_vec_xyz operator + ( float rhs ) const;

      inline vu0_vec_xyz operator - ( vu0_vec_xyz rhs ) const;
      inline vu0_vec_xyz operator - ( vu0_vec_x rhs ) const;
      inline vu0_vec_xyz operator - ( vu0_vec_y rhs ) const;
      inline vu0_vec_xyz operator - ( vu0_vec_z rhs ) const;
      inline vu0_vec_xyz operator - ( vu0_vec_w rhs ) const;
      inline vu0_vec_xyz operator - ( float rhs ) const;

      inline vu0_vec_xyz operator * ( vu0_vec_xyz rhs ) const;
      inline vu0_vec_xyz operator * ( vu0_vec_x rhs ) const;
      inline vu0_vec_xyz operator * ( vu0_vec_y rhs ) const;
      inline vu0_vec_xyz operator * ( vu0_vec_z rhs ) const;
      inline vu0_vec_xyz operator * ( vu0_vec_w rhs ) const;
      inline vu0_vec_xyz operator * ( float rhs ) const;

      inline vu0_vec_xyz operator / ( vu0_vec_x rhs ) const;
      inline vu0_vec_xyz operator / ( vu0_vec_y rhs ) const;
      inline vu0_vec_xyz operator / ( vu0_vec_z rhs ) const;
      inline vu0_vec_xyz operator / ( vu0_vec_w rhs ) const;
      inline vu0_vec_xyz operator / ( float rhs ) const;

      inline void operator += ( vu0_vec_xyz rhs );
      inline void operator += ( vu0_vec_x rhs );
      inline void operator += ( vu0_vec_y rhs );
      inline void operator += ( vu0_vec_z rhs );
      inline void operator += ( vu0_vec_w rhs );
      inline void operator += ( float rhs );

      inline void operator -= ( vu0_vec_xyz rhs );
      inline void operator -= ( vu0_vec_x rhs );
      inline void operator -= ( vu0_vec_y rhs );
      inline void operator -= ( vu0_vec_z rhs );
      inline void operator -= ( vu0_vec_w rhs );
      inline void operator -= ( float rhs );

      inline void operator *= ( vu0_vec_xyz rhs );
      inline void operator *= ( vu0_vec_x rhs );
      inline void operator *= ( vu0_vec_y rhs );
      inline void operator *= ( vu0_vec_z rhs );
      inline void operator *= ( vu0_vec_w rhs );
      inline void operator *= ( float rhs );

      inline void operator /= ( vu0_vec_x rhs );
      inline void operator /= ( vu0_vec_y rhs );
      inline void operator /= ( vu0_vec_z rhs );
      inline void operator /= ( vu0_vec_w rhs );
      inline void operator /= ( float rhs );

      // negate

      inline vu0_vec_xyz operator - () const;

      // absolute values

      inline vu0_vec_xyz abs() const;

      // signs of each element, -1.0 or 1.0

      inline vu0_vec_xyz sign() const;

      // element-wise maximums and minimums

      inline vu0_vec_xyz max( vu0_vec_xyz rhs ) const;
      inline vu0_vec_xyz min( vu0_vec_xyz rhs ) const;

      // reciprocals

      inline vu0_vec_xyz recip() const;

      // the maximum or minimum of all elements

      inline vu0_vec_x max() const;
      inline vu0_vec_x min() const;

      // sum of elements

      inline vu0_vec_x sum() const;

      // linear interpolation:
      // blends from "this" to "other" as "alpha" goes from 0 to 1
      // no clipping: does extrapolation when alpha<0 or alpha>1
      
      inline vu0_vec_xyz interpolate( float alpha, vu0_vec_xyz rhs ) const;

      // dot product

      inline vu0_vec_x dot( vu0_vec_xyz rhs ) const;

      inline vu0_vec_x length_sqr() const;
      inline vu0_vec_x length() const;

      // returns this vector normalized

      inline vu0_vec_xyz normalized() const;

      // normalizes this vector

      inline void normalize();

      // cross product

      inline vu0_vec_xyz cross( vu0_vec_xyz rhs ) const;

      // treat vector as a row vector and pre-multiply by matrix

      inline vu0_vec_xyz row_mul( const vu0_mat_33& rhs ) const;

      // return cross-product matrix used in rigid body dynamics

      inline vu0_mat_33 operator ~() const;

      // multiply cross-product matrix by a matrix (for one use, faster than ~ and multiply)

      inline vu0_mat_33 tilde_mul( const vu0_mat_33& rhs ) const;

      inline void print() const;
      inline void print( const char* name ) const;
};

typedef vu0_vec_xyz vu0_vec_3;

class vu0_vec_xyzw
{
   public:
      __v4sf vec128;

      inline vu0_vec_xyzw( ) { }
      inline vu0_vec_xyzw( __v4sf rhs );
      inline vu0_vec_xyzw( float rhs );
      inline vu0_vec_xyzw( float x, float y, float z, float w );

      explicit inline vu0_vec_xyzw( const core_vec_xyzw& vec );
      explicit inline vu0_vec_xyzw( vu0_vec_x rhs );
      explicit inline vu0_vec_xyzw( vu0_vec_y rhs );
      explicit inline vu0_vec_xyzw( vu0_vec_z rhs );
      explicit inline vu0_vec_xyzw( vu0_vec_w rhs );
      explicit inline vu0_vec_xyzw( vu0_vector_t rhs );
      explicit inline vu0_vec_xyzw( vu0_point_t rhs );

      // get and set

      inline __v4sf get128() const;

      inline void set( __v4sf rhs );
      inline void set( vu0_vec_xyzw rhs );
      inline void set( vu0_vec_x rhs );
      inline void set( vu0_vec_y rhs );
      inline void set( vu0_vec_z rhs );
      inline void set( vu0_vec_w rhs );
      inline void set( vu0_vector_t rhs );
      inline void set( vu0_point_t rhs );

      // set all elements to float

      inline void set( float rhs );

      // set from floats

      inline void set( float x, float y, float z, float w );

      // set elements to zero

      inline void set_zero();

      // set to contents of vf0 register: (0,0,0,1)

      inline void set_vf0();

      // get and set elements:
      // can get elements as floats because of implicit conversions.

      inline vu0_vec_x get_x() const;
      inline vu0_vec_y get_y() const;
      inline vu0_vec_z get_z() const;
      inline vu0_vec_w get_w() const;
      inline void set_x( vu0_vec_x rhs );
      inline void set_x( vu0_vec_y rhs );
      inline void set_x( vu0_vec_z rhs );
      inline void set_x( vu0_vec_w rhs );
      inline void set_x( float rhs );
      inline void set_y( vu0_vec_x rhs );
      inline void set_y( vu0_vec_y rhs );
      inline void set_y( vu0_vec_z rhs );
      inline void set_y( vu0_vec_w rhs );
      inline void set_y( float rhs );
      inline void set_z( vu0_vec_x rhs );
      inline void set_z( vu0_vec_y rhs );
      inline void set_z( vu0_vec_z rhs );
      inline void set_z( vu0_vec_w rhs );
      inline void set_z( float rhs );
      inline void set_w( vu0_vec_x rhs );
      inline void set_w( vu0_vec_y rhs );
      inline void set_w( vu0_vec_z rhs );
      inline void set_w( vu0_vec_w rhs );
      inline void set_w( float rhs );

      inline vu0_vec_xyzw& operator = ( vu0_vec_xyz rhs );
      inline vu0_vec_xyzw& operator = ( vu0_vec_xyzw rhs );
      inline vu0_vec_xyzw& operator = ( vu0_vector_t rhs );
      inline vu0_vec_xyzw& operator = ( vu0_point_t rhs );
      inline vu0_vec_xyzw& operator = ( vu0_vec_x rhs );
      inline vu0_vec_xyzw& operator = ( vu0_vec_y rhs );
      inline vu0_vec_xyzw& operator = ( vu0_vec_z rhs );
      inline vu0_vec_xyzw& operator = ( vu0_vec_w rhs );

      // operations with single element types (and floats) are broadcast operations

      inline vu0_vec_xyzw operator + ( vu0_vec_xyzw rhs ) const;
      inline vu0_vec_xyzw operator + ( vu0_vec_x rhs ) const;
      inline vu0_vec_xyzw operator + ( vu0_vec_y rhs ) const;
      inline vu0_vec_xyzw operator + ( vu0_vec_z rhs ) const;
      inline vu0_vec_xyzw operator + ( vu0_vec_w rhs ) const;
      inline vu0_vec_xyzw operator + ( float rhs ) const;

      inline vu0_vec_xyzw operator - ( vu0_vec_xyzw rhs ) const;
      inline vu0_vec_xyzw operator - ( vu0_vec_x rhs ) const;
      inline vu0_vec_xyzw operator - ( vu0_vec_y rhs ) const;
      inline vu0_vec_xyzw operator - ( vu0_vec_z rhs ) const;
      inline vu0_vec_xyzw operator - ( vu0_vec_w rhs ) const;
      inline vu0_vec_xyzw operator - ( float rhs ) const;

      inline vu0_vec_xyzw operator * ( vu0_vec_xyzw rhs ) const;
      inline vu0_vec_xyzw operator * ( vu0_vec_x rhs ) const;
      inline vu0_vec_xyzw operator * ( vu0_vec_y rhs ) const;
      inline vu0_vec_xyzw operator * ( vu0_vec_z rhs ) const;
      inline vu0_vec_xyzw operator * ( vu0_vec_w rhs ) const;
      inline vu0_vec_xyzw operator * ( float rhs ) const;

      inline vu0_vec_xyzw operator / ( vu0_vec_x rhs ) const;
      inline vu0_vec_xyzw operator / ( vu0_vec_y rhs ) const;
      inline vu0_vec_xyzw operator / ( vu0_vec_z rhs ) const;
      inline vu0_vec_xyzw operator / ( vu0_vec_w rhs ) const;
      inline vu0_vec_xyzw operator / ( float rhs ) const;

      inline void operator += ( vu0_vec_xyzw rhs );
      inline void operator += ( vu0_vec_x rhs );
      inline void operator += ( vu0_vec_y rhs );
      inline void operator += ( vu0_vec_z rhs );
      inline void operator += ( vu0_vec_w rhs );
      inline void operator += ( float rhs );

      inline void operator -= ( vu0_vec_xyzw rhs );
      inline void operator -= ( vu0_vec_x rhs );
      inline void operator -= ( vu0_vec_y rhs );
      inline void operator -= ( vu0_vec_z rhs );
      inline void operator -= ( vu0_vec_w rhs );
      inline void operator -= ( float rhs );

      inline void operator *= ( vu0_vec_xyzw rhs );
      inline void operator *= ( vu0_vec_x rhs );
      inline void operator *= ( vu0_vec_y rhs );
      inline void operator *= ( vu0_vec_z rhs );
      inline void operator *= ( vu0_vec_w rhs );
      inline void operator *= ( float rhs );

      inline void operator /= ( vu0_vec_x rhs );
      inline void operator /= ( vu0_vec_y rhs );
      inline void operator /= ( vu0_vec_z rhs );
      inline void operator /= ( vu0_vec_w rhs );
      inline void operator /= ( float rhs );

      // negate

      inline vu0_vec_xyzw operator - () const;

      // absolute values

      inline vu0_vec_xyzw abs() const;

      // signs of each element, -1.0 or 1.0

      inline vu0_vec_xyzw sign() const;

      // element-wise maximums and minimums

      inline vu0_vec_xyzw max( vu0_vec_xyzw rhs ) const;
      inline vu0_vec_xyzw min( vu0_vec_xyzw rhs ) const;

      // reciprocals

      inline vu0_vec_xyzw recip() const;

      // the maximum or minimum of all elements

      inline vu0_vec_x max() const;
      inline vu0_vec_x min() const;

      // sum of elements

      inline vu0_vec_x sum() const;

      // linear interpolation:
      // blends from "this" to "other" as "alpha" goes from 0 to 1
      // no clipping: does extrapolation when alpha<0 or alpha>1
      
      inline vu0_vec_xyzw interpolate( float alpha, vu0_vec_xyzw rhs ) const;

      // dot product

      inline vu0_vec_x dot( vu0_vec_xyzw rhs ) const;

      inline vu0_vec_x length_sqr() const;
      inline vu0_vec_x length() const;

      // returns this vector normalized

      inline vu0_vec_xyzw normalized() const;

      // normalizes this vector

      inline void normalize();

      // cross product

      inline vu0_vec_xyzw cross( vu0_vec_xyzw rhs ) const;

      inline void print() const;
      inline void print( const char* name ) const;
};

typedef vu0_vec_xyzw vu0_vec_4;

class vu0_vector_t
{
   public:
      __v4sf vec128;

      inline vu0_vector_t( ) { }
      inline vu0_vector_t( __v4sf rhs );
      inline vu0_vector_t( float rhs );
      inline vu0_vector_t( float x, float y, float z );

      explicit inline vu0_vector_t( vu0_vec_x rhs );
      explicit inline vu0_vector_t( vu0_vec_y rhs );
      explicit inline vu0_vector_t( vu0_vec_z rhs );
      explicit inline vu0_vector_t( vu0_vec_w rhs );
      explicit inline vu0_vector_t( vu0_vec_xyz rhs );
      explicit inline vu0_vector_t( vu0_point_t rhs );

      // get and set

      inline __v4sf get128() const;

      inline void set( __v4sf rhs );
      inline void set( vu0_vector_t rhs );
      inline void set( vu0_vec_x rhs );
      inline void set( vu0_vec_y rhs );
      inline void set( vu0_vec_z rhs );
      inline void set( vu0_vec_w rhs );
      inline void set( vu0_vec_xyz rhs );
      inline void set( vu0_point_t rhs );

      // set all elements to float

      inline void set( float rhs );

      // set from floats

      inline void set( float x, float y, float z );

      // set elements to zero

      inline void set_zero();

      // get and set elements:
      // can get elements as floats because of implicit conversions.

      inline vu0_vec_x get_x() const;
      inline vu0_vec_y get_y() const;
      inline vu0_vec_z get_z() const;
      inline void set_x( vu0_vec_x rhs );
      inline void set_x( vu0_vec_y rhs );
      inline void set_x( vu0_vec_z rhs );
      inline void set_x( vu0_vec_w rhs );
      inline void set_x( float rhs );
      inline void set_y( vu0_vec_x rhs );
      inline void set_y( vu0_vec_y rhs );
      inline void set_y( vu0_vec_z rhs );
      inline void set_y( vu0_vec_w rhs );
      inline void set_y( float rhs );
      inline void set_z( vu0_vec_x rhs );
      inline void set_z( vu0_vec_y rhs );
      inline void set_z( vu0_vec_z rhs );
      inline void set_z( vu0_vec_w rhs );
      inline void set_z( float rhs );

      inline vu0_vector_t& operator = ( vu0_vec_xyz rhs );
      inline vu0_vector_t& operator = ( vu0_vector_t rhs );
      inline vu0_vector_t& operator = ( vu0_vec_x rhs );
      inline vu0_vector_t& operator = ( vu0_vec_y rhs );
      inline vu0_vector_t& operator = ( vu0_vec_z rhs );

      // operations with single element types (and floats) are broadcast operations

      inline vu0_vector_t operator + ( vu0_vector_t rhs ) const;
      inline vu0_point_t  operator + ( vu0_point_t rhs ) const;
      inline vu0_vector_t operator - ( vu0_vector_t rhs ) const;
      inline vu0_vector_t operator * ( vu0_vector_t rhs ) const;
      inline vu0_vector_t operator * ( vu0_vec_x rhs ) const;
      inline vu0_vector_t operator * ( vu0_vec_y rhs ) const;
      inline vu0_vector_t operator * ( vu0_vec_z rhs ) const;
      inline vu0_vector_t operator * ( vu0_vec_w rhs ) const;
      inline vu0_vector_t operator * ( float rhs ) const;

      inline vu0_vector_t operator / ( vu0_vec_x rhs ) const;
      inline vu0_vector_t operator / ( vu0_vec_y rhs ) const;
      inline vu0_vector_t operator / ( vu0_vec_z rhs ) const;
      inline vu0_vector_t operator / ( vu0_vec_w rhs ) const;
      inline vu0_vector_t operator / ( float rhs ) const;

      inline void operator += ( vu0_vector_t rhs );
      inline void operator -= ( vu0_vector_t rhs );
      inline void operator *= ( vu0_vector_t rhs );
      inline void operator *= ( vu0_vec_x rhs );
      inline void operator *= ( vu0_vec_y rhs );
      inline void operator *= ( vu0_vec_z rhs );
      inline void operator *= ( vu0_vec_w rhs );
      inline void operator *= ( float rhs );

      inline void operator /= ( vu0_vec_x rhs );
      inline void operator /= ( vu0_vec_y rhs );
      inline void operator /= ( vu0_vec_z rhs );
      inline void operator /= ( vu0_vec_w rhs );
      inline void operator /= ( float rhs );

      // negate

      inline vu0_vector_t operator - () const;

      // absolute values

      inline vu0_vector_t abs() const;

      // signs of each element, -1.0 or 1.0

      inline vu0_vector_t sign() const;

      // element-wise maximums and minimums

      inline vu0_vector_t max( vu0_vector_t rhs ) const;
      inline vu0_vector_t min( vu0_vector_t rhs ) const;

      // the maximum or minimum of all elements

      inline vu0_vec_x max() const;
      inline vu0_vec_x min() const;

      // sum of elements

      inline vu0_vec_x sum() const;

      // linear interpolation:
      // blends from "this" to "other" as "alpha" goes from 0 to 1
      // no clipping: does extrapolation when alpha<0 or alpha>1
      
      inline vu0_vector_t interpolate( float alpha, vu0_vector_t rhs ) const;

      // dot product

      inline vu0_vec_x dot( vu0_vector_t rhs ) const;

      inline vu0_vec_x length_sqr() const;
      inline vu0_vec_x length() const;

      // returns this vector normalized

      inline vu0_vector_t normalized() const;

      // normalizes this vector

      inline void normalize();

      // cross product

      inline vu0_vector_t cross( vu0_vector_t rhs ) const;

      inline void print() const;
      inline void print( const char* name ) const;
};

class vu0_point_t
{
   public:
      __v4sf vec128;

      inline vu0_point_t( ) { }
      inline vu0_point_t( __v4sf rhs );
      inline vu0_point_t( float rhs );
      inline vu0_point_t( float x, float y, float z );

      explicit inline vu0_point_t( vu0_vec_x rhs );
      explicit inline vu0_point_t( vu0_vec_y rhs );
      explicit inline vu0_point_t( vu0_vec_z rhs );
      explicit inline vu0_point_t( vu0_vec_w rhs );
      explicit inline vu0_point_t( vu0_vec_xyz rhs );
      explicit inline vu0_point_t( vu0_vector_t rhs );

      // get and set

      inline __v4sf get128() const;

      inline void set( __v4sf rhs );
      inline void set( vu0_point_t rhs );
      inline void set( vu0_vec_x rhs );
      inline void set( vu0_vec_y rhs );
      inline void set( vu0_vec_z rhs );
      inline void set( vu0_vec_w rhs );
      inline void set( vu0_vec_xyz rhs );
      inline void set( vu0_vector_t rhs );

      // set all elements to float

      inline void set( float rhs );

      // set from floats

      inline void set( float x, float y, float z );

      // set elements to zero

      inline void set_zero();

      // set to contents of vf0 register: (0,0,0,1)

      inline void set_vf0();

      // get and set elements:
      // can get elements as floats because of implicit conversions.

      inline vu0_vec_x get_x() const;
      inline vu0_vec_y get_y() const;
      inline vu0_vec_z get_z() const;
      inline void set_x( vu0_vec_x rhs );
      inline void set_x( vu0_vec_y rhs );
      inline void set_x( vu0_vec_z rhs );
      inline void set_x( vu0_vec_w rhs );
      inline void set_x( float rhs );
      inline void set_y( vu0_vec_x rhs );
      inline void set_y( vu0_vec_y rhs );
      inline void set_y( vu0_vec_z rhs );
      inline void set_y( vu0_vec_w rhs );
      inline void set_y( float rhs );
      inline void set_z( vu0_vec_x rhs );
      inline void set_z( vu0_vec_y rhs );
      inline void set_z( vu0_vec_z rhs );
      inline void set_z( vu0_vec_w rhs );
      inline void set_z( float rhs );

      inline vu0_point_t& operator = ( vu0_vec_xyz rhs );
      inline vu0_point_t& operator = ( vu0_point_t rhs );
      inline vu0_point_t& operator = ( vu0_vec_x rhs );
      inline vu0_point_t& operator = ( vu0_vec_y rhs );
      inline vu0_point_t& operator = ( vu0_vec_z rhs );

      // operations with single element types (and floats) are broadcast operations

      inline vu0_point_t  operator + ( vu0_vector_t rhs ) const;
      inline vu0_vector_t operator - ( vu0_point_t rhs ) const;
      inline vu0_point_t  operator - ( vu0_vector_t rhs ) const;

      inline void operator += ( vu0_vector_t rhs );
      inline void operator -= ( vu0_vector_t rhs );

      // negate

      inline vu0_point_t operator - () const;

      // absolute values

      inline vu0_point_t abs() const;

      // signs of each element, -1.0 or 1.0

      inline vu0_point_t sign() const;

      // element-wise maximums and minimums

      inline vu0_point_t max( vu0_point_t rhs ) const;
      inline vu0_point_t min( vu0_point_t rhs ) const;

      // the maximum or minimum of all elements

      inline vu0_vec_x max() const;
      inline vu0_vec_x min() const;

      // sum of elements

      inline vu0_vec_x sum() const;

      // dot product

      inline vu0_vec_x dot( vu0_vector_t rhs ) const;

      inline vu0_vec_x distance_sqr_from_origin() const;
      inline vu0_vec_x distance_from_origin() const;

      inline vu0_vec_x distance_sqr_from( vu0_point_t rhs ) const;
      inline vu0_vec_x distance_from( vu0_point_t rhs ) const;

      inline vu0_point_t midpoint( vu0_point_t rhs ) const;

      // linear interpolation:
      // blends from "this" to "other" as "alpha" goes from 0 to 1
      // no clipping: does extrapolation when alpha<0 or alpha>1
      
      inline vu0_point_t interpolate( float alpha, vu0_point_t rhs ) const;

      inline void print() const;
      inline void print( const char* name ) const;
};

class vu0_vec_x
{
   public:
      __v4sf vec128;

      inline vu0_vec_x( ) { }
      inline vu0_vec_x( __v4sf rhs );
      inline vu0_vec_x( float rhs );

      explicit inline vu0_vec_x( vu0_vec_y rhs );
      explicit inline vu0_vec_x( vu0_vec_z rhs );
      explicit inline vu0_vec_x( vu0_vec_w rhs );
      explicit inline vu0_vec_x( vu0_vec_xyz rhs );
      explicit inline vu0_vec_x( vu0_vec_xyzw rhs );
      explicit inline vu0_vec_x( vu0_vector_t rhs );
      explicit inline vu0_vec_x( vu0_point_t rhs );

      // get and set

      inline operator float() const;

      inline __v4sf get128() const;

      inline void set( __v4sf rhs );
      inline void set( vu0_vec_x rhs );
      inline void set( vu0_vec_y rhs );
      inline void set( vu0_vec_z rhs );
      inline void set( vu0_vec_w rhs );
      inline void set( vu0_vec_xyz rhs );
      inline void set( vu0_vec_xyzw rhs );
      inline void set( vu0_vector_t rhs );
      inline void set( vu0_point_t rhs );

      // set all elements to float

      inline void set( float rhs );

      // set elements to zero

      inline void set_zero();

      inline vu0_vec_x& operator = ( vu0_vec_x rhs );

      // operations with single element types (and floats) are broadcast operations

      inline vu0_vec_x operator + ( vu0_vec_x rhs ) const;
      inline vu0_vec_x operator + ( vu0_vec_y rhs ) const;
      inline vu0_vec_x operator + ( vu0_vec_z rhs ) const;
      inline vu0_vec_x operator + ( vu0_vec_w rhs ) const;
      inline vu0_vec_x operator + ( float rhs ) const;

      inline vu0_vec_x operator - ( vu0_vec_x rhs ) const;
      inline vu0_vec_x operator - ( vu0_vec_y rhs ) const;
      inline vu0_vec_x operator - ( vu0_vec_z rhs ) const;
      inline vu0_vec_x operator - ( vu0_vec_w rhs ) const;
      inline vu0_vec_x operator - ( float rhs ) const;

      inline vu0_vec_x operator * ( vu0_vec_x rhs ) const;
      inline vu0_vec_x operator * ( vu0_vec_y rhs ) const;
      inline vu0_vec_x operator * ( vu0_vec_z rhs ) const;
      inline vu0_vec_x operator * ( vu0_vec_w rhs ) const;
      inline vu0_vec_x operator * ( float rhs ) const;

      inline vu0_vec_x operator / ( vu0_vec_x rhs ) const;
      inline vu0_vec_x operator / ( vu0_vec_y rhs ) const;
      inline vu0_vec_x operator / ( vu0_vec_z rhs ) const;
      inline vu0_vec_x operator / ( vu0_vec_w rhs ) const;
      inline vu0_vec_x operator / ( float rhs ) const;

      inline void operator += ( vu0_vec_x rhs );
      inline void operator += ( vu0_vec_y rhs );
      inline void operator += ( vu0_vec_z rhs );
      inline void operator += ( vu0_vec_w rhs );
      inline void operator += ( float rhs );

      inline void operator -= ( vu0_vec_x rhs );
      inline void operator -= ( vu0_vec_y rhs );
      inline void operator -= ( vu0_vec_z rhs );
      inline void operator -= ( vu0_vec_w rhs );
      inline void operator -= ( float rhs );

      inline void operator *= ( vu0_vec_x rhs );
      inline void operator *= ( vu0_vec_y rhs );
      inline void operator *= ( vu0_vec_z rhs );
      inline void operator *= ( vu0_vec_w rhs );
      inline void operator *= ( float rhs );

      inline void operator /= ( vu0_vec_x rhs );
      inline void operator /= ( vu0_vec_y rhs );
      inline void operator /= ( vu0_vec_z rhs );
      inline void operator /= ( vu0_vec_w rhs );
      inline void operator /= ( float rhs );

      // negate

      inline vu0_vec_x operator - () const;

      // absolute values

      inline vu0_vec_x abs() const;

      // signs of each element, -1.0 or 1.0

      inline vu0_vec_x sign() const;

      // element-wise maximums and minimums

      inline vu0_vec_x max( vu0_vec_x rhs ) const;
      inline vu0_vec_x min( vu0_vec_x rhs ) const;

      inline void print() const;
      inline void print( const char* name ) const;
};

class vu0_vec_y
{
   public:
      __v4sf vec128;

      inline vu0_vec_y( ) { }
      inline vu0_vec_y( __v4sf rhs );
      inline vu0_vec_y( float rhs );

      explicit inline vu0_vec_y( vu0_vec_x rhs );
      explicit inline vu0_vec_y( vu0_vec_z rhs );
      explicit inline vu0_vec_y( vu0_vec_w rhs );
      explicit inline vu0_vec_y( vu0_vec_xyz rhs );
      explicit inline vu0_vec_y( vu0_vec_xyzw rhs );
      explicit inline vu0_vec_y( vu0_vector_t rhs );
      explicit inline vu0_vec_y( vu0_point_t rhs );

      // get and set

      inline operator float() const;

      inline __v4sf get128() const;

      inline void set( __v4sf rhs );
      inline void set( vu0_vec_y rhs );
      inline void set( vu0_vec_x rhs );
      inline void set( vu0_vec_z rhs );
      inline void set( vu0_vec_w rhs );
      inline void set( vu0_vec_xyz rhs );
      inline void set( vu0_vec_xyzw rhs );
      inline void set( vu0_vector_t rhs );
      inline void set( vu0_point_t rhs );

      // set all elements to float

      inline void set( float rhs );

      // set elements to zero

      inline void set_zero();

      inline vu0_vec_y& operator = ( vu0_vec_y rhs );

      // operations with single element types (and floats) are broadcast operations

      inline vu0_vec_y operator + ( vu0_vec_y rhs ) const;
      inline vu0_vec_y operator + ( vu0_vec_x rhs ) const;
      inline vu0_vec_y operator + ( vu0_vec_z rhs ) const;
      inline vu0_vec_y operator + ( vu0_vec_w rhs ) const;
      inline vu0_vec_y operator + ( float rhs ) const;

      inline vu0_vec_y operator - ( vu0_vec_y rhs ) const;
      inline vu0_vec_y operator - ( vu0_vec_x rhs ) const;
      inline vu0_vec_y operator - ( vu0_vec_z rhs ) const;
      inline vu0_vec_y operator - ( vu0_vec_w rhs ) const;
      inline vu0_vec_y operator - ( float rhs ) const;

      inline vu0_vec_y operator * ( vu0_vec_y rhs ) const;
      inline vu0_vec_y operator * ( vu0_vec_x rhs ) const;
      inline vu0_vec_y operator * ( vu0_vec_z rhs ) const;
      inline vu0_vec_y operator * ( vu0_vec_w rhs ) const;
      inline vu0_vec_y operator * ( float rhs ) const;

      inline vu0_vec_y operator / ( vu0_vec_x rhs ) const;
      inline vu0_vec_y operator / ( vu0_vec_y rhs ) const;
      inline vu0_vec_y operator / ( vu0_vec_z rhs ) const;
      inline vu0_vec_y operator / ( vu0_vec_w rhs ) const;
      inline vu0_vec_y operator / ( float rhs ) const;

      inline void operator += ( vu0_vec_y rhs );
      inline void operator += ( vu0_vec_x rhs );
      inline void operator += ( vu0_vec_z rhs );
      inline void operator += ( vu0_vec_w rhs );
      inline void operator += ( float rhs );

      inline void operator -= ( vu0_vec_y rhs );
      inline void operator -= ( vu0_vec_x rhs );
      inline void operator -= ( vu0_vec_z rhs );
      inline void operator -= ( vu0_vec_w rhs );
      inline void operator -= ( float rhs );

      inline void operator *= ( vu0_vec_y rhs );
      inline void operator *= ( vu0_vec_x rhs );
      inline void operator *= ( vu0_vec_z rhs );
      inline void operator *= ( vu0_vec_w rhs );
      inline void operator *= ( float rhs );

      inline void operator /= ( vu0_vec_x rhs );
      inline void operator /= ( vu0_vec_y rhs );
      inline void operator /= ( vu0_vec_z rhs );
      inline void operator /= ( vu0_vec_w rhs );
      inline void operator /= ( float rhs );

      // negate

      inline vu0_vec_y operator - () const;

      // absolute values

      inline vu0_vec_y abs() const;

      // signs of each element, -1.0 or 1.0

      inline vu0_vec_y sign() const;

      // element-wise maximums and minimums

      inline vu0_vec_y max( vu0_vec_y rhs ) const;
      inline vu0_vec_y min( vu0_vec_y rhs ) const;

      inline void print() const;
      inline void print( const char* name ) const;
};

class vu0_vec_z
{
   public:
      __v4sf vec128;

      inline vu0_vec_z( ) { }
      inline vu0_vec_z( __v4sf rhs );
      inline vu0_vec_z( float rhs );

      explicit inline vu0_vec_z( vu0_vec_x rhs );
      explicit inline vu0_vec_z( vu0_vec_y rhs );
      explicit inline vu0_vec_z( vu0_vec_w rhs );
      explicit inline vu0_vec_z( vu0_vec_xyz rhs );
      explicit inline vu0_vec_z( vu0_vec_xyzw rhs );
      explicit inline vu0_vec_z( vu0_vector_t rhs );
      explicit inline vu0_vec_z( vu0_point_t rhs );

      // get and set

      inline operator float() const;

      inline __v4sf get128() const;

      inline void set( __v4sf rhs );
      inline void set( vu0_vec_z rhs );
      inline void set( vu0_vec_x rhs );
      inline void set( vu0_vec_y rhs );
      inline void set( vu0_vec_w rhs );
      inline void set( vu0_vec_xyz rhs );
      inline void set( vu0_vec_xyzw rhs );
      inline void set( vu0_vector_t rhs );
      inline void set( vu0_point_t rhs );

      // set all elements to float

      inline void set( float rhs );

      // set elements to zero

      inline void set_zero();

      inline vu0_vec_z& operator = ( vu0_vec_z rhs );

      // operations with single element types (and floats) are broadcast operations

      inline vu0_vec_z operator + ( vu0_vec_z rhs ) const;
      inline vu0_vec_z operator + ( vu0_vec_x rhs ) const;
      inline vu0_vec_z operator + ( vu0_vec_y rhs ) const;
      inline vu0_vec_z operator + ( vu0_vec_w rhs ) const;
      inline vu0_vec_z operator + ( float rhs ) const;

      inline vu0_vec_z operator - ( vu0_vec_z rhs ) const;
      inline vu0_vec_z operator - ( vu0_vec_x rhs ) const;
      inline vu0_vec_z operator - ( vu0_vec_y rhs ) const;
      inline vu0_vec_z operator - ( vu0_vec_w rhs ) const;
      inline vu0_vec_z operator - ( float rhs ) const;

      inline vu0_vec_z operator * ( vu0_vec_z rhs ) const;
      inline vu0_vec_z operator * ( vu0_vec_x rhs ) const;
      inline vu0_vec_z operator * ( vu0_vec_y rhs ) const;
      inline vu0_vec_z operator * ( vu0_vec_w rhs ) const;
      inline vu0_vec_z operator * ( float rhs ) const;

      inline vu0_vec_z operator / ( vu0_vec_x rhs ) const;
      inline vu0_vec_z operator / ( vu0_vec_y rhs ) const;
      inline vu0_vec_z operator / ( vu0_vec_z rhs ) const;
      inline vu0_vec_z operator / ( vu0_vec_w rhs ) const;
      inline vu0_vec_z operator / ( float rhs ) const;

      inline void operator += ( vu0_vec_z rhs );
      inline void operator += ( vu0_vec_x rhs );
      inline void operator += ( vu0_vec_y rhs );
      inline void operator += ( vu0_vec_w rhs );
      inline void operator += ( float rhs );

      inline void operator -= ( vu0_vec_z rhs );
      inline void operator -= ( vu0_vec_x rhs );
      inline void operator -= ( vu0_vec_y rhs );
      inline void operator -= ( vu0_vec_w rhs );
      inline void operator -= ( float rhs );

      inline void operator *= ( vu0_vec_z rhs );
      inline void operator *= ( vu0_vec_x rhs );
      inline void operator *= ( vu0_vec_y rhs );
      inline void operator *= ( vu0_vec_w rhs );
      inline void operator *= ( float rhs );

      inline void operator /= ( vu0_vec_x rhs );
      inline void operator /= ( vu0_vec_y rhs );
      inline void operator /= ( vu0_vec_z rhs );
      inline void operator /= ( vu0_vec_w rhs );
      inline void operator /= ( float rhs );

      // negate

      inline vu0_vec_z operator - () const;

      // absolute values

      inline vu0_vec_z abs() const;

      // signs of each element, -1.0 or 1.0

      inline vu0_vec_z sign() const;

      // element-wise maximums and minimums

      inline vu0_vec_z max( vu0_vec_z rhs ) const;
      inline vu0_vec_z min( vu0_vec_z rhs ) const;

      inline void print() const;
      inline void print( const char* name ) const;
};

class vu0_vec_w
{
   public:
      __v4sf vec128;

      inline vu0_vec_w( ) { }
      inline vu0_vec_w( __v4sf rhs );
      inline vu0_vec_w( float rhs );

      explicit inline vu0_vec_w( vu0_vec_x rhs );
      explicit inline vu0_vec_w( vu0_vec_y rhs );
      explicit inline vu0_vec_w( vu0_vec_z rhs );
      explicit inline vu0_vec_w( vu0_vec_xyzw rhs );

      // get and set

      inline operator float() const;

      inline __v4sf get128() const;

      inline void set( __v4sf rhs );
      inline void set( vu0_vec_w rhs );
      inline void set( vu0_vec_x rhs );
      inline void set( vu0_vec_y rhs );
      inline void set( vu0_vec_z rhs );
      inline void set( vu0_vec_xyzw rhs );

      // set all elements to float

      inline void set( float rhs );

      // set elements to zero

      inline void set_zero();

      inline vu0_vec_w& operator = ( vu0_vec_w rhs );

      // operations with single element types (and floats) are broadcast operations

      inline vu0_vec_w operator + ( vu0_vec_w rhs ) const;
      inline vu0_vec_w operator + ( vu0_vec_x rhs ) const;
      inline vu0_vec_w operator + ( vu0_vec_y rhs ) const;
      inline vu0_vec_w operator + ( vu0_vec_z rhs ) const;
      inline vu0_vec_w operator + ( float rhs ) const;

      inline vu0_vec_w operator - ( vu0_vec_w rhs ) const;
      inline vu0_vec_w operator - ( vu0_vec_x rhs ) const;
      inline vu0_vec_w operator - ( vu0_vec_y rhs ) const;
      inline vu0_vec_w operator - ( vu0_vec_z rhs ) const;
      inline vu0_vec_w operator - ( float rhs ) const;

      inline vu0_vec_w operator * ( vu0_vec_w rhs ) const;
      inline vu0_vec_w operator * ( vu0_vec_x rhs ) const;
      inline vu0_vec_w operator * ( vu0_vec_y rhs ) const;
      inline vu0_vec_w operator * ( vu0_vec_z rhs ) const;
      inline vu0_vec_w operator * ( float rhs ) const;

      inline vu0_vec_w operator / ( vu0_vec_x rhs ) const;
      inline vu0_vec_w operator / ( vu0_vec_y rhs ) const;
      inline vu0_vec_w operator / ( vu0_vec_z rhs ) const;
      inline vu0_vec_w operator / ( vu0_vec_w rhs ) const;
      inline vu0_vec_w operator / ( float rhs ) const;

      inline void operator += ( vu0_vec_w rhs );
      inline void operator += ( vu0_vec_x rhs );
      inline void operator += ( vu0_vec_y rhs );
      inline void operator += ( vu0_vec_z rhs );
      inline void operator += ( float rhs );

      inline void operator -= ( vu0_vec_w rhs );
      inline void operator -= ( vu0_vec_x rhs );
      inline void operator -= ( vu0_vec_y rhs );
      inline void operator -= ( vu0_vec_z rhs );
      inline void operator -= ( float rhs );

      inline void operator *= ( vu0_vec_w rhs );
      inline void operator *= ( vu0_vec_x rhs );
      inline void operator *= ( vu0_vec_y rhs );
      inline void operator *= ( vu0_vec_z rhs );
      inline void operator *= ( float rhs );

      inline void operator /= ( vu0_vec_x rhs );
      inline void operator /= ( vu0_vec_y rhs );
      inline void operator /= ( vu0_vec_z rhs );
      inline void operator /= ( vu0_vec_w rhs );
      inline void operator /= ( float rhs );

      // negate

      inline vu0_vec_w operator - () const;

      // absolute values

      inline vu0_vec_w abs() const;

      // signs of each element, -1.0 or 1.0

      inline vu0_vec_w sign() const;

      // element-wise maximums and minimums

      inline vu0_vec_w max( vu0_vec_w rhs ) const;
      inline vu0_vec_w min( vu0_vec_w rhs ) const;

      inline void print() const;
      inline void print( const char* name ) const;
};

inline
vu0_vec_xyz::vu0_vec_xyz( __v4sf rhs )
{
   vec128 = rhs;
}

inline
vu0_vec_xyz::vu0_vec_xyz( float rhs )
{
   set( rhs );
}

inline
vu0_vec_xyz::vu0_vec_xyz( float x, float y, float z )
{
   set( x, y, z );
}

inline
vu0_vec_xyz::vu0_vec_xyz( vu0_vec_x rhs )
{
   set( rhs );
}

inline
vu0_vec_xyz::vu0_vec_xyz( vu0_vec_y rhs )
{
   set( rhs );
}

inline
vu0_vec_xyz::vu0_vec_xyz( vu0_vec_z rhs )
{
   set( rhs );
}

inline
vu0_vec_xyz::vu0_vec_xyz( vu0_vec_w rhs )
{
   set( rhs );
}

inline
vu0_vec_xyz::vu0_vec_xyz( vu0_vec_xyzw rhs )
{
   vec128 = rhs.vec128;
}

inline
vu0_vec_xyz::vu0_vec_xyz( vu0_vector_t rhs )
{
   vec128 = rhs.vec128;
}

inline
vu0_vec_xyz::vu0_vec_xyz( vu0_point_t rhs )
{
   vec128 = rhs.vec128;
}

inline
void
vu0_vec_xyz::set( __v4sf rhs )
{
   vec128 = rhs;
}

inline
void
vu0_vec_xyz::set( vu0_vec_xyz rhs )
{
   vec128 = rhs.vec128;
}

inline
void
vu0_vec_xyz::set( vu0_vec_x rhs )
{
   _vaddbc( vec128, vf_XYZW, vu0_vf0, rhs.vec128, vf_X );
}

inline
void
vu0_vec_xyz::set( vu0_vec_y rhs )
{
   _vaddbc( vec128, vf_XYZW, vu0_vf0, rhs.vec128, vf_Y );
}

inline
void
vu0_vec_xyz::set( vu0_vec_z rhs )
{
   _vaddbc( vec128, vf_XYZW, vu0_vf0, rhs.vec128, vf_Z );
}

inline
void
vu0_vec_xyz::set( vu0_vec_w rhs )
{
   _vaddbc( vec128, vf_XYZW, vu0_vf0, rhs.vec128, vf_W );
}

inline
void
vu0_vec_xyz::set( vu0_vec_xyzw rhs )
{
   vec128 = rhs.vec128;
}

inline
void
vu0_vec_xyz::set( vu0_vector_t rhs )
{
   vec128 = rhs.vec128;
}

inline
void
vu0_vec_xyz::set( vu0_point_t rhs )
{
   vec128 = rhs.vec128;
}

inline
void
vu0_vec_xyz::set( float rhs )
{
   __v4si tmp = (__v4si)_vpromotef( rhs );
   tmp = _pextlw( tmp, tmp );
   vec128 = (__v4sf)_pextlw( tmp, tmp );
}

inline
void
vu0_vec_xyz::set( float x, float y, float z )
{
   __v4si xi = (__v4si)_vpromotef(x);
   __v4si yi = (__v4si)_vpromotef(y);
   __v4si zi = (__v4si)_vpromotef(z);
   vec128 = (__v4sf)_pcpyld( zi, _pextlw( yi, xi ) );
}

inline
void
vu0_vec_xyz::set_zero()
{
   _vsub( vec128, vf_XYZW, vu0_vf0, vu0_vf0 );
}

inline
__v4sf
vu0_vec_xyz::get128() const
{
   return vec128;
}

inline
vu0_vec_xyz&
vu0_vec_xyz::operator = ( vu0_vec_xyz rhs )
{
   vec128 = rhs.vec128;
   return *this;
}

inline
vu0_vec_xyz&
vu0_vec_xyz::operator = ( vu0_vec_x rhs )
{
   _vmove( vec128, vf_X, rhs.vec128 );
   return *this;
}

inline
vu0_vec_xyz&
vu0_vec_xyz::operator = ( vu0_vec_y rhs )
{
   _vmove( vec128, vf_Y, rhs.vec128 );
   return *this;
}

inline
vu0_vec_xyz&
vu0_vec_xyz::operator = ( vu0_vec_z rhs )
{
   _vmove( vec128, vf_Z, rhs.vec128 );
   return *this;
}

inline
vu0_vec_x
vu0_vec_xyz::get_x() const
{
   return vu0_vec_x( *this );
}

inline
vu0_vec_y
vu0_vec_xyz::get_y() const
{
   return vu0_vec_y( *this );
}

inline
vu0_vec_z
vu0_vec_xyz::get_z() const
{
   return vu0_vec_z( *this );
}

inline
void
vu0_vec_xyz::set_x( vu0_vec_x rhs )
{
   _vaddbc( vec128, vf_X, vu0_vf0, rhs.vec128, vf_X );
}

inline
void
vu0_vec_xyz::set_x( vu0_vec_y rhs )
{
   _vaddbc( vec128, vf_X, vu0_vf0, rhs.vec128, vf_Y );
}

inline
void
vu0_vec_xyz::set_x( vu0_vec_z rhs )
{
   _vaddbc( vec128, vf_X, vu0_vf0, rhs.vec128, vf_Z );
}

inline
void
vu0_vec_xyz::set_x( vu0_vec_w rhs )
{
   _vaddbc( vec128, vf_X, vu0_vf0, rhs.vec128, vf_W );
}

inline
void
vu0_vec_xyz::set_x( float rhs )
{
   _vaddf( vec128, vf_X, vu0_vf0, rhs );
}

inline
void
vu0_vec_xyz::set_y( vu0_vec_x rhs )
{
   _vaddbc( vec128, vf_Y, vu0_vf0, rhs.vec128, vf_X );
}

inline
void
vu0_vec_xyz::set_y( vu0_vec_y rhs )
{
   _vaddbc( vec128, vf_Y, vu0_vf0, rhs.vec128, vf_Y );
}

inline
void
vu0_vec_xyz::set_y( vu0_vec_z rhs )
{
   _vaddbc( vec128, vf_Y, vu0_vf0, rhs.vec128, vf_Z );
}

inline
void
vu0_vec_xyz::set_y( vu0_vec_w rhs )
{
   _vaddbc( vec128, vf_Y, vu0_vf0, rhs.vec128, vf_W );
}

inline
void
vu0_vec_xyz::set_y( float rhs )
{
   _vaddf( vec128, vf_Y, vu0_vf0, rhs );
}

inline
void
vu0_vec_xyz::set_z( vu0_vec_x rhs )
{
   _vaddbc( vec128, vf_Z, vu0_vf0, rhs.vec128, vf_X );
}

inline
void
vu0_vec_xyz::set_z( vu0_vec_y rhs )
{
   _vaddbc( vec128, vf_Z, vu0_vf0, rhs.vec128, vf_Y );
}

inline
void
vu0_vec_xyz::set_z( vu0_vec_z rhs )
{
   _vaddbc( vec128, vf_Z, vu0_vf0, rhs.vec128, vf_Z );
}

inline
void
vu0_vec_xyz::set_z( vu0_vec_w rhs )
{
   _vaddbc( vec128, vf_Z, vu0_vf0, rhs.vec128, vf_W );
}

inline
void
vu0_vec_xyz::set_z( float rhs )
{
   _vaddf( vec128, vf_Z, vu0_vf0, rhs );
}

inline
vu0_vec_xyz
vu0_vec_xyz::operator + ( vu0_vec_xyz rhs ) const
{
   __v4sf result;
   _vadd( result, vf_XYZW, vec128, rhs.vec128 );
   return vu0_vec_xyz( result );
}

inline
vu0_vec_xyz
vu0_vec_xyz::operator + ( vu0_vec_x rhs ) const
{
   __v4sf result;
   _vaddbc( result, vf_XYZW, vec128, rhs.vec128, vf_X );
   return vu0_vec_xyz( result );
}

inline
vu0_vec_xyz
vu0_vec_xyz::operator + ( vu0_vec_y rhs ) const
{
   __v4sf result;
   _vaddbc( result, vf_XYZW, vec128, rhs.vec128, vf_Y );
   return vu0_vec_xyz( result );
}

inline
vu0_vec_xyz
vu0_vec_xyz::operator + ( vu0_vec_z rhs ) const
{
   __v4sf result;
   _vaddbc( result, vf_XYZW, vec128, rhs.vec128, vf_Z );
   return vu0_vec_xyz( result );
}

inline
vu0_vec_xyz
vu0_vec_xyz::operator + ( vu0_vec_w rhs ) const
{
   __v4sf result;
   _vaddbc( result, vf_XYZW, vec128, rhs.vec128, vf_W );
   return vu0_vec_xyz( result );
}

inline
vu0_vec_xyz
vu0_vec_xyz::operator + ( float rhs ) const
{
   return *this + vu0_vec_x( rhs );
}

inline
void
vu0_vec_xyz::operator += ( vu0_vec_xyz rhs )
{
   *this = *this + rhs;
}

inline
void
vu0_vec_xyz::operator += ( vu0_vec_x rhs )
{
   *this = *this + rhs;
}

inline
void
vu0_vec_xyz::operator += ( vu0_vec_y rhs )
{
   *this = *this + rhs;
}

inline
void
vu0_vec_xyz::operator += ( vu0_vec_z rhs )
{
   *this = *this + rhs;
}

inline
void
vu0_vec_xyz::operator += ( vu0_vec_w rhs )
{
   *this = *this + rhs;
}

inline
void
vu0_vec_xyz::operator += ( float rhs )
{
   *this = *this + rhs;
}

inline
vu0_vec_xyz
vu0_vec_xyz::operator - ( vu0_vec_xyz rhs ) const
{
   __v4sf result;
   _vsub( result, vf_XYZW, vec128, rhs.vec128 );
   return vu0_vec_xyz( result );
}

inline
vu0_vec_xyz
vu0_vec_xyz::operator - ( vu0_vec_x rhs ) const
{
   __v4sf result;
   _vsubbc( result, vf_XYZW, vec128, rhs.vec128, vf_X );
   return vu0_vec_xyz( result );
}

inline
vu0_vec_xyz
vu0_vec_xyz::operator - ( vu0_vec_y rhs ) const
{
   __v4sf result;
   _vsubbc( result, vf_XYZW, vec128, rhs.vec128, vf_Y );
   return vu0_vec_xyz( result );
}

inline
vu0_vec_xyz
vu0_vec_xyz::operator - ( vu0_vec_z rhs ) const
{
   __v4sf result;
   _vsubbc( result, vf_XYZW, vec128, rhs.vec128, vf_Z );
   return vu0_vec_xyz( result );
}

inline
vu0_vec_xyz
vu0_vec_xyz::operator - ( vu0_vec_w rhs ) const
{
   __v4sf result;
   _vsubbc( result, vf_XYZW, vec128, rhs.vec128, vf_W );
   return vu0_vec_xyz( result );
}

inline
vu0_vec_xyz
vu0_vec_xyz::operator - ( float rhs ) const
{
   return *this - vu0_vec_x( rhs );
}

inline
void
vu0_vec_xyz::operator -= ( vu0_vec_xyz rhs )
{
   *this = *this - rhs;
}

inline
void
vu0_vec_xyz::operator -= ( vu0_vec_x rhs )
{
   *this = *this - rhs;
}

inline
void
vu0_vec_xyz::operator -= ( vu0_vec_y rhs )
{
   *this = *this - rhs;
}

inline
void
vu0_vec_xyz::operator -= ( vu0_vec_z rhs )
{
   *this = *this - rhs;
}

inline
void
vu0_vec_xyz::operator -= ( vu0_vec_w rhs )
{
   *this = *this - rhs;
}

inline
void
vu0_vec_xyz::operator -= ( float rhs )
{
   *this = *this - rhs;
}

inline
vu0_vec_xyz
vu0_vec_xyz::operator * ( vu0_vec_xyz rhs ) const
{
   __v4sf result;
   _vmul( result, vf_XYZW, vec128, rhs.vec128 );
   return vu0_vec_xyz( result );
}

inline
vu0_vec_xyz
vu0_vec_xyz::operator * ( vu0_vec_x rhs ) const
{
   __v4sf result;
   _vmulbc( result, vf_XYZW, vec128, rhs.vec128, vf_X );
   return vu0_vec_xyz( result );
}

inline
vu0_vec_xyz
vu0_vec_xyz::operator * ( vu0_vec_y rhs ) const
{
   __v4sf result;
   _vmulbc( result, vf_XYZW, vec128, rhs.vec128, vf_Y );
   return vu0_vec_xyz( result );
}

inline
vu0_vec_xyz
vu0_vec_xyz::operator * ( vu0_vec_z rhs ) const
{
   __v4sf result;
   _vmulbc( result, vf_XYZW, vec128, rhs.vec128, vf_Z );
   return vu0_vec_xyz( result );
}

inline
vu0_vec_xyz
vu0_vec_xyz::operator * ( vu0_vec_w rhs ) const
{
   __v4sf result;
   _vmulbc( result, vf_XYZW, vec128, rhs.vec128, vf_W );
   return vu0_vec_xyz( result );
}

inline
vu0_vec_xyz
vu0_vec_xyz::operator * ( float rhs ) const
{
   return *this * vu0_vec_x( rhs );
}

inline
void
vu0_vec_xyz::operator *= ( vu0_vec_xyz rhs )
{
   *this = *this * rhs;
}

inline
void
vu0_vec_xyz::operator *= ( vu0_vec_x rhs )
{
   *this = *this * rhs;
}

inline
void
vu0_vec_xyz::operator *= ( vu0_vec_y rhs )
{
   *this = *this * rhs;
}

inline
void
vu0_vec_xyz::operator *= ( vu0_vec_z rhs )
{
   *this = *this * rhs;
}

inline
void
vu0_vec_xyz::operator *= ( vu0_vec_w rhs )
{
   *this = *this * rhs;
}

inline
void
vu0_vec_xyz::operator *= ( float rhs )
{
   *this = *this * rhs;
}

inline
vu0_vec_xyz
vu0_vec_xyz::operator / ( vu0_vec_x rhs ) const
{
   __v4sf result;
   float Q;
   _vdiv( Q, vu0_vf0, vf_W, rhs.vec128, vf_X );
   _vwaitq( Q );
   _vmulf( result, vf_XYZW, vec128, Q );
   return vu0_vec_xyz( result );
}

inline
vu0_vec_xyz
vu0_vec_xyz::operator / ( vu0_vec_y rhs ) const
{
   __v4sf result;
   float Q;
   _vdiv( Q, vu0_vf0, vf_W, rhs.vec128, vf_Y );
   _vwaitq( Q );
   _vmulf( result, vf_XYZW, vec128, Q );
   return vu0_vec_xyz( result );
}

inline
vu0_vec_xyz
vu0_vec_xyz::operator / ( vu0_vec_z rhs ) const
{
   __v4sf result;
   float Q;
   _vdiv( Q, vu0_vf0, vf_W, rhs.vec128, vf_Z );
   _vwaitq( Q );
   _vmulf( result, vf_XYZW, vec128, Q );
   return vu0_vec_xyz( result );
}

inline
vu0_vec_xyz
vu0_vec_xyz::operator / ( vu0_vec_w rhs ) const
{
   __v4sf result;
   float Q;
   _vdiv( Q, vu0_vf0, vf_W, rhs.vec128, vf_W );
   _vwaitq( Q );
   _vmulf( result, vf_XYZW, vec128, Q );
   return vu0_vec_xyz( result );
}

inline
vu0_vec_xyz
vu0_vec_xyz::operator / ( float rhs ) const
{
   return *this / vu0_vec_x( rhs );
}

inline
void
vu0_vec_xyz::operator /= ( vu0_vec_x rhs )
{
   *this = *this / rhs;
}

inline
void
vu0_vec_xyz::operator /= ( vu0_vec_y rhs )
{
   *this = *this / rhs;
}

inline
void
vu0_vec_xyz::operator /= ( vu0_vec_z rhs )
{
   *this = *this / rhs;
}

inline
void
vu0_vec_xyz::operator /= ( vu0_vec_w rhs )
{
   *this = *this / rhs;
}

inline
void
vu0_vec_xyz::operator /= ( float rhs )
{
   *this = *this / rhs;
}

inline
vu0_vec_xyz
vu0_vec_xyz::operator - () const
{
   __v4sf result;
   _vsub(result, vf_XYZW, vu0_vf0, vec128);
   return vu0_vec_xyz( result );
}

inline
vu0_vec_xyz
vu0_vec_xyz::abs() const
{
   __v4sf result;
   _vabs( result, vf_XYZW, vec128 );
   return vu0_vec_xyz( result );
}

inline
vu0_vec_xyz
vu0_vec_xyz::sign() const
{
   __v4sf tmp;
   __v4si ones = (__v4si)(u_int128)0x3f800000;
   
   _vmulbc( tmp, vf_XYZW, vec128, vu0_vf0, vf_X );
   
   ones = _pextlw( ones, ones );
   ones = _pextlw( ones, ones );
   
   return vu0_vec_xyz( (__v4sf)_por( (__v4si)tmp, ones ) );
}

inline
vu0_vec_xyz
vu0_vec_xyz::max( vu0_vec_xyz rhs ) const
{
   __v4sf result;
   _vmax( result, vf_XYZW, vec128, rhs.vec128 );
   return vu0_vec_xyz( result );
}

inline
vu0_vec_xyz
vu0_vec_xyz::min( vu0_vec_xyz rhs ) const
{
   __v4sf result;
   _vmini( result, vf_XYZW, vec128, rhs.vec128 );
   return vu0_vec_xyz( result );
}

inline
vu0_vec_xyz
vu0_vec_xyz::recip() const
{
   __v4sf result;
   float Q;
   _vdiv( Q, vu0_vf0, vf_W, vec128, vf_X );
   _vwaitq( Q );
   _vaddf( result, vf_X, vu0_vf0, Q );
   _vdiv( Q, vu0_vf0, vf_W, vec128, vf_Y );
   _vwaitq( Q );
   _vaddf( result, vf_Y, vu0_vf0, Q );
   _vdiv( Q, vu0_vf0, vf_W, vec128, vf_Z );
   _vwaitq( Q );
   _vaddf( result, vf_Z, vu0_vf0, Q );
   return vu0_vec_xyz( result );
}

inline
vu0_vec_x
vu0_vec_xyz::max() const
{
   __v4sf result;
   _vmaxbc( result, vf_X, vec128, vec128, vf_Y );
   _vmaxbc( result, vf_X, result, vec128, vf_Z );
   return vu0_vec_x( result );
}

inline
vu0_vec_x
vu0_vec_xyz::min() const
{
   __v4sf result;
   _vminibc( result, vf_X, vec128, vec128, vf_Y );
   _vminibc( result, vf_X, result, vec128, vf_Z );
   return vu0_vec_x( result );
}

inline
vu0_vec_x
vu0_vec_xyz::sum() const
{
   __v4sf result;
   _vaddbc( result, vf_X, vec128, vec128, vf_Y );
   _vaddbc( result, vf_X, result, vec128, vf_Z );
   return vu0_vec_x( result );
}

inline
vu0_vec_x
vu0_vec_xyz::dot( vu0_vec_xyz rhs ) const
{
   __v4sf result;
#if 0
   // might give more opportunity for scheduling 
   _vmul( result, vf_XYZW, vec128, rhs.vec128 );
   _vaddbc( result, vf_X, result, result, vf_Y );
   _vaddbc( result, vf_X, result, result, vf_Z );
#else
   __v4sf ACC, one;
   _vmul( result, vf_XYZW, vec128, rhs.vec128 );
   _vaddbc( one, vf_XYZW, vu0_vf0, vu0_vf0, vf_W );
   _vaddbc( ACC, vf_XYZW, result, result, vf_Y );
   _vmaddbc( result, vf_XYZW, ACC, one, result, vf_Z );
#endif
   return vu0_vec_x( result );
}

inline 
vu0_vec_xyz
vu0_vec_xyz::interpolate( float alpha, vu0_vec_xyz rhs ) const
{
   return (*this) + ( ( rhs - (*this) ) * alpha );
}

inline
vu0_vec_x
vu0_vec_xyz::length_sqr() const
{
   return this->dot(*this);
}

inline
vu0_vec_x
vu0_vec_xyz::length() const
{
   return vu0_vec_x( sqrtf( length_sqr() ) );
}

inline
vu0_vec_xyz
vu0_vec_xyz::normalized() const
{
   const float lensqrtol = 1.0e-26f;
   __v4sf result;
   __v4sf dot;
   float Q;
   
   dot = length_sqr().vec128;
   
   _vrsqrt( Q, vu0_vf0, vf_W, dot, vf_X );

   if ( _vgetf( dot, vf_X ) < lensqrtol )
   {
      // length too small, set result to arbitrary unit vector (0,0,1)
      _vmr32( result, vf_XYZW, vu0_vf0 );
   }
   else
   {
      _vwaitq( Q );
      _vmulf( result, vf_XYZW, vec128, Q );
   }
   return vu0_vec_xyz( result );
}

inline
void
vu0_vec_xyz::normalize()
{
   *this = this->normalized();
}

inline
vu0_vec_xyz
vu0_vec_xyz::cross( vu0_vec_xyz rhs ) const
{
   __v4sf result;
   __v4sf ACC;
   _vopmula( ACC, vec128, rhs.vec128 );
   _vopmsub( result, ACC, rhs.vec128, vec128 );
   return vu0_vec_xyz( result );
}

inline
void
vu0_vec_xyz::print() const
{
   printf( "(%f %f %f *)\n",
           float( vu0_vec_x(*this) ), 
           float( vu0_vec_y(*this) ), 
           float( vu0_vec_z(*this) ) );
}

inline
void
vu0_vec_xyz::print( const char* name ) const
{
   printf( "%s: (%f %f %f *)\n", name,
           float( vu0_vec_x(*this) ), 
           float( vu0_vec_y(*this) ), 
           float( vu0_vec_z(*this) ) );
}

inline
vu0_vec_xyzw::vu0_vec_xyzw( __v4sf rhs )
{
   vec128 = rhs;
}

inline
vu0_vec_xyzw::vu0_vec_xyzw( float rhs )
{
   set( rhs );
}

inline
vu0_vec_xyzw::vu0_vec_xyzw( float x, float y, float z, float w )
{
   set( x, y, z, w );
}

inline
vu0_vec_xyzw::vu0_vec_xyzw( vu0_vec_x rhs )
{
   set( rhs );
}

inline
vu0_vec_xyzw::vu0_vec_xyzw( vu0_vec_y rhs )
{
   set( rhs );
}

inline
vu0_vec_xyzw::vu0_vec_xyzw( vu0_vec_z rhs )
{
   set( rhs );
}

inline
vu0_vec_xyzw::vu0_vec_xyzw( vu0_vec_w rhs )
{
   set( rhs );
}

inline
vu0_vec_xyzw::vu0_vec_xyzw( vu0_vector_t rhs )
{
   set( rhs );
}

inline
vu0_vec_xyzw::vu0_vec_xyzw( vu0_point_t rhs )
{
   set( rhs );
}

inline
void
vu0_vec_xyzw::set( __v4sf rhs )
{
   vec128 = rhs;
}

inline
void
vu0_vec_xyzw::set( vu0_vec_xyzw rhs )
{
   vec128 = rhs.vec128;
}

inline
void
vu0_vec_xyzw::set( vu0_vec_x rhs )
{
   _vaddbc( vec128, vf_XYZW, vu0_vf0, rhs.vec128, vf_X );
   _vmulbc( vec128, vf_W, vu0_vf0, rhs.vec128, vf_X );
}

inline
void
vu0_vec_xyzw::set( vu0_vec_y rhs )
{
   _vaddbc( vec128, vf_XYZW, vu0_vf0, rhs.vec128, vf_Y );
   _vmulbc( vec128, vf_W, vu0_vf0, rhs.vec128, vf_Y );
}

inline
void
vu0_vec_xyzw::set( vu0_vec_z rhs )
{
   _vaddbc( vec128, vf_XYZW, vu0_vf0, rhs.vec128, vf_Z );
   _vmulbc( vec128, vf_W, vu0_vf0, rhs.vec128, vf_Z );
}

inline
void
vu0_vec_xyzw::set( vu0_vec_w rhs )
{
   _vaddbc( vec128, vf_XYZW, vu0_vf0, rhs.vec128, vf_W );
   _vmulbc( vec128, vf_W, vu0_vf0, rhs.vec128, vf_W );
}

inline
void
vu0_vec_xyzw::set( vu0_vector_t rhs )
{
   vec128 = rhs.vec128;
   _vsub( vec128, vf_W, vu0_vf0, vu0_vf0 );
}

inline
void
vu0_vec_xyzw::set( vu0_point_t rhs )
{
   vec128 = rhs.vec128;
   _vmove( vec128, vf_W, vu0_vf0 );
}

inline
void
vu0_vec_xyzw::set( float rhs )
{
   __v4si tmp = (__v4si)_vpromotef( rhs );
   tmp = _pextlw( tmp, tmp );
   vec128 = (__v4sf)_pextlw( tmp, tmp );
}

inline
void
vu0_vec_xyzw::set( float x, float y, float z, float w )
{
   __v4si xi = (__v4si)_vpromotef(x);
   __v4si yi = (__v4si)_vpromotef(y);
   __v4si zi = (__v4si)_vpromotef(z);
   __v4si wi = (__v4si)_vpromotef(w);
   vec128 = (__v4sf)_pcpyld( _pextlw( wi, zi ), _pextlw( yi, xi ) );
}

inline
void
vu0_vec_xyzw::set_zero()
{
   _vsub( vec128, vf_XYZW, vu0_vf0, vu0_vf0 );
}

inline
void
vu0_vec_xyzw::set_vf0()
{
   _vmove( vec128, vf_XYZW, vu0_vf0 );
}

inline
__v4sf
vu0_vec_xyzw::get128() const
{
   return vec128;
}

inline
vu0_vec_xyzw&
vu0_vec_xyzw::operator = ( vu0_vec_xyz rhs )
{
   _vmove( vec128, vf_XYZ, rhs.vec128 );
   return *this;
}

inline
vu0_vec_xyzw&
vu0_vec_xyzw::operator = ( vu0_vec_xyzw rhs )
{
   vec128 = rhs.vec128;
   return *this;
}

inline
vu0_vec_xyzw&
vu0_vec_xyzw::operator = ( vu0_vector_t rhs )
{
   _vmove( vec128, vf_XYZ, rhs.vec128 );
   return *this;
}

inline
vu0_vec_xyzw&
vu0_vec_xyzw::operator = ( vu0_point_t rhs )
{
   _vmove( vec128, vf_XYZ, rhs.vec128 );
   return *this;
}

inline
vu0_vec_xyzw&
vu0_vec_xyzw::operator = ( vu0_vec_x rhs )
{
   _vmove( vec128, vf_X, rhs.vec128 );
   return *this;
}

inline
vu0_vec_xyzw&
vu0_vec_xyzw::operator = ( vu0_vec_y rhs )
{
   _vmove( vec128, vf_Y, rhs.vec128 );
   return *this;
}

inline
vu0_vec_xyzw&
vu0_vec_xyzw::operator = ( vu0_vec_z rhs )
{
   _vmove( vec128, vf_Z, rhs.vec128 );
   return *this;
}

inline
vu0_vec_xyzw&
vu0_vec_xyzw::operator = ( vu0_vec_w rhs )
{
   _vmove( vec128, vf_W, rhs.vec128 );
   return *this;
}

inline
vu0_vec_x
vu0_vec_xyzw::get_x() const
{
   return vu0_vec_x( *this );
}

inline
vu0_vec_y
vu0_vec_xyzw::get_y() const
{
   return vu0_vec_y( *this );
}

inline
vu0_vec_z
vu0_vec_xyzw::get_z() const
{
   return vu0_vec_z( *this );
}

inline
vu0_vec_w
vu0_vec_xyzw::get_w() const
{
   return vu0_vec_w( *this );
}

inline
void
vu0_vec_xyzw::set_x( vu0_vec_x rhs )
{
   _vaddbc( vec128, vf_X, vu0_vf0, rhs.vec128, vf_X );
}

inline
void
vu0_vec_xyzw::set_x( vu0_vec_y rhs )
{
   _vaddbc( vec128, vf_X, vu0_vf0, rhs.vec128, vf_Y );
}

inline
void
vu0_vec_xyzw::set_x( vu0_vec_z rhs )
{
   _vaddbc( vec128, vf_X, vu0_vf0, rhs.vec128, vf_Z );
}

inline
void
vu0_vec_xyzw::set_x( vu0_vec_w rhs )
{
   _vaddbc( vec128, vf_X, vu0_vf0, rhs.vec128, vf_W );
}

inline
void
vu0_vec_xyzw::set_x( float rhs )
{
   _vaddf( vec128, vf_X, vu0_vf0, rhs );
}

inline
void
vu0_vec_xyzw::set_y( vu0_vec_x rhs )
{
   _vaddbc( vec128, vf_Y, vu0_vf0, rhs.vec128, vf_X );
}

inline
void
vu0_vec_xyzw::set_y( vu0_vec_y rhs )
{
   _vaddbc( vec128, vf_Y, vu0_vf0, rhs.vec128, vf_Y );
}

inline
void
vu0_vec_xyzw::set_y( vu0_vec_z rhs )
{
   _vaddbc( vec128, vf_Y, vu0_vf0, rhs.vec128, vf_Z );
}

inline
void
vu0_vec_xyzw::set_y( vu0_vec_w rhs )
{
   _vaddbc( vec128, vf_Y, vu0_vf0, rhs.vec128, vf_W );
}

inline
void
vu0_vec_xyzw::set_y( float rhs )
{
   _vaddf( vec128, vf_Y, vu0_vf0, rhs );
}

inline
void
vu0_vec_xyzw::set_z( vu0_vec_x rhs )
{
   _vaddbc( vec128, vf_Z, vu0_vf0, rhs.vec128, vf_X );
}

inline
void
vu0_vec_xyzw::set_z( vu0_vec_y rhs )
{
   _vaddbc( vec128, vf_Z, vu0_vf0, rhs.vec128, vf_Y );
}

inline
void
vu0_vec_xyzw::set_z( vu0_vec_z rhs )
{
   _vaddbc( vec128, vf_Z, vu0_vf0, rhs.vec128, vf_Z );
}

inline
void
vu0_vec_xyzw::set_z( vu0_vec_w rhs )
{
   _vaddbc( vec128, vf_Z, vu0_vf0, rhs.vec128, vf_W );
}

inline
void
vu0_vec_xyzw::set_z( float rhs )
{
   _vaddf( vec128, vf_Z, vu0_vf0, rhs );
}

inline
void
vu0_vec_xyzw::set_w( vu0_vec_x rhs )
{
   _vmulbc( vec128, vf_W, vu0_vf0, rhs.vec128, vf_X );
}

inline
void
vu0_vec_xyzw::set_w( vu0_vec_y rhs )
{
   _vmulbc( vec128, vf_W, vu0_vf0, rhs.vec128, vf_Y );
}

inline
void
vu0_vec_xyzw::set_w( vu0_vec_z rhs )
{
   _vmulbc( vec128, vf_W, vu0_vf0, rhs.vec128, vf_Z );
}

inline
void
vu0_vec_xyzw::set_w( vu0_vec_w rhs )
{
   _vmulbc( vec128, vf_W, vu0_vf0, rhs.vec128, vf_W );
}

inline
void
vu0_vec_xyzw::set_w( float rhs )
{
   _vmulf( vec128, vf_W, vu0_vf0, rhs );
}

inline
vu0_vec_xyzw
vu0_vec_xyzw::operator + ( vu0_vec_xyzw rhs ) const
{
   __v4sf result;
   _vadd( result, vf_XYZW, vec128, rhs.vec128 );
   return vu0_vec_xyzw( result );
}

inline
vu0_vec_xyzw
vu0_vec_xyzw::operator + ( vu0_vec_x rhs ) const
{
   __v4sf result;
   _vaddbc( result, vf_XYZW, vec128, rhs.vec128, vf_X );
   return vu0_vec_xyzw( result );
}

inline
vu0_vec_xyzw
vu0_vec_xyzw::operator + ( vu0_vec_y rhs ) const
{
   __v4sf result;
   _vaddbc( result, vf_XYZW, vec128, rhs.vec128, vf_Y );
   return vu0_vec_xyzw( result );
}

inline
vu0_vec_xyzw
vu0_vec_xyzw::operator + ( vu0_vec_z rhs ) const
{
   __v4sf result;
   _vaddbc( result, vf_XYZW, vec128, rhs.vec128, vf_Z );
   return vu0_vec_xyzw( result );
}

inline
vu0_vec_xyzw
vu0_vec_xyzw::operator + ( vu0_vec_w rhs ) const
{
   __v4sf result;
   _vaddbc( result, vf_XYZW, vec128, rhs.vec128, vf_W );
   return vu0_vec_xyzw( result );
}

inline
vu0_vec_xyzw
vu0_vec_xyzw::operator + ( float rhs ) const
{
   return *this + vu0_vec_x( rhs );
}

inline
void
vu0_vec_xyzw::operator += ( vu0_vec_xyzw rhs )
{
   *this = *this + rhs;
}

inline
void
vu0_vec_xyzw::operator += ( vu0_vec_x rhs )
{
   *this = *this + rhs;
}

inline
void
vu0_vec_xyzw::operator += ( vu0_vec_y rhs )
{
   *this = *this + rhs;
}

inline
void
vu0_vec_xyzw::operator += ( vu0_vec_z rhs )
{
   *this = *this + rhs;
}

inline
void
vu0_vec_xyzw::operator += ( vu0_vec_w rhs )
{
   *this = *this + rhs;
}

inline
void
vu0_vec_xyzw::operator += ( float rhs )
{
   *this = *this + rhs;
}

inline
vu0_vec_xyzw
vu0_vec_xyzw::operator - ( vu0_vec_xyzw rhs ) const
{
   __v4sf result;
   _vsub( result, vf_XYZW, vec128, rhs.vec128 );
   return vu0_vec_xyzw( result );
}

inline
vu0_vec_xyzw
vu0_vec_xyzw::operator - ( vu0_vec_x rhs ) const
{
   __v4sf result;
   _vsubbc( result, vf_XYZW, vec128, rhs.vec128, vf_X );
   return vu0_vec_xyzw( result );
}

inline
vu0_vec_xyzw
vu0_vec_xyzw::operator - ( vu0_vec_y rhs ) const
{
   __v4sf result;
   _vsubbc( result, vf_XYZW, vec128, rhs.vec128, vf_Y );
   return vu0_vec_xyzw( result );
}

inline
vu0_vec_xyzw
vu0_vec_xyzw::operator - ( vu0_vec_z rhs ) const
{
   __v4sf result;
   _vsubbc( result, vf_XYZW, vec128, rhs.vec128, vf_Z );
   return vu0_vec_xyzw( result );
}

inline
vu0_vec_xyzw
vu0_vec_xyzw::operator - ( vu0_vec_w rhs ) const
{
   __v4sf result;
   _vsubbc( result, vf_XYZW, vec128, rhs.vec128, vf_W );
   return vu0_vec_xyzw( result );
}

inline
vu0_vec_xyzw
vu0_vec_xyzw::operator - ( float rhs ) const
{
   return *this - vu0_vec_x( rhs );
}

inline
void
vu0_vec_xyzw::operator -= ( vu0_vec_xyzw rhs )
{
   *this = *this - rhs;
}

inline
void
vu0_vec_xyzw::operator -= ( vu0_vec_x rhs )
{
   *this = *this - rhs;
}

inline
void
vu0_vec_xyzw::operator -= ( vu0_vec_y rhs )
{
   *this = *this - rhs;
}

inline
void
vu0_vec_xyzw::operator -= ( vu0_vec_z rhs )
{
   *this = *this - rhs;
}

inline
void
vu0_vec_xyzw::operator -= ( vu0_vec_w rhs )
{
   *this = *this - rhs;
}

inline
void
vu0_vec_xyzw::operator -= ( float rhs )
{
   *this = *this - rhs;
}

inline
vu0_vec_xyzw
vu0_vec_xyzw::operator * ( vu0_vec_xyzw rhs ) const
{
   __v4sf result;
   _vmul( result, vf_XYZW, vec128, rhs.vec128 );
   return vu0_vec_xyzw( result );
}

inline
vu0_vec_xyzw
vu0_vec_xyzw::operator * ( vu0_vec_x rhs ) const
{
   __v4sf result;
   _vmulbc( result, vf_XYZW, vec128, rhs.vec128, vf_X );
   return vu0_vec_xyzw( result );
}

inline
vu0_vec_xyzw
vu0_vec_xyzw::operator * ( vu0_vec_y rhs ) const
{
   __v4sf result;
   _vmulbc( result, vf_XYZW, vec128, rhs.vec128, vf_Y );
   return vu0_vec_xyzw( result );
}

inline
vu0_vec_xyzw
vu0_vec_xyzw::operator * ( vu0_vec_z rhs ) const
{
   __v4sf result;
   _vmulbc( result, vf_XYZW, vec128, rhs.vec128, vf_Z );
   return vu0_vec_xyzw( result );
}

inline
vu0_vec_xyzw
vu0_vec_xyzw::operator * ( vu0_vec_w rhs ) const
{
   __v4sf result;
   _vmulbc( result, vf_XYZW, vec128, rhs.vec128, vf_W );
   return vu0_vec_xyzw( result );
}

inline
vu0_vec_xyzw
vu0_vec_xyzw::operator * ( float rhs ) const
{
   return *this * vu0_vec_x( rhs );
}

inline
void
vu0_vec_xyzw::operator *= ( vu0_vec_xyzw rhs )
{
   *this = *this * rhs;
}

inline
void
vu0_vec_xyzw::operator *= ( vu0_vec_x rhs )
{
   *this = *this * rhs;
}

inline
void
vu0_vec_xyzw::operator *= ( vu0_vec_y rhs )
{
   *this = *this * rhs;
}

inline
void
vu0_vec_xyzw::operator *= ( vu0_vec_z rhs )
{
   *this = *this * rhs;
}

inline
void
vu0_vec_xyzw::operator *= ( vu0_vec_w rhs )
{
   *this = *this * rhs;
}

inline
void
vu0_vec_xyzw::operator *= ( float rhs )
{
   *this = *this * rhs;
}

inline
vu0_vec_xyzw
vu0_vec_xyzw::operator / ( vu0_vec_x rhs ) const
{
   __v4sf result;
   float Q;
   _vdiv( Q, vu0_vf0, vf_W, rhs.vec128, vf_X );
   _vwaitq( Q );
   _vmulf( result, vf_XYZW, vec128, Q );
   return vu0_vec_xyzw( result );
}

inline
vu0_vec_xyzw
vu0_vec_xyzw::operator / ( vu0_vec_y rhs ) const
{
   __v4sf result;
   float Q;
   _vdiv( Q, vu0_vf0, vf_W, rhs.vec128, vf_Y );
   _vwaitq( Q );
   _vmulf( result, vf_XYZW, vec128, Q );
   return vu0_vec_xyzw( result );
}

inline
vu0_vec_xyzw
vu0_vec_xyzw::operator / ( vu0_vec_z rhs ) const
{
   __v4sf result;
   float Q;
   _vdiv( Q, vu0_vf0, vf_W, rhs.vec128, vf_Z );
   _vwaitq( Q );
   _vmulf( result, vf_XYZW, vec128, Q );
   return vu0_vec_xyzw( result );
}

inline
vu0_vec_xyzw
vu0_vec_xyzw::operator / ( vu0_vec_w rhs ) const
{
   __v4sf result;
   float Q;
   _vdiv( Q, vu0_vf0, vf_W, rhs.vec128, vf_W );
   _vwaitq( Q );
   _vmulf( result, vf_XYZW, vec128, Q );
   return vu0_vec_xyzw( result );
}

inline
vu0_vec_xyzw
vu0_vec_xyzw::operator / ( float rhs ) const
{
   return *this / vu0_vec_x( rhs );
}

inline
void
vu0_vec_xyzw::operator /= ( vu0_vec_x rhs )
{
   *this = *this / rhs;
}

inline
void
vu0_vec_xyzw::operator /= ( vu0_vec_y rhs )
{
   *this = *this / rhs;
}

inline
void
vu0_vec_xyzw::operator /= ( vu0_vec_z rhs )
{
   *this = *this / rhs;
}

inline
void
vu0_vec_xyzw::operator /= ( vu0_vec_w rhs )
{
   *this = *this / rhs;
}

inline
void
vu0_vec_xyzw::operator /= ( float rhs )
{
   *this = *this / rhs;
}

inline
vu0_vec_xyzw
vu0_vec_xyzw::operator - () const
{
   __v4sf result;
  __v4sf ACC;
  _vsub( ACC, vf_XYZW, vu0_vf0, vu0_vf0 );
  _vmsubbc( result, vf_XYZW, ACC, vec128, vu0_vf0, vf_W );
   return vu0_vec_xyzw( result );
}

inline
vu0_vec_xyzw
vu0_vec_xyzw::abs() const
{
   __v4sf result;
   _vabs( result, vf_XYZW, vec128 );
   return vu0_vec_xyzw( result );
}

inline
vu0_vec_xyzw
vu0_vec_xyzw::sign() const
{
   __v4sf tmp;
   __v4si ones = (__v4si)(u_int128)0x3f800000;
   
   _vmulbc( tmp, vf_XYZW, vec128, vu0_vf0, vf_X );
   
   ones = _pextlw( ones, ones );
   ones = _pextlw( ones, ones );
   
   return vu0_vec_xyzw( (__v4sf)_por( (__v4si)tmp, ones ) );
}

inline
vu0_vec_xyzw
vu0_vec_xyzw::max( vu0_vec_xyzw rhs ) const
{
   __v4sf result;
   _vmax( result, vf_XYZW, vec128, rhs.vec128 );
   return vu0_vec_xyzw( result );
}

inline
vu0_vec_xyzw
vu0_vec_xyzw::min( vu0_vec_xyzw rhs ) const
{
   __v4sf result;
   _vmini( result, vf_XYZW, vec128, rhs.vec128 );
   return vu0_vec_xyzw( result );
}

inline
vu0_vec_xyzw
vu0_vec_xyzw::recip() const
{
   __v4sf result;
   float Q;
   _vdiv( Q, vu0_vf0, vf_W, vec128, vf_X );
   _vwaitq( Q );
   _vaddf( result, vf_X, vu0_vf0, Q );
   _vdiv( Q, vu0_vf0, vf_W, vec128, vf_Y );
   _vwaitq( Q );
   _vaddf( result, vf_Y, vu0_vf0, Q );
   _vdiv( Q, vu0_vf0, vf_W, vec128, vf_Z );
   _vwaitq( Q );
   _vaddf( result, vf_Z, vu0_vf0, Q );
   _vdiv( Q, vu0_vf0, vf_W, vec128, vf_W );
   _vwaitq( Q );
   _vmulf( result, vf_W, vu0_vf0, Q );
   return vu0_vec_xyzw( result );
}

inline
vu0_vec_x
vu0_vec_xyzw::max() const
{
   __v4sf result;
   _vmaxbc( result, vf_X, vec128, vec128, vf_Y );
   _vmaxbc( result, vf_X, result, vec128, vf_Z );
   _vmaxbc( result, vf_X, result, vec128, vf_W );
   return vu0_vec_x( result );
}

inline
vu0_vec_x
vu0_vec_xyzw::min() const
{
   __v4sf result;
   _vminibc( result, vf_X, vec128, vec128, vf_Y );
   _vminibc( result, vf_X, result, vec128, vf_Z );
   _vminibc( result, vf_X, result, vec128, vf_W );
   return vu0_vec_x( result );
}

inline
vu0_vec_x
vu0_vec_xyzw::sum() const
{
   __v4sf result;
   _vaddbc( result, vf_X, vec128, vec128, vf_Y );
   _vaddbc( result, vf_X, result, vec128, vf_Z );
   _vaddbc( result, vf_X, result, vec128, vf_W );
   return vu0_vec_x( result );
}

inline
vu0_vec_x
vu0_vec_xyzw::dot( vu0_vec_xyzw rhs ) const
{
   __v4sf result;
#if 0
   // might give more opportunity for scheduling 
   _vmul( result, vf_XYZW, vec128, rhs.vec128 );
   _vaddbc( result, vf_X, result, result, vf_Y );
   _vaddbc( result, vf_X, result, result, vf_Z );
   _vaddbc( result, vf_X, result, result, vf_W );
#else
   __v4sf ACC, one;
   _vmul( result, vf_XYZW, vec128, rhs.vec128 );
   _vaddbc(  one, vf_XYZW, vu0_vf0, vu0_vf0, vf_W );
   _vaddbc(  ACC, vf_XYZW, result, result, vf_Y );
   _vmaddbc( ACC, vf_XYZW, ACC, one, result, vf_Z );
   _vmaddbc( result, vf_XYZW, ACC, one, result, vf_W );
#endif
   return vu0_vec_x( result );
}

inline 
vu0_vec_xyzw
vu0_vec_xyzw::interpolate( float alpha, vu0_vec_xyzw rhs ) const
{
   return (*this) + ( ( rhs - (*this) ) * alpha );
}

inline
vu0_vec_x
vu0_vec_xyzw::length_sqr() const
{
   return this->dot(*this);
}

inline
vu0_vec_x
vu0_vec_xyzw::length() const
{
   return vu0_vec_x( sqrtf( length_sqr() ) );
}

inline
vu0_vec_xyzw
vu0_vec_xyzw::normalized() const
{
   const float lensqrtol = 1.0e-26f;
   __v4sf result;
   __v4sf dot;
   float Q;
   
   dot = length_sqr().vec128;
   
   _vrsqrt( Q, vu0_vf0, vf_W, dot, vf_X );

   if ( _vgetf( dot, vf_X ) < lensqrtol )
   {
      // length too small, set result to arbitrary unit vector (0,0,0,1)
      _vmove( result, vf_XYZW, vu0_vf0 );
   }
   else
   {
      _vwaitq( Q );
      _vmulf( result, vf_XYZW, vec128, Q );
   }
   return vu0_vec_xyzw( result );
}

inline
void
vu0_vec_xyzw::normalize()
{
   *this = this->normalized();
}

inline
vu0_vec_xyzw
vu0_vec_xyzw::cross( vu0_vec_xyzw rhs ) const
{
   __v4sf result;
   __v4sf ACC;
   _vopmula( ACC, vec128, rhs.vec128 );
   _vopmsub( result, ACC, rhs.vec128, vec128 );
   _vmulbc( result, vf_W, vu0_vf0, vu0_vf0, vf_X );
   return vu0_vec_xyzw( result );
}

inline
void
vu0_vec_xyzw::print() const
{
   printf( "(%f %f %f %f)\n",
           float( vu0_vec_x(*this) ), 
           float( vu0_vec_y(*this) ), 
           float( vu0_vec_z(*this) ), 
           float( vu0_vec_w(*this) ) );
}

inline
void
vu0_vec_xyzw::print( const char* name ) const
{
   printf( "%s: (%f %f %f %f)\n", name,
           float( vu0_vec_x(*this) ), 
           float( vu0_vec_y(*this) ), 
           float( vu0_vec_z(*this) ), 
           float( vu0_vec_w(*this) ) );
}

inline
vu0_vector_t::vu0_vector_t( __v4sf rhs )
{
   vec128 = rhs;
}

inline
vu0_vector_t::vu0_vector_t( float rhs )
{
   set( rhs );
}

inline
vu0_vector_t::vu0_vector_t( float x, float y, float z )
{
   set( x, y, z );
}

inline
vu0_vector_t::vu0_vector_t( vu0_vec_x rhs )
{
   set( rhs );
}

inline
vu0_vector_t::vu0_vector_t( vu0_vec_y rhs )
{
   set( rhs );
}

inline
vu0_vector_t::vu0_vector_t( vu0_vec_z rhs )
{
   set( rhs );
}

inline
vu0_vector_t::vu0_vector_t( vu0_vec_w rhs )
{
   set( rhs );
}

inline
vu0_vector_t::vu0_vector_t( vu0_vec_xyz rhs )
{
   vec128 = rhs.vec128;
}

inline
vu0_vector_t::vu0_vector_t( vu0_point_t rhs )
{
   vec128 = rhs.vec128;
}

inline
void
vu0_vector_t::set( __v4sf rhs )
{
   vec128 = rhs;
}

inline
void
vu0_vector_t::set( vu0_vector_t rhs )
{
   vec128 = rhs.vec128;
}

inline
void
vu0_vector_t::set( vu0_vec_x rhs )
{
   _vaddbc( vec128, vf_XYZW, vu0_vf0, rhs.vec128, vf_X );
}

inline
void
vu0_vector_t::set( vu0_vec_y rhs )
{
   _vaddbc( vec128, vf_XYZW, vu0_vf0, rhs.vec128, vf_Y );
}

inline
void
vu0_vector_t::set( vu0_vec_z rhs )
{
   _vaddbc( vec128, vf_XYZW, vu0_vf0, rhs.vec128, vf_Z );
}

inline
void
vu0_vector_t::set( vu0_vec_w rhs )
{
   _vaddbc( vec128, vf_XYZW, vu0_vf0, rhs.vec128, vf_W );
}

inline
void
vu0_vector_t::set( vu0_vec_xyz rhs )
{
   vec128 = rhs.vec128;
}

inline
void
vu0_vector_t::set( vu0_point_t rhs )
{
   vec128 = rhs.vec128;
}

inline
void
vu0_vector_t::set( float rhs )
{
   __v4si tmp = (__v4si)_vpromotef( rhs );
   tmp = _pextlw( tmp, tmp );
   vec128 = (__v4sf)_pextlw( tmp, tmp );
}

inline
void
vu0_vector_t::set( float x, float y, float z )
{
   __v4si xi = (__v4si)_vpromotef(x);
   __v4si yi = (__v4si)_vpromotef(y);
   __v4si zi = (__v4si)_vpromotef(z);
   vec128 = (__v4sf)_pcpyld( zi, _pextlw( yi, xi ) );
}

inline
void
vu0_vector_t::set_zero()
{
   _vsub( vec128, vf_XYZW, vu0_vf0, vu0_vf0 );
}

inline
__v4sf
vu0_vector_t::get128() const
{
   return vec128;
}

inline
vu0_vector_t&
vu0_vector_t::operator = ( vu0_vec_xyz rhs )
{
   _vmove( vec128, vf_XYZ, rhs.vec128 );
   return *this;
}

inline
vu0_vector_t&
vu0_vector_t::operator = ( vu0_vector_t rhs )
{
   vec128 = rhs.vec128;
   return *this;
}

inline
vu0_vector_t&
vu0_vector_t::operator = ( vu0_vec_x rhs )
{
   _vmove( vec128, vf_X, rhs.vec128 );
   return *this;
}

inline
vu0_vector_t&
vu0_vector_t::operator = ( vu0_vec_y rhs )
{
   _vmove( vec128, vf_Y, rhs.vec128 );
   return *this;
}

inline
vu0_vector_t&
vu0_vector_t::operator = ( vu0_vec_z rhs )
{
   _vmove( vec128, vf_Z, rhs.vec128 );
   return *this;
}

inline
vu0_vec_x
vu0_vector_t::get_x() const
{
   return vu0_vec_x( *this );
}

inline
vu0_vec_y
vu0_vector_t::get_y() const
{
   return vu0_vec_y( *this );
}

inline
vu0_vec_z
vu0_vector_t::get_z() const
{
   return vu0_vec_z( *this );
}

inline
void
vu0_vector_t::set_x( vu0_vec_x rhs )
{
   _vaddbc( vec128, vf_X, vu0_vf0, rhs.vec128, vf_X );
}

inline
void
vu0_vector_t::set_x( vu0_vec_y rhs )
{
   _vaddbc( vec128, vf_X, vu0_vf0, rhs.vec128, vf_Y );
}

inline
void
vu0_vector_t::set_x( vu0_vec_z rhs )
{
   _vaddbc( vec128, vf_X, vu0_vf0, rhs.vec128, vf_Z );
}

inline
void
vu0_vector_t::set_x( vu0_vec_w rhs )
{
   _vaddbc( vec128, vf_X, vu0_vf0, rhs.vec128, vf_W );
}

inline
void
vu0_vector_t::set_x( float rhs )
{
   _vaddf( vec128, vf_X, vu0_vf0, rhs );
}

inline
void
vu0_vector_t::set_y( vu0_vec_x rhs )
{
   _vaddbc( vec128, vf_Y, vu0_vf0, rhs.vec128, vf_X );
}

inline
void
vu0_vector_t::set_y( vu0_vec_y rhs )
{
   _vaddbc( vec128, vf_Y, vu0_vf0, rhs.vec128, vf_Y );
}

inline
void
vu0_vector_t::set_y( vu0_vec_z rhs )
{
   _vaddbc( vec128, vf_Y, vu0_vf0, rhs.vec128, vf_Z );
}

inline
void
vu0_vector_t::set_y( vu0_vec_w rhs )
{
   _vaddbc( vec128, vf_Y, vu0_vf0, rhs.vec128, vf_W );
}

inline
void
vu0_vector_t::set_y( float rhs )
{
   _vaddf( vec128, vf_Y, vu0_vf0, rhs );
}

inline
void
vu0_vector_t::set_z( vu0_vec_x rhs )
{
   _vaddbc( vec128, vf_Z, vu0_vf0, rhs.vec128, vf_X );
}

inline
void
vu0_vector_t::set_z( vu0_vec_y rhs )
{
   _vaddbc( vec128, vf_Z, vu0_vf0, rhs.vec128, vf_Y );
}

inline
void
vu0_vector_t::set_z( vu0_vec_z rhs )
{
   _vaddbc( vec128, vf_Z, vu0_vf0, rhs.vec128, vf_Z );
}

inline
void
vu0_vector_t::set_z( vu0_vec_w rhs )
{
   _vaddbc( vec128, vf_Z, vu0_vf0, rhs.vec128, vf_W );
}

inline
void
vu0_vector_t::set_z( float rhs )
{
   _vaddf( vec128, vf_Z, vu0_vf0, rhs );
}

inline
vu0_vector_t
vu0_vector_t::operator + ( vu0_vector_t rhs ) const
{
   __v4sf result;
   _vadd( result, vf_XYZW, vec128, rhs.vec128 );
   return vu0_vector_t( result );
}

inline
vu0_point_t
vu0_vector_t::operator + ( vu0_point_t rhs ) const
{
   __v4sf result;
   _vadd( result, vf_XYZW, vec128, rhs.vec128 );
   return vu0_point_t( result );
}

inline
vu0_vector_t
vu0_vector_t::operator - ( vu0_vector_t rhs ) const
{
   __v4sf result;
   _vsub( result, vf_XYZW, vec128, rhs.vec128 );
   return vu0_vector_t( result );
}

inline
vu0_vector_t
vu0_vector_t::operator * ( vu0_vector_t rhs ) const
{
   __v4sf result;
   _vmul( result, vf_XYZW, vec128, rhs.vec128 );
   return vu0_vector_t( result );
}

inline
vu0_vector_t
vu0_vector_t::operator * ( vu0_vec_x rhs ) const
{
   __v4sf result;
   _vmulbc( result, vf_XYZW, vec128, rhs.vec128, vf_X );
   return vu0_vector_t( result );
}

inline
vu0_vector_t
vu0_vector_t::operator * ( vu0_vec_y rhs ) const
{
   __v4sf result;
   _vmulbc( result, vf_XYZW, vec128, rhs.vec128, vf_Y );
   return vu0_vector_t( result );
}

inline
vu0_vector_t
vu0_vector_t::operator * ( vu0_vec_z rhs ) const
{
   __v4sf result;
   _vmulbc( result, vf_XYZW, vec128, rhs.vec128, vf_Z );
   return vu0_vector_t( result );
}

inline
vu0_vector_t
vu0_vector_t::operator * ( vu0_vec_w rhs ) const
{
   __v4sf result;
   _vmulbc( result, vf_XYZW, vec128, rhs.vec128, vf_W );
   return vu0_vector_t( result );
}

inline
vu0_vector_t
vu0_vector_t::operator * ( float rhs ) const
{
   return *this * vu0_vec_x( rhs );
}

inline
void
vu0_vector_t::operator += ( vu0_vector_t rhs )
{
   *this = *this + rhs;
}

inline
void
vu0_vector_t::operator -= ( vu0_vector_t rhs )
{
   *this = *this - rhs;
}

inline
void
vu0_vector_t::operator *= ( vu0_vector_t rhs )
{
   *this = *this * rhs;
}

inline
void
vu0_vector_t::operator *= ( vu0_vec_x rhs )
{
   *this = *this * rhs;
}

inline
void
vu0_vector_t::operator *= ( vu0_vec_y rhs )
{
   *this = *this * rhs;
}

inline
void
vu0_vector_t::operator *= ( vu0_vec_z rhs )
{
   *this = *this * rhs;
}

inline
void
vu0_vector_t::operator *= ( vu0_vec_w rhs )
{
   *this = *this * rhs;
}

inline
void
vu0_vector_t::operator *= ( float rhs )
{
   *this = *this * rhs;
}

inline
vu0_vector_t
vu0_vector_t::operator / ( vu0_vec_x rhs ) const
{
   __v4sf result;
   float Q;
   _vdiv( Q, vu0_vf0, vf_W, rhs.vec128, vf_X );
   _vwaitq( Q );
   _vmulf( result, vf_XYZW, vec128, Q );
   return vu0_vector_t( result );
}

inline
vu0_vector_t
vu0_vector_t::operator / ( vu0_vec_y rhs ) const
{
   __v4sf result;
   float Q;
   _vdiv( Q, vu0_vf0, vf_W, rhs.vec128, vf_Y );
   _vwaitq( Q );
   _vmulf( result, vf_XYZW, vec128, Q );
   return vu0_vector_t( result );
}

inline
vu0_vector_t
vu0_vector_t::operator / ( vu0_vec_z rhs ) const
{
   __v4sf result;
   float Q;
   _vdiv( Q, vu0_vf0, vf_W, rhs.vec128, vf_Z );
   _vwaitq( Q );
   _vmulf( result, vf_XYZW, vec128, Q );
   return vu0_vector_t( result );
}

inline
vu0_vector_t
vu0_vector_t::operator / ( vu0_vec_w rhs ) const
{
   __v4sf result;
   float Q;
   _vdiv( Q, vu0_vf0, vf_W, rhs.vec128, vf_W );
   _vwaitq( Q );
   _vmulf( result, vf_XYZW, vec128, Q );
   return vu0_vector_t( result );
}

inline
vu0_vector_t
vu0_vector_t::operator / ( float rhs ) const
{
   return *this / vu0_vec_x( rhs );
}

inline
void
vu0_vector_t::operator /= ( vu0_vec_x rhs )
{
   *this = *this / rhs;
}

inline
void
vu0_vector_t::operator /= ( vu0_vec_y rhs )
{
   *this = *this / rhs;
}

inline
void
vu0_vector_t::operator /= ( vu0_vec_z rhs )
{
   *this = *this / rhs;
}

inline
void
vu0_vector_t::operator /= ( vu0_vec_w rhs )
{
   *this = *this / rhs;
}

inline
void
vu0_vector_t::operator /= ( float rhs )
{
   *this = *this / rhs;
}

inline
vu0_vector_t
vu0_vector_t::operator - () const
{
   __v4sf result;
   _vsub(result, vf_XYZW, vu0_vf0, vec128);
   return vu0_vector_t( result );
}

inline
vu0_vector_t
vu0_vector_t::abs() const
{
   __v4sf result;
   _vabs( result, vf_XYZW, vec128 );
   return vu0_vector_t( result );
}

inline
vu0_vector_t
vu0_vector_t::sign() const
{
   __v4sf tmp;
   __v4si ones = (__v4si)(u_int128)0x3f800000;
   
   _vmulbc( tmp, vf_XYZW, vec128, vu0_vf0, vf_X );
   
   ones = _pextlw( ones, ones );
   ones = _pextlw( ones, ones );
   
   return vu0_vector_t( (__v4sf)_por( (__v4si)tmp, ones ) );
}

inline
vu0_vector_t
vu0_vector_t::max( vu0_vector_t rhs ) const
{
   __v4sf result;
   _vmax( result, vf_XYZW, vec128, rhs.vec128 );
   return vu0_vector_t( result );
}

inline
vu0_vector_t
vu0_vector_t::min( vu0_vector_t rhs ) const
{
   __v4sf result;
   _vmini( result, vf_XYZW, vec128, rhs.vec128 );
   return vu0_vector_t( result );
}

inline
vu0_vec_x
vu0_vector_t::max() const
{
   __v4sf result;
   _vmaxbc( result, vf_X, vec128, vec128, vf_Y );
   _vmaxbc( result, vf_X, result, vec128, vf_Z );
   return vu0_vec_x( result );
}

inline
vu0_vec_x
vu0_vector_t::min() const
{
   __v4sf result;
   _vminibc( result, vf_X, vec128, vec128, vf_Y );
   _vminibc( result, vf_X, result, vec128, vf_Z );
   return vu0_vec_x( result );
}

inline
vu0_vec_x
vu0_vector_t::sum() const
{
   __v4sf result;
   _vaddbc( result, vf_X, vec128, vec128, vf_Y );
   _vaddbc( result, vf_X, result, vec128, vf_Z );
   return vu0_vec_x( result );
}

inline
vu0_vec_x
vu0_vector_t::dot( vu0_vector_t rhs ) const
{
   __v4sf result;
#if 0
   // might give more opportunity for scheduling 
   _vmul( result, vf_XYZW, vec128, rhs.vec128 );
   _vaddbc( result, vf_X, result, result, vf_Y );
   _vaddbc( result, vf_X, result, result, vf_Z );
#else
   __v4sf ACC, one;
   _vmul( result, vf_XYZW, vec128, rhs.vec128 );
   _vaddbc( one, vf_XYZW, vu0_vf0, vu0_vf0, vf_W );
   _vaddbc( ACC, vf_XYZW, result, result, vf_Y );
   _vmaddbc( result, vf_XYZW, ACC, one, result, vf_Z );
#endif
   return vu0_vec_x( result );
}

inline 
vu0_vector_t
vu0_vector_t::interpolate( float alpha, vu0_vector_t rhs ) const
{
   return (*this) + ( ( rhs - (*this) ) * alpha );
}

inline
vu0_vec_x
vu0_vector_t::length_sqr() const
{
   return this->dot(*this);
}

inline
vu0_vec_x
vu0_vector_t::length() const
{
   return vu0_vec_x( sqrtf( length_sqr() ) );
}

inline
vu0_vector_t
vu0_vector_t::normalized() const
{
   const float lensqrtol = 1.0e-26f;
   __v4sf result;
   __v4sf dot;
   float Q;
   
   dot = length_sqr().vec128;
   
   _vrsqrt( Q, vu0_vf0, vf_W, dot, vf_X );

   if ( _vgetf( dot, vf_X ) < lensqrtol )
   {
      // length too small, set result to arbitrary unit vector (0,0,1)
      _vmr32( result, vf_XYZW, vu0_vf0 );
   }
   else
   {
      _vwaitq( Q );
      _vmulf( result, vf_XYZW, vec128, Q );
   }
   return vu0_vector_t( result );
}

inline
void
vu0_vector_t::normalize()
{
   *this = this->normalized();
}

inline
vu0_vector_t
vu0_vector_t::cross( vu0_vector_t rhs ) const
{
   __v4sf result;
   __v4sf ACC;
   _vopmula( ACC, vec128, rhs.vec128 );
   _vopmsub( result, ACC, rhs.vec128, vec128 );
   return vu0_vector_t( result );
}

inline
void
vu0_vector_t::print() const
{
   printf( "(%f %f %f 0)\n",
           float( vu0_vec_x(*this) ), 
           float( vu0_vec_y(*this) ), 
           float( vu0_vec_z(*this) ) );
}

inline
void
vu0_vector_t::print( const char* name ) const
{
   printf( "%s: (%f %f %f 0)\n", name,
           float( vu0_vec_x(*this) ), 
           float( vu0_vec_y(*this) ), 
           float( vu0_vec_z(*this) ) );
}

inline
vu0_point_t::vu0_point_t( __v4sf rhs )
{
   vec128 = rhs;
}

inline
vu0_point_t::vu0_point_t( float rhs )
{
   set( rhs );
}

inline
vu0_point_t::vu0_point_t( float x, float y, float z )
{
   set( x, y, z );
}

inline
vu0_point_t::vu0_point_t( vu0_vec_x rhs )
{
   set( rhs );
}

inline
vu0_point_t::vu0_point_t( vu0_vec_y rhs )
{
   set( rhs );
}

inline
vu0_point_t::vu0_point_t( vu0_vec_z rhs )
{
   set( rhs );
}

inline
vu0_point_t::vu0_point_t( vu0_vec_w rhs )
{
   set( rhs );
}

inline
vu0_point_t::vu0_point_t( vu0_vec_xyz rhs )
{
   vec128 = rhs.vec128;
}

inline
vu0_point_t::vu0_point_t( vu0_vector_t rhs )
{
   vec128 = rhs.vec128;
}

inline
void
vu0_point_t::set( __v4sf rhs )
{
   vec128 = rhs;
}

inline
void
vu0_point_t::set( vu0_point_t rhs )
{
   vec128 = rhs.vec128;
}

inline
void
vu0_point_t::set( vu0_vec_x rhs )
{
   _vaddbc( vec128, vf_XYZW, vu0_vf0, rhs.vec128, vf_X );
}

inline
void
vu0_point_t::set( vu0_vec_y rhs )
{
   _vaddbc( vec128, vf_XYZW, vu0_vf0, rhs.vec128, vf_Y );
}

inline
void
vu0_point_t::set( vu0_vec_z rhs )
{
   _vaddbc( vec128, vf_XYZW, vu0_vf0, rhs.vec128, vf_Z );
}

inline
void
vu0_point_t::set( vu0_vec_w rhs )
{
   _vaddbc( vec128, vf_XYZW, vu0_vf0, rhs.vec128, vf_W );
}

inline
void
vu0_point_t::set( vu0_vec_xyz rhs )
{
   vec128 = rhs.vec128;
}

inline
void
vu0_point_t::set( vu0_vector_t rhs )
{
   vec128 = rhs.vec128;
}

inline
void
vu0_point_t::set( float rhs )
{
   __v4si tmp = (__v4si)_vpromotef( rhs );
   tmp = _pextlw( tmp, tmp );
   vec128 = (__v4sf)_pextlw( tmp, tmp );
}

inline
void
vu0_point_t::set( float x, float y, float z )
{
   __v4si xi = (__v4si)_vpromotef(x);
   __v4si yi = (__v4si)_vpromotef(y);
   __v4si zi = (__v4si)_vpromotef(z);
   vec128 = (__v4sf)_pcpyld( zi, _pextlw( yi, xi ) );
}

inline
void
vu0_point_t::set_zero()
{
   _vsub( vec128, vf_XYZW, vu0_vf0, vu0_vf0 );
}

inline
void
vu0_point_t::set_vf0()
{
   _vmove( vec128, vf_XYZW, vu0_vf0 );
}

inline
__v4sf
vu0_point_t::get128() const
{
   return vec128;
}

inline
vu0_point_t&
vu0_point_t::operator = ( vu0_vec_xyz rhs )
{
   _vmove( vec128, vf_XYZ, rhs.vec128 );
   return *this;
}

inline
vu0_point_t&
vu0_point_t::operator = ( vu0_point_t rhs )
{
   vec128 = rhs.vec128;
   return *this;
}

inline
vu0_point_t&
vu0_point_t::operator = ( vu0_vec_x rhs )
{
   _vmove( vec128, vf_X, rhs.vec128 );
   return *this;
}

inline
vu0_point_t&
vu0_point_t::operator = ( vu0_vec_y rhs )
{
   _vmove( vec128, vf_Y, rhs.vec128 );
   return *this;
}

inline
vu0_point_t&
vu0_point_t::operator = ( vu0_vec_z rhs )
{
   _vmove( vec128, vf_Z, rhs.vec128 );
   return *this;
}

inline
vu0_vec_x
vu0_point_t::get_x() const
{
   return vu0_vec_x( *this );
}

inline
vu0_vec_y
vu0_point_t::get_y() const
{
   return vu0_vec_y( *this );
}

inline
vu0_vec_z
vu0_point_t::get_z() const
{
   return vu0_vec_z( *this );
}

inline
void
vu0_point_t::set_x( vu0_vec_x rhs )
{
   _vaddbc( vec128, vf_X, vu0_vf0, rhs.vec128, vf_X );
}

inline
void
vu0_point_t::set_x( vu0_vec_y rhs )
{
   _vaddbc( vec128, vf_X, vu0_vf0, rhs.vec128, vf_Y );
}

inline
void
vu0_point_t::set_x( vu0_vec_z rhs )
{
   _vaddbc( vec128, vf_X, vu0_vf0, rhs.vec128, vf_Z );
}

inline
void
vu0_point_t::set_x( vu0_vec_w rhs )
{
   _vaddbc( vec128, vf_X, vu0_vf0, rhs.vec128, vf_W );
}

inline
void
vu0_point_t::set_x( float rhs )
{
   _vaddf( vec128, vf_X, vu0_vf0, rhs );
}

inline
void
vu0_point_t::set_y( vu0_vec_x rhs )
{
   _vaddbc( vec128, vf_Y, vu0_vf0, rhs.vec128, vf_X );
}

inline
void
vu0_point_t::set_y( vu0_vec_y rhs )
{
   _vaddbc( vec128, vf_Y, vu0_vf0, rhs.vec128, vf_Y );
}

inline
void
vu0_point_t::set_y( vu0_vec_z rhs )
{
   _vaddbc( vec128, vf_Y, vu0_vf0, rhs.vec128, vf_Z );
}

inline
void
vu0_point_t::set_y( vu0_vec_w rhs )
{
   _vaddbc( vec128, vf_Y, vu0_vf0, rhs.vec128, vf_W );
}

inline
void
vu0_point_t::set_y( float rhs )
{
   _vaddf( vec128, vf_Y, vu0_vf0, rhs );
}

inline
void
vu0_point_t::set_z( vu0_vec_x rhs )
{
   _vaddbc( vec128, vf_Z, vu0_vf0, rhs.vec128, vf_X );
}

inline
void
vu0_point_t::set_z( vu0_vec_y rhs )
{
   _vaddbc( vec128, vf_Z, vu0_vf0, rhs.vec128, vf_Y );
}

inline
void
vu0_point_t::set_z( vu0_vec_z rhs )
{
   _vaddbc( vec128, vf_Z, vu0_vf0, rhs.vec128, vf_Z );
}

inline
void
vu0_point_t::set_z( vu0_vec_w rhs )
{
   _vaddbc( vec128, vf_Z, vu0_vf0, rhs.vec128, vf_W );
}

inline
void
vu0_point_t::set_z( float rhs )
{
   _vaddf( vec128, vf_Z, vu0_vf0, rhs );
}

inline
vu0_point_t
vu0_point_t::operator + ( vu0_vector_t rhs ) const
{
   __v4sf result;
   _vadd( result, vf_XYZW, vec128, rhs.vec128 );
   return vu0_point_t( result );
}

inline
vu0_vector_t
vu0_point_t::operator - ( vu0_point_t rhs ) const
{
   __v4sf result;
   _vsub( result, vf_XYZW, vec128, rhs.vec128 );
   return vu0_vector_t( result );
}

inline
vu0_point_t
vu0_point_t::operator - ( vu0_vector_t rhs ) const
{
   __v4sf result;
   _vsub( result, vf_XYZW, vec128, rhs.vec128 );
   return vu0_point_t( result );
}

inline
void
vu0_point_t::operator += ( vu0_vector_t rhs )
{
   *this = *this + rhs;
}

inline
void
vu0_point_t::operator -= ( vu0_vector_t rhs )
{
   *this = *this - rhs;
}

inline
vu0_point_t
vu0_point_t::operator - () const
{
   __v4sf result;
   _vsub(result, vf_XYZW, vu0_vf0, vec128);
   return vu0_point_t( result );
}

inline
vu0_point_t
vu0_point_t::abs() const
{
   __v4sf result;
   _vabs( result, vf_XYZW, vec128 );
   return vu0_point_t( result );
}

inline
vu0_point_t
vu0_point_t::sign() const
{
   __v4sf tmp;
   __v4si ones = (__v4si)(u_int128)0x3f800000;
   
   _vmulbc( tmp, vf_XYZW, vec128, vu0_vf0, vf_X );
   
   ones = _pextlw( ones, ones );
   ones = _pextlw( ones, ones );
   
   return vu0_point_t( (__v4sf)_por( (__v4si)tmp, ones ) );
}

inline
vu0_point_t
vu0_point_t::max( vu0_point_t rhs ) const
{
   __v4sf result;
   _vmax( result, vf_XYZW, vec128, rhs.vec128 );
   return vu0_point_t( result );
}

inline
vu0_point_t
vu0_point_t::min( vu0_point_t rhs ) const
{
   __v4sf result;
   _vmini( result, vf_XYZW, vec128, rhs.vec128 );
   return vu0_point_t( result );
}

inline
vu0_vec_x
vu0_point_t::max() const
{
   __v4sf result;
   _vmaxbc( result, vf_X, vec128, vec128, vf_Y );
   _vmaxbc( result, vf_X, result, vec128, vf_Z );
   return vu0_vec_x( result );
}

inline
vu0_vec_x
vu0_point_t::min() const
{
   __v4sf result;
   _vminibc( result, vf_X, vec128, vec128, vf_Y );
   _vminibc( result, vf_X, result, vec128, vf_Z );
   return vu0_vec_x( result );
}

inline
vu0_vec_x
vu0_point_t::sum() const
{
   __v4sf result;
   _vaddbc( result, vf_X, vec128, vec128, vf_Y );
   _vaddbc( result, vf_X, result, vec128, vf_Z );
   return vu0_vec_x( result );
}

inline
vu0_vec_x
vu0_point_t::dot( vu0_vector_t rhs ) const
{
   __v4sf result;
#if 0
   // might give more opportunity for scheduling 
   _vmul( result, vf_XYZW, vec128, rhs.vec128 );
   _vaddbc( result, vf_X, result, result, vf_Y );
   _vaddbc( result, vf_X, result, result, vf_Z );
#else
   __v4sf ACC, one;
   _vmul( result, vf_XYZW, vec128, rhs.vec128 );
   _vaddbc( one, vf_XYZW, vu0_vf0, vu0_vf0, vf_W );
   _vaddbc( ACC, vf_XYZW, result, result, vf_Y );
   _vmaddbc( result, vf_XYZW, ACC, one, result, vf_Z );
#endif
   return vu0_vec_x( result );
}

inline
vu0_vec_x
vu0_point_t::distance_sqr_from_origin() const
{
   return vu0_vector_t(*this).length_sqr();
}

inline
vu0_vec_x
vu0_point_t::distance_from_origin() const
{
   return vu0_vector_t(*this).length();
}

inline
vu0_vec_x
vu0_point_t::distance_sqr_from( vu0_point_t rhs ) const
{
   return (rhs - *this).length_sqr();
}

inline
vu0_vec_x
vu0_point_t::distance_from( vu0_point_t rhs ) const
{
   return (rhs - *this).length();
}

inline 
vu0_point_t
vu0_point_t::midpoint( vu0_point_t rhs ) const
{
   return vu0_point_t( ( vu0_vector_t(*this) + vu0_vector_t(rhs) ) * vu0_vec_x( 0.5f ) );
}

inline 
vu0_point_t
vu0_point_t::interpolate( float alpha, vu0_point_t rhs ) const
{
   return (*this) + ( ( rhs - (*this) ) * alpha );
}

inline
void
vu0_point_t::print() const
{
   printf( "(%f %f %f 1)\n",
           float( vu0_vec_x(*this) ), 
           float( vu0_vec_y(*this) ), 
           float( vu0_vec_z(*this) ) );
}

inline
void
vu0_point_t::print( const char* name ) const
{
   printf( "%s: (%f %f %f 1)\n", name,
           float( vu0_vec_x(*this) ), 
           float( vu0_vec_y(*this) ), 
           float( vu0_vec_z(*this) ) );
}

inline
vu0_vec_x::vu0_vec_x( __v4sf rhs )
{
   vec128 = rhs;
}

inline
vu0_vec_x::vu0_vec_x( float rhs )
{
   set( rhs );
}

inline
vu0_vec_x::vu0_vec_x( vu0_vec_y rhs )
{
   set( rhs );
}

inline
vu0_vec_x::vu0_vec_x( vu0_vec_z rhs )
{
   set( rhs );
}

inline
vu0_vec_x::vu0_vec_x( vu0_vec_w rhs )
{
   set( rhs );
}

inline
vu0_vec_x::vu0_vec_x( vu0_vec_xyz rhs )
{
   vec128 = rhs.vec128;
}

inline
vu0_vec_x::vu0_vec_x( vu0_vec_xyzw rhs )
{
   vec128 = rhs.vec128;
}

inline
vu0_vec_x::vu0_vec_x( vu0_vector_t rhs )
{
   vec128 = rhs.vec128;
}

inline
vu0_vec_x::vu0_vec_x( vu0_point_t rhs )
{
   vec128 = rhs.vec128;
}

inline
void
vu0_vec_x::set( __v4sf rhs )
{
   vec128 = rhs;
}

inline
void
vu0_vec_x::set( vu0_vec_x rhs )
{
   _vaddbc( vec128, vf_XYZW, vu0_vf0, rhs.vec128, vf_X );
}

inline
void
vu0_vec_x::set( vu0_vec_y rhs )
{
   _vaddbc( vec128, vf_XYZW, vu0_vf0, rhs.vec128, vf_Y );
}

inline
void
vu0_vec_x::set( vu0_vec_z rhs )
{
   _vaddbc( vec128, vf_XYZW, vu0_vf0, rhs.vec128, vf_Z );
}

inline
void
vu0_vec_x::set( vu0_vec_w rhs )
{
   _vaddbc( vec128, vf_XYZW, vu0_vf0, rhs.vec128, vf_W );
}

inline
void
vu0_vec_x::set( vu0_vec_xyz rhs )
{
   vec128 = rhs.vec128;
}

inline
void
vu0_vec_x::set( vu0_vec_xyzw rhs )
{
   vec128 = rhs.vec128;
}

inline
void
vu0_vec_x::set( vu0_vector_t rhs )
{
   vec128 = rhs.vec128;
}

inline
void
vu0_vec_x::set( vu0_point_t rhs )
{
   vec128 = rhs.vec128;
}

inline
void
vu0_vec_x::set( float rhs )
{
   vec128 = _vpromotef( rhs );
}

inline
void
vu0_vec_x::set_zero()
{
   _vsub( vec128, vf_XYZW, vu0_vf0, vu0_vf0 );
}

inline
vu0_vec_x::operator float() const
{
   return _vgetf( vec128, vf_X );
}

inline
__v4sf
vu0_vec_x::get128() const
{
   return vec128;
}

inline
vu0_vec_x&
vu0_vec_x::operator = ( vu0_vec_x rhs )
{
   vec128 = rhs.vec128;
   return *this;
}

inline
vu0_vec_x
vu0_vec_x::operator + ( vu0_vec_x rhs ) const
{
   __v4sf result;
   _vaddbc( result, vf_XYZW, vec128, rhs.vec128, vf_X );
   return vu0_vec_x( result );
}

inline
vu0_vec_x
vu0_vec_x::operator + ( vu0_vec_y rhs ) const
{
   __v4sf result;
   _vaddbc( result, vf_XYZW, vec128, rhs.vec128, vf_Y );
   return vu0_vec_x( result );
}

inline
vu0_vec_x
vu0_vec_x::operator + ( vu0_vec_z rhs ) const
{
   __v4sf result;
   _vaddbc( result, vf_XYZW, vec128, rhs.vec128, vf_Z );
   return vu0_vec_x( result );
}

inline
vu0_vec_x
vu0_vec_x::operator + ( vu0_vec_w rhs ) const
{
   __v4sf result;
   _vaddbc( result, vf_XYZW, vec128, rhs.vec128, vf_W );
   return vu0_vec_x( result );
}

inline
vu0_vec_x
vu0_vec_x::operator + ( float rhs ) const
{
   return *this + vu0_vec_x( rhs );
}

inline
void
vu0_vec_x::operator += ( vu0_vec_x rhs )
{
   *this = *this + rhs;
}

inline
void
vu0_vec_x::operator += ( vu0_vec_y rhs )
{
   *this = *this + rhs;
}

inline
void
vu0_vec_x::operator += ( vu0_vec_z rhs )
{
   *this = *this + rhs;
}

inline
void
vu0_vec_x::operator += ( vu0_vec_w rhs )
{
   *this = *this + rhs;
}

inline
void
vu0_vec_x::operator += ( float rhs )
{
   *this = *this + rhs;
}

inline
vu0_vec_x
vu0_vec_x::operator - ( vu0_vec_x rhs ) const
{
   __v4sf result;
   _vsubbc( result, vf_XYZW, vec128, rhs.vec128, vf_X );
   return vu0_vec_x( result );
}

inline
vu0_vec_x
vu0_vec_x::operator - ( vu0_vec_y rhs ) const
{
   __v4sf result;
   _vsubbc( result, vf_XYZW, vec128, rhs.vec128, vf_Y );
   return vu0_vec_x( result );
}

inline
vu0_vec_x
vu0_vec_x::operator - ( vu0_vec_z rhs ) const
{
   __v4sf result;
   _vsubbc( result, vf_XYZW, vec128, rhs.vec128, vf_Z );
   return vu0_vec_x( result );
}

inline
vu0_vec_x
vu0_vec_x::operator - ( vu0_vec_w rhs ) const
{
   __v4sf result;
   _vsubbc( result, vf_XYZW, vec128, rhs.vec128, vf_W );
   return vu0_vec_x( result );
}

inline
vu0_vec_x
vu0_vec_x::operator - ( float rhs ) const
{
   return *this - vu0_vec_x( rhs );
}

inline
void
vu0_vec_x::operator -= ( vu0_vec_x rhs )
{
   *this = *this - rhs;
}

inline
void
vu0_vec_x::operator -= ( vu0_vec_y rhs )
{
   *this = *this - rhs;
}

inline
void
vu0_vec_x::operator -= ( vu0_vec_z rhs )
{
   *this = *this - rhs;
}

inline
void
vu0_vec_x::operator -= ( vu0_vec_w rhs )
{
   *this = *this - rhs;
}

inline
void
vu0_vec_x::operator -= ( float rhs )
{
   *this = *this - rhs;
}

inline
vu0_vec_x
vu0_vec_x::operator * ( vu0_vec_x rhs ) const
{
   __v4sf result;
   _vmulbc( result, vf_XYZW, vec128, rhs.vec128, vf_X );
   return vu0_vec_x( result );
}

inline
vu0_vec_x
vu0_vec_x::operator * ( vu0_vec_y rhs ) const
{
   __v4sf result;
   _vmulbc( result, vf_XYZW, vec128, rhs.vec128, vf_Y );
   return vu0_vec_x( result );
}

inline
vu0_vec_x
vu0_vec_x::operator * ( vu0_vec_z rhs ) const
{
   __v4sf result;
   _vmulbc( result, vf_XYZW, vec128, rhs.vec128, vf_Z );
   return vu0_vec_x( result );
}

inline
vu0_vec_x
vu0_vec_x::operator * ( vu0_vec_w rhs ) const
{
   __v4sf result;
   _vmulbc( result, vf_XYZW, vec128, rhs.vec128, vf_W );
   return vu0_vec_x( result );
}

inline
vu0_vec_x
vu0_vec_x::operator * ( float rhs ) const
{
   return *this * vu0_vec_x( rhs );
}

inline
void
vu0_vec_x::operator *= ( vu0_vec_x rhs )
{
   *this = *this * rhs;
}

inline
void
vu0_vec_x::operator *= ( vu0_vec_y rhs )
{
   *this = *this * rhs;
}

inline
void
vu0_vec_x::operator *= ( vu0_vec_z rhs )
{
   *this = *this * rhs;
}

inline
void
vu0_vec_x::operator *= ( vu0_vec_w rhs )
{
   *this = *this * rhs;
}

inline
void
vu0_vec_x::operator *= ( float rhs )
{
   *this = *this * rhs;
}

inline
vu0_vec_x
vu0_vec_x::operator / ( vu0_vec_x rhs ) const
{
   __v4sf result;
   float Q;
   _vdiv( Q, vu0_vf0, vf_W, rhs.vec128, vf_X );
   _vwaitq( Q );
   _vmulf( result, vf_XYZW, vec128, Q );
   return vu0_vec_x( result );
}

inline
vu0_vec_x
vu0_vec_x::operator / ( vu0_vec_y rhs ) const
{
   __v4sf result;
   float Q;
   _vdiv( Q, vu0_vf0, vf_W, rhs.vec128, vf_Y );
   _vwaitq( Q );
   _vmulf( result, vf_XYZW, vec128, Q );
   return vu0_vec_x( result );
}

inline
vu0_vec_x
vu0_vec_x::operator / ( vu0_vec_z rhs ) const
{
   __v4sf result;
   float Q;
   _vdiv( Q, vu0_vf0, vf_W, rhs.vec128, vf_Z );
   _vwaitq( Q );
   _vmulf( result, vf_XYZW, vec128, Q );
   return vu0_vec_x( result );
}

inline
vu0_vec_x
vu0_vec_x::operator / ( vu0_vec_w rhs ) const
{
   __v4sf result;
   float Q;
   _vdiv( Q, vu0_vf0, vf_W, rhs.vec128, vf_W );
   _vwaitq( Q );
   _vmulf( result, vf_XYZW, vec128, Q );
   return vu0_vec_x( result );
}

inline
vu0_vec_x
vu0_vec_x::operator / ( float rhs ) const
{
   return *this / vu0_vec_x( rhs );
}

inline
void
vu0_vec_x::operator /= ( vu0_vec_x rhs )
{
   *this = *this / rhs;
}

inline
void
vu0_vec_x::operator /= ( vu0_vec_y rhs )
{
   *this = *this / rhs;
}

inline
void
vu0_vec_x::operator /= ( vu0_vec_z rhs )
{
   *this = *this / rhs;
}

inline
void
vu0_vec_x::operator /= ( vu0_vec_w rhs )
{
   *this = *this / rhs;
}

inline
void
vu0_vec_x::operator /= ( float rhs )
{
   *this = *this / rhs;
}

inline
vu0_vec_x
vu0_vec_x::operator - () const
{
   __v4sf result;
   _vsub(result, vf_XYZW, vu0_vf0, vec128);
   return vu0_vec_x( result );
}

inline
vu0_vec_x
vu0_vec_x::abs() const
{
   __v4sf result;
   _vabs( result, vf_XYZW, vec128 );
   return vu0_vec_x( result );
}

inline
vu0_vec_x
vu0_vec_x::sign() const
{
   __v4sf tmp;
   __v4si ones = (__v4si)(u_int128)0x3f800000;
   
   _vmulbc( tmp, vf_XYZW, vec128, vu0_vf0, vf_X );
   
   ones = _pextlw( ones, ones );
   ones = _pextlw( ones, ones );
   
   return vu0_vec_x( (__v4sf)_por( (__v4si)tmp, ones ) );
}

inline
vu0_vec_x
vu0_vec_x::max( vu0_vec_x rhs ) const
{
   __v4sf result;
   _vmax( result, vf_XYZW, vec128, rhs.vec128 );
   return vu0_vec_x( result );
}

inline
vu0_vec_x
vu0_vec_x::min( vu0_vec_x rhs ) const
{
   __v4sf result;
   _vmini( result, vf_XYZW, vec128, rhs.vec128 );
   return vu0_vec_x( result );
}

inline 
void 
vu0_vec_x::print() const 
{
   printf("(%f * * *)\n", float(*this));
}

inline 
void 
vu0_vec_x::print( const char* name ) const
{
   printf("%s: (%f * * *)\n", name, float(*this));
}

inline
vu0_vec_y::vu0_vec_y( __v4sf rhs )
{
   vec128 = rhs;
}

inline
vu0_vec_y::vu0_vec_y( float rhs )
{
   set( rhs );
}

inline
vu0_vec_y::vu0_vec_y( vu0_vec_x rhs )
{
   set( rhs );
}

inline
vu0_vec_y::vu0_vec_y( vu0_vec_z rhs )
{
   set( rhs );
}

inline
vu0_vec_y::vu0_vec_y( vu0_vec_w rhs )
{
   set( rhs );
}

inline
vu0_vec_y::vu0_vec_y( vu0_vec_xyz rhs )
{
   vec128 = rhs.vec128;
}

inline
vu0_vec_y::vu0_vec_y( vu0_vec_xyzw rhs )
{
   vec128 = rhs.vec128;
}

inline
vu0_vec_y::vu0_vec_y( vu0_vector_t rhs )
{
   vec128 = rhs.vec128;
}

inline
vu0_vec_y::vu0_vec_y( vu0_point_t rhs )
{
   vec128 = rhs.vec128;
}

inline
void
vu0_vec_y::set( __v4sf rhs )
{
   vec128 = rhs;
}

inline
void
vu0_vec_y::set( vu0_vec_y rhs )
{
   _vaddbc( vec128, vf_XYZW, vu0_vf0, rhs.vec128, vf_Y );
}

inline
void
vu0_vec_y::set( vu0_vec_x rhs )
{
   _vaddbc( vec128, vf_XYZW, vu0_vf0, rhs.vec128, vf_X );
}

inline
void
vu0_vec_y::set( vu0_vec_z rhs )
{
   _vaddbc( vec128, vf_XYZW, vu0_vf0, rhs.vec128, vf_Z );
}

inline
void
vu0_vec_y::set( vu0_vec_w rhs )
{
   _vaddbc( vec128, vf_XYZW, vu0_vf0, rhs.vec128, vf_W );
}

inline
void
vu0_vec_y::set( vu0_vec_xyz rhs )
{
   vec128 = rhs.vec128;
}

inline
void
vu0_vec_y::set( vu0_vec_xyzw rhs )
{
   vec128 = rhs.vec128;
}

inline
void
vu0_vec_y::set( vu0_vector_t rhs )
{
   vec128 = rhs.vec128;
}

inline
void
vu0_vec_y::set( vu0_point_t rhs )
{
   vec128 = rhs.vec128;
}

inline
void
vu0_vec_y::set( float rhs )
{
   __v4si tmp = (__v4si)_vpromotef( rhs );
   vec128 = (__v4sf)_pextlw( tmp, tmp );
}

inline
void
vu0_vec_y::set_zero()
{
   _vsub( vec128, vf_XYZW, vu0_vf0, vu0_vf0 );
}

inline
vu0_vec_y::operator float() const
{
   return _vgetf( vec128, vf_Y );
}

inline
__v4sf
vu0_vec_y::get128() const
{
   return vec128;
}

inline
vu0_vec_y&
vu0_vec_y::operator = ( vu0_vec_y rhs )
{
   vec128 = rhs.vec128;
   return *this;
}

inline
vu0_vec_y
vu0_vec_y::operator + ( vu0_vec_y rhs ) const
{
   __v4sf result;
   _vaddbc( result, vf_XYZW, vec128, rhs.vec128, vf_Y );
   return vu0_vec_y( result );
}

inline
vu0_vec_y
vu0_vec_y::operator + ( vu0_vec_x rhs ) const
{
   __v4sf result;
   _vaddbc( result, vf_XYZW, vec128, rhs.vec128, vf_X );
   return vu0_vec_y( result );
}

inline
vu0_vec_y
vu0_vec_y::operator + ( vu0_vec_z rhs ) const
{
   __v4sf result;
   _vaddbc( result, vf_XYZW, vec128, rhs.vec128, vf_Z );
   return vu0_vec_y( result );
}

inline
vu0_vec_y
vu0_vec_y::operator + ( vu0_vec_w rhs ) const
{
   __v4sf result;
   _vaddbc( result, vf_XYZW, vec128, rhs.vec128, vf_W );
   return vu0_vec_y( result );
}

inline
vu0_vec_y
vu0_vec_y::operator + ( float rhs ) const
{
   return *this + vu0_vec_x( rhs );
}

inline
void
vu0_vec_y::operator += ( vu0_vec_y rhs )
{
   *this = *this + rhs;
}

inline
void
vu0_vec_y::operator += ( vu0_vec_x rhs )
{
   *this = *this + rhs;
}

inline
void
vu0_vec_y::operator += ( vu0_vec_z rhs )
{
   *this = *this + rhs;
}

inline
void
vu0_vec_y::operator += ( vu0_vec_w rhs )
{
   *this = *this + rhs;
}

inline
void
vu0_vec_y::operator += ( float rhs )
{
   *this = *this + rhs;
}

inline
vu0_vec_y
vu0_vec_y::operator - ( vu0_vec_y rhs ) const
{
   __v4sf result;
   _vsubbc( result, vf_XYZW, vec128, rhs.vec128, vf_Y );
   return vu0_vec_y( result );
}

inline
vu0_vec_y
vu0_vec_y::operator - ( vu0_vec_x rhs ) const
{
   __v4sf result;
   _vsubbc( result, vf_XYZW, vec128, rhs.vec128, vf_X );
   return vu0_vec_y( result );
}

inline
vu0_vec_y
vu0_vec_y::operator - ( vu0_vec_z rhs ) const
{
   __v4sf result;
   _vsubbc( result, vf_XYZW, vec128, rhs.vec128, vf_Z );
   return vu0_vec_y( result );
}

inline
vu0_vec_y
vu0_vec_y::operator - ( vu0_vec_w rhs ) const
{
   __v4sf result;
   _vsubbc( result, vf_XYZW, vec128, rhs.vec128, vf_W );
   return vu0_vec_y( result );
}

inline
vu0_vec_y
vu0_vec_y::operator - ( float rhs ) const
{
   return *this - vu0_vec_x( rhs );
}

inline
void
vu0_vec_y::operator -= ( vu0_vec_y rhs )
{
   *this = *this - rhs;
}

inline
void
vu0_vec_y::operator -= ( vu0_vec_x rhs )
{
   *this = *this - rhs;
}

inline
void
vu0_vec_y::operator -= ( vu0_vec_z rhs )
{
   *this = *this - rhs;
}

inline
void
vu0_vec_y::operator -= ( vu0_vec_w rhs )
{
   *this = *this - rhs;
}

inline
void
vu0_vec_y::operator -= ( float rhs )
{
   *this = *this - rhs;
}

inline
vu0_vec_y
vu0_vec_y::operator * ( vu0_vec_y rhs ) const
{
   __v4sf result;
   _vmulbc( result, vf_XYZW, vec128, rhs.vec128, vf_Y );
   return vu0_vec_y( result );
}

inline
vu0_vec_y
vu0_vec_y::operator * ( vu0_vec_x rhs ) const
{
   __v4sf result;
   _vmulbc( result, vf_XYZW, vec128, rhs.vec128, vf_X );
   return vu0_vec_y( result );
}

inline
vu0_vec_y
vu0_vec_y::operator * ( vu0_vec_z rhs ) const
{
   __v4sf result;
   _vmulbc( result, vf_XYZW, vec128, rhs.vec128, vf_Z );
   return vu0_vec_y( result );
}

inline
vu0_vec_y
vu0_vec_y::operator * ( vu0_vec_w rhs ) const
{
   __v4sf result;
   _vmulbc( result, vf_XYZW, vec128, rhs.vec128, vf_W );
   return vu0_vec_y( result );
}

inline
vu0_vec_y
vu0_vec_y::operator * ( float rhs ) const
{
   return *this * vu0_vec_x( rhs );
}

inline
void
vu0_vec_y::operator *= ( vu0_vec_y rhs )
{
   *this = *this * rhs;
}

inline
void
vu0_vec_y::operator *= ( vu0_vec_x rhs )
{
   *this = *this * rhs;
}

inline
void
vu0_vec_y::operator *= ( vu0_vec_z rhs )
{
   *this = *this * rhs;
}

inline
void
vu0_vec_y::operator *= ( vu0_vec_w rhs )
{
   *this = *this * rhs;
}

inline
void
vu0_vec_y::operator *= ( float rhs )
{
   *this = *this * rhs;
}

inline
vu0_vec_y
vu0_vec_y::operator / ( vu0_vec_x rhs ) const
{
   __v4sf result;
   float Q;
   _vdiv( Q, vu0_vf0, vf_W, rhs.vec128, vf_X );
   _vwaitq( Q );
   _vmulf( result, vf_XYZW, vec128, Q );
   return vu0_vec_y( result );
}

inline
vu0_vec_y
vu0_vec_y::operator / ( vu0_vec_y rhs ) const
{
   __v4sf result;
   float Q;
   _vdiv( Q, vu0_vf0, vf_W, rhs.vec128, vf_Y );
   _vwaitq( Q );
   _vmulf( result, vf_XYZW, vec128, Q );
   return vu0_vec_y( result );
}

inline
vu0_vec_y
vu0_vec_y::operator / ( vu0_vec_z rhs ) const
{
   __v4sf result;
   float Q;
   _vdiv( Q, vu0_vf0, vf_W, rhs.vec128, vf_Z );
   _vwaitq( Q );
   _vmulf( result, vf_XYZW, vec128, Q );
   return vu0_vec_y( result );
}

inline
vu0_vec_y
vu0_vec_y::operator / ( vu0_vec_w rhs ) const
{
   __v4sf result;
   float Q;
   _vdiv( Q, vu0_vf0, vf_W, rhs.vec128, vf_W );
   _vwaitq( Q );
   _vmulf( result, vf_XYZW, vec128, Q );
   return vu0_vec_y( result );
}

inline
vu0_vec_y
vu0_vec_y::operator / ( float rhs ) const
{
   return *this / vu0_vec_x( rhs );
}

inline
void
vu0_vec_y::operator /= ( vu0_vec_x rhs )
{
   *this = *this / rhs;
}

inline
void
vu0_vec_y::operator /= ( vu0_vec_y rhs )
{
   *this = *this / rhs;
}

inline
void
vu0_vec_y::operator /= ( vu0_vec_z rhs )
{
   *this = *this / rhs;
}

inline
void
vu0_vec_y::operator /= ( vu0_vec_w rhs )
{
   *this = *this / rhs;
}

inline
void
vu0_vec_y::operator /= ( float rhs )
{
   *this = *this / rhs;
}

inline
vu0_vec_y
vu0_vec_y::operator - () const
{
   __v4sf result;
   _vsub(result, vf_XYZW, vu0_vf0, vec128);
   return vu0_vec_y( result );
}

inline
vu0_vec_y
vu0_vec_y::abs() const
{
   __v4sf result;
   _vabs( result, vf_XYZW, vec128 );
   return vu0_vec_y( result );
}

inline
vu0_vec_y
vu0_vec_y::sign() const
{
   __v4sf tmp;
   __v4si ones = (__v4si)(u_int128)0x3f800000;
   
   _vmulbc( tmp, vf_XYZW, vec128, vu0_vf0, vf_X );
   
   ones = _pextlw( ones, ones );
   ones = _pextlw( ones, ones );
   
   return vu0_vec_y( (__v4sf)_por( (__v4si)tmp, ones ) );
}

inline
vu0_vec_y
vu0_vec_y::max( vu0_vec_y rhs ) const
{
   __v4sf result;
   _vmax( result, vf_XYZW, vec128, rhs.vec128 );
   return vu0_vec_y( result );
}

inline
vu0_vec_y
vu0_vec_y::min( vu0_vec_y rhs ) const
{
   __v4sf result;
   _vmini( result, vf_XYZW, vec128, rhs.vec128 );
   return vu0_vec_y( result );
}

inline
void
vu0_vec_y::print() const
{
   printf("(* %f * *)\n", float(*this));
}

inline
void
vu0_vec_y::print( const char* name ) const 
{
   printf("%s: (* %f * *)\n", name, float(*this));
}

inline
vu0_vec_z::vu0_vec_z( __v4sf rhs )
{
   vec128 = rhs;
}

inline
vu0_vec_z::vu0_vec_z( float rhs )
{
   set( rhs );
}

inline
vu0_vec_z::vu0_vec_z( vu0_vec_x rhs )
{
   set( rhs );
}

inline
vu0_vec_z::vu0_vec_z( vu0_vec_y rhs )
{
   set( rhs );
}

inline
vu0_vec_z::vu0_vec_z( vu0_vec_w rhs )
{
   set( rhs );
}

inline
vu0_vec_z::vu0_vec_z( vu0_vec_xyz rhs )
{
   vec128 = rhs.vec128;
}

inline
vu0_vec_z::vu0_vec_z( vu0_vec_xyzw rhs )
{
   vec128 = rhs.vec128;
}

inline
vu0_vec_z::vu0_vec_z( vu0_vector_t rhs )
{
   vec128 = rhs.vec128;
}

inline
vu0_vec_z::vu0_vec_z( vu0_point_t rhs )
{
   vec128 = rhs.vec128;
}

inline
void
vu0_vec_z::set( __v4sf rhs )
{
   vec128 = rhs;
}

inline
void
vu0_vec_z::set( vu0_vec_z rhs )
{
   _vaddbc( vec128, vf_XYZW, vu0_vf0, rhs.vec128, vf_Z );
}

inline
void
vu0_vec_z::set( vu0_vec_x rhs )
{
   _vaddbc( vec128, vf_XYZW, vu0_vf0, rhs.vec128, vf_X );
}

inline
void
vu0_vec_z::set( vu0_vec_y rhs )
{
   _vaddbc( vec128, vf_XYZW, vu0_vf0, rhs.vec128, vf_Y );
}

inline
void
vu0_vec_z::set( vu0_vec_w rhs )
{
   _vaddbc( vec128, vf_XYZW, vu0_vf0, rhs.vec128, vf_W );
}

inline
void
vu0_vec_z::set( vu0_vec_xyz rhs )
{
   vec128 = rhs.vec128;
}

inline
void
vu0_vec_z::set( vu0_vec_xyzw rhs )
{
   vec128 = rhs.vec128;
}

inline
void
vu0_vec_z::set( vu0_vector_t rhs )
{
   vec128 = rhs.vec128;
}

inline
void
vu0_vec_z::set( vu0_point_t rhs )
{
   vec128 = rhs.vec128;
}

inline
void
vu0_vec_z::set( float rhs )
{
   __v4si tmp = (__v4si)_vpromotef( rhs );
   vec128 = (__v4sf)_pexew( tmp );
}

inline
void
vu0_vec_z::set_zero()
{
   _vsub( vec128, vf_XYZW, vu0_vf0, vu0_vf0 );
}

inline
vu0_vec_z::operator float() const
{
   return _vgetf( vec128, vf_Z );
}

inline
__v4sf
vu0_vec_z::get128() const
{
   return vec128;
}

inline
vu0_vec_z&
vu0_vec_z::operator = ( vu0_vec_z rhs )
{
   vec128 = rhs.vec128;
   return *this;
}

inline
vu0_vec_z
vu0_vec_z::operator + ( vu0_vec_z rhs ) const
{
   __v4sf result;
   _vaddbc( result, vf_XYZW, vec128, rhs.vec128, vf_Z );
   return vu0_vec_z( result );
}

inline
vu0_vec_z
vu0_vec_z::operator + ( vu0_vec_x rhs ) const
{
   __v4sf result;
   _vaddbc( result, vf_XYZW, vec128, rhs.vec128, vf_X );
   return vu0_vec_z( result );
}

inline
vu0_vec_z
vu0_vec_z::operator + ( vu0_vec_y rhs ) const
{
   __v4sf result;
   _vaddbc( result, vf_XYZW, vec128, rhs.vec128, vf_Y );
   return vu0_vec_z( result );
}

inline
vu0_vec_z
vu0_vec_z::operator + ( vu0_vec_w rhs ) const
{
   __v4sf result;
   _vaddbc( result, vf_XYZW, vec128, rhs.vec128, vf_W );
   return vu0_vec_z( result );
}

inline
vu0_vec_z
vu0_vec_z::operator + ( float rhs ) const
{
   return *this + vu0_vec_x( rhs );
}

inline
void
vu0_vec_z::operator += ( vu0_vec_z rhs )
{
   *this = *this + rhs;
}

inline
void
vu0_vec_z::operator += ( vu0_vec_x rhs )
{
   *this = *this + rhs;
}

inline
void
vu0_vec_z::operator += ( vu0_vec_y rhs )
{
   *this = *this + rhs;
}

inline
void
vu0_vec_z::operator += ( vu0_vec_w rhs )
{
   *this = *this + rhs;
}

inline
void
vu0_vec_z::operator += ( float rhs )
{
   *this = *this + rhs;
}

inline
vu0_vec_z
vu0_vec_z::operator - ( vu0_vec_z rhs ) const
{
   __v4sf result;
   _vsubbc( result, vf_XYZW, vec128, rhs.vec128, vf_Z );
   return vu0_vec_z( result );
}

inline
vu0_vec_z
vu0_vec_z::operator - ( vu0_vec_x rhs ) const
{
   __v4sf result;
   _vsubbc( result, vf_XYZW, vec128, rhs.vec128, vf_X );
   return vu0_vec_z( result );
}

inline
vu0_vec_z
vu0_vec_z::operator - ( vu0_vec_y rhs ) const
{
   __v4sf result;
   _vsubbc( result, vf_XYZW, vec128, rhs.vec128, vf_Y );
   return vu0_vec_z( result );
}

inline
vu0_vec_z
vu0_vec_z::operator - ( vu0_vec_w rhs ) const
{
   __v4sf result;
   _vsubbc( result, vf_XYZW, vec128, rhs.vec128, vf_W );
   return vu0_vec_z( result );
}

inline
vu0_vec_z
vu0_vec_z::operator - ( float rhs ) const
{
   return *this - vu0_vec_x( rhs );
}

inline
void
vu0_vec_z::operator -= ( vu0_vec_z rhs )
{
   *this = *this - rhs;
}

inline
void
vu0_vec_z::operator -= ( vu0_vec_x rhs )
{
   *this = *this - rhs;
}

inline
void
vu0_vec_z::operator -= ( vu0_vec_y rhs )
{
   *this = *this - rhs;
}

inline
void
vu0_vec_z::operator -= ( vu0_vec_w rhs )
{
   *this = *this - rhs;
}

inline
void
vu0_vec_z::operator -= ( float rhs )
{
   *this = *this - rhs;
}

inline
vu0_vec_z
vu0_vec_z::operator * ( vu0_vec_z rhs ) const
{
   __v4sf result;
   _vmulbc( result, vf_XYZW, vec128, rhs.vec128, vf_Z );
   return vu0_vec_z( result );
}

inline
vu0_vec_z
vu0_vec_z::operator * ( vu0_vec_x rhs ) const
{
   __v4sf result;
   _vmulbc( result, vf_XYZW, vec128, rhs.vec128, vf_X );
   return vu0_vec_z( result );
}

inline
vu0_vec_z
vu0_vec_z::operator * ( vu0_vec_y rhs ) const
{
   __v4sf result;
   _vmulbc( result, vf_XYZW, vec128, rhs.vec128, vf_Y );
   return vu0_vec_z( result );
}

inline
vu0_vec_z
vu0_vec_z::operator * ( vu0_vec_w rhs ) const
{
   __v4sf result;
   _vmulbc( result, vf_XYZW, vec128, rhs.vec128, vf_W );
   return vu0_vec_z( result );
}

inline
vu0_vec_z
vu0_vec_z::operator * ( float rhs ) const
{
   return *this * vu0_vec_x( rhs );
}

inline
void
vu0_vec_z::operator *= ( vu0_vec_z rhs )
{
   *this = *this * rhs;
}

inline
void
vu0_vec_z::operator *= ( vu0_vec_x rhs )
{
   *this = *this * rhs;
}

inline
void
vu0_vec_z::operator *= ( vu0_vec_y rhs )
{
   *this = *this * rhs;
}

inline
void
vu0_vec_z::operator *= ( vu0_vec_w rhs )
{
   *this = *this * rhs;
}

inline
void
vu0_vec_z::operator *= ( float rhs )
{
   *this = *this * rhs;
}

inline
vu0_vec_z
vu0_vec_z::operator / ( vu0_vec_x rhs ) const
{
   __v4sf result;
   float Q;
   _vdiv( Q, vu0_vf0, vf_W, rhs.vec128, vf_X );
   _vwaitq( Q );
   _vmulf( result, vf_XYZW, vec128, Q );
   return vu0_vec_z( result );
}

inline
vu0_vec_z
vu0_vec_z::operator / ( vu0_vec_y rhs ) const
{
   __v4sf result;
   float Q;
   _vdiv( Q, vu0_vf0, vf_W, rhs.vec128, vf_Y );
   _vwaitq( Q );
   _vmulf( result, vf_XYZW, vec128, Q );
   return vu0_vec_z( result );
}

inline
vu0_vec_z
vu0_vec_z::operator / ( vu0_vec_z rhs ) const
{
   __v4sf result;
   float Q;
   _vdiv( Q, vu0_vf0, vf_W, rhs.vec128, vf_Z );
   _vwaitq( Q );
   _vmulf( result, vf_XYZW, vec128, Q );
   return vu0_vec_z( result );
}

inline
vu0_vec_z
vu0_vec_z::operator / ( vu0_vec_w rhs ) const
{
   __v4sf result;
   float Q;
   _vdiv( Q, vu0_vf0, vf_W, rhs.vec128, vf_W );
   _vwaitq( Q );
   _vmulf( result, vf_XYZW, vec128, Q );
   return vu0_vec_z( result );
}

inline
vu0_vec_z
vu0_vec_z::operator / ( float rhs ) const
{
   return *this / vu0_vec_x( rhs );
}

inline
void
vu0_vec_z::operator /= ( vu0_vec_x rhs )
{
   *this = *this / rhs;
}

inline
void
vu0_vec_z::operator /= ( vu0_vec_y rhs )
{
   *this = *this / rhs;
}

inline
void
vu0_vec_z::operator /= ( vu0_vec_z rhs )
{
   *this = *this / rhs;
}

inline
void
vu0_vec_z::operator /= ( vu0_vec_w rhs )
{
   *this = *this / rhs;
}

inline
void
vu0_vec_z::operator /= ( float rhs )
{
   *this = *this / rhs;
}

inline
vu0_vec_z
vu0_vec_z::operator - () const
{
   __v4sf result;
   _vsub(result, vf_XYZW, vu0_vf0, vec128);
   return vu0_vec_z( result );
}

inline
vu0_vec_z
vu0_vec_z::abs() const
{
   __v4sf result;
   _vabs( result, vf_XYZW, vec128 );
   return vu0_vec_z( result );
}

inline
vu0_vec_z
vu0_vec_z::sign() const
{
   __v4sf tmp;
   __v4si ones = (__v4si)(u_int128)0x3f800000;
   
   _vmulbc( tmp, vf_XYZW, vec128, vu0_vf0, vf_X );
   
   ones = _pextlw( ones, ones );
   ones = _pextlw( ones, ones );
   
   return vu0_vec_z( (__v4sf)_por( (__v4si)tmp, ones ) );
}

inline
vu0_vec_z
vu0_vec_z::max( vu0_vec_z rhs ) const
{
   __v4sf result;
   _vmax( result, vf_XYZW, vec128, rhs.vec128 );
   return vu0_vec_z( result );
}

inline
vu0_vec_z
vu0_vec_z::min( vu0_vec_z rhs ) const
{
   __v4sf result;
   _vmini( result, vf_XYZW, vec128, rhs.vec128 );
   return vu0_vec_z( result );
}

inline
void
vu0_vec_z::print() const
{
   printf("(* * %f *)\n", float(*this));
}

inline
void
vu0_vec_z::print( const char* name ) const
{
   printf("%s: (* * %f *)\n", name, float(*this));
}

inline
vu0_vec_w::vu0_vec_w( __v4sf rhs )
{
   vec128 = rhs;
}

inline
vu0_vec_w::vu0_vec_w( float rhs )
{
   set( rhs );
}

inline
vu0_vec_w::vu0_vec_w( vu0_vec_x rhs )
{
   set( rhs );
}

inline
vu0_vec_w::vu0_vec_w( vu0_vec_y rhs )
{
   set( rhs );
}

inline
vu0_vec_w::vu0_vec_w( vu0_vec_z rhs )
{
   set( rhs );
}

inline
vu0_vec_w::vu0_vec_w( vu0_vec_xyzw rhs )
{
   vec128 = rhs.vec128;
}

inline
void
vu0_vec_w::set( __v4sf rhs )
{
   vec128 = rhs;
}

inline
void
vu0_vec_w::set( vu0_vec_w rhs )
{
   _vmulbc( vec128, vf_XYZW, vu0_vf0, rhs.vec128, vf_W );
}

inline
void
vu0_vec_w::set( vu0_vec_x rhs )
{
   _vmulbc( vec128, vf_XYZW, vu0_vf0, rhs.vec128, vf_X );
}

inline
void
vu0_vec_w::set( vu0_vec_y rhs )
{
   _vmulbc( vec128, vf_XYZW, vu0_vf0, rhs.vec128, vf_Y );
}

inline
void
vu0_vec_w::set( vu0_vec_z rhs )
{
   _vmulbc( vec128, vf_XYZW, vu0_vf0, rhs.vec128, vf_Z );
}

inline
void
vu0_vec_w::set( vu0_vec_xyzw rhs )
{
   vec128 = rhs.vec128;
}

inline
void
vu0_vec_w::set( float rhs )
{
   __v4si tmp = (__v4si)_vpromotef( rhs );
   tmp = _pextlw( tmp, tmp );
   vec128 = (__v4sf)_pextlw( tmp, tmp );
}

inline
void
vu0_vec_w::set_zero()
{
   _vsub( vec128, vf_XYZW, vu0_vf0, vu0_vf0 );
}

inline
vu0_vec_w::operator float() const
{
   return _vgetf( vec128, vf_W );
}

inline
__v4sf
vu0_vec_w::get128() const
{
   return vec128;
}

inline
vu0_vec_w&
vu0_vec_w::operator = ( vu0_vec_w rhs )
{
   vec128 = rhs.vec128;
   return *this;
}

inline
vu0_vec_w
vu0_vec_w::operator + ( vu0_vec_w rhs ) const
{
   __v4sf result;
   _vaddbc( result, vf_XYZW, vec128, rhs.vec128, vf_W );
   return vu0_vec_w( result );
}

inline
vu0_vec_w
vu0_vec_w::operator + ( vu0_vec_x rhs ) const
{
   __v4sf result;
   _vaddbc( result, vf_XYZW, vec128, rhs.vec128, vf_X );
   return vu0_vec_w( result );
}

inline
vu0_vec_w
vu0_vec_w::operator + ( vu0_vec_y rhs ) const
{
   __v4sf result;
   _vaddbc( result, vf_XYZW, vec128, rhs.vec128, vf_Y );
   return vu0_vec_w( result );
}

inline
vu0_vec_w
vu0_vec_w::operator + ( vu0_vec_z rhs ) const
{
   __v4sf result;
   _vaddbc( result, vf_XYZW, vec128, rhs.vec128, vf_Z );
   return vu0_vec_w( result );
}

inline
vu0_vec_w
vu0_vec_w::operator + ( float rhs ) const
{
   return *this + vu0_vec_x( rhs );
}

inline
void
vu0_vec_w::operator += ( vu0_vec_w rhs )
{
   *this = *this + rhs;
}

inline
void
vu0_vec_w::operator += ( vu0_vec_x rhs )
{
   *this = *this + rhs;
}

inline
void
vu0_vec_w::operator += ( vu0_vec_y rhs )
{
   *this = *this + rhs;
}

inline
void
vu0_vec_w::operator += ( vu0_vec_z rhs )
{
   *this = *this + rhs;
}

inline
void
vu0_vec_w::operator += ( float rhs )
{
   *this = *this + rhs;
}

inline
vu0_vec_w
vu0_vec_w::operator - ( vu0_vec_w rhs ) const
{
   __v4sf result;
   _vsubbc( result, vf_XYZW, vec128, rhs.vec128, vf_W );
   return vu0_vec_w( result );
}

inline
vu0_vec_w
vu0_vec_w::operator - ( vu0_vec_x rhs ) const
{
   __v4sf result;
   _vsubbc( result, vf_XYZW, vec128, rhs.vec128, vf_X );
   return vu0_vec_w( result );
}

inline
vu0_vec_w
vu0_vec_w::operator - ( vu0_vec_y rhs ) const
{
   __v4sf result;
   _vsubbc( result, vf_XYZW, vec128, rhs.vec128, vf_Y );
   return vu0_vec_w( result );
}

inline
vu0_vec_w
vu0_vec_w::operator - ( vu0_vec_z rhs ) const
{
   __v4sf result;
   _vsubbc( result, vf_XYZW, vec128, rhs.vec128, vf_Z );
   return vu0_vec_w( result );
}

inline
vu0_vec_w
vu0_vec_w::operator - ( float rhs ) const
{
   return *this - vu0_vec_x( rhs );
}

inline
void
vu0_vec_w::operator -= ( vu0_vec_w rhs )
{
   *this = *this - rhs;
}

inline
void
vu0_vec_w::operator -= ( vu0_vec_x rhs )
{
   *this = *this - rhs;
}

inline
void
vu0_vec_w::operator -= ( vu0_vec_y rhs )
{
   *this = *this - rhs;
}

inline
void
vu0_vec_w::operator -= ( vu0_vec_z rhs )
{
   *this = *this - rhs;
}

inline
void
vu0_vec_w::operator -= ( float rhs )
{
   *this = *this - rhs;
}

inline
vu0_vec_w
vu0_vec_w::operator * ( vu0_vec_w rhs ) const
{
   __v4sf result;
   _vmulbc( result, vf_XYZW, vec128, rhs.vec128, vf_W );
   return vu0_vec_w( result );
}

inline
vu0_vec_w
vu0_vec_w::operator * ( vu0_vec_x rhs ) const
{
   __v4sf result;
   _vmulbc( result, vf_XYZW, vec128, rhs.vec128, vf_X );
   return vu0_vec_w( result );
}

inline
vu0_vec_w
vu0_vec_w::operator * ( vu0_vec_y rhs ) const
{
   __v4sf result;
   _vmulbc( result, vf_XYZW, vec128, rhs.vec128, vf_Y );
   return vu0_vec_w( result );
}

inline
vu0_vec_w
vu0_vec_w::operator * ( vu0_vec_z rhs ) const
{
   __v4sf result;
   _vmulbc( result, vf_XYZW, vec128, rhs.vec128, vf_Z );
   return vu0_vec_w( result );
}

inline
vu0_vec_w
vu0_vec_w::operator * ( float rhs ) const
{
   return *this * vu0_vec_x( rhs );
}

inline
void
vu0_vec_w::operator *= ( vu0_vec_w rhs )
{
   *this = *this * rhs;
}

inline
void
vu0_vec_w::operator *= ( vu0_vec_x rhs )
{
   *this = *this * rhs;
}

inline
void
vu0_vec_w::operator *= ( vu0_vec_y rhs )
{
   *this = *this * rhs;
}

inline
void
vu0_vec_w::operator *= ( vu0_vec_z rhs )
{
   *this = *this * rhs;
}

inline
void
vu0_vec_w::operator *= ( float rhs )
{
   *this = *this * rhs;
}

inline
vu0_vec_w
vu0_vec_w::operator / ( vu0_vec_x rhs ) const
{
   __v4sf result;
   float Q;
   _vdiv( Q, vu0_vf0, vf_W, rhs.vec128, vf_X );
   _vwaitq( Q );
   _vmulf( result, vf_XYZW, vec128, Q );
   return vu0_vec_w( result );
}

inline
vu0_vec_w
vu0_vec_w::operator / ( vu0_vec_y rhs ) const
{
   __v4sf result;
   float Q;
   _vdiv( Q, vu0_vf0, vf_W, rhs.vec128, vf_Y );
   _vwaitq( Q );
   _vmulf( result, vf_XYZW, vec128, Q );
   return vu0_vec_w( result );
}

inline
vu0_vec_w
vu0_vec_w::operator / ( vu0_vec_z rhs ) const
{
   __v4sf result;
   float Q;
   _vdiv( Q, vu0_vf0, vf_W, rhs.vec128, vf_Z );
   _vwaitq( Q );
   _vmulf( result, vf_XYZW, vec128, Q );
   return vu0_vec_w( result );
}

inline
vu0_vec_w
vu0_vec_w::operator / ( vu0_vec_w rhs ) const
{
   __v4sf result;
   float Q;
   _vdiv( Q, vu0_vf0, vf_W, rhs.vec128, vf_W );
   _vwaitq( Q );
   _vmulf( result, vf_XYZW, vec128, Q );
   return vu0_vec_w( result );
}

inline
vu0_vec_w
vu0_vec_w::operator / ( float rhs ) const
{
   return *this / vu0_vec_x( rhs );
}

inline
void
vu0_vec_w::operator /= ( vu0_vec_x rhs )
{
   *this = *this / rhs;
}

inline
void
vu0_vec_w::operator /= ( vu0_vec_y rhs )
{
   *this = *this / rhs;
}

inline
void
vu0_vec_w::operator /= ( vu0_vec_z rhs )
{
   *this = *this / rhs;
}

inline
void
vu0_vec_w::operator /= ( vu0_vec_w rhs )
{
   *this = *this / rhs;
}

inline
void
vu0_vec_w::operator /= ( float rhs )
{
   *this = *this / rhs;
}

inline
vu0_vec_w
vu0_vec_w::operator - () const
{
   __v4sf result;
  __v4sf ACC;
  _vsub( ACC, vf_XYZW, vu0_vf0, vu0_vf0 );
  _vmsubbc( result, vf_XYZW, ACC, vec128, vu0_vf0, vf_W );
   return vu0_vec_w( result );
}

inline
vu0_vec_w
vu0_vec_w::abs() const
{
   __v4sf result;
   _vabs( result, vf_XYZW, vec128 );
   return vu0_vec_w( result );
}

inline
vu0_vec_w
vu0_vec_w::sign() const
{
   __v4sf tmp;
   __v4si ones = (__v4si)(u_int128)0x3f800000;
   
   _vmulbc( tmp, vf_XYZW, vec128, vu0_vf0, vf_X );
   
   ones = _pextlw( ones, ones );
   ones = _pextlw( ones, ones );
   
   return vu0_vec_w( (__v4sf)_por( (__v4si)tmp, ones ) );
}

inline
vu0_vec_w
vu0_vec_w::max( vu0_vec_w rhs ) const
{
   __v4sf result;
   _vmax( result, vf_XYZW, vec128, rhs.vec128 );
   return vu0_vec_w( result );
}

inline
vu0_vec_w
vu0_vec_w::min( vu0_vec_w rhs ) const
{
   __v4sf result;
   _vmini( result, vf_XYZW, vec128, rhs.vec128 );
   return vu0_vec_w( result );
}

inline
void
vu0_vec_w::print() const
{
   printf("(* * * %f)\n", float(*this));
}

inline
void
vu0_vec_w::print( const char* name ) const
{
   printf("%s: (* * * %f)\n", name, float(*this));
}

/********************************************
 * We need to switch the order of expressions of the form
 * scalar * vector or scalar + vector, where 'scalar' is
 * a float or vec_? and 'vector' is a multi-element vector.
 */
 
inline vu0_vec_xyz operator + ( float scalar, vu0_vec_xyz vec ) { return vec + scalar; }
inline vu0_vec_xyzw operator + ( float scalar, vu0_vec_xyzw vec ) { return vec + scalar; }
inline vu0_vec_x operator + ( float scalar, vu0_vec_x vec ) { return vec + scalar; }
inline vu0_vec_y operator + ( float scalar, vu0_vec_y vec ) { return vec + scalar; }
inline vu0_vec_z operator + ( float scalar, vu0_vec_z vec ) { return vec + scalar; }
inline vu0_vec_w operator + ( float scalar, vu0_vec_w vec ) { return vec + scalar; }

inline
vu0_vec_xyz
operator + ( vu0_vec_x scalar, vu0_vec_xyz vec )
{
   return vec + scalar;
}

inline
vu0_vec_xyzw
operator + ( vu0_vec_x scalar, vu0_vec_xyzw vec )
{
   return vec + scalar;
}

inline
vu0_vec_xyz
operator + ( vu0_vec_y scalar, vu0_vec_xyz vec )
{
   return vec + scalar;
}

inline
vu0_vec_xyzw
operator + ( vu0_vec_y scalar, vu0_vec_xyzw vec )
{
   return vec + scalar;
}

inline
vu0_vec_xyz
operator + ( vu0_vec_z scalar, vu0_vec_xyz vec )
{
   return vec + scalar;
}

inline
vu0_vec_xyzw
operator + ( vu0_vec_z scalar, vu0_vec_xyzw vec )
{
   return vec + scalar;
}

inline
vu0_vec_xyz
operator + ( vu0_vec_w scalar, vu0_vec_xyz vec )
{
   return vec + scalar;
}

inline
vu0_vec_xyzw
operator + ( vu0_vec_w scalar, vu0_vec_xyzw vec )
{
   return vec + scalar;
}

inline vu0_vec_xyz operator * ( float scalar, vu0_vec_xyz vec ) { return vec * scalar; }
inline vu0_vec_xyzw operator * ( float scalar, vu0_vec_xyzw vec ) { return vec * scalar; }
inline vu0_vector_t operator * ( float scalar, vu0_vector_t vec ) { return vec * scalar; }
inline vu0_vec_x operator * ( float scalar, vu0_vec_x vec ) { return vec * scalar; }
inline vu0_vec_y operator * ( float scalar, vu0_vec_y vec ) { return vec * scalar; }
inline vu0_vec_z operator * ( float scalar, vu0_vec_z vec ) { return vec * scalar; }
inline vu0_vec_w operator * ( float scalar, vu0_vec_w vec ) { return vec * scalar; }

inline
vu0_vec_xyz
operator * ( vu0_vec_x scalar, vu0_vec_xyz vec )
{
   return vec * scalar;
}

inline
vu0_vec_xyzw
operator * ( vu0_vec_x scalar, vu0_vec_xyzw vec )
{
   return vec * scalar;
}

inline
vu0_vector_t
operator * ( vu0_vec_x scalar, vu0_vector_t vec )
{
   return vec * scalar;
}

inline
vu0_vec_xyz
operator * ( vu0_vec_y scalar, vu0_vec_xyz vec )
{
   return vec * scalar;
}

inline
vu0_vec_xyzw
operator * ( vu0_vec_y scalar, vu0_vec_xyzw vec )
{
   return vec * scalar;
}

inline
vu0_vector_t
operator * ( vu0_vec_y scalar, vu0_vector_t vec )
{
   return vec * scalar;
}

inline
vu0_vec_xyz
operator * ( vu0_vec_z scalar, vu0_vec_xyz vec )
{
   return vec * scalar;
}

inline
vu0_vec_xyzw
operator * ( vu0_vec_z scalar, vu0_vec_xyzw vec )
{
   return vec * scalar;
}

inline
vu0_vector_t
operator * ( vu0_vec_z scalar, vu0_vector_t vec )
{
   return vec * scalar;
}

inline
vu0_vec_xyz
operator * ( vu0_vec_w scalar, vu0_vec_xyz vec )
{
   return vec * scalar;
}

inline
vu0_vec_xyzw
operator * ( vu0_vec_w scalar, vu0_vec_xyzw vec )
{
   return vec * scalar;
}

inline
vu0_vector_t
operator * ( vu0_vec_w scalar, vu0_vector_t vec )
{
   return vec * scalar;
}

#endif // vu0_vector_h
