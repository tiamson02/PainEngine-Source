/*           Copyright (C) 2003 Sony Computer Entertainment America
                              All Rights Reserved
                               SCEA Confidential                                */

#ifndef core_vector_h
#define core_vector_h

#include <stdio.h>
#include <math.h>
#include "vu0_vector.h"
                             
/********************************************
 * core_vec_xyz (or core_vec_3)
 */

class core_vec_xyz
{
   public:
      float x, y, z;

      inline core_vec_xyz() { }
      
      inline core_vec_xyz( const core_vec_xyz &rhs ) 
      {
         x = rhs.x; y = rhs.y; z = rhs.z;
      }
      
      inline core_vec_xyz( float _x, float _y, float _z )
      {
         x = _x; y = _y; z = _z;
      }
      
      inline void set ( float _x, float _y, float _z ) 
      {
         x = _x; y = _y; z = _z;
      }

      inline void set ( const __v4sf vec )
      {
         __v4sf temp0;
         asm volatile (
                "mtsab  $0, 4      \n" // get ready to shift right 4 bytes
                "move   %0, %4     \n" // x = value.x
                "qfsrv  %3, $0, %4 \n" // temp0 = value >> 32  
                "move   %1, %3     \n" // y = value.y
                "qfsrv  %3, $0, %3 \n" // temp0 >>= 32
                "move   %2, %3     \n" // z = value.z
                :  "=&r" (x), "=&r" (y), "=&r" (z), "=&r" (temp0)
                :  "r" (vec)
         );
      }

      explicit inline core_vec_xyz( const vu0_vec_xyz rhs ) { set(rhs.vec128); }

      inline float& operator [] (const int index) 
      {
         if (index == 0) return x;
         else if (index == 1) return y;
         else return z;
      }

      // use () to access individual elements when you don't need write access
      inline float operator () (const int index) const 
      {
         if (index == 0) return x;
         else if (index == 1) return y;
         else return z;
      }

      inline core_vec_xyz& operator = (const core_vec_xyz& rhs) 
      {
         x = rhs.x; y = rhs.y; z = rhs.z;
         return *this;
      }

      inline bool operator == (const core_vec_xyz &rhs) 
      {
         return ( x == rhs.x
               && y == rhs.y
               && z == rhs.z );
      }

      inline bool operator != (const core_vec_xyz &rhs) {
         return ( x != rhs.x
               || y != rhs.y
               || z != rhs.z );
      }

      inline core_vec_xyz operator + (const core_vec_xyz& rhs) const;
      inline core_vec_xyz operator - (const core_vec_xyz& rhs) const;
      inline core_vec_xyz operator * (const core_vec_xyz& rhs) const;
      inline core_vec_xyz operator - ();
      inline core_vec_xyz operator * (float scalar) const;
      inline float dot(const core_vec_xyz &vec) const;
      inline core_vec_xyz cross(const core_vec_xyz &vec) const;
      inline core_vec_xyz normalized() const;
      inline void normalize();

      void print() const 
      {
         printf("%f %f %f\n", x, y, z);
      }
};

typedef core_vec_xyz core_vec_3;

/********************************************
 * core_vec_xyzw (or core_vec_4)
 */

class core_vec_xyzw
{
   public:
      float x, y, z, w;

      inline core_vec_xyzw() { }

      inline core_vec_xyzw( const core_vec_xyzw &rhs ) 
      {
         x = rhs.x; y = rhs.y; z = rhs.z; w = rhs.w;
      };

      inline core_vec_xyzw( float _x, float _y, float _z, float _w ) 
      {
         x = _x; y = _y; z = _z; w = _w;
      }

      inline void set ( float _x, float _y, float _z, float _w ) 
      {
         x = _x; y = _y; z = _z; w = _w;
      }

      inline void set ( const __v4sf vec )
      {
         __v4sf temp0;
         asm volatile (               
               "mtsab  $0, 4      \n"  // get ready to shift right 4 bytes  
               "move   %0, %5     \n"  // x = value.x  
               "qfsrv  %4, $0, %5 \n"  // temp0 = value >> 32  
               "move   %1, %4     \n"  // y = value.y 
               "qfsrv  %4, $0, %4 \n"  // temp0 >>= 32 
               "move   %2, %4     \n"  // z = value.z 
               "qfsrv  %4, $0, %4 \n"  // temp0 >>= 32 
               "move   %3, %4     \n"  // w = value.w 
               :  "=&r" (x), "=&r" (y), "=&r" (z), "=&r" (w), "=&r" (temp0)
               :  "r" (vec)
         );
      }

      explicit inline core_vec_xyzw( const vu0_vec_xyzw rhs ) { set(rhs.vec128); }

      inline float& operator [] (const int index) 
      {
         if (index == 0) return x;
         else if (index == 1) return y;
         else if (index == 2) return z;
         else return w;
      }

      // use () to access individual elements when you don't need write access
      inline float operator () (const int index) const 
      {
         if (index == 0) return x;
         else if (index == 1) return y;
         else if (index == 2) return z;
         else return w;
      }

      inline core_vec_xyzw& operator = (const core_vec_xyzw &rhs) 
      {
         x = rhs.x; y = rhs.y; z = rhs.z; w = rhs.w;
         return *this;
      }

      inline core_vec_xyzw& operator = (const core_vec_xyz &rhs) 
      {
         x = rhs.x; y = rhs.y; z = rhs.z;
         return *this;
      }

