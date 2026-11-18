#ifndef  __VECTOR_INL__
# define __VECTOR_INL__

#if defined ( TRANSGAMING )
//*************************************** Vector Implementation ************************************//
Vector::Vector(const Vector& vec) 
{
	X = vec.X;
	Y = vec.Y;
	Z = vec.Z;
}

Vector::Vector(const Vector4& vec){
	X = vec.X;
	Y = vec.Y;
	Z = vec.Z;
}

Vector::Vector(const float* ptr) 
{
	X = ptr[0];
	Y = ptr[1];
	Z = ptr[2];
}
Vector::Vector(const float a)
{
	X = a;
	Y = a;
	Z = a; 
}

Vector::Vector(const float x, const float y, const float z)	
{
	X = x;
	Y = y;
	Z = z; 
}

// There were PS2 linking issues without this.
Vector::Vector(BaseVector const& base_vec )
{
	X = base_vec.X;
	Y = base_vec.Y;
	Z = base_vec.Z;
}

Vector &Vector::operator = (const Vector &v){
    X = v.X;
    Y = v.Y;
    Z = v.Z;

    return *this;
}

Vector &Vector::operator = (const Vector4 &v){
    X = v.X;
    Y = v.Y;
    Z = v.Z;

    return *this;
}

void Vector::Maximize(const Vector4& vec){
	if(vec.X > X) X=vec.X;
	if(vec.Y > Y) Y=vec.Y;
	if(vec.Z > Z) Z=vec.Z;
}

void Vector::Minimize(const Vector4& vec){
	if(vec.X < X) X=vec.X;
	if(vec.Y < Y) Y=vec.Y;
	if(vec.Z < Z) Z=vec.Z;
}

//*************************************** Vector4 Implementation ************************************//
Vector4::Vector4(Vector const &v){
	X = v.X;
	Y = v.Y;
	Z = v.Z;
	W = 1.0f;
}

Vector4::Vector4(const float *f){
    X = f[0];
    Y = f[1];
    Z = f[2];
    W = 1.0f;
}

Vector4::Vector4(const float x, const float y, const float z, const float w){
    X = x;
    Y = y;
    Z = z;
    W = 1.0f;
}


Vector4 &Vector4::operator = (const Vector &v){
    X = v.X;
    Y = v.Y;
    Z = v.Z;
    W = 1.0f;

    return *this;
}

Vector4 &Vector4::operator = (const Vector4 &v){
    Assert(v.W == 1.0f);
    X = v.X;
    Y = v.Y;
    Z = v.Z;
    W = 1.0f;

    return *this;
}


Vector4 Vector4::operator+(const Vector4& v) const{
    Assert(W == 1.0f && v.W == 1.0f);
    return Vector4(X + v.X, Y + v.Y, Z + v.Z, 1.0f);
}

Vector4 Vector4::operator - (const Vector4 &v) const{
    Assert(W == 1.0f && v.W == 1.0f);
    return Vector4(X - v.X, Y - v.Y, Z - v.Z, 1.0f);
}

//inline Vector4 Vector4::operator*(const Vector4& vec) const;
//inline Vector4 Vector4::operator/(const Vector4& vec) const;
// arithmetic assignment
//inline Vector4& Vector4::operator+=(const Vector4& vec);
inline Vector4& Vector4::operator-=(const Vector4& vec){
    Assert(W == 1.0f && vec.W == 1.0f);
    X -= vec.X;
    Y -= vec.Y;
    Z -= vec.Z;
    W = 1.0f;

    return *this;
}
//inline Vector4& Vector4::operator*=(const Vector4& vec); 
//inline Vector4& Vector4::operator/=(const Vector4& vec);	
// scalar operators
//inline Vector4 Vector4::operator+(const float a) const;
//inline Vector4 Vector4::operator-(const float a) const;
Vector4 Vector4::operator*(const float a) const{
    Assert(W == 1.0f);
    return Vector4(X * a, Y * a, Z * a, 1.0f);
}
//inline Vector4 Vector4::operator/(const float a) const;

// arithmetic scalar operators
//inline Vector4& Vector4::operator+=(const float a);
//inline Vector4& Vector4::operator-=(const float a);
Vector4 &Vector4::operator*=(const float a){
    Assert(W == 1.0f);
	X*=a; 
    Y*=a; 
    Z*=a;
    W = 1.0f;

	return *this;
}
//inline Vector4& Vector4::operator/=(const float a);

// vector operators
Vector Vector4::operator^(const Vector4& v) const{
    Assert(W == 1.0f && v.W == 1.0f);
    return Vector(  Y * v.Z - Z * v.Y,
                    Z * v.X - X * v.Z,
                    X * v.Y - Y * v.X);
}

