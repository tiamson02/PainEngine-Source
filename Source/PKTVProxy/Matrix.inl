
Matrix::Matrix(const Matrix& mat)
:	X(mat.X), Y(mat.Y),	Z(mat.Z), W(mat.W)
{
}

Matrix::Matrix(const Plane& x, const Plane& y, const Plane& z, const Plane& w)
:   X(x), Y(y), Z(z), W(w)
{
}

Matrix::Matrix(const float* ptr)
{
    ::memcpy(mArray,ptr,16*sizeof(float));
}	

Matrix::Matrix(Vector &scale, Quaternion &rotation, Vector &translation)
{
	*this = rotation.ToMatrix();
	X.Normal *= scale.X;
	Y.Normal *= scale.Y;
	Z.Normal *= scale.Z;
	W.Normal  = translation;
	A03 = A13 = A23 = 0;
	A33 = 1;
}

Matrix::Matrix(float _A00, float _A01, float _A02, 
			   float _A10, float _A11, float _A12, 
			   float _A20, float _A21, float _A22)
{
	A00 = _A00; A01 = _A01; A02 = _A02; 
	A10 = _A10; A11 = _A11; A12 = _A12; 
	A20 = _A20; A21 = _A21; A22 = _A22; 
	A03 = A13 = A23 = A30 = A31 = A32 = 0.0f; A33 = 1.0f; 
}

// conversion to float*
Matrix::operator const float*(void) const { return &mArray[0]; }
//	float operator[](int Index) const { return mArray[Index]; }

const float& Matrix::operator()(const int &i, const int &j) const 
{ 
	return mArray[ j + i*4 ]; 
}

float& Matrix::operator()(const int &i, const int &j) 
{ 
	return mArray[ j + i*4 ]; 
}

void Matrix::Decompose(Vector &scale, Quaternion &rotation, Vector &translation) const
{
	scale = GetScale();
	translation = GetTranslation();
	rotation = Quaternion(GetRotation());
}

// homogenous transform
Vector Matrix::Transform(const Vector& vec) const
{             
	float d=1.0f/(vec.X*A03+vec.Y*A13+vec.Z*A23+A33);
	return Vector
	(
		(vec.X * A00 + vec.Y * A10 + vec.Z * A20 + A30) * d,
		(vec.X * A01 + vec.Y * A11 + vec.Z * A21 + A31) * d,
		(vec.X * A02 + vec.Y * A12 + vec.Z * A22 + A32) * d
	);                
}

Vector Matrix::TransformNormal(const Vector& vec) const
{        
	return Vector
	(
		vec.X * A00 + vec.Y * A10 + vec.Z * A20,
		vec.X * A01 + vec.Y * A11 + vec.Z * A21,
		vec.X * A02 + vec.Y * A12 + vec.Z * A22
	);        
}

Vector Matrix::TransformCoord(const Vector& vec) const
{
	return Vector
	(
		vec.X * A00 + vec.Y * A10 + vec.Z * A20 + A30,
		vec.X * A01 + vec.Y * A11 + vec.Z * A21 + A31,
		vec.X * A02 + vec.Y * A12 + vec.Z * A22 + A32
	);                
}

Plane  Matrix::TransformPlane(const Plane& plane) const
{
	return Plane
	(
		plane.Normal.X * A00 + plane.Normal.Y * A10 + plane.Normal.Z * A20 - plane.Dist * A30,
		plane.Normal.X * A01 + plane.Normal.Y * A11 + plane.Normal.Z * A21 - plane.Dist * A31,
		plane.Normal.X * A02 + plane.Normal.Y * A12 + plane.Normal.Z * A22 - plane.Dist * A32,
	  -(plane.Normal.X * A03 + plane.Normal.Y * A13 + plane.Normal.Z * A23 - plane.Dist * A33)
	);
}

