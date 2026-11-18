// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/04 Bart Sekura
//----------------------------------------------------------------------------

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
