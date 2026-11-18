#include "Engine.h"
#include "Render/Light.h"
#include "Engine/PCFSystem.h"
extern ENGINE_API PCFSystem* GEngine;
#include "Engine/World.h"
#include "Render/Video.h"
#include "Render/Material.h"
#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;
#include "Render/View.h"

// ==========================================================================

class CStaticLight
{
public:
	struct ParticleVertex
	{
		Vector	Pos;
		float	Index;
		ParticleVertex()
		{
		}
		ParticleVertex(const Vector& pos, const float& ind) : Pos(pos), Index(ind)
		{			
		}
	};

	int Buf;
	int BufSize;
	static const int num = 400*6;
	ParticleVertex Pos[num];

	CStaticLight() : Buf(0)
	{
		RenderDevice* dev=GEngine->GetRenderDevice();

		for (int i=0; i<num; i+=6)
		{
			float
				x = FloatRand(),
				y = FloatRand(),
				z = FloatRand();
			Pos[i+0]=ParticleVertex(Vector(x,y,z),1);
			Pos[i+1]=ParticleVertex(Vector(x,y,z),3);
			Pos[i+2]=ParticleVertex(Vector(x,y,z),2);

			Pos[i+3]=ParticleVertex(Vector(x,y,z),1);
			Pos[i+4]=ParticleVertex(Vector(x,y,z),2);
			Pos[i+5]=ParticleVertex(Vector(x,y,z),0);
		}
/*
		int w = 16;
		int i=0;

		const int num = 16*16*16;
		SimpleVertex Pos[num];
		for (int x=0; x<w; x++)
		for (int y=0; y<w; y++)
		for (int z=0; z<w; z++)
		{
			Pos[i++] = SimpleVertex(1.0f*x/w,1.0f*y/w,1.0f*z/w,0,0,0);
		}
*/
		Buf = dev->CreateStream(&Pos[0],0,num,sizeof(ParticleVertex));
		BufSize = num/3;
	}
};

static CStaticLight *StaticLight = 0;

Light::Light()
:   Entity(Entity::ETypeLight), LightType(ELightNone)
,	Radius(8), StartFalloff(4), Color(1,1,1), Direction(0,-1,0), Intensity(1), ConeAngleCos(0), FalloffAngleCos(0)
,	EnvRefCount(0), Dindex(-1), Projector(0), Volumetric(false), ImportantDynamic(false)
{
	LocalBox.Reset();
	LocalBox.Expand(Radius);
	SpotFrustum.PlaneCount=6;

	Box=LocalBox;
	Sphere.SetRadius(Radius);

	Flags |= Entity::EEntNoDraw;
	Sort = Entity::ESortTrans;
}

Light::~Light()
{
#ifdef	TRANSGAMING
	if ((Flags & ELightDynamic) == ELightDynamic)
	{
		Assert(Dindex!=-1);
		GEngine->pcfWorld->DLights[Dindex] = 0;
	}
#else
	if ((Flags & ELightDynamic) == ELightDynamic)
	{
		Assert(Dindex!=-1);
		GEngine->World->DLights[Dindex] = 0;
	}
#endif

	if (GEngine->Video)
	{
		MaterialSystem* matSys = GEngine->Video->MaterialSys;
		matSys->ReleaseTexture(Projector);
	}
}

void Light::SetProjectorTexture(const char *tex)
{
    if (tex == NULL || ProjectorName.AsChar() == NULL)
        return;


	if (strcmp(tex, ProjectorName.AsChar()))
	{
	    MaterialSystem* matSys = GEngine->Video->MaterialSys;
		ProjectorName = tex;

		matSys->ReleaseTexture(Projector);
		Projector = 0;

		if (ProjectorName.Len())
		{
			Projector = matSys->CreateTexture(ProjectorName.AsChar());
		}
	}
}

void Light::SetDirection(const Vector& vec)
{
    //TRACE("Light::SetDirection(): changing light direction to <%g, %g, %g>\n", vec.X, vec.Y, vec.Z);
    /*if (vec.X == 0 && vec.Y == 0 && vec.Z == 0){
        TRACE("Light::SetDirection(): AHHH!!! Invalid light direction!\n");
    }//*/

	Direction = vec;
}

void Light::SetConeAngleCos(float angle)
{
	ConeAngleCos = angle;
}

void Light::SetImportantDynamic(bool en)
{
	ImportantDynamic = en;
}