Plane  Matrix::TransformVec4(const Plane& plane) const
{
	return Plane
	(
		plane.Normal.X * A00 + plane.Normal.Y * A10 + plane.Normal.Z * A20 + plane.Dist * A30,
		plane.Normal.X * A01 + plane.Normal.Y * A11 + plane.Normal.Z * A21 + plane.Dist * A31,
		plane.Normal.X * A02 + plane.Normal.Y * A12 + plane.Normal.Z * A22 + plane.Dist * A32,
		plane.Normal.X * A03 + plane.Normal.Y * A13 + plane.Normal.Z * A23 + plane.Dist * A33
	);
}

Matrix  Matrix::operator*(const Matrix& rhs) const
{
	Matrix r;

	r.A00 = A00 * rhs.A00 + A01 * rhs.A10 + A02 * rhs.A20 + A03 * rhs.A30;
	r.A01 = A00 * rhs.A01 + A01 * rhs.A11 + A02 * rhs.A21 + A03 * rhs.A31;
	r.A02 = A00 * rhs.A02 + A01 * rhs.A12 + A02 * rhs.A22 + A03 * rhs.A32;
	r.A03 = A00 * rhs.A03 + A01 * rhs.A13 + A02 * rhs.A23 + A03 * rhs.A33;

	r.A10 = A10 * rhs.A00 + A11 * rhs.A10 + A12 * rhs.A20 + A13 * rhs.A30;
	r.A11 = A10 * rhs.A01 + A11 * rhs.A11 + A12 * rhs.A21 + A13 * rhs.A31;
	r.A12 = A10 * rhs.A02 + A11 * rhs.A12 + A12 * rhs.A22 + A13 * rhs.A32;
	r.A13 = A10 * rhs.A03 + A11 * rhs.A13 + A12 * rhs.A23 + A13 * rhs.A33;

	r.A20 = A20 * rhs.A00 + A21 * rhs.A10 + A22 * rhs.A20 + A23 * rhs.A30;
	r.A21 = A20 * rhs.A01 + A21 * rhs.A11 + A22 * rhs.A21 + A23 * rhs.A31;
	r.A22 = A20 * rhs.A02 + A21 * rhs.A12 + A22 * rhs.A22 + A23 * rhs.A32;
	r.A23 = A20 * rhs.A03 + A21 * rhs.A13 + A22 * rhs.A23 + A23 * rhs.A33;

	r.A30 = A30 * rhs.A00 + A31 * rhs.A10 + A32 * rhs.A20 + A33 * rhs.A30;
	r.A31 = A30 * rhs.A01 + A31 * rhs.A11 + A32 * rhs.A21 + A33 * rhs.A31;
	r.A32 = A30 * rhs.A02 + A31 * rhs.A12 + A32 * rhs.A22 + A33 * rhs.A32;
	r.A33 = A30 * rhs.A03 + A31 * rhs.A13 + A32 * rhs.A23 + A33 * rhs.A33;

	return r;
}

