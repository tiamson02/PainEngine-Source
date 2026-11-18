/*           Copyright (C) 2003 Sony Computer Entertainment America
                              All Rights Reserved
                               SCEA Confidential                                */

#include "core_matrix.h"

void
core_mat_44::set_rotate( float angle, const core_vec_xyz& vec )
{
   core_vec_xyz axis = vec.normalized();
   float x = axis(0), y = axis(1), z = axis(2);

   float c = cosf(angle), s = sinf(angle);

   set_col0( core_vec_xyzw( x*x*(1-c) + c, 
                            y*x*(1-c) + z*s, 
                            x*z*(1-c) - y*s, 
                            0 ) );
   set_col1( core_vec_xyzw( x*y*(1-c) - z*s, 
                            y*y*(1-c) + c,
                            y*z*(1-c) + x*s,
                            0 ) );
   set_col2( core_vec_xyzw( x*z*(1-c) + y*s,
                            y*z*(1-c) - x*s,
                            z*z*(1-c) + c,
                            0 ) );
   set_col3( core_vec_xyzw(0, 0, 0, 1) );
}