void Light::EnableDynamic(bool en)
{
	if (en == ((Flags & ELightDynamic) == ELightDynamic))
		return;

#ifdef	TRANSGAMING
	if (en)
	{
		Flags |= ELightDynamic;
		Dindex = SlotAdd(GEngine->pcfWorld->DLights,this);
	}
	else 
	{
		Assert(Dindex!=-1);
		Flags &= ~ELightDynamic;
		GEngine->pcfWorld->DLights[Dindex] = 0;
		Dindex = -1;
	}    
#else
	if (en)
	{
		Flags |= ELightDynamic;
		Dindex = SlotAdd(GEngine->World->DLights,this);
	}
	else 
	{
		Assert(Dindex!=-1);
		Flags &= ~ELightDynamic;
		GEngine->World->DLights[Dindex] = 0;
		Dindex = -1;
	}    
#endif
}

void Light::EnableFakeSpecular(bool en)
{
	if (en)
		Flags |= ELightFakeSpecular;
	else
		Flags &= ~ELightFakeSpecular;
}

void Light::UpdateProj()
{
	float angle = acosf(ConeAngleCos);
    //TRACE("Light::UpdateProj(): updating projection for Translation = <%g, %g, %g>, Direction = <%g, %g, %g>\n", Translation.X, Translation.Y, Translation.Z, Direction.X, Direction.Y, Direction.Z);
	ProjMatrix = CameraLookAtMatrix(Translation, Translation+Direction, Vector(0,1,0));
	ProjMatrix *= ProjectionMatrix(angle,1,1,.1f,Radius);
	//ProjMatrix *= ScaleMatrix(Vector(Radius*cosf(ConeAngleCos),Radius*cosf(ConeAngleCos),Radius));
	//ProjMatrix *= ScaleMatrix(Vector(0.1,0.1,0.1));


    // Left clipping plane
    SpotFrustum.BoundingPlanes[0].X = -ProjMatrix.A03 - ProjMatrix.A00;
    SpotFrustum.BoundingPlanes[0].Y = -ProjMatrix.A13 - ProjMatrix.A10;
    SpotFrustum.BoundingPlanes[0].Z = -ProjMatrix.A23 - ProjMatrix.A20;
    SpotFrustum.BoundingPlanes[0].W = +ProjMatrix.A33 + ProjMatrix.A30;
	SpotFrustum.BoundingPlanes[0].Normalize();

    // Right clipping plane
    SpotFrustum.BoundingPlanes[1].X = -ProjMatrix.A03 + ProjMatrix.A00;
    SpotFrustum.BoundingPlanes[1].Y = -ProjMatrix.A13 + ProjMatrix.A10;
    SpotFrustum.BoundingPlanes[1].Z = -ProjMatrix.A23 + ProjMatrix.A20;
    SpotFrustum.BoundingPlanes[1].W = +ProjMatrix.A33 - ProjMatrix.A30;
	SpotFrustum.BoundingPlanes[1].Normalize();

    // Top clipping plane
    SpotFrustum.BoundingPlanes[2].X = -ProjMatrix.A03 + ProjMatrix.A01;
    SpotFrustum.BoundingPlanes[2].Y = -ProjMatrix.A13 + ProjMatrix.A11;
    SpotFrustum.BoundingPlanes[2].Z = -ProjMatrix.A23 + ProjMatrix.A21;
    SpotFrustum.BoundingPlanes[2].W = +ProjMatrix.A33 - ProjMatrix.A31;
	SpotFrustum.BoundingPlanes[2].Normalize();

    // Bottom clipping plane
    SpotFrustum.BoundingPlanes[3].X = -ProjMatrix.A03 - ProjMatrix.A01;
    SpotFrustum.BoundingPlanes[3].Y = -ProjMatrix.A13 - ProjMatrix.A11;
    SpotFrustum.BoundingPlanes[3].Z = -ProjMatrix.A23 - ProjMatrix.A21;
    SpotFrustum.BoundingPlanes[3].W = +ProjMatrix.A33 + ProjMatrix.A31;
	SpotFrustum.BoundingPlanes[3].Normalize();

    // Near clipping plane
    SpotFrustum.BoundingPlanes[4].X = -ProjMatrix.A02;
    SpotFrustum.BoundingPlanes[4].Y = -ProjMatrix.A12;
    SpotFrustum.BoundingPlanes[4].Z = -ProjMatrix.A22;
	SpotFrustum.BoundingPlanes[4].W = +ProjMatrix.A32;
	SpotFrustum.BoundingPlanes[4].Normalize();

	// far clipping planes
    SpotFrustum.BoundingPlanes[5].X = -ProjMatrix.A03 + ProjMatrix.A02;
    SpotFrustum.BoundingPlanes[5].Y = -ProjMatrix.A13 + ProjMatrix.A12;
    SpotFrustum.BoundingPlanes[5].Z = -ProjMatrix.A23 + ProjMatrix.A22;
	SpotFrustum.BoundingPlanes[5].W = +ProjMatrix.A33 - ProjMatrix.A32;
	SpotFrustum.BoundingPlanes[5].Normalize();

	BoundingBox box;

	for (int p0=0; p0<=1; p0++)
		for (int p1=2; p1<=3; p1++)
			for (int p2=4; p2<=5; p2++)
			{
				Vector isect;
				const Vector &N0 = SpotFrustum.BoundingPlanes[p0].Normal;
				const Vector &N1 = SpotFrustum.BoundingPlanes[p1].Normal;
				const Vector &N2 = SpotFrustum.BoundingPlanes[p2].Normal;
				float d0 = SpotFrustum.BoundingPlanes[p0].Dist;
				float d1 = SpotFrustum.BoundingPlanes[p1].Dist;
				float d2 = SpotFrustum.BoundingPlanes[p2].Dist;

				float denominator = N0.Dot(N1^N2);

				if (fabs(denominator)<F_EPS)
					continue; // the planes are parellel

				Vector P = ((N1^N2)*d0 + (N2^N0)*d1 + (N0^N1)*d2) / denominator;
				box += P;
			}

	LocalBox = box;
	LocalBox.Min -= Translation;
	LocalBox.Max -= Translation;

	// final matrix - including texture scale+pan
	ProjMatrix = ProjMatrix
			   * ScaleMatrix(Vector(0.5f,-0.5f,0))
			   * TranslationMatrix(Vector(0.5f,0.5f,1));
}

