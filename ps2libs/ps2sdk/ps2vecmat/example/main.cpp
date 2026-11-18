#include <stdio.h>

#include "ps2_vector.h"
#include "ps2_matrix.h"

inline void StartClock( void ) {
   asm __volatile__ (
      "mtc0    $0, $9\n"
      "sync.p        \n"
   );
}

inline unsigned int GetTicks( void ) {
   unsigned int ret;
   asm __volatile__ (
      "mfc0        %0, $9\n"
      "sync.p            \n"
      : "=r" (ret)
   );
   return ret;
}

mat_33
DoRandomMath()
{
   vec_3 axyz, bxyz, cxyz, dxyz, exyz, fxyz, gxyz, hxyz, ixyz;

   axyz.set_x(1.0f);
   axyz.set_y(2.0f);
   axyz.set_z(3.0f);
   bxyz.set(4.0f,5.0f,6.0f);

   mat_33 m33( axyz, bxyz, cxyz );
   mat_33 n33( cxyz, bxyz, axyz );
   mat_33 o33( axyz, cxyz, bxyz );

   const int nSteps = 10000;

   for (int i = 0; i < nSteps; i++) 
   {
      axyz = m33 * axyz;
      bxyz += -axyz;

      cxyz = (axyz * bxyz).abs();

      dxyz = axyz.sign() * cxyz;
      exyz = (axyz.max(cxyz)).min(dxyz);
      fxyz = axyz * exyz.max();
      fxyz *= exyz.min();

      fxyz += exyz.sum();

      gxyz = fxyz.normalized() * exyz.dot(bxyz);

      hxyz = gxyz.cross(cxyz);

      m33 = ~fxyz;
      m33 *= vec_x(bxyz);
      m33 += n33;
      ixyz = m33 * gxyz;

      n33 = ~ixyz * m33;
      o33 = n33 * n33.transposed();

      m33 = (o33 * n33).inverse();
   }

   return m33;
}

int main()
{
   StartClock();
   unsigned int starttick = GetTicks();

   DoRandomMath();

   unsigned int endtick = GetTicks();
   printf("total cycles: %d\n", endtick - starttick);
}
