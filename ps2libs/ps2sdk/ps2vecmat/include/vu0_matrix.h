/*           Copyright (C) 2003 Sony Computer Entertainment America
                              All Rights Reserved
                               SCEA Confidential                                */
#ifndef vu0_matrix_h
#define vu0_matrix_h

#include "vu0_vector.h"

/********************************************
 * Forward Declarations
 */

class vu0_mat_33;
class vu0_mat_34;
class vu0_mat_43;
class vu0_mat_44;
class vu0_transform_t;
class core_mat_44;

class vu0_mat_33
{
   public:
      vu0_vec_3 col0;
      vu0_vec_3 col1;
      vu0_vec_3 col2;

      inline vu0_mat_33() { }
      inline vu0_mat_33( const vu0_mat_33& rhs );
      inline vu0_mat_33( vu0_vec_3 col_0, vu0_vec_3 col_1, vu0_vec_3 col_2 );

      // construct from unit quaternion

      explicit inline vu0_mat_33( vu0_vec_4 quat );

      // get rows and columns

      inline vu0_vec_3 get_col0() const;
      inline vu0_vec_3 get_col1() const;
      inline vu0_vec_3 get_col2() const;

      inline vu0_vec_3 get_row0() const;
      inline vu0_vec_3 get_row1() const;
      inline vu0_vec_3 get_row2() const;

      // sets

      inline void set( const vu0_mat_33& rhs );
      inline void set( vu0_vec_3 col_0, vu0_vec_3 col_1, vu0_vec_3 col_2 );
      inline void set( vu0_vec_4 quat );

      // set rows and columns

      inline void set_col0( vu0_vec_3 rhs );
      inline void set_col1( vu0_vec_3 rhs );
      inline void set_col2( vu0_vec_3 rhs );

      inline void set_row0( vu0_vec_3 rhs );
      inline void set_row1( vu0_vec_3 rhs );
      inline void set_row2( vu0_vec_3 rhs );

      // set matrix to zeros

      inline void set_zero();

      // set entire matrix for a transformation

      inline void set_identity();
      inline void set_scale( vu0_vec_3 scale );
      inline void set_rotate_x( float angle );
      inline void set_rotate_y( float angle );
      inline void set_rotate_z( float angle );

      // rotate around axis by angle.  expects unit vector

      void set_rotate( float angle, vu0_vec_3 axis );
      inline void set_translate( vu0_vec_3 translation );

      // assignments

      inline vu0_mat_33& operator = ( const vu0_mat_33& rhs );

      // get a transposed matrix

      inline vu0_mat_33 transposed() const;

      // transpose this matrix

      inline void transpose();

      // get an inverse of the matrix

      inline vu0_mat_33 inverse() const;

      // invert this matrix

      inline void invert();

      inline vu0_vec_x determinant() const;

      // add, subtract, negate, absolute value

      inline vu0_mat_33 operator + ( const vu0_mat_33& rhs ) const;
      inline vu0_mat_33 operator - ( const vu0_mat_33& rhs ) const;

      inline void operator += ( const vu0_mat_33& rhs );
      inline void operator -= ( const vu0_mat_33& rhs );

      inline vu0_mat_33 operator - () const;

      inline vu0_mat_33 abs() const;

      // matrix scalar multiplies

      inline vu0_mat_33 operator * ( vu0_vec_x rhs ) const;
      inline vu0_mat_33 operator * ( vu0_vec_y rhs ) const;
      inline vu0_mat_33 operator * ( vu0_vec_z rhs ) const;
      inline vu0_mat_33 operator * ( vu0_vec_w rhs ) const;
      inline vu0_mat_33 operator * ( float rhs ) const;

      inline void operator *= ( vu0_vec_x rhs );
      inline void operator *= ( vu0_vec_y rhs );
      inline void operator *= ( vu0_vec_z rhs );
      inline void operator *= ( vu0_vec_w rhs );
      inline void operator *= ( float rhs );

      // matrix vector multiplies

      inline vu0_vec_3 operator * ( vu0_vec_3 rhs ) const;

      // matrix matrix multiplies

      inline vu0_mat_33 operator * ( const vu0_mat_33& rhs ) const;
      inline vu0_mat_34 operator * ( const vu0_mat_34& rhs ) const;

      // print

      inline void print() const;
      inline void print( const char* name ) const;
};

class vu0_mat_34
{
   public:
      vu0_vec_3 col0;
      vu0_vec_3 col1;
      vu0_vec_3 col2;
      vu0_vec_3 col3;

      inline vu0_mat_34() { }
      inline vu0_mat_34( const vu0_mat_34& rhs );
      inline vu0_mat_34( vu0_vec_3 col_0, vu0_vec_3 col_1, vu0_vec_3 col_2, vu0_vec_3 col_3 );

      // get rows and columns

      inline vu0_vec_3 get_col0() const;
      inline vu0_vec_3 get_col1() const;
      inline vu0_vec_3 get_col2() const;
      inline vu0_vec_3 get_col3() const;

      inline vu0_vec_4 get_row0() const;
      inline vu0_vec_4 get_row1() const;
      inline vu0_vec_4 get_row2() const;

      // sets

      inline void set( const vu0_mat_34& rhs );
      inline void set( vu0_vec_3 col_0, vu0_vec_3 col_1, vu0_vec_3 col_2, vu0_vec_3 col_3 );

      // set rows and columns

      inline void set_col0( vu0_vec_3 rhs );
      inline void set_col1( vu0_vec_3 rhs );
      inline void set_col2( vu0_vec_3 rhs );
      inline void set_col3( vu0_vec_3 rhs );

      inline void set_row0( vu0_vec_4 rhs );
      inline void set_row1( vu0_vec_4 rhs );
      inline void set_row2( vu0_vec_4 rhs );

      // set matrix to zeros

      inline void set_zero();

      // assignments

      inline vu0_mat_34& operator = ( const vu0_mat_33& rhs );
      inline vu0_mat_34& operator = ( const vu0_mat_34& rhs );

      // get a transposed matrix

      inline vu0_mat_43 transposed() const;

      // add, subtract, negate, absolute value

      inline vu0_mat_34 operator + ( const vu0_mat_34& rhs ) const;
      inline vu0_mat_34 operator - ( const vu0_mat_34& rhs ) const;

      inline void operator += ( const vu0_mat_34& rhs );
      inline void operator -= ( const vu0_mat_34& rhs );

      inline vu0_mat_34 operator - () const;

      inline vu0_mat_34 abs() const;

      // matrix scalar multiplies

      inline vu0_mat_34 operator * ( vu0_vec_x rhs ) const;
      inline vu0_mat_34 operator * ( vu0_vec_y rhs ) const;
      inline vu0_mat_34 operator * ( vu0_vec_z rhs ) const;
      inline vu0_mat_34 operator * ( vu0_vec_w rhs ) const;
      inline vu0_mat_34 operator * ( float rhs ) const;

      inline void operator *= ( vu0_vec_x rhs );
      inline void operator *= ( vu0_vec_y rhs );
      inline void operator *= ( vu0_vec_z rhs );
      inline void operator *= ( vu0_vec_w rhs );
      inline void operator *= ( float rhs );

      // matrix vector multiplies

      inline vu0_vec_3 operator * ( vu0_vec_4 rhs ) const;
      inline vu0_vec_3 operator * ( vu0_vector_t rhs ) const;
      inline vu0_vec_3 operator * ( vu0_point_t rhs ) const;

      // matrix matrix multiplies

      inline vu0_mat_33 operator * ( const vu0_mat_43& rhs ) const;
      inline vu0_mat_34 operator * ( const vu0_mat_44& rhs ) const;
      inline vu0_mat_34 operator * ( const vu0_transform_t& rhs ) const;

      // print

      inline void print() const;
      inline void print( const char* name ) const;
};

class vu0_mat_43
{
   public:
      vu0_vec_4 col0;
      vu0_vec_4 col1;
      vu0_vec_4 col2;

      inline vu0_mat_43() { }
      inline vu0_mat_43( const vu0_mat_43& rhs );
      inline vu0_mat_43( vu0_vec_4 col_0, vu0_vec_4 col_1, vu0_vec_4 col_2 );

      // get rows and columns

      inline vu0_vec_4 get_col0() const;
      inline vu0_vec_4 get_col1() const;
      inline vu0_vec_4 get_col2() const;

      inline vu0_vec_3 get_row0() const;
      inline vu0_vec_3 get_row1() const;
      inline vu0_vec_3 get_row2() const;
      inline vu0_vec_3 get_row3() const;

      // sets

      inline void set( const vu0_mat_43& rhs );
      inline void set( vu0_vec_4 col_0, vu0_vec_4 col_1, vu0_vec_4 col_2 );

      // set rows and columns

      inline void set_col0( vu0_vec_4 rhs );
      inline void set_col1( vu0_vec_4 rhs );
      inline void set_col2( vu0_vec_4 rhs );

      inline void set_row0( vu0_vec_3 rhs );
      inline void set_row1( vu0_vec_3 rhs );
      inline void set_row2( vu0_vec_3 rhs );
      inline void set_row3( vu0_vec_3 rhs );

      // set matrix to zeros

      inline void set_zero();

      // assignments

      inline vu0_mat_43& operator = ( const vu0_mat_33& rhs );
      inline vu0_mat_43& operator = ( const vu0_mat_43& rhs );

      // get a transposed matrix

      inline vu0_mat_34 transposed() const;

      // add, subtract, negate, absolute value

      inline vu0_mat_43 operator + ( const vu0_mat_43& rhs ) const;
      inline vu0_mat_43 operator - ( const vu0_mat_43& rhs ) const;

      inline void operator += ( const vu0_mat_43& rhs );
      inline void operator -= ( const vu0_mat_43& rhs );

      inline vu0_mat_43 operator - () const;

      inline vu0_mat_43 abs() const;

      // matrix scalar multiplies

      inline vu0_mat_43 operator * ( vu0_vec_x rhs ) const;
      inline vu0_mat_43 operator * ( vu0_vec_y rhs ) const;
      inline vu0_mat_43 operator * ( vu0_vec_z rhs ) const;
      inline vu0_mat_43 operator * ( vu0_vec_w rhs ) const;
      inline vu0_mat_43 operator * ( float rhs ) const;

      inline void operator *= ( vu0_vec_x rhs );
      inline void operator *= ( vu0_vec_y rhs );
      inline void operator *= ( vu0_vec_z rhs );
      inline void operator *= ( vu0_vec_w rhs );
      inline void operator *= ( float rhs );

      // matrix vector multiplies

      inline vu0_vec_4 operator * ( vu0_vec_3 rhs ) const;

      // matrix matrix multiplies

