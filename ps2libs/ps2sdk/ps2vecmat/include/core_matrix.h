/*           Copyright (C) 2003 Sony Computer Entertainment America
                              All Rights Reserved
                               SCEA Confidential                                */

#ifndef core_matrix_h
#define core_matrix_h

#include "core_vector.h"
#include "vu0_matrix.h"

class core_mat_44 
{
   public:
      core_vec_4 col0, col1, col2, col3;

      inline core_mat_44() { }

      inline core_mat_44( const core_mat_44& rhs )
      {
         col0 = rhs.col0;
         col1 = rhs.col1;
         col2 = rhs.col2;
         col3 = rhs.col3;
      }

      inline core_mat_44( core_vec_4 c0, core_vec_4 c1, core_vec_4 c2, core_vec_4 c3 )
      {
         col0 = c0;
         col1 = c1;
         col2 = c2;
         col3 = c3;
      }

      explicit inline core_mat_44( const vu0_mat_44& rhs )
      {
         col0 = core_vec_4(rhs.col0);
         col1 = core_vec_4(rhs.col1);
         col2 = core_vec_4(rhs.col2);
         col3 = core_vec_4(rhs.col3);
      }

      inline core_vec_4& operator [] (const int index)
      {
         if (index == 0) return col0;
         else if (index == 1) return col1;
         else if (index == 2) return col2;
         else return col3;
      }

      // use () to access individual columns when you don't need write access
      inline const core_vec_4& operator () (const int index) const 
      {
         if (index == 0) return col0;
         else if (index == 1) return col1;
         else if (index == 2) return col2;
         else return col3;
      }

      inline void
      set_identity() 
      {
         col0.set( 1, 0, 0, 0 );
         col1.set( 0, 1, 0, 0 );
         col2.set( 0, 0, 1, 0 );
         col3.set( 0, 0, 0, 1 );
      }

      inline core_vec_4 get_col0() const { return col0; }
      inline core_vec_4 get_col1() const { return col1; }
      inline core_vec_4 get_col2() const { return col2; }
      inline core_vec_4 get_col3() const { return col3; }

      inline void set_col0( const core_vec_xyzw& col ) { col0 = col; }
      inline void set_col1( const core_vec_xyzw& col ) { col1 = col; }
      inline void set_col2( const core_vec_xyzw& col ) { col2 = col; }
      inline void set_col3( const core_vec_xyzw& col ) { col3 = col; }

      core_mat_44 &
      operator = ( const core_mat_44& rhs ) 
      {
         col0 = rhs.col0;
         col1 = rhs.col1;
         col2 = rhs.col2;
         col3 = rhs.col3;
         return *this;
      }

      inline core_vec_4
      operator * ( const core_vec_4 &rhs ) const;

      inline core_mat_44
      operator * ( const core_mat_44 &rhs ) const;

      void
      set_scale( const core_vec_3& scale ) 
      {
         col0.set(scale.x, 0, 0, 0);
         col1.set(0, scale.y, 0, 0);
         col2.set(0, 0, scale.z, 0);
         col3.set(0, 0, 0, 1);
      }

      void
      set_translate( const core_vec_3& xlate_amount ) 
      {
         col0.set(1, 0, 0, 0);
         col1.set(0, 1, 0, 0);
         col2.set(0, 0, 1, 0);
         col3.set(xlate_amount.x, xlate_amount.y, xlate_amount.z, 1);
      }

      void set_rotate( float angle, const core_vec_xyz& axis );

      core_mat_44 transposed() const;
};

inline core_vec_4
core_mat_44::operator * ( const core_vec_4 &rhs ) const
{
   return core_vec_4( core_vec_4( col0.x, col1.x, col2.x, col3.x ).dot(rhs),
                      core_vec_4( col0.y, col1.y, col2.y, col3.y ).dot(rhs),
                      core_vec_4( col0.z, col1.z, col2.z, col3.z ).dot(rhs),
                      core_vec_4( col0.w, col1.w, col2.w, col3.w ).dot(rhs) );
}

inline core_mat_44
core_mat_44::operator * ( const core_mat_44 &rhs ) const
{
   return core_mat_44( *this * rhs.get_col0(),
                       *this * rhs.get_col1(),
                       *this * rhs.get_col2(),
                       *this * rhs.get_col3() );
}

inline
core_mat_44
core_mat_44::transposed() const 
{
   return core_mat_44( core_vec_4( col0.x, col1.x, col2.x, col3.x ),
                       core_vec_4( col0.y, col1.y, col2.y, col3.y ),
                       core_vec_4( col0.z, col1.z, col2.z, col3.z ),
                       core_vec_4( col0.w, col1.w, col2.w, col3.w ) );
}

#endif // core_matrix_h
