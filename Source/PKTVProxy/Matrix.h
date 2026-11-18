// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/04 Bart Sekura
//----------------------------------------------------------------------------

class Matrix
{
public:
	union {
		struct {
			Plane	X;
			Plane	Y;
			Plane	Z;
			Plane	W;
		};
		struct
		{
			float	A00,A01,A02,A03;
			float	A10,A11,A12,A13;
			float	A20,A21,A22,A23;
			float	A30,A31,A32,A33;
		};

		float mArray[16];
		float m4x4[4][4];
	};

	Matrix() {}
	inline Matrix(const Matrix& mat);
    inline Matrix(const Plane& x, const Plane& y, const Plane& z, const Plane& w);
    inline Matrix(const float* ptr);
	inline Matrix(Vector &scale, Quaternion &rotation, Vector &translation);
	inline Matrix(float A00, float A01, float A02, 
				  float A10, float A11, float A12, 
				  float A20, float A21, float A22);

    // conversion to float*
    inline operator const float*(void) const;
	inline const float& operator()(const int &i, const int &j) const;
	inline float& operator()(const int &i, const int &j);

	// homogenous transform
	inline Vector Transform(const Vector& vec) const;
	inline Vector TransformNormal(const Vector& vec) const;
	inline Vector TransformCoord(const Vector& vec) const;
	inline Plane  TransformPlane(const Plane& plane) const;
	inline Plane  TransformVec4(const Plane& plane) const;
    inline Matrix  operator*(const Matrix& rhs) const;
    inline Matrix& operator*=(const Matrix& rhs);
    inline float Determinant() const;

	inline Matrix GetTranspose() const;
	inline Matrix& Transpose();

    inline Matrix Inverse() const;
	inline Matrix OrthoNormalInverse() const;

    inline Vector GetTranslation() const;
 	inline Vector GetScale() const;
 	inline Matrix GetRotation() const;
	inline void Decompose(Vector &scale, Quaternion &rotation, Vector &translation) const;
	
    inline void SetTranslation(const Vector& vec);

    inline bool ToEulerAnglesXYZ (Vector &ypr) const;
    inline void FromEulerAnglesXYZ (const Vector &ypr);

	inline void OrthoNormalize();
 	inline friend FileObject& operator<<(FileObject& file, const Matrix& mat);
  	inline friend FileObject& operator>>(FileObject& file, Matrix& mat);
};


// ====================================================================
// util matrices

class IdentityMatrix : public Matrix
{
public:
    IdentityMatrix()
    :   Matrix
        (
	        Plane(1.f,0.f,0.f,0.f),
		    Plane(0.f,1.f,0.f,0.f),
		    Plane(0.f,0.f,1.f,0.f),
		    Plane(0.f,0.f,0.f,1.f)
        )
    {}
};

class ProjectionMatrix : public Matrix
{
public:

	ProjectionMatrix(float fov, float fx, float fy, float minz, float maxz) 
    {
        const float t=tanf(fov);
        const float d=maxz/(maxz-minz);
        
        X=Plane(1.0f/t,  0.0f,    0.0f,    0.0f);
        Y=Plane(0.0f,    fx/t/fy, 0.0f,    0.0f);
        Z=Plane(0.0f,    0.0f,    d,       1.0f);
        W=Plane(0.0f,    0.0f,    -minz*d, 0.0f);
    }
};

class OrthoMatrix : public Matrix
{
public:

	OrthoMatrix(float l, float r, float b, float t, float n, float f) 
    {
		float w = r-l;
		float h = t-b;
		float d = f-n;
        
        X=Plane(2.0f/w,  0.0f,         0.0f,0.0f);
        Y=Plane(0.0f,  2.0f/h,         0.0f,0.0f);
        Z=Plane(0.0f,    0.0f,      -2.0f/d,0.0f);
        W=Plane(-(r+l)/w,-(t+b)/h,-(f+n)/d,1.0f);
    }
};