      inline vu0_mat_43 operator * ( const vu0_mat_33& rhs ) const;
      inline vu0_mat_44 operator * ( const vu0_mat_34& rhs ) const;

      // print

      inline void print() const;
      inline void print( const char* name ) const;
};

class vu0_mat_44
{
   public:
      vu0_vec_4 col0;
      vu0_vec_4 col1;
      vu0_vec_4 col2;
      vu0_vec_4 col3;

      inline vu0_mat_44() { }
      inline vu0_mat_44( const vu0_mat_44& rhs );
      inline vu0_mat_44( vu0_vec_4 col_0, vu0_vec_4 col_1, vu0_vec_4 col_2, vu0_vec_4 col_3 );

      explicit inline vu0_mat_44( const vu0_transform_t& rhs );
      explicit inline vu0_mat_44( const core_mat_44& rhs );

      // get rows and columns

      inline vu0_vec_4 get_col0() const;
      inline vu0_vec_4 get_col1() const;
      inline vu0_vec_4 get_col2() const;
      inline vu0_vec_4 get_col3() const;

      inline vu0_vec_4 get_row0() const;
      inline vu0_vec_4 get_row1() const;
      inline vu0_vec_4 get_row2() const;
      inline vu0_vec_4 get_row3() const;

      // sets

      inline void set( const vu0_mat_44& rhs );
      inline void set( vu0_vec_4 col_0, vu0_vec_4 col_1, vu0_vec_4 col_2, vu0_vec_4 col_3 );
      inline void set( const vu0_transform_t& rhs );

      // set rows and columns

      inline void set_col0( vu0_vec_4 rhs );
      inline void set_col1( vu0_vec_4 rhs );
      inline void set_col2( vu0_vec_4 rhs );
      inline void set_col3( vu0_vec_4 rhs );

      inline void set_row0( vu0_vec_4 rhs );
      inline void set_row1( vu0_vec_4 rhs );
      inline void set_row2( vu0_vec_4 rhs );
      inline void set_row3( vu0_vec_4 rhs );

      // set matrix to zeros

      inline void set_zero();

      // set entire matrix for a transformation

      inline void set_identity();
      inline void set_scale( vu0_vec_3 scale );
      inline void set_rotate_x( float angle );
      inline void set_rotate_y( float angle );
      inline void set_rotate_z( float angle );

      // rotate around axis by angle.  expects unit vector

      void set_rotate( float angle, vu0_vec_3 axis );
      inline void set_translate( vu0_vec_3 translation );

      // assignments

      inline vu0_mat_44& operator = ( const vu0_mat_33& rhs );
      inline vu0_mat_44& operator = ( const vu0_mat_34& rhs );
      inline vu0_mat_44& operator = ( const vu0_mat_43& rhs );
      inline vu0_mat_44& operator = ( const vu0_mat_44& rhs );
      inline vu0_mat_44& operator = ( const vu0_transform_t& rhs );

      // get a transposed matrix

      inline vu0_mat_44 transposed() const;

      // transpose this matrix

      inline void transpose();

      // add, subtract, negate, absolute value

      inline vu0_mat_44 operator + ( const vu0_mat_44& rhs ) const;
      inline vu0_mat_44 operator - ( const vu0_mat_44& rhs ) const;

      inline void operator += ( const vu0_mat_44& rhs );
      inline void operator -= ( const vu0_mat_44& rhs );

      inline vu0_mat_44 operator - () const;

      inline vu0_mat_44 abs() const;

      // matrix scalar multiplies

      inline vu0_mat_44 operator * ( vu0_vec_x rhs ) const;
      inline vu0_mat_44 operator * ( vu0_vec_y rhs ) const;
      inline vu0_mat_44 operator * ( vu0_vec_z rhs ) const;
      inline vu0_mat_44 operator * ( vu0_vec_w rhs ) const;
      inline vu0_mat_44 operator * ( float rhs ) const;

      inline void operator *= ( vu0_vec_x rhs );
      inline void operator *= ( vu0_vec_y rhs );
      inline void operator *= ( vu0_vec_z rhs );
      inline void operator *= ( vu0_vec_w rhs );
      inline void operator *= ( float rhs );

      // matrix vector multiplies

      inline vu0_vec_4 operator * ( vu0_vec_4 rhs ) const;
      inline vu0_vec_4 operator * ( vu0_vector_t rhs ) const;
      inline vu0_vec_4 operator * ( vu0_point_t rhs ) const;

      // matrix matrix multiplies

      inline vu0_mat_43 operator * ( const vu0_mat_43& rhs ) const;
      inline vu0_mat_44 operator * ( const vu0_mat_44& rhs ) const;
      inline vu0_mat_44 operator * ( const vu0_transform_t& rhs ) const;

      // print

      inline void print() const;
      inline void print( const char* name ) const;
};

class vu0_transform_t
{
   public:
      vu0_vector_t col0;
      vu0_vector_t col1;
      vu0_vector_t col2;
      vu0_point_t col3;

      inline vu0_transform_t() { }
      inline vu0_transform_t( const vu0_transform_t& rhs );
      inline vu0_transform_t( vu0_vector_t col_0, vu0_vector_t col_1, vu0_vector_t col_2, vu0_point_t col_3 );

      // construct from 3x3 matrix and translation

      inline vu0_transform_t( const vu0_mat_33& mat, vu0_vec_3 trans );

      // get rows and columns

      inline vu0_vector_t get_col0() const;
      inline vu0_vector_t get_col1() const;
      inline vu0_vector_t get_col2() const;
      inline vu0_point_t get_col3() const;

      inline vu0_vec_4 get_row0() const;
      inline vu0_vec_4 get_row1() const;
      inline vu0_vec_4 get_row2() const;
      inline vu0_vec_4 get_row3() const;

      // get mat_33 and vec_3 parts

      inline vu0_mat_33 get_mat_33() const;
      inline vu0_vec_3  get_vec_3() const;

      // sets

      inline void set( const vu0_transform_t& rhs );
      inline void set( vu0_vector_t col_0, vu0_vector_t col_1, vu0_vector_t col_2, vu0_point_t col_3 );
      inline void set( const vu0_mat_33& mat, vu0_vec_3 trans );

      // set rows and columns

      inline void set_col0( vu0_vector_t rhs );
      inline void set_col1( vu0_vector_t rhs );
      inline void set_col2( vu0_vector_t rhs );
      inline void set_col3( vu0_point_t rhs );

      inline void set_row0( vu0_vec_4 rhs );
      inline void set_row1( vu0_vec_4 rhs );
      inline void set_row2( vu0_vec_4 rhs );

      // set mat_33 and vec_3 parts

      inline void set_mat_33( const vu0_mat_33& rhs );
      inline void set_vec_3( vu0_vec_3 rhs );

      // set matrix to zeros

      inline void set_zero();

      // set entire matrix for a transformation

      inline void set_identity();
      inline void set_scale( vu0_vec_3 scale );
      inline void set_rotate_x( float angle );
      inline void set_rotate_y( float angle );
      inline void set_rotate_z( float angle );

      // rotate around axis by angle.  expects unit vector

      void set_rotate( float angle, vu0_vec_3 axis );
      inline void set_translate( vu0_vec_3 translation );

      // assignments

      inline vu0_transform_t& operator = ( const vu0_transform_t& rhs );
      inline vu0_transform_t& operator = ( const vu0_mat_33& rhs );
      inline vu0_transform_t& operator = ( const vu0_mat_34& rhs );

      // get an inverse of the matrix

      inline vu0_transform_t inverse() const;

      // invert this matrix

      inline void invert();

      // orthogonal inverse (inverse if rigid transformation)

      inline vu0_transform_t orthogonal_inverse() const;
      inline void orthogonal_invert();

      // matrix vector multiplies

      inline vu0_vec_4 operator * ( vu0_vec_4 rhs ) const;
      inline vu0_vector_t operator * ( vu0_vector_t rhs ) const;
      inline vu0_point_t operator * ( vu0_point_t rhs ) const;

      // transform vec_3 as a vector (apply 3x3 matrix, not translation)

      inline vu0_vec_3 mul_vector( vu0_vec_3 rhs ) const;

      // transform vec_3 as a point (apply 3x3 matrix and translation)

      inline vu0_vec_3 mul_point( vu0_vec_3 rhs ) const;

      // matrix matrix multiplies

      inline vu0_mat_43 operator * ( const vu0_mat_43& rhs ) const;
      inline vu0_mat_44 operator * ( const vu0_mat_44& rhs ) const;
      inline vu0_transform_t operator * ( const vu0_transform_t& rhs ) const;

      // print

      inline void print() const;
      inline void print( const char* name ) const;
};

inline
vu0_mat_33::vu0_mat_33( const vu0_mat_33& rhs )
{
   col0 = rhs.col0;
   col1 = rhs.col1;
   col2 = rhs.col2;
}

inline
vu0_mat_33::vu0_mat_33( vu0_vec_3 col_0, vu0_vec_3 col_1, vu0_vec_3 col_2 )
{
   col0 = col_0;
   col1 = col_1;
   col2 = col_2;
}

inline
vu0_mat_33::vu0_mat_33( vu0_vec_4 quat )
{
   set( quat );
}

inline
vu0_vec_3
vu0_mat_33::get_col0() const
{
   return col0;
}

inline
vu0_vec_3
vu0_mat_33::get_col1() const
{
   return col1;
}

inline
vu0_vec_3
vu0_mat_33::get_col2() const
{
   return col2;
}

inline
vu0_vec_3
vu0_mat_33::get_row0() const
{
   vu0_vec_3 row;
   row.set_x( col0.get_x() );
   row.set_y( col1.get_x() );
   row.set_z( col2.get_x() );
   return row;
}

inline
vu0_vec_3
vu0_mat_33::get_row1() const
{
   vu0_vec_3 row;
   row.set_x( col0.get_y() );
   row.set_y( col1.get_y() );
   row.set_z( col2.get_y() );
   return row;
}

inline
vu0_vec_3
vu0_mat_33::get_row2() const
{
   vu0_vec_3 row;
   row.set_x( col0.get_z() );
   row.set_y( col1.get_z() );
   row.set_z( col2.get_z() );
   return row;
}

inline
void
vu0_mat_33::set( const vu0_mat_33& rhs )
{
   col0 = rhs.col0;
   col1 = rhs.col1;
   col2 = rhs.col2;
}

inline
void
vu0_mat_33::set( vu0_vec_3 col_0, vu0_vec_3 col_1, vu0_vec_3 col_2 )
{
   col0 = col_0;
   col1 = col_1;
   col2 = col_2;
}