Matrix& Matrix::operator*=(const Matrix& rhs)
{
	Matrix r;

	r.A00 = A00 * rhs.A00 + A01 * rhs.A10 + A02 * rhs.A20 + A03 * rhs.A30;
	r.A01 = A00 * rhs.A01 + A01 * rhs.A11 + A02 * rhs.A21 + A03 * rhs.A31;
	r.A02 = A00 * rhs.A02 + A01 * rhs.A12 + A02 * rhs.A22 + A03 * rhs.A32;
	r.A03 = A00 * rhs.A03 + A01 * rhs.A13 + A02 * rhs.A23 + A03 * rhs.A33;

	r.A10 = A10 * rhs.A00 + A11 * rhs.A10 + A12 * rhs.A20 + A13 * rhs.A30;
	r.A11 = A10 * rhs.A01 + A11 * rhs.A11 + A12 * rhs.A21 + A13 * rhs.A31;
	r.A12 = A10 * rhs.A02 + A11 * rhs.A12 + A12 * rhs.A22 + A13 * rhs.A32;
	r.A13 = A10 * rhs.A03 + A11 * rhs.A13 + A12 * rhs.A23 + A13 * rhs.A33;

	r.A20 = A20 * rhs.A00 + A21 * rhs.A10 + A22 * rhs.A20 + A23 * rhs.A30;
	r.A21 = A20 * rhs.A01 + A21 * rhs.A11 + A22 * rhs.A21 + A23 * rhs.A31;
	r.A22 = A20 * rhs.A02 + A21 * rhs.A12 + A22 * rhs.A22 + A23 * rhs.A32;
	r.A23 = A20 * rhs.A03 + A21 * rhs.A13 + A22 * rhs.A23 + A23 * rhs.A33;

	r.A30 = A30 * rhs.A00 + A31 * rhs.A10 + A32 * rhs.A20 + A33 * rhs.A30;
	r.A31 = A30 * rhs.A01 + A31 * rhs.A11 + A32 * rhs.A21 + A33 * rhs.A31;
	r.A32 = A30 * rhs.A02 + A31 * rhs.A12 + A32 * rhs.A22 + A33 * rhs.A32;
	r.A33 = A30 * rhs.A03 + A31 * rhs.A13 + A32 * rhs.A23 + A33 * rhs.A33;

    (*this)=r;
    return *this;
}

float Matrix::Determinant() const
{
    return	+A00 * (A11 * (A22 * A33 - A23 * A32) - A21 * (A12 * A33 - A13 * A32) + A31 * (A12 * A23 - A13 * A22))
		    -A10 * (A01 * (A22 * A33 - A23 * A32) - A21 * (A02 * A33 - A03 * A32) + A31 * (A02 * A23 - A03 * A22)) 
		    +A20 * (A01 * (A12 * A33 - A13 * A32) - A11 * (A02 * A33 - A03 * A32) + A31 * (A02 * A13 - A03 * A12)) 
		    -A30 * (A01 * (A12 * A23 - A13 * A22) - A11 * (A02 * A23 - A03 * A22) + A21 * (A02 * A13 - A03 * A12));
}

Matrix Matrix::Inverse() const
{
    const float det = Determinant();
    if(0.0f == det)
        return Matrix(Plane(1,0,0,0),Plane(0,1,0,0),Plane(0,0,1,0),Plane(0,0,0,1));

    const float rdet = 1.0f/det;
    Matrix r;

	r.A00 = +rdet * (A11 * (A22 * A33 - A23 * A32) - A21 * (A12 * A33 - A13 * A32) + A31 * (A12 * A23 - A13 * A22));
	r.A01 = -rdet * (A01 * (A22 * A33 - A23 * A32) - A21 * (A02 * A33 - A03 * A32) + A31 * (A02 * A23 - A03 * A22));
	r.A02 = +rdet * (A01 * (A12 * A33 - A13 * A32) - A11 * (A02 * A33 - A03 * A32) + A31 * (A02 * A13 - A03 * A12));
	r.A03 = -rdet * (A01 * (A12 * A23 - A13 * A22) - A11 * (A02 * A23 - A03 * A22) + A21 * (A02 * A13 - A03 * A12));
	r.A10 = -rdet * (A10 * (A22 * A33 - A23 * A32) - A20 * (A12 * A33 - A13 * A32) + A30 * (A12 * A23 - A13 * A22));
	r.A11 = +rdet * (A00 * (A22 * A33 - A23 * A32) - A20 * (A02 * A33 - A03 * A32) + A30 * (A02 * A23 - A03 * A22));
	r.A12 = -rdet * (A00 * (A12 * A33 - A13 * A32) - A10 * (A02 * A33 - A03 * A32) + A30 * (A02 * A13 - A03 * A12));
	r.A13 = +rdet * (A00 * (A12 * A23 - A13 * A22) - A10 * (A02 * A23 - A03 * A22) + A20 * (A02 * A13 - A03 * A12));
	r.A20 = +rdet * (A10 * (A21 * A33 - A23 * A31) - A20 * (A11 * A33 - A13 * A31) + A30 * (A11 * A23 - A13 * A21));
	r.A21 = -rdet * (A00 * (A21 * A33 - A23 * A31) - A20 * (A01 * A33 - A03 * A31) + A30 * (A01 * A23 - A03 * A21));
	r.A22 = +rdet * (A00 * (A11 * A33 - A13 * A31) - A10 * (A01 * A33 - A03 * A31) + A30 * (A01 * A13 - A03 * A11));
	r.A23 = -rdet * (A00 * (A11 * A23 - A13 * A21) - A10 * (A01 * A23 - A03 * A21) + A20 * (A01 * A13 - A03 * A11));
	r.A30 = -rdet * (A10 * (A21 * A32 - A22 * A31) - A20 * (A11 * A32 - A12 * A31) + A30 * (A11 * A22 - A12 * A21));
	r.A31 = +rdet * (A00 * (A21 * A32 - A22 * A31) - A20 * (A01 * A32 - A02 * A31) + A30 * (A01 * A22 - A02 * A21));
	r.A32 = -rdet * (A00 * (A11 * A32 - A12 * A31) - A10 * (A01 * A32 - A02 * A31) + A30 * (A01 * A12 - A02 * A11));
	r.A33 = +rdet * (A00 * (A11 * A22 - A12 * A21) - A10 * (A01 * A22 - A02 * A21) + A20 * (A01 * A12 - A02 * A11));

	return r;
}

