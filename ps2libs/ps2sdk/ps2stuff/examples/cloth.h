/*	  Copyright (C) 2000,2001,2002  Sony Computer Entertainment America
       	  
       	  This file is subject to the terms and conditions of the GNU Lesser
	  General Public License Version 2.1. See the file "COPYING" in the
	  main directory of this archive for more details.                             */

#ifndef cloth_h
#define cloth_h

#include "libvu0.h"

#include "ps2s/vector.h"
#include "ps2s/packet.h"

class cloth_pt
{
   public:
      vec_xyzw		position, velocity, normal;
};

class cloth_pt_libvu0
{
   public:
      sceVu0FVECTOR	position, velocity, normal;
};


class cloth
{
      cloth_pt		*points;
      vec_xyzw		gravity;

      int		num_rows, num_cols;
      float		side_len_row, side_len_col;

      vec_xyzw		lower_left_corner;

      float		stretch_rest_length_row, stretch_rest_length_col;
      float		shear_rest_length;
      float		bend_rest_length_row, bend_rest_length_col;


   public:
      cloth( int _num_rows, int _num_cols,
	     float _side_len_row, float _side_len_col,
	     vec_xyzw _lower_right_corner );
      ~cloth();

      void
      update_normals( cloth_pt *pts );

      vec_xyzw
      get_spring_force( const cloth_pt &pt1, const cloth_pt &pt2,
			float spring_const, float damp_const, float rest_length );

      void
      update( float timestep, vec_xyzw wind_dir, float wind_strength );

      void get_spring_force_lv( sceVu0FVECTOR result,
				const cloth_pt_libvu0 &pt1, const cloth_pt_libvu0 &pt2,
				float spring_const, float damp_const, float rest_length );

      void update_libvu0( float timestep, sceVu0FVECTOR wind_dir, float wind_strength );

      void
      draw( CVifSCDmaPacket& packet, const mat_44& tex_xform );

      void
      xfer_buffer_header( CVifSCDmaPacket &packet,
			  int vu1QuadsPerVert,
			  int numVerts, int numStrips,
			  unsigned short *stripOffsets );

      void
      reset( );

      void
      print();
};

#endif // cloth_h