inline
void
vu0_mat_33::set( vu0_vec_4 quat )
{
   __v4sf ones, q_yzx, q_zxy;
   __v4sf ACC;
   _vmr32( q_yzx, vf_XY, quat.vec128 );
   _vaddbc( q_yzx, vf_Z, vu0_vf0, quat.vec128, vf_X );
   _vaddbc( q_zxy, vf_X, vu0_vf0, quat.vec128, vf_Z );
   _vaddbc( q_zxy, vf_Y, vu0_vf0, quat.vec128, vf_X );
   _vaddbc( q_zxy, vf_Z, vu0_vf0, quat.vec128, vf_Y );
   _vmul( ACC, vf_XYZW, quat.vec128, q_yzx );
   _vmsubbc( col0.vec128, vf_Z, ACC, q_zxy, quat.vec128, vf_W );
   _vmsubbc( col1.vec128, vf_X, ACC, q_zxy, quat.vec128, vf_W );
   _vmsubbc( col2.vec128, vf_Y, ACC, q_zxy, quat.vec128, vf_W );
   _vmul( ACC, vf_XYZW, quat.vec128, q_zxy );
   _vmaddbc( col0.vec128, vf_Y, ACC, q_yzx, quat.vec128, vf_W );
   _vmaddbc( col1.vec128, vf_Z, ACC, q_yzx, quat.vec128, vf_W );
   _vmaddbc( col2.vec128, vf_X, ACC, q_yzx, quat.vec128, vf_W );
   _vmul( ACC, vf_XYZW, q_zxy, q_zxy );
   _vmadd( col0.vec128, vf_X, ACC, q_yzx, q_yzx );
   _vmadd( col1.vec128, vf_Y, ACC, q_yzx, q_yzx );
   _vmadd( col2.vec128, vf_Z, ACC, q_yzx, q_yzx );
   _vmaxbc( ones, vf_XYZW, vu0_vf0, vu0_vf0, vf_W );
   _vadd( col0.vec128, vf_XYZW, col0.vec128, col0.vec128 );
   _vadd( col1.vec128, vf_XYZW, col1.vec128, col1.vec128 );
   _vadd( col2.vec128, vf_XYZW, col2.vec128, col2.vec128 );
   _vsub( col0.vec128, vf_X, ones, col0.vec128 );
   _vsub( col1.vec128, vf_Y, ones, col1.vec128 );
   _vsub( col2.vec128, vf_Z, ones, col2.vec128 );
}

inline
void
vu0_mat_33::set_col0( vu0_vec_3 rhs )
{
   col0 = rhs;
}

inline
void
vu0_mat_33::set_col1( vu0_vec_3 rhs )
{
   col1 = rhs;
}

inline
void
vu0_mat_33::set_col2( vu0_vec_3 rhs )
{
   col2 = rhs;
}

inline
void
vu0_mat_33::set_row0( vu0_vec_3 rhs )
{
   col0.set_x( rhs.get_x() );
   col1.set_x( rhs.get_y() );
   col2.set_x( rhs.get_z() );
}

inline
void
vu0_mat_33::set_row1( vu0_vec_3 rhs )
{
   col0.set_y( rhs.get_x() );
   col1.set_y( rhs.get_y() );
   col2.set_y( rhs.get_z() );
}

inline
void
vu0_mat_33::set_row2( vu0_vec_3 rhs )
{
   col0.set_z( rhs.get_x() );
   col1.set_z( rhs.get_y() );
   col2.set_z( rhs.get_z() );
}

inline
void
vu0_mat_33::set_zero()
{
   col0.set_zero();
   col1.set_zero();
   col2.set_zero();
}

inline
void
vu0_mat_33::set_identity()
{
   _vsub( col0.vec128, vf_XYZW, vu0_vf0, vu0_vf0 );
   _vsub( col1.vec128, vf_XYZW, vu0_vf0, vu0_vf0 );
   _vmr32( col2.vec128, vf_XYZW, vu0_vf0 );
   _vaddbc( col0.vec128, vf_X, vu0_vf0, vu0_vf0, vf_W );
   _vaddbc( col1.vec128, vf_Y, vu0_vf0, vu0_vf0, vf_W );
}

inline
void
vu0_mat_33::set_scale( vu0_vec_3 scale ) 
{
   col0.set_zero();
   col1.set_zero();
   col2.set_zero();
   col0 = vu0_vec_x( scale );
   col1 = vu0_vec_y( scale );
   col2 = vu0_vec_z( scale );   
}

inline
void
vu0_mat_33::set_rotate_x( float angle ) 
{
   float cs = cosf(angle);
   float sn = sinf(angle);
   col0.set( 1.0f, 0.0f, 0.0f );
   col1.set( 0.0f, cs, sn );
   col2.set( 0.0f, -sn, cs );
}

inline
void
vu0_mat_33::set_rotate_y( float angle ) 
{
   float cs = cosf(angle);
   float sn = sinf(angle);
   col0.set( cs, 0.0f, -sn );
   col1.set( 0.0f, 1.0f, 0.0f );
   col2.set( sn, 0.0f, cs );
}

inline
void
vu0_mat_33::set_rotate_z( float angle ) 
{
   float cs = cosf(angle);
   float sn = sinf(angle);
   col0.set( cs, sn, 0.0f );
   col1.set( -sn, cs, 0.0f );
   col2.set( 0.0f, 0.0f, 1.0f );
}

inline
vu0_mat_33&
vu0_mat_33::operator = ( const vu0_mat_33& rhs )
{
   col0 = rhs.col0;
   col1 = rhs.col1;
   col2 = rhs.col2;
   return *this;
}

inline
vu0_mat_33
vu0_mat_33::transposed() const
{
   __v4sf res0, res1, res2;
   __v4si tmp0, tmp1, tmp2, tmp3;
   tmp0 = _pextlw( (__v4si)col1.vec128, (__v4si)col0.vec128 );
   tmp1 = _pextuw( (__v4si)col1.vec128, (__v4si)col0.vec128 );
   tmp2 = _pextlw( 0, (__v4si)col2.vec128 );
   tmp3 = _pextuw( 0, (__v4si)col2.vec128 );
   res0 = (__v4sf)_pcpyld( tmp2, tmp0 );
   res1 = (__v4sf)_pcpyud( tmp0, tmp2 );
   res2 = (__v4sf)_pcpyld( tmp3, tmp1 );
   return vu0_mat_33( res0, res1, res2 );
}

inline
void
vu0_mat_33::transpose()
{
   *this = this->transposed();   
}

inline
vu0_mat_33
vu0_mat_33::inverse() const
{
   __v4sf inv0, inv1, inv2;
   __v4sf tmp, det;
   __v4sf ACC;
   float Q;
   _vopmula( ACC, col0.vec128, col1.vec128 );
   _vopmsub( inv2, ACC, col1.vec128, col0.vec128 );
   _vopmula( ACC, col1.vec128, col2.vec128 );
   _vopmsub( inv0, ACC, col2.vec128, col1.vec128 );
   _vmul( det, vf_XYZW, col2.vec128, inv2 );
   _vaddbc( tmp, vf_X, vu0_vf0, vu0_vf0, vf_W );
   _vopmula( ACC, col2.vec128, col0.vec128 );
   _vopmsub( inv1, ACC, col0.vec128, col2.vec128 );
   _vaddbc( ACC, vf_X, det, det, vf_Y );
   _vmaddbc( det, vf_X, ACC, tmp, det, vf_Z );
   _vaddbc( tmp, vf_Y, vu0_vf0, inv2, vf_X );
   _vadd( tmp, vf_XZ, vu0_vf0, inv1 );
   _vaddbc( inv1, vf_X, vu0_vf0, inv0, vf_Y );
   _vdiv( Q, vu0_vf0, vf_W, det, vf_X );
   _vaddbc( inv1, vf_Z, vu0_vf0, inv2, vf_Y );
   _vaddbc( inv2, vf_X, vu0_vf0, inv0, vf_Z );
   _vaddbc( inv0, vf_Z, vu0_vf0, tmp, vf_Y );
   _vaddbc( inv0, vf_Y, vu0_vf0, tmp, vf_X );
   _vaddbc( inv2, vf_Y, vu0_vf0, tmp, vf_Z );
   _vwaitq( Q );
   _vmulf( inv1, vf_XYZ, inv1, Q );
   _vmulf( inv0, vf_XYZ, inv0, Q );
   _vmulf( inv2, vf_XYZ, inv2, Q );
   return vu0_mat_33( inv0, inv1, inv2 );
}

inline
void
vu0_mat_33::invert()
{
   *this = this->inverse();
}

inline
vu0_vec_x
vu0_mat_33::determinant() const
{
   return col2.dot(col0.cross(col1));
}

inline
vu0_mat_33
vu0_mat_33::operator + ( const vu0_mat_33& rhs ) const
{
   return vu0_mat_33( col0 + rhs.col0, col1 + rhs.col1, col2 + rhs.col2 );
}

inline
vu0_mat_33
vu0_mat_33::operator - ( const vu0_mat_33& rhs ) const
{
   return vu0_mat_33( col0 - rhs.col0, col1 - rhs.col1, col2 - rhs.col2 );
}

inline
void
vu0_mat_33::operator += ( const vu0_mat_33& rhs )
{
   *this = *this + rhs;
}

inline
void
vu0_mat_33::operator -= ( const vu0_mat_33& rhs )
{
   *this = *this - rhs;
}

inline
vu0_mat_33
vu0_mat_33::operator - () const
{
   return vu0_mat_33( -col0, -col1, -col2 );
}

inline
vu0_mat_33
vu0_mat_33::abs() const
{
   return vu0_mat_33( col0.abs(), col1.abs(), col2.abs() );
}

inline
vu0_mat_33
vu0_mat_33::operator * ( vu0_vec_x rhs ) const
{
   return vu0_mat_33( col0 * rhs, col1 * rhs, col2 * rhs );
}

inline
vu0_mat_33
vu0_mat_33::operator * ( vu0_vec_y rhs ) const
{
   return vu0_mat_33( col0 * rhs, col1 * rhs, col2 * rhs );
}

inline
vu0_mat_33
vu0_mat_33::operator * ( vu0_vec_z rhs ) const
{
   return vu0_mat_33( col0 * rhs, col1 * rhs, col2 * rhs );
}

inline
vu0_mat_33
vu0_mat_33::operator * ( vu0_vec_w rhs ) const
{
   return vu0_mat_33( col0 * rhs, col1 * rhs, col2 * rhs );
}

inline
vu0_mat_33
vu0_mat_33::operator * ( float rhs ) const
{
   vu0_vec_x rhs_x( rhs );
   return vu0_mat_33( col0 * rhs_x, col1 * rhs_x, col2 * rhs_x );
}