void Vector4::Normalize(){
    Assert(W == 1.0f);
    float d = sqrtf((X * X) + (Y * Y) + (Z * Z));


    X /= d;
    Y /= d;
    Z /= d;
    W = 1.0f;
}

float Vector4::Dot(const Vector4& v) const{
    Assert(W == 1.0f && v.W == 1.0f);
    return X * v.X + Y * v.Y + Z * v.Z;
}

void Vector4::SetToClosestFromLine(const Vector4 &closestTo, const Vector4 &linePoint1, const Vector4 &linePoint2){
    Assert(W == 1.0f && closestTo.W == 1.0f && linePoint1.W == 1.0f && linePoint2.W == 1.0f);
	Vector4 dir = linePoint2 - linePoint1; 
	Vector4 toClosest = closestTo - linePoint1;

    dir.Normalize();
	(*this) = linePoint1 + dir * dir.Dot(toClosest);
    W = 1.0f;
}

float Vector4::MagnitudeSquared() const{
    Assert(W == 1.0f);
    return X * X + Y * Y + Z * Z;
}

//************************************** BaseVector Implementation ***************************************//
void BaseVector::Set(const float x, const float y, const float z)
{
	X = x;
	Y = y;
	Z = z;
}

void BaseVector::Set(const float a)
{
	X = Y = Z = a;
}

// accessors
const float BaseVector::operator[](int Index) const { return mArray[Index]; }
float& BaseVector::operator[](int Index)            { return mArray[Index]; }

// unary negation
BaseVector BaseVector::operator-() const 
{ 
	BaseVector bv;
	bv.X = -X; bv.Y = -Y; bv.Z = -Z;
	return bv;
	//return BaseVector(-X,-Y,-Z); 
}

// comparison
BOOL BaseVector::operator==(const BaseVector& vec) const
{
	return X==vec.X && Y==vec.Y && Z==vec.Z;
}
BOOL BaseVector::operator!=(const BaseVector& vec) const
{
	return X!=vec.X || Y!=vec.Y || Z!=vec.Z;
}
// component by component
BaseVector BaseVector::operator+(const BaseVector& vec) const
{
	BaseVector bv;
	bv.X = X+vec.X; bv.Y = Y+vec.Y; bv.Z = Z+vec.Z;
	return bv;
	//return BaseVector(X+vec.X,Y+vec.Y,Z+vec.Z);
}
BaseVector BaseVector::operator-(const BaseVector& vec) const
{
	BaseVector bv;
	bv.X = X-vec.X; bv.Y = Y-vec.Y; bv.Z = Z-vec.Z;
	return bv;
	//return BaseVector(X-vec.X,Y-vec.Y,Z-vec.Z);
}

BaseVector BaseVector::operator - (const Vector4 &v) const{
    BaseVector bv;


    bv.X = X - v.X;
    bv.Y = Y - v.Y;
    bv.Z = Z - v.Z;

    return bv;
}

BaseVector BaseVector::operator*(const BaseVector& vec) const
{
	BaseVector bv;
	bv.X = X*vec.X; bv.Y = Y*vec.Y; bv.Z = Z*vec.Z;
	return bv;
	//return BaseVector(X*vec.X,Y*vec.Y,Z*vec.Z);
}
BaseVector BaseVector::operator/(const BaseVector& vec) const
{
	BaseVector bv;
	bv.X = X/vec.X; bv.Y = Y/vec.Y; bv.Z = Z/vec.Z;
	return bv;
	//return BaseVector(X/vec.X,Y/vec.Y,Z/vec.Z);
}

// arithmetic assignment
BaseVector& BaseVector::operator+=(const BaseVector& vec) 
{
	X+=vec.X, Y+=vec.Y, Z+=vec.Z;
	return *this;
}
BaseVector& BaseVector::operator-=(const BaseVector& vec) 
{
	X-=vec.X, Y-=vec.Y, Z-=vec.Z;
	return *this;
}
BaseVector& BaseVector::operator*=(const BaseVector& vec) 
{
	X*=vec.X, Y*=vec.Y, Z*=vec.Z;
	return *this;
}
BaseVector& BaseVector::operator/=(const BaseVector& vec) 
{
	X/=vec.X, Y/=vec.Y, Z/=vec.Z;
	return *this;
}

// scalar operators
BaseVector BaseVector::operator+(const float a) const
{
	BaseVector bv;
	bv.X = X+a; bv.Y = Y+a; bv.Z = Z+a;
	return bv;
	//return BaseVector(X+a,Y+a,Z+a);
}
BaseVector BaseVector::operator-(const float a) const
{
	BaseVector bv;
	bv.X = X-a; bv.Y = Y-a; bv.Z = Z-a;
	return bv;
	//return BaseVector(X-a,Y-a,Z-a);
}
BaseVector BaseVector::operator*(const float a) const
{
	BaseVector bv;
	bv.X = X*a; bv.Y = Y*a; bv.Z = Z*a;
	return bv;
	//return BaseVector(X*a,Y*a,Z*a);
}
BaseVector BaseVector::operator/(const float a) const
{
	const float r=1.0f/a;
	BaseVector bv;
	bv.X = X*r; bv.Y = Y*r; bv.Z = Z*r;
	return bv;
	//return BaseVector(X*r,Y*r,Z*r);
}

