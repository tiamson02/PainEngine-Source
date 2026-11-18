
#include "Core/Math/LeMath.h"

class ENGINE_API Camera
{
#ifdef	TRANSGAMING_PS2	
	friend class PlayerController;
#endif
	
    float   Fov;        // degrees
    Vector  Position;
	Vector  Displacement;
	Vector  DisplacementRot;
    Vector  Rotation;
    Matrix  ViewMatrix;
	Matrix	CubemapViewMatrix; // this matrix stores original VM while rendering to cubemap

public:
	Matrix  AdditionalRotation;
	bool    Interpolation;

    Camera()
    :    Fov(90.f), Interpolation(false), Position(Vector(0,0,0)), Displacement(Vector(0,0,0)), DisplacementRot(Vector(0,0,0)), Rotation(Vector(0,0,0))
    {
		AdditionalRotation.SetIdentity();   // = GIdentityMatrix;
	}

    inline void		InverseTransform();
	inline void		LookAt(const Vector &at);
	inline Vector	GetForwardVector() const;
	inline Vector	GetRightVector() const;
	inline Vector	GetUpVector() const;
	inline void		UpdateCubemapViewMatrix() { CubemapViewMatrix = ViewMatrix;	}
	inline Vector	GetCubemapForwardVector() const;
	inline Vector	GetCubemapRightVector() const;
	inline Vector	GetCubemapUpVector() const;
	inline Matrix   WorldToCamera() const;
	inline Matrix   RawWorldToCamera() const;

	void	SetFOV(float fov)					{ Fov = fov; }
	void	SetPosition(const Vector& pos)		{ Position = pos; }
	void	SetRotation(const Vector& rot)		{ Rotation = rot; }
	void	SetDispPosition(const Vector& dpos)	{ Displacement = dpos; }
	void	SetDispRotation(const Vector& drot)	{ DisplacementRot = drot; }
	void	SetViewMatrix(const Matrix& mat)	
	{ 
		ViewMatrix = mat; 
		/* not tested
		Quaternion rot;
		Vector scale;
		ViewMatrix.Decompose(scale,rot,Position);
		rot.ToEulerAnglesXYZ(Rotation);
		Rotation.X = FDeg2Rad(Rotation.X);		
		Rotation.Y = FDeg2Rad(Rotation.Y);		
		Rotation.Z = FDeg2Rad(Rotation.Z)'*/
	}

	const float  GetFOV() const			{ return Fov; }
	const Vector GetPosition() const	{ return Position+Displacement; }
	const Vector GetRotation() const	{ return Rotation+DisplacementRot; }

	const Vector GetRawPosition() const	{ return Position; }
	const Vector GetRawRotation() const	{ return Rotation; }
	const Matrix GetViewMatrix() const	{ return ViewMatrix; }
};

// ================================================================

inline Matrix Camera::WorldToCamera() const 
{
	Vector rot = GetRotation();
	if( rot.Z != 0.0f )
		return XRotationMatrix(-FDeg2Rad(rot.Y))
			  *ZRotationMatrix(-FDeg2Rad(rot.Z))
			  *YRotationMatrix(-FDeg2Rad(rot.X))
			  *AdditionalRotation
	          *TranslationMatrix(GetPosition());	
	else
		return XRotationMatrix(-FDeg2Rad(rot.Y))
			  *YRotationMatrix(-FDeg2Rad(rot.X))
			  *AdditionalRotation
	          *TranslationMatrix(GetPosition());	
}

inline Matrix Camera::RawWorldToCamera() const 
{
	Vector rot = GetRawRotation();
	if( rot.Z != 0.0f )
		return XRotationMatrix(-FDeg2Rad(rot.Y))
			  *ZRotationMatrix(-FDeg2Rad(rot.Z))
			  *YRotationMatrix(-FDeg2Rad(rot.X))
	          *TranslationMatrix(GetRawPosition());	
	else
		return XRotationMatrix(-FDeg2Rad(rot.Y))
			  *YRotationMatrix(-FDeg2Rad(rot.X))
	          *TranslationMatrix(GetRawPosition());	
}

inline void Camera::InverseTransform()
{
	Vector rot = GetRotation();
	ViewMatrix = TranslationMatrix(-GetPosition())
              *YRotationMatrix(FDeg2Rad(rot.X))
              *ZRotationMatrix(FDeg2Rad(rot.Z))
              *XRotationMatrix(FDeg2Rad(rot.Y))
			  *ScaleMatrix(Vector(1,1,-1));
}


inline void Camera::LookAt(const Vector &at)
{
	ViewMatrix = CameraLookAtMatrix(GetPosition(),at,Vector(0,1,0));
}

// util functions
inline Vector Camera::GetForwardVector() const
{
	return Vector(ViewMatrix.A02,
                  ViewMatrix.A12,
                  ViewMatrix.A22);
}

inline Vector Camera::GetRightVector() const
{
	return Vector(ViewMatrix.A00,
                  ViewMatrix.A10,
                  ViewMatrix.A20);
}

inline Vector Camera::GetUpVector() const
{
	return Vector(ViewMatrix.A01,
                  ViewMatrix.A11,
                  ViewMatrix.A21);
}


// cubemap util functions
inline Vector Camera::GetCubemapForwardVector() const
{
	return Vector(CubemapViewMatrix.A02,
				  CubemapViewMatrix.A12,
				  CubemapViewMatrix.A22);
}

inline Vector Camera::GetCubemapRightVector() const
{
	return Vector(CubemapViewMatrix.A00,
				  CubemapViewMatrix.A10,
				  CubemapViewMatrix.A20);
}

inline Vector Camera::GetCubemapUpVector() const
{
	return Vector(CubemapViewMatrix.A01,
				  CubemapViewMatrix.A11,
				  CubemapViewMatrix.A21);
}

