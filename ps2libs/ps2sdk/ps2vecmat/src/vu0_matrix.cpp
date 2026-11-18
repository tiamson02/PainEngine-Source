/*           Copyright (C) 2003 Sony Computer Entertainment America
                              All Rights Reserved
                               SCEA Confidential                                */
                               
#include "vu0_matrix.h"
#include "core_vector.h"

// rotate around axis by angle.  expects unit vector

void
vu0_mat_33::set_rotate( float angle, vu0_vec_xyz axis )
{
   core_vec_xyz a = core_vec_xyz(axis);
   float x = a.x, y = a.y, z = a.z;
   float c = cosf(angle), s = sinf(angle);

   set_col0( vu0_vec_3( x*x*(1-c) + c, y*x*(1-c) + z*s, x*z*(1-c) - y*s ) );
   set_col1( vu0_vec_3( x*y*(1-c) - z*s, y*y*(1-c) + c, y*z*(1-c) + x*s ) );
   set_col2( vu0_vec_3( x*z*(1-c) + y*s, y*z*(1-c) - x*s, z*z*(1-c) + c ) );
}

void
vu0_mat_44::set_rotate( float angle, vu0_vec_xyz axis )
{
   core_vec_xyz a = core_vec_xyz(axis);
   float x = a.x;
   float y = a.y;
   float z = a.z;
   float c = cosf(angle);
   float s = sinf(angle);

   set_col0( vu0_vec_4( x*x*(1-c) + c, y*x*(1-c) + z*s, x*z*(1-c) - y*s, 0 ) );
   set_col1( vu0_vec_4( x*y*(1-c) - z*s, y*y*(1-c) + c, y*z*(1-c) + x*s, 0 ) );
   set_col2( vu0_vec_4( x*z*(1-c) + y*s, y*z*(1-c) - x*s, z*z*(1-c) + c, 0 ) );
   col3.set_vf0();
}

void
vu0_transform_t::set_rotate( float angle, vu0_vec_xyz axis )
{
   vu0_mat_33 rotation;
   rotation.set_rotate(angle,axis);

   *this = rotation;
   col3.set_vf0();
}

