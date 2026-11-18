
#pragma once

#define F_MAX       3.402823466e+38f
#define F_PI		3.14159265358979323846264338327950288f
#define F_HALFPI	(F_PI*0.5f)
#define F_TWOPI     (F_PI*2.0f)
#define F_EPS       0.0000001f
#define F_DEG2RAD	(F_PI/180.0f)

#define FDeg2Rad(x)	((x)*F_DEG2RAD)
#define FRad2Deg(x)	((x)/F_DEG2RAD)

class Vector;
class Plane;
class Quaternion;
class Matrix;
class BoundingBox;
class BoundingSphere;
class ConvexPolygon;
class ConvexVolume;
class RaySegment;

#include "Debug.h"
#include "DynArray.h"

#include "Vector.h"
#include "Plane.h"
#include "Quaternion.h"
#include "Matrix.h"
#include "BoundingBox.h"
#include "BoundingSphere.h"
#include "ConvexPolygon.h"
#include "ConvexVolume.h"
#include "RaySegment.h"

#include "Vector.inl"
#include "Matrix.inl"
#include "Quaternion.inl"
#include "ConvexVolume.inl"

//#include "WaveFunc.h"

// type traits
template <> struct ComplexType<Vector>      { enum { Value=false }; };
template <> struct ComplexType<Plane>       { enum { Value=false }; };
template <> struct ComplexType<Quaternion>  { enum { Value=false }; };
template <> struct ComplexType<Matrix>      { enum { Value=false }; };

extern  IdentityMatrix      GIdentityMatrix;
