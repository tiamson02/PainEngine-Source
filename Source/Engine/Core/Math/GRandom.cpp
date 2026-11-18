/************************** MERSENNE.CPP ******************** AgF 2001-10-18 *
*  Random Number generator 'Mersenne Twister'                                *
*                                                                            *
*  This random number generator is described in the article by               *
*  M. Matsumoto & T. Nishimura, in:                                          *
*  ACM Transactions on Modeling and Computer Simulation,                     *
*  vol. 8, no. 1, 1998, pp. 3-30.                                            *
*                                                                            *
*  Experts consider this an excellent random number generator.               *
*                                                                            *
*****************************************************************************/
 
#include "Engine.h"
#include <windows.h>
#if defined(TRANSGAMING)
#include <mmsystem.h>

#else
#include <Mmsystem.h>
#endif
#include "GRandom.h"


static TRandomMersenne *initRNG();

TRandomMersenne *gRand = initRNG();


static TRandomMersenne *initRNG(){
    return new TRandomMersenne(timeGetTime());
}

void TRandomMersenne::RandomInit(int seed) {
  // re-seed generator
  unsigned int s = (unsigned int)seed;
  for (mti=0; mti<N; mti++) {
    s = s * 29943829 - 1;
    mt[mti] = s;}
}


unsigned int TRandomMersenne::BRandom() {
  // generate 32 random bits
  unsigned int y;

  if (mti >= N) {
    // generate N words at one time
    const unsigned int LOWER_MASK = (1u << R) - 1; // lower R bits
    const unsigned int UPPER_MASK = (unsigned int) (-1 << R);       // upper 32-R bits
    int kk, km;
    for (kk=0, km=M; kk < N-1; kk++) {
      y = (mt[kk] & UPPER_MASK) | (mt[kk+1] & LOWER_MASK);
      mt[kk] = mt[km] ^ (y >> 1) ^ (-(signed int)(y & 1) & MATRIX_A);
      if (++km >= N) km = 0;}

    y = (mt[N-1] & UPPER_MASK) | (mt[0] & LOWER_MASK);
    mt[N-1] = mt[M-1] ^ (y >> 1) ^ (-(signed int)(y & 1) & MATRIX_A);
    mti = 0;}

  y = mt[mti++];

  // Tempering (May be omitted):
  y ^=  y >> TEMU;
  y ^= (y << TEMS) & TEMB;
  y ^= (y << TEMT) & TEMC;
  y ^=  y >> TEML;

  return y;
}

//#pragma optimize("g", off)

#ifdef TRANSGAMING
float TRandomMersenne::Random() {
    // get 32 random bits and convert to float
    unsigned int r = BRandom();
    float f = (float)(r / ((65536.0f * 65536.0f) - 1));

    return f;
}
#else
double TRandomMersenne::Random() {
  // output random float number in the interval 0 <= x < 1
/*  union {
    double f;
    unsigned int i[2];}
  convert;
  // get 32 random bits and convert to float (nie dziala z wlaczona optymalizacja)
  unsigned int r = BRandom();
  convert.i[0] =  r << 20;
  convert.i[1] = (r >> 12) | 0x3FF00000;
  return convert.f - 1.0;*/

// get 32 random bits and convert to float
  unsigned int r = BRandom();
  double f = (double)(r/(65536.0*65536.0));
  return f;
}
#endif

  
int TRandomMersenne::IRandom(int min, int max) {
  // output random integer in the interval min <= x <= max
  int r;
#ifdef TRANSGAMING
  //let's multiply by the correct interval length to give us an inclusive result...
  r = int((max - min) * Random()) + min; // multiply interval with random and truncate
#else
  r = int((max - min + 1) * Random()) + min; // multiply interval with random and truncate
#endif
  if (r > max)
	  r = max;
  if (max < min)
		return max;
  return r;
}

//#pragma optimize("g", on)

// for C - LUA
extern "C" int IRandom(int min, int max)
{
    return gRand->IRandom(min,max);
}

extern "C" float FRandom(float min, float max){
    return gRand->Random() * (max - min + 1) + min;
}

extern "C" float Random(){
    return gRand->Random();
}