inline Matrix& Matrix::Transpose()
{
	float tmp;

	tmp = A10;
	A10 = A01;
	A01 = tmp;

	tmp = A20;
	A20 = A02;
	A02 = tmp;

	tmp = A30;
	A30 = A03;
	A03 = tmp;

	tmp = A21;
	A21 = A12;
	A12 = tmp;

	tmp = A31;
	A31 = A13;
	A13 = tmp;

	tmp = A32;
	A32 = A23;
	A23 = tmp;

	return *this;
}

inline Matrix Matrix::GetTranspose() const
{
	return Matrix
	(
		Plane(A00, A10, A20, A30),
		Plane(A01, A11, A21, A31),
		Plane(A02, A12, A22, A32),
		Plane(A03, A13, A23, A33)
	);
}


Matrix Matrix::OrthoNormalInverse() const
{
	return Matrix
    (
		Plane(A00, A10, A20, 0),
		Plane(A01, A11, A21, 0),
		Plane(A02, A12, A22, 0),
		Plane(1.0f/A03, 1.0f/A13, 1.0f/A23, 1.0f)
    );
}

Vector Matrix::GetTranslation() const
{
    return Vector(A30,A31,A32);
}

void Matrix::SetTranslation(const Vector& vec)
{
    A30=vec.X;
    A31=vec.Y;
    A32=vec.Z;
}

Vector Matrix::GetScale() const
{
	return Vector
    (
		::sqrtf(A00*A00 + A01*A01 + A02*A02), 
		::sqrtf(A10*A10 + A11*A11 + A12*A12), 
		::sqrtf(A20*A20 + A21*A21 + A22*A22)
    );
}

Matrix Matrix::GetRotation() const
{
	Matrix m;

	m.X = X; 
	m.Y = Y; 
	m.Z = Z; 
	m.W = Plane(0,0,0,1);

	m.OrthoNormalize();

	return m;
}

void Matrix::OrthoNormalize()
{
	X.Normal.Normalize();
	Y.Normal.Normalize();
	Z.Normal = X.Normal ^ Y.Normal;
	Z.Normal.Normalize();
}

// ================================================================================

#define mi(w,t,i,j)	4*((i*w+j)*(1-t)+(j*w+i)*t)

