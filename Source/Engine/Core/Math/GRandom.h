#ifndef RANDOMC_H
# define RANDOMC_H

# include <math.h>
# include <assert.h>
# include <stdio.h>



class TRandomMersenne {                  // encapsulate random number generator
  enum constants {   
#  if 1
    // define constants for MT11213A
    N = 351, M = 175, R = 19, MATRIX_A = 0xEABD75F5,
    TEMU = 11, TEMS = 7, TEMT = 15, TEML = 17, TEMB = 0x655E5280, TEMC = 0xFFD58000
#  else    
    // or constants for MT19937
    N = 624, M = 397, R = 31, MATRIX_A = 0x9908B0DF,
    TEMU = 11, TEMS = 7, TEMT = 15, TEML = 18, TEMB = 0x9D2C5680, TEMC = 0xEFC60000
#  endif    
  };
  public:
  TRandomMersenne(int seed) {           // constructor
    RandomInit(seed);}
  void RandomInit(int seed);            // re-seed
  int IRandom(int min, int max);          // output random integer
#ifdef TRANSGAMING
  float Random();                           // output random float
#else
  double Random();                           // output random float
#endif
  unsigned int BRandom();                   // output random bits
  private:
  unsigned int mt[N];                       // state vector
  int mti;                                   // index into mt
  };    
  
extern TRandomMersenne *gRand;

extern "C"{
int     IRandom(int min, int max);
float   FRandom(float min, float max);
float   Random();
}
#endif