      inline bool operator == (const core_vec_xyzw &rhs) 
      {
         return ( x == rhs.x
               && y == rhs.y
               && z == rhs.z
               && w == rhs.w );
      }

      inline bool operator != (const core_vec_xyzw &rhs) 
      {
         return ( x != rhs.x
               || y != rhs.y
               || z != rhs.z
               || w != rhs.w );
      }

      inline core_vec_xyzw operator + (const core_vec_xyzw& rhs) const;
      inline core_vec_xyzw operator - (const core_vec_xyzw& rhs) const;
      inline core_vec_xyzw operator * (const core_vec_xyzw& rhs) const;
      inline core_vec_xyzw operator - ();
      inline core_vec_xyzw operator * (float scalar) const;
      inline float dot(const core_vec_xyzw &vec) const;
      inline float length() const;
      inline core_vec_xyzw normalized() const;
      inline void normalize();

      void print() const 
      {
         printf("%f %f %f %f\n", x, y, z, w);
      }
};

typedef core_vec_xyzw core_vec_4;

/********************************************
 * core_vec_xyz methods
 */

inline
core_vec_xyz
core_vec_xyz::operator + (const core_vec_xyz &rhs) const
{
   return core_vec_xyz( x + rhs.x, y + rhs.y, z + rhs.z );
}

inline
core_vec_xyz
core_vec_xyz::operator - (const core_vec_xyz &rhs) const
{
   return core_vec_xyz( x - rhs.x, y - rhs.y, z - rhs.z );
}

inline
core_vec_xyz
core_vec_xyz::operator * (const core_vec_xyz &rhs) const
{
   return core_vec_xyz( x * rhs.x, y * rhs.y, z * rhs.z );
}

inline
core_vec_xyz
core_vec_xyz::operator - ()
{
   return core_vec_xyz( -x, -y, -z );
}

inline
core_vec_xyz
core_vec_xyz::operator * (float scalar) const
{
   return core_vec_xyz( x * scalar, y * scalar, z * scalar );
}

inline
float
core_vec_xyz::dot(const core_vec_xyz& rhs) const
{
   return ( x * rhs.x + y * rhs.y + z * rhs.z );
}

inline
core_vec_xyz
core_vec_xyz::cross(const core_vec_xyz& rhs) const
{
   return core_vec_xyz( y * rhs.z - z * rhs.y, 
                        z * rhs.x - x * rhs.z,
                        x * rhs.y - y * rhs.x );
}

inline
core_vec_xyz
core_vec_xyz::normalized() const
{
   float lengthInv = 1.0f/sqrtf(this->dot(*this));
   return ( *this * lengthInv );
}

inline 
void 
core_vec_xyz::normalize()
{
   *this = this->normalized();
}

/********************************************
 * core_vec_xyzw methods
 */

inline
core_vec_xyzw
core_vec_xyzw::operator + (const core_vec_xyzw &rhs) const
{
   return core_vec_xyzw( x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w );
}

inline
core_vec_xyzw
core_vec_xyzw::operator - (const core_vec_xyzw &rhs) const
{
   return core_vec_xyzw( x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w );
}

inline
core_vec_xyzw
core_vec_xyzw::operator * (const core_vec_xyzw &rhs) const
{
   return core_vec_xyzw( x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w );
}

inline
core_vec_xyzw
core_vec_xyzw::operator - ()
{
   return core_vec_xyzw( -x, -y, -z, -w );
}

inline
core_vec_xyzw
core_vec_xyzw::operator * (float scalar) const
{
   return core_vec_xyzw( x * scalar, y * scalar, z * scalar, w * scalar );
}

inline
float
core_vec_xyzw::dot(const core_vec_xyzw &rhs) const
{
   return ( x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w );
}

inline 
float
core_vec_xyzw::length() const
{
   return sqrtf(this->dot(*this));
}

inline
core_vec_xyzw
core_vec_xyzw::normalized() const
{
   float lengthInv = 1.0f/length();
   return *this * lengthInv;
}

inline
void
core_vec_xyzw::normalize()
{
   *this = normalized();
}

/********************************************
 * initialization of vu0 vectors with core_vectors
 */

inline
vu0_vec_xyz::vu0_vec_xyz( const core_vec_xyz &rhs ) 
{
   __v4sf temp;
   asm(
         "### construct vu0_vec_xyz with core_vec_xyz ### \n"
         "   pcpyld %1, %3, %2 \n"
         "   ppacw %0, %4, %1 \n"
         :  "=&r" (vec128), "=&r" (temp)
         :  "r" (rhs.x), "r" (rhs.y), "r" (rhs.z)
   );
}

inline
vu0_vec_xyzw::vu0_vec_xyzw( const core_vec_xyzw &rhs ) 
{
   __v4sf tempxz, tempyw;
   asm(
         "### construct vu0_vec_xyzw with core_vec_xyzw ### \n"
         "   pextlw %1, %5, %3 \n"
         "   pextlw %2, %6, %4 \n"
         "   pextlw %0, %2, %1 \n"
         :  "=&r" (vec128), "=&r" (tempxz), "=&r" (tempyw)
         :  "r" (rhs.x), "r" (rhs.y), "r" (rhs.z), "r" (rhs.w)
   );
}

#endif // core_vector_h