void Light::UpdateTransform(bool updateInWorld)
{
	if (LightType == ELightSpot)
		UpdateProj();
	else
	{
		LocalBox.Reset();
		LocalBox.Expand(Radius);
	}
	Entity::UpdateTransform(updateInWorld);
	Sphere.Center = GetWorldPosition();	
}

void Light::SetRadius(const float radius)
{
	Radius = radius;
	Sphere.SetRadius(radius);
	UpdateTransform(true);
}

void Light::SetFalloff(const float startFalloff)
{
	StartFalloff = startFalloff;
}

void Light::SetType(int type)
{
	LightType = type;
	
	//Volumetric = (type == ELightSpot);

	if (Volumetric)
	{
        if (!StaticLight){
            m_setTag("Light");
			StaticLight = new CStaticLight;
        }

		Flags &= ~Entity::EEntNoDraw;
	}
	else
	{
		Flags |= Entity::EEntNoDraw;
	}
		
}

BOOL Light::Intersect(const BoundingBox& box)
{
	if (LightType == ELightDir)
		return TRUE;

	if (Sphere.Intersect(box))
	{
		if (LightType == ELightSpot)
		{
			return SpotFrustum.BoxTest(box.GetCenter(), box.GetExtent());
		}
		return TRUE;
	}

	return FALSE;
}

void Light::GetDirAndAttCol(const Entity* entity, Plane& lightDir, Plane& lightCol) const
{
	if (LightType == ELightDir)
	{
		lightDir = -Direction;
#ifdef	TRANSGAMING
		lightCol = Plane(Vector(BaseVector(Color))*Intensity, Intensity);
#else
		lightCol = Plane(Vector(Color)*Intensity, Intensity);
#endif
	}
	else if (LightType == ELightPoint)
	{
		lightDir = GetWorldPosition() - (entity->GetCenter());
		float dist = lightDir.Normalize();
		float objAtt = 1;

		if (dist>Radius) 
			objAtt = 0;
		else if (dist>StartFalloff) 
			objAtt = (dist-Radius)/(StartFalloff-Radius);

#ifdef	TRANSGAMING
		lightCol = Plane(Vector(BaseVector(Color))*objAtt*Intensity, objAtt*Intensity);
#else
		lightCol = Plane(Vector(Color)*objAtt*Intensity, objAtt*Intensity);
#endif
	}
	else if (LightType == ELightSpot)
	{
		lightDir = GetWorldPosition() - entity->GetCenter();
		float dist = lightDir.Normal.Normalize();
		float dot = -lightDir.Normal.Dot(Direction);
		float objAtt;

		if (dot<ConeAngleCos)
			objAtt = 0;
		else if (dot>FalloffAngleCos)
			objAtt = 1;
		else 
			objAtt = (ConeAngleCos-dot)/(ConeAngleCos-FalloffAngleCos);

		if (dist>Radius) 
			objAtt = 0;
		else if (dist>StartFalloff) 
			objAtt = objAtt*(dist-Radius)/(StartFalloff-Radius);

#ifdef	TRANSGAMING
		lightCol = Plane(Vector(BaseVector(Color))*objAtt*Intensity, objAtt*Intensity);
#else
		lightCol = Plane(Vector(Color)*objAtt*Intensity, objAtt*Intensity);
#endif
	}
}

