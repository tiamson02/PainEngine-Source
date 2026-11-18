// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/08 Klaudiusz Zych
//----------------------------------------------------------------------------

// #pragma once
#ifndef _QUATERNION_INL_
#define _QUATERNION_INL_

Quaternion::Quaternion(float W, float X, float Y, float Z): W(W), X(X), Y(Y), Z(Z) 
{
}

Quaternion::Quaternion(const Quaternion& quat) : W(quat.W), X(quat.X), Y(quat.Y), Z(quat.Z) 
{
}

Quaternion::Quaternion(const Matrix& mat)
{
    // Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
    // article "Quaternion Calculus and Fast Animation".

	float trace = mat(0, 0) + mat(1, 1) + mat(2, 2);

	if (trace > 0) {
		float scale = sqrtf(trace + 1.0f);
		W = 0.5f * scale;
		scale = 0.5f / scale;
		X = scale * (mat(2, 1) - mat(1, 2));
		Y = scale * (mat(0, 2) - mat(2, 0));
		Z = scale * (mat(1, 0) - mat(0, 1));
	}
	else {
		static int neXt[] = { 1, 2, 0 };
		int i = 0;
		if (mat(1, 1) > mat(0, 0))
			i = 1;
		if (mat(2, 2) > mat(i, i))
			i = 2;
		int j = neXt[i];
		int k = neXt[j];
		float scale = sqrtf(mat(i, i) - mat(j, j) - mat(k, k) + 1);
		float* q[] = { &X, &Y, &Z };
		*q[i] = 0.5f * scale;
		scale = 0.5f / scale;
		W = scale * (mat(k, j) - mat(j, k));
		*q[j] = scale * (mat(j, i) + mat(i, j));
		*q[k] = scale * (mat(k, i) + mat(i, k));
	}
}

void Quaternion::ToAngleAxis(float &angle, Vector &axis) const
{
    // assert:  axis[] is unit length
    //
	// The quaternion representing the rotation is
	//   q = cos(A/2)+sin(A/2)*(X*i+Y*j+Z*k)
    float fHalfAngle = acosf(W);
	angle = fHalfAngle*2.0f;
	float fSin = sinf(fHalfAngle);
	if (fabsf(fSin)<0.01)
	{
		axis.X = 1;
		axis.Y = 0;
		axis.Z = 0;
	}
	else
	{
		float fSinInv = 1.0f/fSin;
		axis.X = X*fSinInv;
		axis.Y = Y*fSinInv;
		axis.Z = Z*fSinInv;
	}
}

Matrix Quaternion::ToMatrix() const
{
	Matrix mat;
	float x2 = X * 2;
	float y2 = Y * 2;
	float z2 = Z * 2;
	float wx = x2 * W;
	float wy = y2 * W;
	float wz = z2 * W;
	float xx = x2 * X;
	float xy = y2 * X;
	float xz = z2 * X;
	float yy = y2 * Y;
	float yz = z2 * Y;
	float zz = z2 * Z;
	mat(0, 0) = 1 - (yy + zz);
	mat(0, 1) = xy - wz;
	mat(0, 2) = xz + wy;
	mat(0, 3) = 0;
	mat(1, 0) = xy + wz;
	mat(1, 1) = 1 - (xx + zz);
	mat(1, 2) = yz - wx;
	mat(1, 3) = 0;
	mat(2, 0) = xz - wy;
	mat(2, 1) = yz + wx;
	mat(2, 2) = 1 - (xx + yy);
	mat(2, 3) = 0;
	mat(3, 0) = 0;
	mat(3, 1) = 0;
	mat(3, 2) = 0;
	mat(3, 3) = 1;

	return mat;
}

Quaternion::Quaternion(float angle, const Vector& axis)
{
    // assert:  axis[] is unit length
    //
	// The quaternion representing the rotation is
	//   q = cos(A/2)+sin(A/2)*(X*i+Y*j+Z*k)

    float fHalfAngle = 0.5f*angle;
    float fSin = sinf(fHalfAngle);
    W = cosf(fHalfAngle);
    X = fSin*axis.X;
    Y = fSin*axis.Y;
    Z = fSin*axis.Z;
}

bool Quaternion::operator== (const Quaternion& rkQ) const
{
	return ( ( W == rkQ.W && X == rkQ.X && Y == rkQ.Y && Z == rkQ.Z ) ||
			 ( W == -rkQ.W && X == -rkQ.X && Y == -rkQ.Y && Z == -rkQ.Z ) );
}

Quaternion& Quaternion::operator= (const Quaternion& rkQ)
{
    W = rkQ.W;
    X = rkQ.X;
    Y = rkQ.Y;
    Z = rkQ.Z;
    return *this;
}

Quaternion Quaternion::operator+ (const Quaternion& rkQ) const
{
    return Quaternion(W+rkQ.W,X+rkQ.X,Y+rkQ.Y,Z+rkQ.Z);
}