inline
void
vu0_mat_33::operator *= ( vu0_vec_x rhs )
{
   *this = *this * rhs;
}

inline
void
vu0_mat_33::operator *= ( vu0_vec_y rhs )
{
   *this = *this * rhs;
}

inline
void
vu0_mat_33::operator *= ( vu0_vec_z rhs )
{
   *this = *this * rhs;
}

inline
void
vu0_mat_33::operator *= ( vu0_vec_w rhs )
{
   *this = *this * rhs;
}

inline
void
vu0_mat_33::operator *= ( float rhs )
{
   *this = *this * rhs;
}

inline
vu0_vec_3
vu0_mat_33::operator * ( vu0_vec_3 rhs ) const
{
   __v4sf result;
   __v4sf ACC;
   _vmulbc( ACC, vf_XYZW, col0.vec128, rhs.vec128, vf_X );
   _vmaddbc( ACC, vf_XYZW, ACC, col1.vec128, rhs.vec128, vf_Y );
   _vmaddbc( result, vf_XYZW, ACC, col2.vec128, rhs.vec128, vf_Z );
   return vu0_vec_3(result);
}

inline
vu0_mat_33
vu0_mat_33::operator * ( const vu0_mat_33& rhs ) const
{
   return vu0_mat_33( *this * rhs.col0, *this * rhs.col1, *this * rhs.col2 );
}

inline
vu0_mat_34
vu0_mat_33::operator * ( const vu0_mat_34& rhs ) const
{
   return vu0_mat_34( *this * rhs.col0, *this * rhs.col1, *this * rhs.col2, *this * rhs.col3 );
}

inline
void
vu0_mat_33::print() const
{
   get_row0().print();
   get_row1().print();
   get_row2().print();
}

inline
void
vu0_mat_33::print( const char *name ) const
{
   printf("%s:\n", name);
   print();
}

inline
vu0_mat_34::vu0_mat_34( const vu0_mat_34& rhs )
{
   col0 = rhs.col0;
   col1 = rhs.col1;
   col2 = rhs.col2;
   col3 = rhs.col3;
}

inline
vu0_mat_34::vu0_mat_34( vu0_vec_3 col_0, vu0_vec_3 col_1, vu0_vec_3 col_2, vu0_vec_3 col_3 )
{
   col0 = col_0;
   col1 = col_1;
   col2 = col_2;
   col3 = col_3;
}

inline
vu0_vec_3
vu0_mat_34::get_col0() const
{
   return col0;
}

inline
vu0_vec_3
vu0_mat_34::get_col1() const
{
   return col1;
}

inline
vu0_vec_3
vu0_mat_34::get_col2() const
{
   return col2;
}

inline
vu0_vec_3
vu0_mat_34::get_col3() const
{
   return col3;
}

inline
vu0_vec_4
vu0_mat_34::get_row0() const
{
   vu0_vec_4 row;
   row.set_x( col0.get_x() );
   row.set_y( col1.get_x() );
   row.set_z( col2.get_x() );
   row.set_w( col3.get_x() );
   return row;
}

inline
vu0_vec_4
vu0_mat_34::get_row1() const
{
   vu0_vec_4 row;
   row.set_x( col0.get_y() );
   row.set_y( col1.get_y() );
   row.set_z( col2.get_y() );
   row.set_w( col3.get_y() );
   return row;
}

inline
vu0_vec_4
vu0_mat_34::get_row2() const
{
   vu0_vec_4 row;
   row.set_x( col0.get_z() );
   row.set_y( col1.get_z() );
   row.set_z( col2.get_z() );
   row.set_w( col3.get_z() );
   return row;
}

inline
void
vu0_mat_34::set( const vu0_mat_34& rhs )
{
   col0 = rhs.col0;
   col1 = rhs.col1;
   col2 = rhs.col2;
   col3 = rhs.col3;
}

inline
void
vu0_mat_34::set( vu0_vec_3 col_0, vu0_vec_3 col_1, vu0_vec_3 col_2, vu0_vec_3 col_3 )
{
   col0 = col_0;
   col1 = col_1;
   col2 = col_2;
   col3 = col_3;
}

inline
void
vu0_mat_34::set_col0( vu0_vec_3 rhs )
{
   col0 = rhs;
}

inline
void
vu0_mat_34::set_col1( vu0_vec_3 rhs )
{
   col1 = rhs;
}

inline
void
vu0_mat_34::set_col2( vu0_vec_3 rhs )
{
   col2 = rhs;
}

inline
void
vu0_mat_34::set_col3( vu0_vec_3 rhs )
{
   col3 = rhs;
}

inline
void
vu0_mat_34::set_row0( vu0_vec_4 rhs )
{
   col0.set_x( rhs.get_x() );
   col1.set_x( rhs.get_y() );
   col2.set_x( rhs.get_z() );
   col3.set_x( rhs.get_w() );
}

inline
void
vu0_mat_34::set_row1( vu0_vec_4 rhs )
{
   col0.set_y( rhs.get_x() );
   col1.set_y( rhs.get_y() );
   col2.set_y( rhs.get_z() );
   col3.set_y( rhs.get_w() );
}

inline
void
vu0_mat_34::set_row2( vu0_vec_4 rhs )
{
   col0.set_z( rhs.get_x() );
   col1.set_z( rhs.get_y() );
   col2.set_z( rhs.get_z() );
   col3.set_z( rhs.get_w() );
}

inline
void
vu0_mat_34::set_zero()
{
   col0.set_zero();
   col1.set_zero();
   col2.set_zero();
   col3.set_zero();
}

inline
vu0_mat_34&
vu0_mat_34::operator = ( const vu0_mat_33& rhs )
{
   col0 = rhs.col0;
   col1 = rhs.col1;
   col2 = rhs.col2;
   return *this;
}

inline
vu0_mat_34&
vu0_mat_34::operator = ( const vu0_mat_34& rhs )
{
   col0 = rhs.col0;
   col1 = rhs.col1;
   col2 = rhs.col2;
   col3 = rhs.col3;
   return *this;
}

inline
vu0_mat_43
vu0_mat_34::transposed() const
{
   __v4sf res0, res1, res2;
   __v4si tmp0, tmp1, tmp2, tmp3;
   tmp0 = _pextlw( (__v4si)col1.vec128, (__v4si)col0.vec128 );
   tmp1 = _pextuw( (__v4si)col1.vec128, (__v4si)col0.vec128 );
   tmp2 = _pextlw( (__v4si)col3.vec128, (__v4si)col2.vec128 );
   tmp3 = _pextuw( (__v4si)col3.vec128, (__v4si)col2.vec128 );
   res0 = (__v4sf)_pcpyld( tmp2, tmp0 );
   res1 = (__v4sf)_pcpyud( tmp0, tmp2 );
   res2 = (__v4sf)_pcpyld( tmp3, tmp1 );
   return vu0_mat_43( res0, res1, res2 );
}

inline
vu0_mat_34
vu0_mat_34::operator + ( const vu0_mat_34& rhs ) const
{
   return vu0_mat_34( col0 + rhs.col0, col1 + rhs.col1, col2 + rhs.col2, col3 + rhs.col3 );
}

inline
vu0_mat_34
vu0_mat_34::operator - ( const vu0_mat_34& rhs ) const
{
   return vu0_mat_34( col0 - rhs.col0, col1 - rhs.col1, col2 - rhs.col2, col3 - rhs.col3 );
}

inline
void
vu0_mat_34::operator += ( const vu0_mat_34& rhs )
{
   *this = *this + rhs;
}

inline
void
vu0_mat_34::operator -= ( const vu0_mat_34& rhs )
{
   *this = *this - rhs;
}

inline
vu0_mat_34
vu0_mat_34::operator - () const
{
   return vu0_mat_34( -col0, -col1, -col2, -col3 );
}

inline
vu0_mat_34
vu0_mat_34::abs() const
{
   return vu0_mat_34( col0.abs(), col1.abs(), col2.abs(), col3.abs() );
}

inline
vu0_mat_34
vu0_mat_34::operator * ( vu0_vec_x rhs ) const
{
   return vu0_mat_34( col0 * rhs, col1 * rhs, col2 * rhs, col3 * rhs );
}

inline
vu0_mat_34
vu0_mat_34::operator * ( vu0_vec_y rhs ) const
{
   return vu0_mat_34( col0 * rhs, col1 * rhs, col2 * rhs, col3 * rhs );
}

inline
vu0_mat_34
vu0_mat_34::operator * ( vu0_vec_z rhs ) const
{
   return vu0_mat_34( col0 * rhs, col1 * rhs, col2 * rhs, col3 * rhs );
}

inline
vu0_mat_34
vu0_mat_34::operator * ( vu0_vec_w rhs ) const
{
   return vu0_mat_34( col0 * rhs, col1 * rhs, col2 * rhs, col3 * rhs );
}

inline
vu0_mat_34
vu0_mat_34::operator * ( float rhs ) const
{
   vu0_vec_x rhs_x( rhs );
   return vu0_mat_34( col0 * rhs_x, col1 * rhs_x, col2 * rhs_x, col3 * rhs_x );
}

inline
void
vu0_mat_34::operator *= ( vu0_vec_x rhs )
{
   *this = *this * rhs;
}

inline
void
vu0_mat_34::operator *= ( vu0_vec_y rhs )
{
   *this = *this * rhs;
}

inline
void
vu0_mat_34::operator *= ( vu0_vec_z rhs )
{
   *this = *this * rhs;
}

inline
void
vu0_mat_34::operator *= ( vu0_vec_w rhs )
{
   *this = *this * rhs;
}

inline
void
vu0_mat_34::operator *= ( float rhs )
{
   *this = *this * rhs;
}

inline
vu0_vec_3
vu0_mat_34::operator * ( vu0_vec_4 rhs ) const
{
   __v4sf result;
   __v4sf ACC;
   _vmulbc( ACC, vf_XYZW, col0.vec128, rhs.vec128, vf_X );
   _vmaddbc( ACC, vf_XYZW, ACC, col1.vec128, rhs.vec128, vf_Y );
   _vmaddbc( ACC, vf_XYZW, ACC, col2.vec128, rhs.vec128, vf_Z );
   _vmaddbc( result, vf_XYZW, ACC, col3.vec128, rhs.vec128, vf_W );
   return vu0_vec_3(result);
}

inline
vu0_vec_3
vu0_mat_34::operator * ( vu0_vector_t rhs ) const
{
   __v4sf result;
   __v4sf ACC;
   _vmulbc( ACC, vf_XYZW, col0.vec128, rhs.vec128, vf_X );
   _vmaddbc( ACC, vf_XYZW, ACC, col1.vec128, rhs.vec128, vf_Y );
   _vmaddbc( result, vf_XYZW, ACC, col2.vec128, rhs.vec128, vf_Z );
   return vu0_vec_3(result);
}

