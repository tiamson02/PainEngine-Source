// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/04 Bart Sekura
//----------------------------------------------------------------------------
#ifndef  __VECTOR_H__
# define __VECTOR_H__


#ifdef TRANSGAMING
class Vector4;


class BaseVector
{
public:
	// data
	union {
		struct  { float	X,Y,Z; };
		float	mArray[3];
	};

	// constructors
	inline void Set(const float x, const float y, const float z);
	inline void Set(const float a);

	// accessors
	inline const float operator[](int Index) const;
	inline float& operator[](int Index);

	// unary negation
	inline BaseVector operator-() const;

    // comparison
	inline BOOL operator==(const BaseVector& vec) const;
	inline BOOL operator!=(const BaseVector& vec) const;
	// component by component
	inline BaseVector operator+(const BaseVector& vec) const;
	inline BaseVector operator-(const BaseVector& vec) const;
	inline BaseVector operator*(const BaseVector& vec) const;
	inline BaseVector operator/(const BaseVector& vec) const;
	// arithmetic assignment
	inline BaseVector& operator+=(const BaseVector& vec);
	inline BaseVector& operator-=(const BaseVector& vec);
	inline BaseVector& operator*=(const BaseVector& vec); 
	inline BaseVector& operator/=(const BaseVector& vec);	
	// scalar operators
	inline BaseVector operator+(const float a) const;
	inline BaseVector operator-(const float a) const;
	inline BaseVector operator*(const float a) const;
	inline BaseVector operator/(const float a) const;

	// arithmetic scalar operators
	inline BaseVector& operator+=(const float a);
	inline BaseVector& operator-=(const float a);
	inline BaseVector& operator*=(const float a);
	inline BaseVector& operator/=(const float a);

	// vector operators
	inline BaseVector operator^(const BaseVector& vec) const;
	inline float Dot(const BaseVector& vec) const;
    inline float Dot(const Vector4 &v) const;

    inline BaseVector operator-(const Vector4 &v) const;


	inline float MagnitudeSquared() const;
	inline float Magnitude() const;

    inline float DistSquared(const BaseVector &v) const;
    inline float Dist(const BaseVector &v) const;

	inline float Normalize();

	inline BaseVector GetNormal() const;

	inline void Maximize(const BaseVector& vec);
	inline void Minimize(const BaseVector& vec);

	inline void SetToClosestFromLine( const BaseVector& closestTo, const BaseVector& linePoint1, const BaseVector& linePoint2 );

	inline int Sign() const; // returns pointing direction as bitfield, bit 0 is sign(X), bit 1 is sign(Y), bit2 is sign(Z)
    inline BaseVector MinimalAxis() const;
    inline BaseVector MaximalAxis() const;
	inline void RandomNormalized();

    inline BOOL AlmostZero() const;

    // serialization
  	inline friend FileObject& operator<<(FileObject& file, const BaseVector& v);
  	inline friend FileObject& operator>>(FileObject& file, BaseVector& v);
};


class Vector: public BaseVector
{
public:
    Vector() {}
	inline Vector(const Vector& vec);
	inline Vector(const Vector4& vec);
	inline Vector(const BaseVector& vec);
	inline Vector(const float* ptr);
	inline Vector(const float a);
	inline Vector(const float x, const float y, const float z);

    inline Vector &operator = (const Vector &v);
    inline Vector &operator = (const Vector4 &v);

	inline void Maximize(const Vector4& vec);
	inline void Minimize(const Vector4& vec);
};

class Vector4
{
public:
    float X, Y, Z, W;


    inline Vector4(){ W = 1.0f; }
	inline Vector4(const Vector& vec);
    inline Vector4(const float *f);
    inline Vector4(const float x, const float y, const float z, const float w = 1.0f);

    inline Vector4 &operator = (const Vector &v);
    inline Vector4 &operator = (const Vector4 &v);

	inline Vector4 operator+(const Vector4& vec) const;
	inline Vector4 operator-(const Vector4& vec) const;
	//inline Vector4 operator*(const Vector4& vec) const;
	//inline Vector4 operator/(const Vector4& vec) const;
	// arithmetic assignment
	//inline Vector4& operator+=(const Vector4& vec);
	inline Vector4& operator-=(const Vector4& vec);
	//inline Vector4& operator*=(const Vector4& vec); 
	//inline Vector4& operator/=(const Vector4& vec);	
	// scalar operators
	//inline Vector4 operator+(const float a) const;
	//inline Vector4 operator-(const float a) const;
	inline Vector4 operator*(const float a) const;
	//inline Vector4 operator/(const float a) const;

	// arithmetic scalar operators
	//inline Vector4& operator+=(const float a);
	//inline Vector4& operator-=(const float a);
	inline Vector4& operator*=(const float a);
	//inline Vector4& operator/=(const float a);

	// vector operators
	inline Vector operator^(const Vector4& vec) const;
	inline float Dot(const Vector4& vec) const;

	inline float MagnitudeSquared() const;
    inline void Normalize();

	inline void SetToClosestFromLine(const Vector4 &closestTo, const Vector4 &linePoint1, const Vector4 &linePoint2);
};

#else

class Vector
{
public:
	// data
	union {
		struct  { float	X,Y,Z; };
		float	mArray[3];
	};

	// constructors
	Vector() {}
	inline Vector(const Vector& vec);
    inline Vector(const float* ptr);
	inline Vector(const float a);
	inline Vector(const float x, const float y, const float z);

	inline void Set(const float x, const float y, const float z);
	inline void Set(const float a);

	// accessors
	inline const float operator[](int Index) const;
	inline float& operator[](int Index);

	// unary negation
	inline Vector operator-() const;

    // comparison
	inline BOOL operator==(const Vector& vec) const;
	inline BOOL operator!=(const Vector& vec) const;
	// component by component
	inline Vector operator+(const Vector& vec) const;
	inline Vector operator-(const Vector& vec) const;
	inline Vector operator*(const Vector& vec) const;
	inline Vector operator/(const Vector& vec) const;
	// arithmetic assignment
	inline Vector& operator+=(const Vector& vec);
	inline Vector& operator-=(const Vector& vec);
	inline Vector& operator*=(const Vector& vec); 
	inline Vector& operator/=(const Vector& vec);
	// scalar operators
	inline Vector operator+(const float a) const;
	inline Vector operator-(const float a) const;
	inline Vector operator*(const float a) const;
	inline Vector operator/(const float a) const;

	// arithmetic scalar operators
	inline Vector& operator+=(const float a);
	inline Vector& operator-=(const float a);
	inline Vector& operator*=(const float a);
	inline Vector& operator/=(const float a);

	// vector operators
	inline Vector operator^(const Vector& vec) const;
	inline float Dot(const Vector& vec) const;

	inline float MagnitudeSquared() const;
	inline float Magnitude() const;

	inline float Normalize();

	inline Vector GetNormal() const;

	inline void Maximize(const Vector& vec);
	inline void Minimize(const Vector& vec);

	inline int Sign() const; // returns pointing direction as bitfield, bit 0 is sign(X), bit 1 is sign(Y), bit2 is sign(Z)
    inline Vector MinimalAxis() const;
    inline Vector MaximalAxis() const;
	inline void RandomNormalized();

	inline void SetToClosestFromLine( const Vector& closestTo, const Vector& linePoint1, const Vector& linePoint2 );

    inline BOOL AlmostZero() const;

    // serialization
  	inline friend FileObject& operator<<(FileObject& file, const Vector& v);
  	inline friend FileObject& operator>>(FileObject& file, Vector& v);
};
#endif

#endif
