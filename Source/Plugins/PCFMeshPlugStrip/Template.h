// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/04 Bart Sekura
//----------------------------------------------------------------------------


template <class T> inline T Max(const T a, const T b)
{
	return (a>=b) ? a : b;
}
template <class T> inline T Min(const T a, const T b)
{
	return (a<=b) ? a : b;
}

template <class T> inline void Swap(T& a, T& b)
{
	const T tmp = a;
	a = b;
	b = tmp;
}

template <class T> inline T Abs(const T a)
{
	return (a>=T(0)) ? a : -a;
}

template <class T> inline T Clamp(const T x, const T min, const T max)
{
	return x<min ? min : x<max ? x : max;
}


template <class T>
struct Tuple
{
    T First;
    T Second;
};