inline
vu0_vec_3
vu0_mat_34::operator * ( vu0_point_t rhs ) const
{
   __v4sf result;
   __v4sf ACC;
   _vmulbc( ACC, vf_XYZW, col0.vec128, rhs.vec128, vf_X );
   _vmaddbc( ACC, vf_XYZW, ACC, col1.vec128, rhs.vec128, vf_Y );
   _vmaddbc( ACC, vf_XYZW, ACC, col2.vec128, rhs.vec128, vf_Z );
   _vmaddbc( result, vf_XYZW, ACC, col3.vec128, vu0_vf0, vf_W );
   return vu0_vec_3(result);
}

inline
vu0_mat_33
vu0_mat_34::operator * ( const vu0_mat_43& rhs ) const
{
   return vu0_mat_33( *this * rhs.col0, *this * rhs.col1, *this * rhs.col2 );
}

inline
vu0_mat_34
vu0_mat_34::operator * ( const vu0_mat_44& rhs ) const
{
   return vu0_mat_34( *this * rhs.col0, *this * rhs.col1, *this * rhs.col2, *this * rhs.col3 );
}

inline
vu0_mat_34
vu0_mat_34::operator * ( const vu0_transform_t& rhs ) const
{
   return vu0_mat_34( *this * rhs.col0, *this * rhs.col1, *this * rhs.col2, *this * rhs.col3 );
}

inline
void
vu0_mat_34::print() const
{
   get_row0().print();
   get_row1().print();
   get_row2().print();
}

inline
void
vu0_mat_34::print( const char *name ) const
{
   printf("%s:\n", name);
   print();
}

inline
vu0_mat_43::vu0_mat_43( const vu0_mat_43& rhs )
{
   col0 = rhs.col0;
   col1 = rhs.col1;
   col2 = rhs.col2;
}

inline
vu0_mat_43::vu0_mat_43( vu0_vec_4 col_0, vu0_vec_4 col_1, vu0_vec_4 col_2 )
{
   col0 = col_0;
   col1 = col_1;
   col2 = col_2;
}

inline
vu0_vec_4
vu0_mat_43::get_col0() const
{
   return col0;
}

inline
vu0_vec_4
vu0_mat_43::get_col1() const
{
   return col1;
}

inline
vu0_vec_4
vu0_mat_43::get_col2() const
{
   return col2;
}

inline
vu0_vec_3
vu0_mat_43::get_row0() const
{
   vu0_vec_3 row;
   row.set_x( col0.get_x() );
   row.set_y( col1.get_x() );
   row.set_z( col2.get_x() );
   return row;
}

inline
vu0_vec_3
vu0_mat_43::get_row1() const
{
   vu0_vec_3 row;
   row.set_x( col0.get_y() );
   row.set_y( col1.get_y() );
   row.set_z( col2.get_y() );
   return row;
}

inline
vu0_vec_3
vu0_mat_43::get_row2() const
{
   vu0_vec_3 row;
   row.set_x( col0.get_z() );
   row.set_y( col1.get_z() );
   row.set_z( col2.get_z() );
   return row;
}

inline
vu0_vec_3
vu0_mat_43::get_row3() const
{
   vu0_vec_3 row;
   row.set_x( col0.get_w() );
   row.set_y( col1.get_w() );
   row.set_z( col2.get_w() );
   return row;
}

inline
void
vu0_mat_43::set( const vu0_mat_43& rhs )
{
   col0 = rhs.col0;
   col1 = rhs.col1;
   col2 = rhs.col2;
}

inline
void
vu0_mat_43::set( vu0_vec_4 col_0, vu0_vec_4 col_1, vu0_vec_4 col_2 )
{
   col0 = col_0;
   col1 = col_1;
   col2 = col_2;
}

inline
void
vu0_mat_43::set_col0( vu0_vec_4 rhs )
{
   col0 = rhs;
}

inline
void
vu0_mat_43::set_col1( vu0_vec_4 rhs )
{
   col1 = rhs;
}

inline
void
vu0_mat_43::set_col2( vu0_vec_4 rhs )
{
   col2 = rhs;
}

inline
void
vu0_mat_43::set_row0( vu0_vec_3 rhs )
{
   col0.set_x( rhs.get_x() );
   col1.set_x( rhs.get_y() );
   col2.set_x( rhs.get_z() );
}

inline
void
vu0_mat_43::set_row1( vu0_vec_3 rhs )
{
   col0.set_y( rhs.get_x() );
   col1.set_y( rhs.get_y() );
   col2.set_y( rhs.get_z() );
}

inline
void
vu0_mat_43::set_row2( vu0_vec_3 rhs )
{
   col0.set_z( rhs.get_x() );
   col1.set_z( rhs.get_y() );
   col2.set_z( rhs.get_z() );
}

inline
void
vu0_mat_43::set_row3( vu0_vec_3 rhs )
{
   col0.set_w( rhs.get_x() );
   col1.set_w( rhs.get_y() );
   col2.set_w( rhs.get_z() );
}

inline
void
vu0_mat_43::set_zero()
{
   col0.set_zero();
   col1.set_zero();
   col2.set_zero();
}

inline
vu0_mat_43&
vu0_mat_43::operator = ( const vu0_mat_33& rhs )
{
   col0 = rhs.col0;
   col1 = rhs.col1;
   col2 = rhs.col2;
   return *this;
}

inline
vu0_mat_43&
vu0_mat_43::operator = ( const vu0_mat_43& rhs )
{
   col0 = rhs.col0;
   col1 = rhs.col1;
   col2 = rhs.col2;
   return *this;
}

inline
vu0_mat_34
vu0_mat_43::transposed() const
{
   __v4sf res0, res1, res2, res3;
   __v4si tmp0, tmp1, tmp2, tmp3;
   tmp0 = _pextlw( (__v4si)col1.vec128, (__v4si)col0.vec128 );
   tmp1 = _pextuw( (__v4si)col1.vec128, (__v4si)col0.vec128 );
   tmp2 = _pextlw( 0, (__v4si)col2.vec128 );
   tmp3 = _pextuw( 0, (__v4si)col2.vec128 );
   res0 = (__v4sf)_pcpyld( tmp2, tmp0 );
   res1 = (__v4sf)_pcpyud( tmp0, tmp2 );
   res2 = (__v4sf)_pcpyld( tmp3, tmp1 );
   res3 = (__v4sf)_pcpyud( tmp1, tmp3 );
   return vu0_mat_34( res0, res1, res2, res3 );
}

inline
vu0_mat_43
vu0_mat_43::operator + ( const vu0_mat_43& rhs ) const
{
   return vu0_mat_43( col0 + rhs.col0, col1 + rhs.col1, col2 + rhs.col2 );
}

inline
vu0_mat_43
vu0_mat_43::operator - ( const vu0_mat_43& rhs ) const
{
   return vu0_mat_43( col0 - rhs.col0, col1 - rhs.col1, col2 - rhs.col2 );
}

inline
void
vu0_mat_43::operator += ( const vu0_mat_43& rhs )
{
   *this = *this + rhs;
}

inline
void
vu0_mat_43::operator -= ( const vu0_mat_43& rhs )
{
   *this = *this - rhs;
}

inline
vu0_mat_43
vu0_mat_43::operator - () const
{
   return vu0_mat_43( -col0, -col1, -col2 );
}

inline
vu0_mat_43
vu0_mat_43::abs() const
{
   return vu0_mat_43( col0.abs(), col1.abs(), col2.abs() );
}

inline
vu0_mat_43
vu0_mat_43::operator * ( vu0_vec_x rhs ) const
{
   return vu0_mat_43( col0 * rhs, col1 * rhs, col2 * rhs );
}

inline
vu0_mat_43
vu0_mat_43::operator * ( vu0_vec_y rhs ) const
{
   return vu0_mat_43( col0 * rhs, col1 * rhs, col2 * rhs );
}

inline
vu0_mat_43
vu0_mat_43::operator * ( vu0_vec_z rhs ) const
{
   return vu0_mat_43( col0 * rhs, col1 * rhs, col2 * rhs );
}

inline
vu0_mat_43
vu0_mat_43::operator * ( vu0_vec_w rhs ) const
{
   return vu0_mat_43( col0 * rhs, col1 * rhs, col2 * rhs );
}

inline
vu0_mat_43
vu0_mat_43::operator * ( float rhs ) const
{
   vu0_vec_x rhs_x( rhs );
   return vu0_mat_43( col0 * rhs_x, col1 * rhs_x, col2 * rhs_x );
}

inline
void
vu0_mat_43::operator *= ( vu0_vec_x rhs )
{
   *this = *this * rhs;
}

inline
void
vu0_mat_43::operator *= ( vu0_vec_y rhs )
{
   *this = *this * rhs;
}

inline
void
vu0_mat_43::operator *= ( vu0_vec_z rhs )
{
   *this = *this * rhs;
}

inline
void
vu0_mat_43::operator *= ( vu0_vec_w rhs )
{
   *this = *this * rhs;
}

inline
void
vu0_mat_43::operator *= ( float rhs )
{
   *this = *this * rhs;
}

inline
vu0_vec_4
vu0_mat_43::operator * ( vu0_vec_3 rhs ) const
{
   __v4sf result;
   __v4sf ACC;
   _vmulbc( ACC, vf_XYZW, col0.vec128, rhs.vec128, vf_X );
   _vmaddbc( ACC, vf_XYZW, ACC, col1.vec128, rhs.vec128, vf_Y );
   _vmaddbc( result, vf_XYZW, ACC, col2.vec128, rhs.vec128, vf_Z );
   return vu0_vec_4(result);
}

inline
vu0_mat_43
vu0_mat_43::operator * ( const vu0_mat_33& rhs ) const
{
   return vu0_mat_43( *this * rhs.col0, *this * rhs.col1, *this * rhs.col2 );
}

inline
vu0_mat_44
vu0_mat_43::operator * ( const vu0_mat_34& rhs ) const
{
   return vu0_mat_44( *this * rhs.col0, *this * rhs.col1, *this * rhs.col2, *this * rhs.col3 );
}

inline
void
vu0_mat_43::print() const
{
   get_row0().print();
   get_row1().print();
   get_row2().print();
   get_row3().print();
}

inline
void
vu0_mat_43::print( const char *name ) const
{
   printf("%s:\n", name);
   print();
}

inline
vu0_mat_44::vu0_mat_44( const vu0_mat_44& rhs )
{
   col0 = rhs.col0;
   col1 = rhs.col1;
   col2 = rhs.col2;
   col3 = rhs.col3;
}

