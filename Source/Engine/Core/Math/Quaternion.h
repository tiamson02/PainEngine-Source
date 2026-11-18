// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/08 Klaudiusz Zych
//----------------------------------------------------------------------------
// #pragma once
#ifndef _QUATERNION_
#define _QUATERNION_
 

class Quaternion
{
public:
	union {
		struct { float W, X, Y, Z; };
		struct {
			float	Real;
#if defined ( TRANSGAMING )
			BaseVector	Imaginary;
#else
			Vector	Imaginary;
#endif
		};
	};

	Quaternion() {}
	inline Quaternion(float W, float X, float Y, float Z);
	inline Quaternion(const Quaternion& quat);
	inline Quaternion(const Matrix& rotationMatrix);
	inline Quaternion(float angle, const Vector& axis);

	// utility
	inline void AlignQuaternionWith(Quaternion &quat0, const Quaternion &quat1);

	inline Matrix ToMatrix() const;
	inline void ToAngleAxis(float &angle, Vector &axis) const;

    // arithmetic operations
	inline bool operator==(const Quaternion& rkQ) const;
    inline Quaternion& operator=(const Quaternion& rkQ);
    inline Quaternion operator+(const Quaternion& rkQ) const;
    inline Quaternion operator-(const Quaternion& rkQ) const;
    inline Quaternion operator*(const Quaternion& rkQ) const;
    inline Quaternion operator*(float fScalar) const;
    inline Quaternion operator*=(float fScalar);
    inline Quaternion operator-() const;

    // functions of a quaternion
    inline float Dot(const Quaternion& rkQ) const;		// dot product
    inline float Norm() const;							// squared-length
	inline float Normalize();							// normalize 
    inline Quaternion Inverse() const;					// apply to non-zero quaternion
    inline Quaternion UnitInverse() const;				// apply to unit-length quaternion
    inline Quaternion Exp() const;
    inline Quaternion Log() const;

    // from/to euler angles
    inline void ToEulerAnglesXYZ (Vector &pyr) const;
    inline void FromEulerAnglesXYZ (const Vector &pyr);

    // rotation of a vector by a quaternion
    inline Vector operator*(const Vector& rkVector) const;

    // spherical linear interpolation
    inline static Quaternion Slerp(float fT, const Quaternion& rkP,
        const Quaternion& rkQ);

    inline static Quaternion SlerpExtraSpins(float fT,
        const Quaternion& rkP, const Quaternion& rkQ,
        int iExtraSpins);

    // setup for spherical quadratic interpolation
    inline static void Intermediate(const Quaternion& rkQ0,
        const Quaternion& rkQ1, const Quaternion& rkQ2,
        Quaternion& rka, Quaternion& rkB);

    // spherical quadratic interpolation
    inline static Quaternion Squad(float fT, const Quaternion& rkP,
        const Quaternion& rkA, const Quaternion& rkB,
        const Quaternion& rkQ);

	// serialization
	inline friend FileObject& operator<<(FileObject& file, const Quaternion& q);
	inline friend FileObject& operator>>(FileObject& file, Quaternion& q);
};

// serialization
FileObject& operator<<(FileObject& file, const Quaternion& q)
{
	file.Write(&q.Imaginary.X,sizeof(Vector));
	file.Write(&q.Real,sizeof(float));
	return file;
}

FileObject& operator>>(FileObject& file, Quaternion& q)
{
	file.Read(&q.Imaginary.X,sizeof(Vector));
	file.Read(&q.Real,sizeof(float));
	return file;	
}

//----------------------------------------------------------------------------

class ZeroQuaternion : public Quaternion
{
public:
    ZeroQuaternion() : Quaternion(0,0,0,0) {}
};

//----------------------------------------------------------------------------

class IdentityQuaternion : public Quaternion
{
public:
	IdentityQuaternion() : Quaternion(1,0,0,0) {}
};

#endif //_QUATERNION_