Quaternion Quaternion::operator- (const Quaternion& rkQ) const
{
    return Quaternion(W-rkQ.W,X-rkQ.X,Y-rkQ.Y,Z-rkQ.Z);
}

float Quaternion::Dot (const Quaternion& rkQ) const
{
    return W*rkQ.W+X*rkQ.X+Y*rkQ.Y+Z*rkQ.Z;
}

Quaternion Quaternion::UnitInverse () const
{
    // assert:  'this' is unit length
    return Quaternion(W,-X,-Y,-Z);
}

Quaternion Quaternion::Exp () const
{
    // If q = A*(X*i+Y*j+Z*k) where (X,Y,Z) is unit length, then
    // exp(q) = cos(A)+sin(A)*(X*i+Y*j+Z*k).  If sin(A) is near zero,
    // use exp(q) = cos(A)+A*(X*i+Y*j+Z*k) since A/sin(A) has limit 1.

    float fAngle = sqrtf(X*X+Y*Y+Z*Z);
    float fSin = sinf(fAngle);

    Quaternion kResult;
    kResult.W = cosf(fAngle);

    if ( fabsf(fSin) >= F_EPS )
    {
        float fCoeff = fSin/fAngle;
        kResult.X = fCoeff*X;
        kResult.Y = fCoeff*Y;
        kResult.Z = fCoeff*Z;
    }
    else
    {
        kResult.X = X;
        kResult.Y = Y;
        kResult.Z = Z;
    }

    return kResult;
}

Quaternion Quaternion::Log () const
{
    // If q = cos(A)+sin(A)*(X*i+Y*j+Z*k) where (X,Y,Z) is unit length, then
    // log(q) = A*(X*i+Y*j+Z*k).  If sin(A) is near zero, use log(q) =
    // sin(A)*(X*i+Y*j+Z*k) since sin(A)/A has limit 1.

    Quaternion kResult;
    kResult.W = 0.0f;

    if (fabsf(W) < 1.0f)
    {
        float fAngle = cosf(W);
        float fSin = sinf(fAngle);
        if (fabsf(fSin) >= F_EPS)
        {
            float fCoeff = fAngle/fSin;
            kResult.X = fCoeff*X;
            kResult.Y = fCoeff*Y;
            kResult.Z = fCoeff*Z;
            return kResult;
        }
    }

    kResult.X = X;
    kResult.Y = Y;
    kResult.Z = Z;

    return kResult;
}

Vector Quaternion::operator* (const Vector& rkVector) const
{
    Matrix rot = ToMatrix();
	return rot.TransformCoord(rkVector);
}


Quaternion Quaternion::operator* (const Quaternion& rkQ) const
{
    // NOTE:  Multiplication is not generally commutative, so in most
    // cases p*q != q*p.

    return Quaternion
    (
        W*rkQ.W-X*rkQ.X-Y*rkQ.Y-Z*rkQ.Z,
        W*rkQ.X+X*rkQ.W+Y*rkQ.Z-Z*rkQ.Y,
        W*rkQ.Y+Y*rkQ.W+Z*rkQ.X-X*rkQ.Z,
        W*rkQ.Z+Z*rkQ.W+X*rkQ.Y-Y*rkQ.X
    );
}

Quaternion Quaternion::operator* (float fScalar) const
{
    return Quaternion(fScalar*W,fScalar*X,fScalar*Y,fScalar*Z);
}

Quaternion Quaternion::operator*= (float fScalar)
{
    W *= fScalar;
    X *= fScalar;
    Y *= fScalar;
    Z *= fScalar;
	return *this;
}

Quaternion Quaternion::operator- () const
{
    return Quaternion(-W,-X,-Y,-Z);
}

Quaternion Quaternion::Slerp(float t, const Quaternion& quat0, const Quaternion& quat1)
{
    float dot = quat0.Dot(quat1);
	bool flip = false;

	if (dot < 0)
	{
		// check out if quat0 is aligned with quat1
		// #warning: Quaternion(x,y,z,w) == Quaternion(-x,-y,-z,-w), but they do not interpolate nicely!
		dot   = - dot;
		flip  = true;
	}

	float coeff0;
	float coeff1;

	const float SLERP_THRESHOLD = 0.001f;

	if ( (1.0f - dot) < SLERP_THRESHOLD )
	{
		// do a simpler linear interpolation
		coeff0 = 1 - t;
		coeff1 = t;
	}
	else
	{
		// do a full spherical interpolation
		float angle  = acosf(dot);

		float invSin = 1.0f/GSinFloat(angle);

		coeff0 = GSinFloat((1.0f-t)*angle)*invSin;
		coeff1 = GSinFloat(t*angle)*invSin;
	}

	if (flip)
	{
		return quat0*coeff0 - quat1*coeff1;
	}
	else
	{
		return quat0*coeff0 + quat1*coeff1;
	}
}