inline
vu0_mat_44::vu0_mat_44( vu0_vec_4 col_0, vu0_vec_4 col_1, vu0_vec_4 col_2, vu0_vec_4 col_3 )
{
   col0 = col_0;
   col1 = col_1;
   col2 = col_2;
   col3 = col_3;
}

inline
vu0_mat_44::vu0_mat_44( const vu0_transform_t& rhs )
{
   set(rhs);
}

inline
vu0_vec_4
vu0_mat_44::get_col0() const
{
   return col0;
}

inline
vu0_vec_4
vu0_mat_44::get_col1() const
{
   return col1;
}

inline
vu0_vec_4
vu0_mat_44::get_col2() const
{
   return col2;
}

inline
vu0_vec_4
vu0_mat_44::get_col3() const
{
   return col3;
}

inline
vu0_vec_4
vu0_mat_44::get_row0() const
{
   vu0_vec_4 row;
   row.set_x( col0.get_x() );
   row.set_y( col1.get_x() );
   row.set_z( col2.get_x() );
   row.set_w( col3.get_x() );
   return row;
}

inline
vu0_vec_4
vu0_mat_44::get_row1() const
{
   vu0_vec_4 row;
   row.set_x( col0.get_y() );
   row.set_y( col1.get_y() );
   row.set_z( col2.get_y() );
   row.set_w( col3.get_y() );
   return row;
}

inline
vu0_vec_4
vu0_mat_44::get_row2() const
{
   vu0_vec_4 row;
   row.set_x( col0.get_z() );
   row.set_y( col1.get_z() );
   row.set_z( col2.get_z() );
   row.set_w( col3.get_z() );
   return row;
}

inline
vu0_vec_4
vu0_mat_44::get_row3() const
{
   vu0_vec_4 row;
   row.set_x( col0.get_w() );
   row.set_y( col1.get_w() );
   row.set_z( col2.get_w() );
   row.set_w( col3.get_w() );
   return row;
}

inline
void
vu0_mat_44::set( const vu0_mat_44& rhs )
{
   col0 = rhs.col0;
   col1 = rhs.col1;
   col2 = rhs.col2;
   col3 = rhs.col3;
}

inline
void
vu0_mat_44::set( vu0_vec_4 col_0, vu0_vec_4 col_1, vu0_vec_4 col_2, vu0_vec_4 col_3 )
{
   col0 = col_0;
   col1 = col_1;
   col2 = col_2;
   col3 = col_3;
}

inline
void
vu0_mat_44::set( const vu0_transform_t& rhs )
{
   col0 = vu0_vec_4( rhs.col0 );
   col1 = vu0_vec_4( rhs.col1 );
   col2 = vu0_vec_4( rhs.col2 );
   col3 = vu0_vec_4( rhs.col3 );
}

inline
void
vu0_mat_44::set_col0( vu0_vec_4 rhs )
{
   col0 = rhs;
}

inline
void
vu0_mat_44::set_col1( vu0_vec_4 rhs )
{
   col1 = rhs;
}

inline
void
vu0_mat_44::set_col2( vu0_vec_4 rhs )
{
   col2 = rhs;
}

inline
void
vu0_mat_44::set_col3( vu0_vec_4 rhs )
{
   col3 = rhs;
}

inline
void
vu0_mat_44::set_row0( vu0_vec_4 rhs )
{
   col0.set_x( rhs.get_x() );
   col1.set_x( rhs.get_y() );
   col2.set_x( rhs.get_z() );
   col3.set_x( rhs.get_w() );
}

inline
void
vu0_mat_44::set_row1( vu0_vec_4 rhs )
{
   col0.set_y( rhs.get_x() );
   col1.set_y( rhs.get_y() );
   col2.set_y( rhs.get_z() );
   col3.set_y( rhs.get_w() );
}

inline
void
vu0_mat_44::set_row2( vu0_vec_4 rhs )
{
   col0.set_z( rhs.get_x() );
   col1.set_z( rhs.get_y() );
   col2.set_z( rhs.get_z() );
   col3.set_z( rhs.get_w() );
}

inline
void
vu0_mat_44::set_row3( vu0_vec_4 rhs )
{
   col0.set_w( rhs.get_x() );
   col1.set_w( rhs.get_y() );
   col2.set_w( rhs.get_z() );
   col3.set_w( rhs.get_w() );
}

inline
void
vu0_mat_44::set_zero()
{
   col0.set_zero();
   col1.set_zero();
   col2.set_zero();
   col3.set_zero();
}

inline
void
vu0_mat_44::set_identity()
{
   _vsub( col0.vec128, vf_XYZW, vu0_vf0, vu0_vf0 );
   _vsub( col1.vec128, vf_XYZW, vu0_vf0, vu0_vf0 );
   _vmr32( col2.vec128, vf_XYZW, vu0_vf0 );
   _vmove( col3.vec128, vf_XYZW, vu0_vf0 );
   _vaddbc( col0.vec128, vf_X, vu0_vf0, vu0_vf0, vf_W );
   _vaddbc( col1.vec128, vf_Y, vu0_vf0, vu0_vf0, vf_W );
}

inline
void
vu0_mat_44::set_scale( vu0_vec_3 scale ) 
{
   col0.set_zero();
   col1.set_zero();
   col2.set_zero();
   col0 = vu0_vec_x( scale );
   col1 = vu0_vec_y( scale );
   col2 = vu0_vec_z( scale );   
   col3.set_vf0();
}

inline
void
vu0_mat_44::set_rotate_x( float angle ) 
{
   float cs = cosf(angle);
   float sn = sinf(angle);
   col0.set( 1.0f, 0.0f, 0.0f, 0.0f );
   col1.set( 0.0f, cs, sn, 0.0f );
   col2.set( 0.0f, -sn, cs, 0.0f );
   col3.set_vf0();
}

inline
void
vu0_mat_44::set_rotate_y( float angle ) 
{
   float cs = cosf(angle);
   float sn = sinf(angle);
   col0.set( cs, 0.0f, -sn, 0.0f );
   col1.set( 0.0f, 1.0f, 0.0f, 0.0f );
   col2.set( sn, 0.0f, cs, 0.0f );
   col3.set_vf0();
}

inline
void
vu0_mat_44::set_rotate_z( float angle ) 
{
   float cs = cosf(angle);
   float sn = sinf(angle);
   col0.set( cs, sn, 0.0f, 0.0f );
   col1.set( -sn, cs, 0.0f, 0.0f );
   col2.set( 0.0f, 0.0f, 1.0f, 0.0f );
   col3.set_vf0();
}

inline
void
vu0_mat_44::set_translate( vu0_vec_3 translation ) 
{
   set_identity();
   col3 = translation;
}

inline
vu0_mat_44&
vu0_mat_44::operator = ( const vu0_mat_33& rhs )
{
   col0 = rhs.col0;
   col1 = rhs.col1;
   col2 = rhs.col2;
   return *this;
}

inline
vu0_mat_44&
vu0_mat_44::operator = ( const vu0_mat_34& rhs )
{
   col0 = rhs.col0;
   col1 = rhs.col1;
   col2 = rhs.col2;
   col3 = rhs.col3;
   return *this;
}

inline
vu0_mat_44&
vu0_mat_44::operator = ( const vu0_mat_43& rhs )
{
   col0 = rhs.col0;
   col1 = rhs.col1;
   col2 = rhs.col2;
   return *this;
}

inline
vu0_mat_44&
vu0_mat_44::operator = ( const vu0_mat_44& rhs )
{
   col0 = rhs.col0;
   col1 = rhs.col1;
   col2 = rhs.col2;
   col3 = rhs.col3;
   return *this;
}

inline
vu0_mat_44&
vu0_mat_44::operator = ( const vu0_transform_t& rhs )
{
   col0 = rhs.col0;
   col1 = rhs.col1;
   col2 = rhs.col2;
   col3 = rhs.col3;
   return *this;
}

inline
vu0_mat_44
vu0_mat_44::transposed() const
{
   __v4sf res0, res1, res2, res3;
   __v4si tmp0, tmp1, tmp2, tmp3;
   tmp0 = _pextlw( (__v4si)col1.vec128, (__v4si)col0.vec128 );
   tmp1 = _pextuw( (__v4si)col1.vec128, (__v4si)col0.vec128 );
   tmp2 = _pextlw( (__v4si)col3.vec128, (__v4si)col2.vec128 );
   tmp3 = _pextuw( (__v4si)col3.vec128, (__v4si)col2.vec128 );
   res0 = (__v4sf)_pcpyld( tmp2, tmp0 );
   res1 = (__v4sf)_pcpyud( tmp0, tmp2 );
   res2 = (__v4sf)_pcpyld( tmp3, tmp1 );
   res3 = (__v4sf)_pcpyud( tmp1, tmp3 );
   return vu0_mat_44( res0, res1, res2, res3 );
}

inline
void
vu0_mat_44::transpose()
{
   *this = this->transposed();   
}

inline
vu0_mat_44
vu0_mat_44::operator + ( const vu0_mat_44& rhs ) const
{
   return vu0_mat_44( col0 + rhs.col0, col1 + rhs.col1, col2 + rhs.col2, col3 + rhs.col3 );
}

inline
vu0_mat_44
vu0_mat_44::operator - ( const vu0_mat_44& rhs ) const
{
   return vu0_mat_44( col0 - rhs.col0, col1 - rhs.col1, col2 - rhs.col2, col3 - rhs.col3 );
}

inline
void
vu0_mat_44::operator += ( const vu0_mat_44& rhs )
{
   *this = *this + rhs;
}

inline
void
vu0_mat_44::operator -= ( const vu0_mat_44& rhs )
{
   *this = *this - rhs;
}

inline
vu0_mat_44
vu0_mat_44::operator - () const
{
   return vu0_mat_44( -col0, -col1, -col2, -col3 );
}

inline
vu0_mat_44
vu0_mat_44::abs() const
{
   return vu0_mat_44( col0.abs(), col1.abs(), col2.abs(), col3.abs() );
}

inline
vu0_mat_44
vu0_mat_44::operator * ( vu0_vec_x rhs ) const
{
   return vu0_mat_44( col0 * rhs, col1 * rhs, col2 * rhs, col3 * rhs );
}

inline
vu0_mat_44
vu0_mat_44::operator * ( vu0_vec_y rhs ) const
{
   return vu0_mat_44( col0 * rhs, col1 * rhs, col2 * rhs, col3 * rhs );
}

inline
vu0_mat_44
vu0_mat_44::operator * ( vu0_vec_z rhs ) const
{
   return vu0_mat_44( col0 * rhs, col1 * rhs, col2 * rhs, col3 * rhs );
}