float Light::GetAttIntensity(const Entity* entity) const
{
	if (LightType == ELightDir)
	{
		 return Intensity;
	}
	else if (LightType == ELightPoint)
	{
		Vector lightDir = GetWorldPosition() - entity->GetCenter();
		float dist = lightDir.Normalize();
		float objAtt = 1;

		if (dist>Radius) 
			objAtt = 0;
		else if (dist>StartFalloff) 
			objAtt = (dist-Radius)/(StartFalloff-Radius);

		return objAtt*Intensity;
	}
	else if (LightType == ELightSpot)
	{
		Vector lightDir = GetWorldPosition() - entity->GetCenter();
		float dist = lightDir.Normalize();

		float dot = -lightDir.Dot(Direction);
		float objAtt;

		if (dot<ConeAngleCos)
			return 0;
		else if (dot>FalloffAngleCos)
			objAtt = 1;
		else 
			objAtt = (ConeAngleCos-dot)/(ConeAngleCos-FalloffAngleCos);


		if (dist>Radius) 
			objAtt = 0;
		else if (dist>StartFalloff) 
			objAtt = objAtt*(dist-Radius)/(StartFalloff-Radius);

		return objAtt*Intensity;
	}
	
	return 0;
}

Matrix Light::GetProjMatrix() const
{
	return ProjMatrix;
}

#define LIGHT_LOAD_SAVE_TOKEN 0x5ea0

bool Light::SaveEntity( FileObject* f )
{
// Data for World to recreate us
	*f << (int)ETypeLight << FileName << Name << Scale << (char) (0);

	if( !f->PutToken( LIGHT_LOAD_SAVE_TOKEN ) ) return false;

	Entity::SaveEntity( f );

	// Data for us
	Vector pos = GetPosition();
	Quaternion rot = GetRotation();
	*f << Flags << pos;

	*f << LightType << DWORD(Color) << Intensity << Direction << ConeAngleCos << FalloffAngleCos;
	*f << Radius << StartFalloff << ProjectorName;

	return true;
}

bool Light::LoadEntity( FileObject* f )
{
	if( !f->TestToken( LIGHT_LOAD_SAVE_TOKEN ) )
	{
		GLog.Print( "Token test failure: Light::LoadEntity()\n" );
		return false;
	}

	Entity::LoadEntity( f );

	String tex;
	DWORD col;

	// load basic properties
	Vector pos;

	*f >> Flags >> pos;

	*f >> LightType >> col >> Intensity >> Direction >> ConeAngleCos >> FalloffAngleCos;
	*f >> Radius >> StartFalloff >> tex;

    TRACE("Light::LoadEntity(): setting Direction = <%g, %g, %g>\n", Direction.X, Direction.Y, Direction.Z);
    

	SetPosition(pos);

	Color = col;

	// init dynamic property
	if ((Flags & ELightDynamic) == ELightDynamic)
	{
		Flags &= ~ELightDynamic;
		EnableDynamic(true);
	}

	// init projector
	if (tex.Len())
	{
		SetProjectorTexture(tex.AsChar());
	}

	// init radius
	SetRadius(Radius);

	return true;
}