Quaternion Quaternion::SlerpExtraSpins(float t,	const Quaternion& quat0, const Quaternion& quat1, int iExtraSpins)
{
    float dot = quat0.Dot(quat1);
    float angle = acosf(dot);

    if (fabsf(angle) < F_EPS)
        return quat0;

    float fSin = sinf(angle);
    float fPhase = F_PI*iExtraSpins*t;
    float invSin = 1.0f/fSin;
    float coeff0 = sinf((1.0f-t)*angle - fPhase)*invSin;
    float coeff1 = sinf(t*angle + fPhase)*invSin;
    return quat0*coeff0 + quat1*coeff1;
}

void Quaternion::Intermediate (const Quaternion& rkQ0, const Quaternion& rkQ1, const Quaternion& rkQ2, 
							   Quaternion& rkA, Quaternion& rkB)
{
    // assert:  q0, q1, q2 are unit quaternions

    Quaternion kQ0inv = rkQ0.UnitInverse();
    Quaternion kQ1inv = rkQ1.UnitInverse();
    Quaternion rkP0 = kQ0inv*rkQ1;
    Quaternion rkP1 = kQ1inv*rkQ2;
    Quaternion kArg = (rkP0.Log()-rkP1.Log())*0.25f;
    Quaternion kMinusArg = -kArg;

    rkA = rkQ1*kArg.Exp();
    rkB = rkQ1*kMinusArg.Exp();
}

Quaternion Quaternion::Squad (float fT, const Quaternion& rkP, 
							  const Quaternion& rkA, const Quaternion& rkB, 
							  const Quaternion& rkQ)
{
    float fSlerpT = 2.0f*fT*(1.0f-fT);
    Quaternion kSlerpP = Slerp(fT,rkP,rkQ);
    Quaternion kSlerpQ = Slerp(fT,rkA,rkB);
    return Slerp(fSlerpT,kSlerpP,kSlerpQ);
}

void Quaternion::AlignQuaternionWith(Quaternion &quat0, const Quaternion &quat1)
{
	Quaternion q0Minusq1 = quat0-quat1; 
	Quaternion q0Plusq1  = quat0+quat1;

	if (q0Minusq1.Dot(q0Minusq1) > q0Plusq1.Dot(q0Plusq1))
	{
		quat0 = -quat0;
	}

}

inline float Quaternion::Norm( void ) const
{
	return W*W + X*X + Y*Y + Z*Z;
}

inline float Quaternion::Normalize()
{
	float l = W*W + X*X + Y*Y + Z*Z;
	if (l!=1)
	{
		if (l!=0)
		{
			l = ::sqrtf(l);
			*this *= 1/l;
		}
		else
		{
			*this = IdentityQuaternion();
		}
	}
	return l;
}

inline void Quaternion::FromEulerAnglesXYZ (const Vector &pyr)
{
	float roll  = pyr.X;
	float pitch = pyr.Y;
	float yaw   = pyr.Z;

	float cr = cos(roll/2);
	float cp = cos(pitch/2);
	float cy = cos(yaw/2);

	float sr = sin(roll/2);
	float sp = sin(pitch/2);
	float sy = sin(yaw/2);

	float cpcy = cp * cy;
	float spsy = sp * sy;

	W = cr * cpcy + sr * spsy;
	X = sr * cpcy - cr * spsy;
	Y = cr * sp * cy + sr * cp * sy;
	Z = cr * cp * sy - sr * sp * cy;
}


inline void Quaternion::ToEulerAnglesXYZ (Vector &pyr) const
{
	const float QuatToEulerAccuracy = 0.001f;

	float sinv, cosv, sinf, cosf;
	float cost;

	float sint = (2 * W * Y) - (2 * X * Z);
	float cost_temp = 1.0 - (sint * sint);

	if (fabs(cost_temp) > QuatToEulerAccuracy)
		cost = sqrtf(cost_temp);
	else
		cost = 0;
	

	if (fabs(cost) > QuatToEulerAccuracy)
	{
		sinv = ((2 * Y * Z) + (2 * W * X)) / cost;
		cosv = (1 - (2 * X * X) - (2 * Y * Y)) / cost;
		sinf = ((2 * X * Y) + (2 * W * Z)) / cost;
		cosf = (1 - (2 * Y * Y) - (2 * Z * Z)) / cost;
	}
	else
	{
		sinv = (2 * W * X) - (2 * Y * Z);
		cosv = 1 - (2 * X * X) - (2 * Z * Z);
		sinf = 0;
		cosf = 1;
	}
	
	pyr.X = atan2f(sinv, cosv);
    pyr.Y = atan2f(sint, cost);
	pyr.Z = atan2f(sinf, cosf);
}


#endif //_QUATERNION_INL_