inline
vu0_mat_44
vu0_mat_44::operator * ( vu0_vec_w rhs ) const
{
   return vu0_mat_44( col0 * rhs, col1 * rhs, col2 * rhs, col3 * rhs );
}

inline
vu0_mat_44
vu0_mat_44::operator * ( float rhs ) const
{
   vu0_vec_x rhs_x( rhs );
   return vu0_mat_44( col0 * rhs_x, col1 * rhs_x, col2 * rhs_x, col3 * rhs_x );
}

inline
void
vu0_mat_44::operator *= ( vu0_vec_x rhs )
{
   *this = *this * rhs;
}

inline
void
vu0_mat_44::operator *= ( vu0_vec_y rhs )
{
   *this = *this * rhs;
}

inline
void
vu0_mat_44::operator *= ( vu0_vec_z rhs )
{
   *this = *this * rhs;
}

inline
void
vu0_mat_44::operator *= ( vu0_vec_w rhs )
{
   *this = *this * rhs;
}

inline
void
vu0_mat_44::operator *= ( float rhs )
{
   *this = *this * rhs;
}

inline
vu0_vec_4
vu0_mat_44::operator * ( vu0_vec_4 rhs ) const
{
   __v4sf result;
   __v4sf ACC;
   _vmulbc( ACC, vf_XYZW, col0.vec128, rhs.vec128, vf_X );
   _vmaddbc( ACC, vf_XYZW, ACC, col1.vec128, rhs.vec128, vf_Y );
   _vmaddbc( ACC, vf_XYZW, ACC, col2.vec128, rhs.vec128, vf_Z );
   _vmaddbc( result, vf_XYZW, ACC, col3.vec128, rhs.vec128, vf_W );
   return vu0_vec_4(result);
}

inline
vu0_vec_4
vu0_mat_44::operator * ( vu0_vector_t rhs ) const
{
   __v4sf result;
   __v4sf ACC;
   _vmulbc( ACC, vf_XYZW, col0.vec128, rhs.vec128, vf_X );
   _vmaddbc( ACC, vf_XYZW, ACC, col1.vec128, rhs.vec128, vf_Y );
   _vmaddbc( result, vf_XYZW, ACC, col2.vec128, rhs.vec128, vf_Z );
   return vu0_vec_4(result);
}

inline
vu0_vec_4
vu0_mat_44::operator * ( vu0_point_t rhs ) const
{
   __v4sf result;
   __v4sf ACC;
   _vmulbc( ACC, vf_XYZW, col0.vec128, rhs.vec128, vf_X );
   _vmaddbc( ACC, vf_XYZW, ACC, col1.vec128, rhs.vec128, vf_Y );
   _vmaddbc( ACC, vf_XYZW, ACC, col2.vec128, rhs.vec128, vf_Z );
   _vmaddbc( result, vf_XYZW, ACC, col3.vec128, vu0_vf0, vf_W );
   return vu0_vec_4(result);
}

inline
vu0_mat_43
vu0_mat_44::operator * ( const vu0_mat_43& rhs ) const
{
   return vu0_mat_43( *this * rhs.col0, *this * rhs.col1, *this * rhs.col2 );
}

inline
vu0_mat_44
vu0_mat_44::operator * ( const vu0_mat_44& rhs ) const
{
   return vu0_mat_44( *this * rhs.col0, *this * rhs.col1, *this * rhs.col2, *this * rhs.col3 );
}

inline
vu0_mat_44
vu0_mat_44::operator * ( const vu0_transform_t& rhs ) const
{
   return vu0_mat_44( *this * rhs.col0, *this * rhs.col1, *this * rhs.col2, *this * rhs.col3 );
}

inline
void
vu0_mat_44::print() const
{
   get_row0().print();
   get_row1().print();
   get_row2().print();
   get_row3().print();
}

inline
void
vu0_mat_44::print( const char *name ) const
{
   printf("%s:\n", name);
   print();
}

inline
vu0_transform_t::vu0_transform_t( const vu0_transform_t& rhs )
{
   col0 = rhs.col0;
   col1 = rhs.col1;
   col2 = rhs.col2;
   col3 = rhs.col3;
}

inline
vu0_transform_t::vu0_transform_t( vu0_vector_t col_0, vu0_vector_t col_1, vu0_vector_t col_2, vu0_point_t col_3 )
{
   col0 = col_0;
   col1 = col_1;
   col2 = col_2;
   col3 = col_3;
}

inline
vu0_transform_t::vu0_transform_t( const vu0_mat_33& mat, vu0_vec_3 trans )
{
   set( mat, trans );
}

inline
vu0_vector_t
vu0_transform_t::get_col0() const
{
   return col0;
}

inline
vu0_vector_t
vu0_transform_t::get_col1() const
{
   return col1;
}

inline
vu0_vector_t
vu0_transform_t::get_col2() const
{
   return col2;
}

inline
vu0_point_t
vu0_transform_t::get_col3() const
{
   return col3;
}

inline
vu0_vec_4
vu0_transform_t::get_row0() const
{
   vu0_vec_4 row;
   row.set_x( col0.get_x() );
   row.set_y( col1.get_x() );
   row.set_z( col2.get_x() );
   row.set_w( col3.get_x() );
   return row;
}

inline
vu0_vec_4
vu0_transform_t::get_row1() const
{
   vu0_vec_4 row;
   row.set_x( col0.get_y() );
   row.set_y( col1.get_y() );
   row.set_z( col2.get_y() );
   row.set_w( col3.get_y() );
   return row;
}

inline
vu0_vec_4
vu0_transform_t::get_row2() const
{
   vu0_vec_4 row;
   row.set_x( col0.get_z() );
   row.set_y( col1.get_z() );
   row.set_z( col2.get_z() );
   row.set_w( col3.get_z() );
   return row;
}

inline
vu0_vec_4
vu0_transform_t::get_row3() const
{
   vu0_vec_4 row;
   row.set_vf0();
   return row;
}

inline
vu0_mat_33
vu0_transform_t::get_mat_33() const
{
   return vu0_mat_33( vu0_vec_3(col0), vu0_vec_3(col1), vu0_vec_3(col2) );
}

inline
vu0_vec_3
vu0_transform_t::get_vec_3() const
{
   return vu0_vec_3(col3);
}

inline
void
vu0_transform_t::set( const vu0_transform_t& rhs )
{
   col0 = rhs.col0;
   col1 = rhs.col1;
   col2 = rhs.col2;
   col3 = rhs.col3;
}

inline
void
vu0_transform_t::set( vu0_vector_t col_0, vu0_vector_t col_1, vu0_vector_t col_2, vu0_point_t col_3 )
{
   col0 = col_0;
   col1 = col_1;
   col2 = col_2;
   col3 = col_3;
}

inline
void
vu0_transform_t::set( const vu0_mat_33& mat, vu0_vec_3 trans )
{
   col0 = vu0_vector_t( mat.col0 );
   col1 = vu0_vector_t( mat.col1 );
   col2 = vu0_vector_t( mat.col2 );
   col3 = vu0_point_t( trans );
}

inline
void
vu0_transform_t::set_col0( vu0_vector_t rhs )
{
   col0 = rhs;
}

inline
void
vu0_transform_t::set_col1( vu0_vector_t rhs )
{
   col1 = rhs;
}

inline
void
vu0_transform_t::set_col2( vu0_vector_t rhs )
{
   col2 = rhs;
}

inline
void
vu0_transform_t::set_col3( vu0_point_t rhs )
{
   col3 = rhs;
}

inline
void
vu0_transform_t::set_row0( vu0_vec_4 rhs )
{
   col0.set_x( rhs.get_x() );
   col1.set_x( rhs.get_y() );
   col2.set_x( rhs.get_z() );
   col3.set_x( rhs.get_w() );
}

inline
void
vu0_transform_t::set_row1( vu0_vec_4 rhs )
{
   col0.set_y( rhs.get_x() );
   col1.set_y( rhs.get_y() );
   col2.set_y( rhs.get_z() );
   col3.set_y( rhs.get_w() );
}

inline
void
vu0_transform_t::set_row2( vu0_vec_4 rhs )
{
   col0.set_z( rhs.get_x() );
   col1.set_z( rhs.get_y() );
   col2.set_z( rhs.get_z() );
   col3.set_z( rhs.get_w() );
}

inline
void
vu0_transform_t::set_mat_33( const vu0_mat_33& rhs )
{
   col0 = vu0_vector_t( rhs.col0 );
   col1 = vu0_vector_t( rhs.col1 );
   col2 = vu0_vector_t( rhs.col2 );
}

inline
void
vu0_transform_t::set_vec_3( vu0_vec_3 rhs )
{
   col3 = vu0_point_t( rhs );
}

inline
void
vu0_transform_t::set_zero()
{
   col0.set_zero();
   col1.set_zero();
   col2.set_zero();
   col3.set_zero();
}

inline
void
vu0_transform_t::set_identity()
{
   _vsub( col0.vec128, vf_XYZW, vu0_vf0, vu0_vf0 );
   _vsub( col1.vec128, vf_XYZW, vu0_vf0, vu0_vf0 );
   _vmr32( col2.vec128, vf_XYZW, vu0_vf0 );
   _vmove( col3.vec128, vf_XYZW, vu0_vf0 );
   _vaddbc( col0.vec128, vf_X, vu0_vf0, vu0_vf0, vf_W );
   _vaddbc( col1.vec128, vf_Y, vu0_vf0, vu0_vf0, vf_W );
}

inline
void
vu0_transform_t::set_scale( vu0_vec_3 scale ) 
{
   col0.set_zero();
   col1.set_zero();
   col2.set_zero();
   col0 = vu0_vec_x( scale );
   col1 = vu0_vec_y( scale );
   col2 = vu0_vec_z( scale );   
   col3.set_vf0();
}

inline
void
vu0_transform_t::set_rotate_x( float angle ) 
{
   float cs = cosf(angle);
   float sn = sinf(angle);
   col0.set( 1.0f, 0.0f, 0.0f );
   col1.set( 0.0f, cs, sn );
   col2.set( 0.0f, -sn, cs );
   col3.set_vf0();
}

inline
void
vu0_transform_t::set_rotate_y( float angle ) 
{
   float cs = cosf(angle);
   float sn = sinf(angle);
   col0.set( cs, 0.0f, -sn );
   col1.set( 0.0f, 1.0f, 0.0f );
   col2.set( sn, 0.0f, cs );
   col3.set_vf0();
}

inline
void
vu0_transform_t::set_rotate_z( float angle ) 
{
   float cs = cosf(angle);
   float sn = sinf(angle);
   col0.set( cs, sn, 0.0f );
   col1.set( -sn, cs, 0.0f );
   col2.set( 0.0f, 0.0f, 1.0f );
   col3.set_vf0();
}