void Light::Draw(RenderDevice* dev, int mode)
{
	if (mode & View::EModeMultiPass && StaticLight)
	{
		MaterialSystem *matSys = GEngine->Video->MaterialSys;
		Texture* notex = matSys->Textures.NoTex;
		View *view = GEngine->Video->GetActiveView();
		Viewport &pViewport = view->LocalViewport;

		Material*	mat = matSys->CreateMaterial("particle_vol_proj");
		Matrix cloud = ScaleMatrix(Box.Max-Box.Min);
#ifdef	TRANSGAMING
		cloud.W = Plane(Box.Min);
#else
		cloud.W = Box.Min;
#endif
		Matrix cloudInv = cloud.Inverse();

		dev->SetStream(StaticLight->Buf);
		dev->SetShaderMemory(0, &(cloud*pViewport.WorldToScreen).GetTranspose()[0], 4);
		Vector move = Vector(0.02f,0.012f,0.004f)*GEngine->SysDriver.GetCurrentTime();
//		Vector move = Vector(0,0,0);

		dev->SetShaderMemory(4,(cloud*GetProjMatrix()).GetTranspose(),4);

#ifdef	TRANSGAMING
        Plane p(move, 0);

		dev->SetShaderMemory(8, &p,1);

        p.set(Color);
		dev->SetShaderMemory(10, &p,1);

        p.set(0, 0, 1, 1);
		dev->SetShaderMemory(16,&p,1);

        p.set(0, 1, 0, 1);
		dev->SetShaderMemory(17,&p,1);
#else
		dev->SetShaderMemory(8, &Plane(move,0),1);
		dev->SetShaderMemory(10, &Plane(Color),1);
		dev->SetShaderMemory(16,&Plane(0,0,1,1),1);
		dev->SetShaderMemory(17,&Plane(0,1,0,1),1);
#endif



		const Camera& camera = GEngine->Video->GetActiveView()->mCamera;
		const float size = 0.1f;
		Vector Right = camera.GetRightVector() * size;
		Vector Up    = camera.GetUpVector()    * size;
		Matrix Corners(-Right-Up, -Right+Up, Right-Up, Right+Up);


#ifdef TRANSGAMING
        Corners.GetTranspose();

        dev->SetShaderMemory(18, &Corners, 3);
#else
		dev->SetShaderMemory(18,&Corners.GetTranspose(),3);
#endif

		Vector OSLightPos = cloudInv.TransformCoord(GetWorldPosition());
		Vector dir = cloudInv.TransformNormal(Direction);		
		Plane z = Plane(dir*GetRadius(),OSLightPos);
		z.Dist = -z.Dist;
		dev->SetShaderMemory(14,&z,1);

		for (int iPass=0; iPass<mat->PassCount; ++iPass)
		{
			RenderPass& pass = mat->Passes[iPass];	
#ifdef TRANSGAMING
            MaterialInput mi(Projector?Projector:notex, notex, notex);


			pass.Activate();

			pass.SetInput(&mi);
#else
			pass.Activate();

			pass.SetInput(&MaterialInput(Projector?Projector:notex, notex, notex));
#endif

			dev->Draw(RenderDevice::EPrimTriList,0,StaticLight->BufSize);
		}
	}
}
//============================================================================
#ifndef TRANSGAMING
bool Light::DemoReadEntity( FileObject *f )
{
	Entity::DemoReadEntity( f );
	return true;
}


bool Light::DemoWriteEntity( FileObject *f )
{
	Entity::DemoWriteEntity( f );
	return true;
}


bool Light::DemoReadFull( FileObject *f )
{
	Entity::DemoReadFull( f );

	String tex;
	DWORD col;

	*f >> LightType >> col >> Intensity >> Direction >> ConeAngleCos >> FalloffAngleCos;
	*f >> Radius >> StartFalloff >> tex;

    TRACE("Light::DemoReadFull(): setting Direction = <%g, %g, %g>\n", Direction.X, Direction.Y, Direction.Z);


	Color = col;

	// init dynamic property
	if ((Flags & ELightDynamic) == ELightDynamic)
	{
		Flags &= ~ELightDynamic;
		EnableDynamic(true);
	}

	// init projector
	if (tex.Len())
	{
		SetProjectorTexture(tex.AsChar());
	}

	// init radius
	SetRadius(Radius);

	if( GIndex != 0 )
		GLog.Print( "Full info for light %d read\n", GIndex );

	return true;
}


bool Light::DemoWriteFull( FileObject *f )
{
	Entity::DemoWriteFull( f );
	*f << LightType << DWORD(Color) << Intensity << Direction << ConeAngleCos << FalloffAngleCos;
	*f << Radius << StartFalloff << ProjectorName;
	return true;
}
#endif
