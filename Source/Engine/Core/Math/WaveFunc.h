
// #pragma once
#ifndef _WAVEFUNC_
#define _WAVEFUNC_


#include "Core/Math/LeMath.h"

class WaveFunc 
{
public:
    enum 
    {
        EWaveNone           = 0,
        EWaveSin            = 1,
        EWaveTriangle       = 2,
        EWaveSquare         = 3,
        EWaveSawTooth       = 4,
        EWaveInvSawTooth    = 5,
        EWaveNoise          = 6
    };

    WaveFunc() 
    :   Type(EWaveNone), Base(0), Amp(0), Phase(0), Freq(0)
    {}

    inline float Eval(const float time);

public:
    int     Type;
    float   Base;
    float   Amp;
    float   Phase;
    float   Freq;
};

// ====================================================================

inline float WaveFunc::Eval(const float time)
{
    float x = (Phase+time)*Freq;
    x-=(float)floor(x);

    float y;
    switch (Type) 
    {
    case EWaveSin:
        y = GSinFloat(x*F_TWOPI); 
        break;

    case EWaveTriangle:
        y = (x<0.5f) ? 2.0f*x-1.0f : -2.0f*x+2.0f;
        break;

    case EWaveSquare:
        y = (x<0.5f) ? +1.0f : -1.0f;
        break;

    case EWaveSawTooth:
        y=x;
        break;

    case EWaveInvSawTooth:
        y=1.0f-x;
        break;

    case EWaveNoise: // FIXME
    default:
        return 1.0f;
    }

    return Base + y*Amp;
}


#endif //_WAVEFUNC_

