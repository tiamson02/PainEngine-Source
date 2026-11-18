
Vector::Vector(const Vector& vec)   : X(vec.X), Y(vec.Y), Z(vec.Z) {}
Vector::Vector(const float* ptr)	: X(ptr[0]), Y(ptr[1]), Z(ptr[2]) {}
Vector::Vector(const float a)		: X(a), Y(a), Z(a) {}

Vector::Vector(const float x, const float y, const float z)	
:	X(x), Y(y), Z(z) 
{}

void Vector::Set(const float x, const float y, const float z)
{
	X = x;
	Y = y;
	Z = z;
}

void Vector::Set(const float a)
{
	X = Y = Z = a;
}

// accessors
const float Vector::operator[](int Index) const { return mArray[Index]; }
float& Vector::operator[](int Index)            { return mArray[Index]; }

// unary negation
Vector Vector::operator-() const { return Vector(-X,-Y,-Z); }

// comparison
BOOL Vector::operator==(const Vector& vec) const
{
	return X==vec.X && Y==vec.Y && Z==vec.Z;
}
BOOL Vector::operator!=(const Vector& vec) const
{
	return X!=vec.X || Y!=vec.Y || Z!=vec.Z;
}
// component by component
Vector Vector::operator+(const Vector& vec) const
{
	return Vector(X+vec.X,Y+vec.Y,Z+vec.Z);
}
Vector Vector::operator-(const Vector& vec) const
{
	return Vector(X-vec.X,Y-vec.Y,Z-vec.Z);
}
Vector Vector::operator*(const Vector& vec) const
{
	return Vector(X*vec.X,Y*vec.Y,Z*vec.Z);
}
Vector Vector::operator/(const Vector& vec) const
{
	return Vector(X/vec.X,Y/vec.Y,Z/vec.Z);
}

// arithmetic assignment
Vector& Vector::operator+=(const Vector& vec) 
{
	X+=vec.X, Y+=vec.Y, Z+=vec.Z;
	return *this;
}
Vector& Vector::operator-=(const Vector& vec) 
{
	X-=vec.X, Y-=vec.Y, Z-=vec.Z;
	return *this;
}
Vector& Vector::operator*=(const Vector& vec) 
{
	X*=vec.X, Y*=vec.Y, Z*=vec.Z;
	return *this;
}
Vector& Vector::operator/=(const Vector& vec) 
{
	X/=vec.X, Y/=vec.Y, Z/=vec.Z;
	return *this;
}

// scalar operators
Vector Vector::operator+(const float a) const
{
	return Vector(X+a,Y+a,Z+a);
}
Vector Vector::operator-(const float a) const
{
	return Vector(X-a,Y-a,Z-a);
}
Vector Vector::operator*(const float a) const
{
	return Vector(X*a,Y*a,Z*a);
}
Vector Vector::operator/(const float a) const
{
	const float r=1.0f/a;
	return Vector(X*r,Y*r,Z*r);
}

// arithmetic scalar operators
Vector& Vector::operator+=(const float a) 
{
	X+=a, Y+=a, Z+=a;
	return *this;
}
Vector& Vector::operator-=(const float a) 
{
	X-=a, Y-=a, Z-=a;
	return *this;
}
Vector& Vector::operator*=(const float a) 
{
	X*=a, Y*=a, Z*=a;
	return *this;
}
Vector& Vector::operator/=(const float a) 
{
	const float r=1.0f/a;
	X*=r, Y*=r, Z*=r;
	return *this;
}

// vector operators
Vector Vector::operator^(const Vector& vec) const
{
	return Vector
	(
		Y*vec.Z-Z*vec.Y,
		Z*vec.X-X*vec.Z,
		X*vec.Y-Y*vec.X
	);
}
float Vector::Dot(const Vector& vec) const
{
	return X*vec.X + Y*vec.Y + Z*vec.Z;
}

float Vector::MagnitudeSquared() const
{
	return X*X + Y*Y + Z*Z;
}
float Vector::Magnitude() const
{
    return ::sqrtf(X*X + Y*Y + Z*Z);
}

float Vector::Normalize()
{
	const float Len=Magnitude();
	if(Len > F_EPS) {
		*this /= Len;
		return Len;
	}
	return 0.0f;
}

Vector Vector::GetNormal() const
{
    Vector tmp(*this);
    tmp.Normalize();
    return tmp;
}

void Vector::Maximize(const Vector& vec)
{
	if(vec.X > X) X=vec.X;
	if(vec.Y > Y) Y=vec.Y;
	if(vec.Z > Z) Z=vec.Z;
}
void Vector::Minimize(const Vector& vec)
{
	if(vec.X < X) X=vec.X;
	if(vec.Y < Y) Y=vec.Y;
	if(vec.Z < Z) Z=vec.Z;
}

Vector Vector::MinimalAxis() const
{
    if (fabsf(X)<fabsf(Y))
    {
        if (fabsf(X)<fabsf(Z))
            return Vector(1,0,0);
        else
            return Vector(0,0,1);
    }
    else // Y<=X
    {
        if (fabsf(Y)<fabsf(Z))
            return Vector(0,1,0);
        else
            return Vector(0,0,1);
    }
}

Vector Vector::MaximalAxis() const
{
    if (fabsf(X)>fabsf(Y))
    {
        if (fabsf(X)>fabsf(Z))
            return Vector(1,0,0);
        else
            return Vector(0,0,1);
    }
    else // Y>=X
    {
        if (fabsf(Y)>fabsf(Z))
            return Vector(0,1,0);
        else
            return Vector(0,0,1);
    }
}

BOOL Vector::AlmostZero() const
{
    return fabsf(X)<F_EPS && fabsf(Y)<F_EPS && fabsf(Z)<F_EPS;
}

inline void Vector::RandomNormalized()
{
	// try to get normal random distribution,
	// but agree to any random if well distributed cant be find in few passes
	for (int i=0; i<2; ++i) {

		X = 2.0f*rand()/(float)RAND_MAX - 1.0f;
		Y = 2.0f*rand()/(float)RAND_MAX - 1.0f;
		Z = 2.0f*rand()/(float)RAND_MAX - 1.0f;

		if (MagnitudeSquared()<1) break;
	}

	Normalize();    
}

inline int Vector::Sign() const
{
	return (X>=0) + ((Y>=0)<<1) + ((Z>=0)<<2);
}

inline void Vector::SetToClosestFromLine( const Vector& closestTo, const Vector& linePoint1, const Vector& linePoint2 )
{
	Vector dir = linePoint2-linePoint1; dir.Normalize();
	Vector toClosest = closestTo-linePoint1;
	(*this) = linePoint1 + dir*dir.Dot(toClosest);
}