class ProjectionMatrixOGL : public Matrix
{
public:
	ProjectionMatrixOGL(float fovy, float w, float h, float n, float f)
	{
		float xmin, xmax, ymin, ymax;

		ymax = n * tanf(FDeg2Rad(fovy) * 0.5f);
		ymin = -ymax;

		float aspect = w/h;

		xmin = ymin * aspect;
		xmax = ymax * aspect;

		A00 = (2*n) / (xmax-xmin);
		A01 = 0.0;
		A02 = 0.0;
		A03 = 0.0;

		A10 = 0.0;
		A11 = (2*n) / (ymax-ymin);
		A12 = 0.0;
		A13 = 0.0;

		A20 = (xmax+xmin) / (xmax-xmin);
		A21 = (ymax+ymin) / (ymax-ymin);
		A22 = -(f+n) / (f-n);
		A23 = -1;

		A30 = 0.0;
		A31 = 0.0;
		A32 = -(2*f*n) / (f-n);
		A33 = 0.0;
	}
};

class XRotationMatrix : public Matrix
{
public:
    XRotationMatrix(float f)
    {
        const float Cos=GCosFloat(f);
        const float Sin=GSinFloat(f);

        X=Plane(1.0f, 0.0f, 0.0f, 0.0f);
        Y=Plane(0.0f, +Cos, +Sin, 0.0f);
        Z=Plane(0.0f, -Sin, +Cos, 0.0f);
        W=Plane(0.0f, 0.0f, 0.0f, 1.0f);
    }
};

class YRotationMatrix : public Matrix
{
public:
    YRotationMatrix(float f)
    {
        const float Cos=GCosFloat(f);
        const float Sin=GSinFloat(f);

        X=Plane(+Cos, 0.0f, -Sin, 0.0f);
        Y=Plane(0.0f, 1.0f, 0.0f, 0.0f);
        Z=Plane(+Sin, 0.0f, +Cos, 0.0f);
        W=Plane(0.0f, 0.0f, 0.0f, 1.0f);
    }
};

class ZRotationMatrix : public Matrix
{
public:
    ZRotationMatrix(float f)
    {
        const float Cos=GCosFloat(f);
        const float Sin=GSinFloat(f);

        X=Plane(+Cos, +Sin, 0.0f, 0.0f);
        Y=Plane(-Sin, +Cos, 0.0f, 0.0f);
        Z=Plane(0.0f, 0.0f, 1.0f, 0.0f);
        W=Plane(0.0f, 0.0f, 0.0f, 1.0f);
    }
};

class NormalXRotationMatrix : public Matrix
{
public:
    NormalXRotationMatrix (const Vector &N)
    {
		Vector T = N.MinimalAxis()^N;
		T.Normalize();
		Vector B = N^T;
		B.Normalize();

		X=Plane(N,0);
		Y=Plane(T,0);
		Z=Plane(B,0);
        W=Plane(0.0f, 0.0f, 0.0f, 1.0f);
    }
};

class NormalYRotationMatrix : public Matrix
{
public:
    NormalYRotationMatrix (const Vector &N)
    {
		Vector T = N.MinimalAxis()^N;
		T.Normalize();
		Vector B = N^T;
		B.Normalize();

		X=Plane(B,0);
		Y=Plane(N,0);
		Z=Plane(T,0);
        W=Plane(0.0f, 0.0f, 0.0f, 1.0f);
    }
};

class NormalZRotationMatrix : public Matrix
{
public:
    NormalZRotationMatrix (const Vector &N)
    {
		Vector T = N.MinimalAxis()^N;
		T.Normalize();
		Vector B = N^T;
		B.Normalize();

		X=Plane(T,0); 
		Y=Plane(B,0); 
		Z=Plane(N,0); 
        W=Plane(0.0f, 0.0f, 0.0f, 1.0f);
    }
};

class TranslationMatrix : public Matrix
{
public:
    TranslationMatrix(const Vector& v)
    :   Matrix
        (
            Plane(1.0f, 0.0f, 0.0f, 0.0f),
            Plane(0.0f, 1.0f, 0.0f, 0.0f),
            Plane(0.0f, 0.0f, 1.0f, 0.0f),
            Plane(v.X,  v.Y,  v.Z,  1.0f)
        )
    {}
};

class TexturePanMatrix : public Matrix
{
public:
    TexturePanMatrix(float PanX, float PanY)
    :   Matrix
        (
            Plane(1.0f, 0.0f, 0.0f, 0.0f),
            Plane(0.0f, 1.0f, 0.0f, 0.0f),
            Plane(0.0f, 0.0f, 1.0f, 0.0f),
            Plane(PanX, PanY, 0.0f, 1.0f)
        )
    {}
};