#define flm(k,i,j,m,n,a,b) \
	__asm fld	dword ptr [edx + mi(m,a,i,k)] \
	__asm fmul	dword ptr [ecx + mi(n,b,k,j)]
	
#define e4(i,j,l,m,n,a,b) \
	flm(0,i,j,m,n,a,b) \
	flm(1,i,j,m,n,a,b) \
	flm(2,i,j,m,n,a,b) \
	flm(3,i,j,m,n,a,b) \
	__asm faddp	st(1), st(0) \
	__asm fxch	st(2) \
	__asm faddp	st(1), st(0) \
	__asm faddp	st(1), st(0) \
	__asm fstp	dword ptr [eax + mi(l,0,i,j)]

__forceinline void MatrixMult(const float* src1, const float* src2, float* dst)
{
	__asm mov edx, DWORD PTR src1
	__asm mov ecx, DWORD PTR src2
	__asm mov eax, DWORD PTR dst
	e4(0,0,4,4,4,0,0)
	e4(0,1,4,4,4,0,0)
	e4(0,2,4,4,4,0,0)
	e4(0,3,4,4,4,0,0)
	e4(1,0,4,4,4,0,0)
	e4(1,1,4,4,4,0,0)
	e4(1,2,4,4,4,0,0)
	e4(1,3,4,4,4,0,0)
	e4(2,0,4,4,4,0,0)
	e4(2,1,4,4,4,0,0)
	e4(2,2,4,4,4,0,0)
	e4(2,3,4,4,4,0,0)
	e4(3,0,4,4,4,0,0)
	e4(3,1,4,4,4,0,0)
	e4(3,2,4,4,4,0,0)
	e4(3,3,4,4,4,0,0)	
}

// ================================================================================

inline bool Matrix::ToEulerAnglesXYZ (Vector &ypr) const
{
    // rot =  cy*cz          -cy*sz           sy
    //        cz*sx*sy+cx*sz  cx*cz-sx*sy*sz -cy*sx
    //       -cx*cz*sy+sx*sz  cz*sx+cx*sy*sz  cx*cy

    if ( m4x4[0][2] < 1.0f )
    {
        if ( m4x4[0][2] > -1.0f )
        {
            ypr.X = atan2(-m4x4[1][2],m4x4[2][2]);
            ypr.Y = (float)asin(m4x4[0][2]);
            ypr.Z = atan2(-m4x4[0][1],m4x4[0][0]);
            return true;
        }
        else
        {
            // WARNING.  Not unique.  XA - ZA = -atan2(r10,r11)
            ypr.X = -atan2(m4x4[1][0],m4x4[1][1]);
            ypr.Y = -F_HALFPI;
            ypr.Z = 0.0f;
            return false;
        }
    }
    else
    {
        // WARNING.  Not unique.  XAngle + ZAngle = atan2(r10,r11)
        ypr.X = atan2(m4x4[1][0],m4x4[1][1]);
        ypr.Y = F_HALFPI;
        ypr.Z = 0.0f;
        return false;
    }
}

// ================================================================================

void Matrix::FromEulerAnglesXYZ (const Vector &ypr)
{
    float fCos, fSin;

    fCos = cosf(ypr.X);
    fSin = sinf(ypr.X);
    Matrix kXMat(1.0f,0.0f,0.0f,0.0f,fCos,-fSin,0.0f,fSin,fCos);

    fCos = cosf(ypr.Y);
    fSin = sinf(ypr.Y);
    Matrix kYMat(fCos,0.0f,fSin,0.0f,1.0f,0.0f,-fSin,0.0f,fCos);

    fCos = cosf(ypr.Z);
    fSin = sinf(ypr.Z);
    Matrix kZMat(fCos,-fSin,0.0f,fSin,fCos,0.0f,0.0f,0.0f,1.0f);

    *this = kXMat*(kYMat*kZMat);
}

// ================================================================================

#define MMult(a,b,c)	MatrixMult((const float*)(a),(const float*)(b),&(c).mArray[0])