// arithmetic scalar operators
BaseVector& BaseVector::operator+=(const float a) 
{
	X+=a, Y+=a, Z+=a;
	return *this;
}
BaseVector& BaseVector::operator-=(const float a) 
{
	X-=a, Y-=a, Z-=a;
	return *this;
}
BaseVector& BaseVector::operator*=(const float a) 
{
	X*=a, Y*=a, Z*=a;
	return *this;
}
BaseVector& BaseVector::operator/=(const float a) 
{
	const float r=1.0f/a;
	X*=r, Y*=r, Z*=r;
	return *this;
}

// vector operators
BaseVector BaseVector::operator^(const BaseVector& vec) const
{
	BaseVector bv;
	bv.X = Y*vec.Z-Z*vec.Y; bv.Y = Z*vec.X-X*vec.Z; bv.Z = X*vec.Y-Y*vec.X;
	return bv;
	//return BaseVector
	//(
	//	Y*vec.Z-Z*vec.Y,
	//	Z*vec.X-X*vec.Z,
	//	X*vec.Y-Y*vec.X
	//);
}
float BaseVector::Dot(const BaseVector& vec) const
{
	return X*vec.X + Y*vec.Y + Z*vec.Z;
}

inline float BaseVector::Dot(const Vector4 &v) const{
    return X * v.X + Y * v.Y + Z * v.Z;
}


float BaseVector::MagnitudeSquared() const
{
	return X*X + Y*Y + Z*Z;
}
float BaseVector::Magnitude() const
{
    return ::sqrtf(X*X + Y*Y + Z*Z);
}

float BaseVector::DistSquared(const BaseVector &v) const{
    float dx = X - v.X;
    float dy = Y - v.Y;
    float dz = Z - v.Z;

    return dx * dx + dy * dy + dz * dz;
}

float BaseVector::Dist(const BaseVector &v) const{
    float dx = X - v.X;
    float dy = Y - v.Y;
    float dz = Z - v.Z;

    return ::sqrtf(dx * dx + dy * dy + dz * dz);
}


float BaseVector::Normalize()
{
	const float Len=Magnitude();
	if(Len > F_EPS) {
		*this /= Len;
		return Len;
	}
	return 0.0f;
}

BaseVector BaseVector::GetNormal() const
{
    BaseVector tmp(*this);
    tmp.Normalize();
    return tmp;
}

void BaseVector::Maximize(const BaseVector& vec)
{
	if(vec.X > X) X=vec.X;
	if(vec.Y > Y) Y=vec.Y;
	if(vec.Z > Z) Z=vec.Z;
}
void BaseVector::Minimize(const BaseVector& vec)
{
	if(vec.X < X) X=vec.X;
	if(vec.Y < Y) Y=vec.Y;
	if(vec.Z < Z) Z=vec.Z;
}

BaseVector BaseVector::MinimalAxis() const
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

BaseVector BaseVector::MaximalAxis() const
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

BOOL BaseVector::AlmostZero() const
{
    return fabsf(X)<F_EPS && fabsf(Y)<F_EPS && fabsf(Z)<F_EPS;
}

inline void BaseVector::RandomNormalized()
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

inline int BaseVector::Sign() const
{
	return (X>=0) + ((Y>=0)<<1) + ((Z>=0)<<2);
}

inline void BaseVector::SetToClosestFromLine( const BaseVector& closestTo, const BaseVector& linePoint1, const BaseVector& linePoint2 )
{
	BaseVector dir = linePoint2-linePoint1; dir.Normalize();
	BaseVector toClosest = closestTo-linePoint1;
	(*this) = linePoint1 + dir*dir.Dot(toClosest);
}

// serialization
FileObject& operator<<(FileObject& file, const BaseVector& v)
{
    file.Write(&v.X,sizeof(BaseVector));
    return file;
}

FileObject& operator>>(FileObject& file, BaseVector& v)
{
    file.Read(&v.X,sizeof(BaseVector));
    return file;
}

#else

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

// serialization
FileObject& operator<<(FileObject& file, const Vector& v)
{
    file.Write(&v.X,sizeof(Vector));
    return file;
}

FileObject& operator>>(FileObject& file, Vector& v)
{
    file.Read(&v.X,sizeof(Vector));
    return file;
}

#endif

#endif