class TextureRotMatrix : public Matrix
{
public:
    TextureRotMatrix(float rot)
	{
		// TODO: optimize it
		Matrix m = TexturePanMatrix(-.5f,-.5f) * ZRotationMatrix(rot) * TexturePanMatrix(.5f,.5f);
		X = m.X;
		Y = m.Y;
		Z = m.Z;
		W = m.W;
    }
};

class ScaleMatrix : public Matrix
{
public:
    ScaleMatrix(const Vector& v)
    :   Matrix
        (
            Plane(v.X,  0.0f, 0.0f, 0.0f),
            Plane(0.0f, v.Y,  0.0f, 0.0f),
            Plane(0.0f, 0.0f, v.Z,  0.0f),
            Plane(0.0f, 0.0f, 0.0f, 1.0f)
        )
    {}    
    ScaleMatrix(float f)
    :   Matrix
        (
            Plane(f   , 0.0f, 0.0f, 0.0f),
            Plane(0.0f, f   , 0.0f, 0.0f),
            Plane(0.0f, 0.0f, f   , 0.0f),
            Plane(0.0f, 0.0f, 0.0f, 1.0f)
        )
    {}    
};

class ReflectionMatrix : public Matrix
{
public:
	ReflectionMatrix(const Plane& P)
	{
		X = Plane(-2 * P.X * P.X + 1, -2 * P.Y * P.X    , -2 * P.Z * P.X    ,   0);
		Y = Plane(-2 * P.X * P.Y    , -2 * P.Y * P.Y + 1, -2 * P.Z * P.Y    ,   0);
		Z = Plane(-2 * P.X * P.Z    , -2 * P.Y * P.Z    , -2 * P.Z * P.Z + 1,   0);
		W = Plane(-2 * P.X * P.W    , -2 * P.Y * P.W    , -2 * P.Z * P.W    ,   1);
	}
};

class LookAtMatrix : public Matrix
{
public:
    LookAtMatrix(const Vector& eye,
                 const Vector& at,
                 const Vector& up)
    {
        Vector zaxis=at-eye;   zaxis.Normalize();
        Vector xaxis=up^zaxis; xaxis.Normalize();
        Vector yaxis=zaxis^xaxis;

        X=Plane(xaxis, 0.f);
        Y=Plane(yaxis, 0.f);
        Z=Plane(zaxis, 0.f);
		W=Plane(eye, 1.f);
    }    
};


class CameraLookAtMatrix : public Matrix
{
public:
    CameraLookAtMatrix(const Vector& eye,
        const Vector& at,
        const Vector& up)
    {
        Vector zaxis=eye-at;   zaxis.Normalize();
        Vector xaxis=up^zaxis; xaxis.Normalize();
        Vector yaxis=zaxis^xaxis;

        X=Plane(xaxis.X, yaxis.X, zaxis.X, 0.f);
        Y=Plane(xaxis.Y, yaxis.Y, zaxis.Y, 0.f);
        Z=Plane(xaxis.Z, yaxis.Z, zaxis.Z, 0.f);
        W=Plane(-xaxis.Dot(eye), -yaxis.Dot(eye), -zaxis.Dot(eye), 1.f);
        *this *= ScaleMatrix(Vector(1,1,-1));
    }    
};