inline
void
vu0_transform_t::set_translate( vu0_vec_3 translation ) 
{
   set_identity();
   col3 = vu0_point_t(translation);
}

inline
vu0_transform_t&
vu0_transform_t::operator = ( const vu0_transform_t& rhs )
{
   col0 = rhs.col0;
   col1 = rhs.col1;
   col2 = rhs.col2;
   col3 = rhs.col3;
   return *this;
}

inline
vu0_transform_t&
vu0_transform_t::operator = ( const vu0_mat_33& rhs )
{
   col0 = rhs.col0;
   col1 = rhs.col1;
   col2 = rhs.col2;
   return *this;
}

inline
vu0_transform_t&
vu0_transform_t::operator = ( const vu0_mat_34& rhs )
{
   col0 = rhs.col0;
   col1 = rhs.col1;
   col2 = rhs.col2;
   col3 = rhs.col3;
   return *this;
}

inline
vu0_transform_t
vu0_transform_t::inverse() const
{
   __v4sf inv0, inv1, inv2, inv3;
   __v4sf temp, det;
   __v4sf ACC;
   float Q;
   _vopmula( ACC, col0.vec128, col1.vec128 );
   _vopmsub( inv2, ACC, col1.vec128, col0.vec128 );
   _vopmula( ACC, col1.vec128, col2.vec128 );
   _vopmsub( inv0, ACC, col2.vec128, col1.vec128 );
   _vmul( det, vf_XYZW, col2.vec128, inv2 );
   _vaddbc( temp, vf_X, vu0_vf0, vu0_vf0, vf_W );
   _vopmula( ACC, col2.vec128, col0.vec128 );
   _vopmsub( inv1, ACC, col0.vec128, col2.vec128 );
   _vaddbc( ACC, vf_X, det, det, vf_Y );
   _vmaddbc( det, vf_X, ACC, temp, det, vf_Z );
   _vaddbc( temp, vf_Y, vu0_vf0, inv2, vf_X );
   _vadd( temp, vf_XZ, vu0_vf0, inv1 );
   _vaddbc( inv1, vf_X, vu0_vf0, inv0, vf_Y );
   _vdiv( Q, vu0_vf0, vf_W, det, vf_X );
   _vaddbc( inv1, vf_Z, vu0_vf0, inv2, vf_Y );
   _vaddbc( inv2, vf_X, vu0_vf0, inv0, vf_Z );
   _vaddbc( inv0, vf_Z, vu0_vf0, temp, vf_Y );
   _vaddbc( inv0, vf_Y, vu0_vf0, temp, vf_X );
   _vaddbc( inv2, vf_Y, vu0_vf0, temp, vf_Z );
   _vadd( ACC, vf_XYZW, vu0_vf0, vu0_vf0 );
   _vmsubbc( ACC, vf_XYZW, ACC, inv1, col3.vec128, vf_Y );
   _vmsubbc( ACC, vf_XYZW, ACC, inv0, col3.vec128, vf_X );
   _vmsubbc( inv3, vf_XYZW, ACC, inv2, col3.vec128, vf_Z );
   _vmulf( inv0, vf_XYZ, inv0, Q );
   _vmulf( inv1, vf_XYZ, inv1, Q );
   _vmulf( inv2, vf_XYZ, inv2, Q );
   _vmulf( inv3, vf_XYZ, inv3, Q );
   return vu0_transform_t( inv0, inv1, inv2, inv3 );
}

inline
void
vu0_transform_t::invert()
{
   *this = this->inverse();
}

inline
vu0_transform_t
vu0_transform_t::orthogonal_inverse() const
{
   __v4sf inv0, inv1, inv2, inv3;
   __v4sf ACC;
   _vadd( inv0, vf_X, vu0_vf0, col0.vec128 );
   _vadd( inv1, vf_Y, vu0_vf0, col1.vec128 );
   _vadd( inv2, vf_Z, vu0_vf0, col2.vec128 );
   _vaddbc( inv0, vf_Y, vu0_vf0, col1.vec128, vf_X );
   _vaddbc( inv1, vf_X, vu0_vf0, col0.vec128, vf_Y );
   _vaddbc( inv2, vf_X, vu0_vf0, col0.vec128, vf_Z );
   _vaddbc( inv0, vf_Z, vu0_vf0, col2.vec128, vf_X );
   _vaddbc( inv1, vf_Z, vu0_vf0, col2.vec128, vf_Y );
   _vaddbc( inv2, vf_Y, vu0_vf0, col1.vec128, vf_Z );
   _vadd( ACC, vf_XYZW, vu0_vf0, vu0_vf0 );
   _vmsubbc( ACC, vf_XYZW, ACC, inv0, col3.vec128, vf_X );
   _vmsubbc( ACC, vf_XYZW, ACC, inv1, col3.vec128, vf_Y );
   _vmsubbc( inv3, vf_XYZW, ACC, inv2, col3.vec128, vf_Z );
   return vu0_transform_t( inv0, inv1, inv2, inv3 );
}

inline
void
vu0_transform_t::orthogonal_invert()
{
   *this = this->orthogonal_inverse();
}

inline
vu0_vec_4
vu0_transform_t::operator * ( vu0_vec_4 rhs ) const
{
   __v4sf result;
   __v4sf ACC;
   _vmove( result, vf_W, rhs.vec128 );
   _vmulbc( ACC, vf_XYZW, col0.vec128, rhs.vec128, vf_X );
   _vmaddbc( ACC, vf_XYZW, ACC, col1.vec128, rhs.vec128, vf_Y );
   _vmaddbc( ACC, vf_XYZW, ACC, col2.vec128, rhs.vec128, vf_Z );
   _vmaddbc( result, vf_XYZ, ACC, col3.vec128, rhs.vec128, vf_W );
   return vu0_vec_4(result);
}

inline
vu0_vector_t
vu0_transform_t::operator * ( vu0_vector_t rhs ) const
{
   __v4sf result;
   __v4sf ACC;
   _vmulbc( ACC, vf_XYZW, col0.vec128, rhs.vec128, vf_X );
   _vmaddbc( ACC, vf_XYZW, ACC, col1.vec128, rhs.vec128, vf_Y );
   _vmaddbc( result, vf_XYZW, ACC, col2.vec128, rhs.vec128, vf_Z );
   return vu0_vector_t(result);
}

inline
vu0_point_t
vu0_transform_t::operator * ( vu0_point_t rhs ) const
{
   __v4sf result;
   __v4sf ACC;
   _vmulbc( ACC, vf_XYZW, col0.vec128, rhs.vec128, vf_X );
   _vmaddbc( ACC, vf_XYZW, ACC, col1.vec128, rhs.vec128, vf_Y );
   _vmaddbc( ACC, vf_XYZW, ACC, col2.vec128, rhs.vec128, vf_Z );
   _vmaddbc( result, vf_XYZW, ACC, col3.vec128, vu0_vf0, vf_W );
   return vu0_point_t(result);
}

inline
vu0_vec_3
vu0_transform_t::mul_vector( vu0_vec_3 rhs ) const
{
   return vu0_vec_3( *this * vu0_vector_t(rhs) );
}

inline
vu0_vec_3
vu0_transform_t::mul_point( vu0_vec_3 rhs ) const
{
   return vu0_vec_3( *this * vu0_point_t(rhs) );
}

inline
vu0_mat_43
vu0_transform_t::operator * ( const vu0_mat_43& rhs ) const
{
   return vu0_mat_43( *this * rhs.col0, *this * rhs.col1, *this * rhs.col2 );
}

inline
vu0_mat_44
vu0_transform_t::operator * ( const vu0_mat_44& rhs ) const
{
   return vu0_mat_44( *this * rhs.col0, *this * rhs.col1, *this * rhs.col2, *this * rhs.col3 );
}

inline
vu0_transform_t
vu0_transform_t::operator * ( const vu0_transform_t& rhs ) const
{
   return vu0_transform_t( *this * rhs.col0, *this * rhs.col1, *this * rhs.col2, *this * rhs.col3 );
}

inline
void
vu0_transform_t::print() const
{
   get_row0().print();
   get_row1().print();
   get_row2().print();
   get_row3().print();
}

inline
void
vu0_transform_t::print( const char *name ) const
{
   printf("%s:\n", name);
   print();
}

inline
vu0_vec_xyz
vu0_vec_xyz::row_mul( const vu0_mat_33& mat ) const
{
   __v4sf ones, tmp0, tmp1, tmp2, result;
   __v4sf ACC;
   _vmul( tmp0, vf_XYZW, vec128, mat.col0.vec128 );
   _vmaxbc( ones, vf_XYZW, vu0_vf0, vu0_vf0, vf_W );
   _vmul( tmp1, vf_XYZW, vec128, mat.col1.vec128 );
   _vmul( tmp2, vf_XYZW, vec128, mat.col2.vec128 );
   _vaddbc( ACC, vf_X, tmp0, tmp0, vf_Y );
   _vmaddbc( result, vf_X, ACC, ones, tmp0, vf_Z );
   _vaddbc( ACC, vf_Y, tmp1, tmp1, vf_X );
   _vmaddbc( result, vf_Y, ACC, ones, tmp1, vf_Z );
   _vaddbc( ACC, vf_Z, tmp2, tmp2, vf_X );
   _vmaddbc( result, vf_Z, ACC, ones, tmp2, vf_Y );
   return vu0_vec_xyz( result );
}

inline
vu0_mat_33
vu0_vec_xyz::operator ~() const   
{
   __v4sf res0, res1, res2;
   __v4sf ACC;
   _vsub( res0, vf_XYZW, vu0_vf0, vu0_vf0 );
   _vsub( res1, vf_XYZW, vu0_vf0, vu0_vf0 );
   _vmr32( res2, vf_XYZW, vu0_vf0 );
   _vaddbc( res0, vf_X, vu0_vf0, vu0_vf0, vf_W );
   _vaddbc( res1, vf_Y, vu0_vf0, vu0_vf0, vf_W );
   _vopmula( ACC, vec128, res0 );
   _vopmsub( res0, ACC, res0, vec128 );
   _vopmula( ACC, vec128, res1 );
   _vopmsub( res1, ACC, res1, vec128 );
   _vopmula( ACC, vec128, res2 );
   _vopmsub( res2, ACC, res2, vec128 );
   return vu0_mat_33( res0, res1, res2 );
}

inline
vu0_mat_33
vu0_vec_xyz::tilde_mul( const vu0_mat_33& mat ) const 
{
   return vu0_mat_33( this->cross(mat.col0), this->cross(mat.col1), this->cross(mat.col2) );
}

#endif // vu0_matrix_h