class RotationMatrix : public Matrix
{
public:
	RotationMatrix(float angle, float x, float y, float z)
	{

		float xx, yy, zz, xy, yz, zx, xs, ys, zs, one_c, s, c;
		bool optimized;

		s = GSinFloat(angle);
		c = GCosFloat(angle);

	    X = Plane(1.f,0.f,0.f,0.f);
		Y = Plane(0.f,1.f,0.f,0.f);
		Z = Plane(0.f,0.f,1.f,0.f);
		W = Plane(0.f,0.f,0.f,1.f);

		optimized = false;

	#define M(row,col)  mArray[col*4+row]

		if (x == 0.0f) {
			if (y == 0.0F) {
				if (z != 0.0F) {
					optimized = true;
					/* rotate only around z-axis */
					M(0,0) = c;
					M(1,1) = c;
					if (z < 0.0f) {
						M(0,1) = s;
						M(1,0) = -s;
					}
					else {
						M(0,1) = -s;
						M(1,0) = s;
					}
				}
			}
			else if (z == 0.0f) {
				optimized = true;
				/* rotate only around y-axis */
				M(0,0) = c;
				M(2,2) = c;
				if (y < 0.0f) {
					M(0,2) = -s;
					M(2,0) = s;
				}
				else {
					M(0,2) = s;
					M(2,0) = -s;
				}
			}
		}
		else if (y == 0.0f) {
			if (z == 0.0f) {
				optimized = true;
				/* rotate only around x-axis */
				M(1,1) = c;
				M(2,2) = c;
				if (y < 0.0f) {
					M(1,2) = s;
					M(2,1) = -s;
				}
				else {
					M(1,2) = -s;
					M(2,1) = s;
				}
			}
		}

		if (!optimized) {
			const float mag = (float) sqrt(x * x + y * y + z * z);

			if (mag <= 1.0e-4f) {
				/* no rotation, leave mat as-is */
				return;
			}

			x /= mag;
			y /= mag;
			z /= mag;


			/*
			*     Arbitrary axis rotation matrix.
			*
			*  This is composed of 5 matrices, Rz, Ry, T, Ry', Rz', multiplied
			*  like so:  Rz * Ry * T * Ry' * Rz'.  T is the final rotation
			*  (which is about the X-axis), and the two composite transforms
			*  Ry' * Rz' and Rz * Ry are (respectively) the rotations necessary
			*  from the arbitrary axis to the X-axis then back.  They are
			*  all elementary rotations.
			*
			*  Rz' is a rotation about the Z-axis, to bring the axis vector
			*  into the x-z plane.  Then Ry' is applied, rotating about the
			*  Y-axis to bring the axis vector parallel with the X-axis.  The
			*  rotation about the X-axis is then performed.  Ry and Rz are
			*  simply the respective inverse transforms to bring the arbitrary
			*  axis back to it's original orientation.  The first transforms
			*  Rz' and Ry' are considered inverses, since the data from the
			*  arbitrary axis gives you info on how to get to it, not how
			*  to get away from it, and an inverse must be applied.
			*
			*  The basic calculation used is to recognize that the arbitrary
			*  axis vector (x, y, z), since it is of unit length, actually
			*  represents the sines and cosines of the angles to rotate the
			*  X-axis to the same orientation, with theta being the angle about
			*  Z and phi the angle about Y (in the order described above)
			*  as follows:
			*
			*  cos ( theta ) = x / sqrt ( 1 - z^2 )
			*  sin ( theta ) = y / sqrt ( 1 - z^2 )
			*
			*  cos ( phi ) = sqrt ( 1 - z^2 )
			*  sin ( phi ) = z
			*
			*  Note that cos ( phi ) can further be inserted to the above
			*  formulas:
			*
			*  cos ( theta ) = x / cos ( phi )
			*  sin ( theta ) = y / sin ( phi )
			*
			*  ...etc.  Because of those relations and the standard trigonometric
			*  relations, it is pssible to reduce the transforms down to what
			*  is used below.  It may be that any primary axis chosen will give the
			*  same results (modulo a sign convention) using thie method.
			*
			*  Particularly nice is to notice that all divisions that might
			*  have caused trouble when parallel to certain planes or
			*  axis go away with care paid to reducing the expressions.
			*  After checking, it does perform correctly under all cases, since
			*  in all the cases of division where the denominator would have
			*  been zero, the numerator would have been zero as well, giving
			*  the expected result.
			*/

			xx = x * x;
			yy = y * y;
			zz = z * z;
			xy = x * y;
			yz = y * z;
			zx = z * x;
			xs = x * s;
			ys = y * s;
			zs = z * s;
			one_c = 1.0f - c;

			/* We already hold the identity-matrix so we can skip some statements */
			M(0,0) = (one_c * xx) + c;
			M(0,1) = (one_c * xy) - zs;
			M(0,2) = (one_c * zx) + ys;
			//M(0,3) = 0.0f;

			M(1,0) = (one_c * xy) + zs;
			M(1,1) = (one_c * yy) + c;
			M(1,2) = (one_c * yz) - xs;
			//M(1,3) = 0.0f;

			M(2,0) = (one_c * zx) - ys;
			M(2,1) = (one_c * yz) + xs;
			M(2,2) = (one_c * zz) + c;
			//M(2,3) = 0.0f;

			/*
			M(3,0) = 0.0f;
			M(3,1) = 0.0f;
			M(3,2) = 0.0f;
			M(3,3) = 1.0f;
			*/
		}
	#undef M
	}

	/*
	RotationMatrix(const Vector &ypr)
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
	*/
};
