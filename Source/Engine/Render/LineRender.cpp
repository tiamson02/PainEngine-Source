// $Header: /Game/Source/Engine/Render/LineRender.cpp 91    05-04-08 13:10 Krystian $
#ifdef TRANSGAMING_USE_EDITOR

#include "Engine.h"

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;
#include "Render/Material.h"
#include "Engine/Portal.h"
#include "Engine/Mesh.h"
#include "Render/LineRender.h"
#include "Render/Video.h"
#include "Pathfinding/WaypointSet.h"
#include "Engine/World.h"
#include "Render/View.h"

BOOL LineRender::Init()
{
	RenderDevice* dev = GEngine->Video->RenderDev;
	Assert(dev);
	if(VBuf.Create(dev, 1024, true)) 
		return TRUE;

	GLog.Print("ERROR Failed to create line vertex buffer\n");
	return FALSE;
}

void LineRender::RenderCleanup()
{
	VBuf.Release();
}

void LineRender::SetState(RenderDevice* dev)
{
	dev->SetColorMode();

	dev->SetTexture(0, EInvalidIndex);
	dev->SetTexture(1, EInvalidIndex);

    dev->SetScreenTransform(&GEngine->Video->GetActiveView()->LocalViewport.CameraToScreen);
	dev->SetCameraTransform(&GEngine->Video->GetActiveView()->LocalViewport.WorldToCamera);

	dev->SetStream(VBuf.GetStream());	
	dev->SetShader(GEngine->Video->MaterialSys->Shaders.FixedPos,RenderDevice::EShaderVertex);
	dev->SetShader(EInvalidIndex,RenderDevice::EShaderPixel);
}

inline static void AddBox(Vector*& v, const BoundingBox& box)
{
    const Vector P1=Vector(box.Min.X,box.Min.Y,box.Min.Z);
    const Vector P2=Vector(box.Max.X,box.Min.Y,box.Min.Z);                       
    const Vector P3=Vector(box.Min.X,box.Max.Y,box.Min.Z);                       
    const Vector P4=Vector(box.Max.X,box.Max.Y,box.Min.Z);                       
    const Vector P5=Vector(box.Min.X,box.Min.Y,box.Max.Z);                       
    const Vector P6=Vector(box.Max.X,box.Min.Y,box.Max.Z);                       
    const Vector P7=Vector(box.Min.X,box.Max.Y,box.Max.Z);                       
    const Vector P8=Vector(box.Max.X,box.Max.Y,box.Max.Z);                       

    const float ext=.3f;

#define CORNER(p1,p2,p3,p4)     \
        v[0]=p1;                \
        v[1]=p1+(p2-p1)*ext;    \
        v[2]=p1;                \
        v[3]=p1+(p3-p1)*ext;    \
        v[4]=p1;                \
        v[5]=p1+(p4-p1)*ext;    \
        v+=6;
        
        CORNER(P1,P2,P3,P5);
        CORNER(P2,P1,P4,P6);
        CORNER(P3,P1,P4,P7);
        CORNER(P4,P3,P2,P8);

        CORNER(P7,P3,P5,P8);
        CORNER(P5,P1,P6,P7);
        CORNER(P6,P2,P5,P8);
        CORNER(P8,P4,P6,P7);

#undef CORNER
}

inline static void AddFilledBox( Vector*& v, const BoundingBox& box )
{
    const Vector P1=Vector(box.Min.X,box.Min.Y,box.Min.Z);
    const Vector P2=Vector(box.Max.X,box.Min.Y,box.Min.Z);                       
    const Vector P3=Vector(box.Min.X,box.Max.Y,box.Min.Z);                       
    const Vector P4=Vector(box.Max.X,box.Max.Y,box.Min.Z);                       
    const Vector P5=Vector(box.Min.X,box.Min.Y,box.Max.Z);                       
    const Vector P6=Vector(box.Max.X,box.Min.Y,box.Max.Z);                       
    const Vector P7=Vector(box.Min.X,box.Max.Y,box.Max.Z);                       
    const Vector P8=Vector(box.Max.X,box.Max.Y,box.Max.Z);

	v[0] = P1; v[1] = P2; v[2] = P3;
	v[3] = P2; v[4] = P4; v[5] = P3;
	v[6] = P2; v[7] = P6; v[8] = P4;
	v[9] = P6; v[10] = P8; v[11] = P4;
	v[12] = P6; v[13] = P5; v[14] = P8;
	v[15] = P5; v[16] = P7; v[17] = P8;
	v[18] = P5; v[19] = P1; v[20] = P7;
	v[21] = P1; v[22] = P3; v[23] = P7;
	v[24] = P5; v[25] = P6; v[26] = P1;
	v[27] = P6; v[28] = P2; v[29] = P1;
	v[30] = P3; v[31] = P4; v[32] = P7;
	v[33] = P4; v[34] = P8; v[35] = P7;
}


void LineRender::DrawPlane(const Plane& p)
{
	Vector* v;
	const float size = 30.f;
	Vector mid = p.Normal*p.Dist;
	Vector tan = p.Normal^p.Normal.MinimalAxis();
	tan *= size/tan.Magnitude();
	Vector bin = p.Normal^tan;
	bin *= size/bin.Magnitude();

    RenderDevice* dev=VBuf.GetDevice();
	SetState(dev);

	dev->SetObjectTransform(GIdentityMatrix);
	RenderState state;
	state.SetBlendMode(RenderState::EBlendAdd);
    state.SetConstColor(0x007F00);
	dev->SetState(state);

    v=VBuf.Lock(4);
	v[0] = mid+tan+bin;
	v[1] = mid+tan-bin;
	v[2] = mid-tan-bin;
	v[3] = mid-tan+bin;
    dev->Draw(RenderDevice::EPrimTriFan,VBuf.Unlock(),2);

    state.SetConstColor(0x7F0000);
	dev->SetState(state);

	v=VBuf.Lock(4);
	v[0] = mid+tan+bin;
	v[1] = mid-tan+bin;
	v[2] = mid-tan-bin;
	v[3] = mid+tan-bin;
    dev->Draw(RenderDevice::EPrimTriFan,VBuf.Unlock(),2);
}

void LineRender::DrawBox(const BoundingBox& box, DWORD color)
{
    Vector* v=VBuf.Lock(6*8);
    AddBox(v,box);

    RenderDevice* dev=VBuf.GetDevice();

	SetState(dev);

	RenderState state;
    state.SetConstColor(color);
	state.SetDepthTest(true);
	state.SetDepthWrite(false);
	state.SetAlphaTest(false);
	dev->SetState(state);

    dev->Draw(RenderDevice::EPrimLineList,VBuf.Unlock(),(6*8)/2);
}

void LineRender::DrawTranslucentBox(const BoundingBox& box, DWORD color)
{
    Vector* v=VBuf.Lock(3*6*2);
    AddFilledBox(v,box);

    RenderDevice* dev=VBuf.GetDevice();
    SetState(dev);

    RenderState state;
    state.SetConstColor(color);
    state.SetCullMode(RenderState::ECullNone);
    state.SetBlendMode(RenderState::EBlendAdd);
    state.SetDepthTest(true);
    state.SetDepthWrite(false);
    dev->SetState(state);

    dev->Draw(RenderDevice::EPrimTriList,VBuf.Unlock(),6*2);
}

void LineRender::DrawFilledBox( const BoundingBox& box, DWORD color, bool ztest )
{
    Vector* v=VBuf.Lock(3*6*2);
    AddFilledBox(v,box);

    RenderDevice* dev=VBuf.GetDevice();

	SetState(dev);

	RenderState state;
    state.SetConstColor(color);
    state.SetDepthTest(ztest);
    state.SetDepthWrite(ztest);
    dev->SetState(state);

	dev->Draw(RenderDevice::EPrimTriList,VBuf.Unlock(),6*2);
}

void LineRender::DrawTri(const Vector tri[3], DWORD color)
{
	Vector* v=VBuf.Lock(4);
	v[0]=tri[0];
	v[1]=tri[1];
	v[2]=tri[2];
	v[3]=tri[0];

    RenderDevice* dev=VBuf.GetDevice();

	SetState(dev);

	RenderState state;
    state.SetConstColor(color);
	dev->SetState(state);

    dev->Draw(RenderDevice::EPrimLineStrip,VBuf.Unlock(),3);
}

void LineRender::DrawPoly(const ConvexPolygon& poly, DWORD color)
{
	Vector* v=VBuf.Lock(poly.VertexCount*2);
	for(int i=0; i<poly.VertexCount; ++i)
	{
		v[i*2+0]=poly.Verts[i];
		v[i*2+1]=poly.Verts[(i+1)%poly.VertexCount];
	}

    RenderDevice* dev=VBuf.GetDevice();

	SetState(dev);

	RenderState state;
    state.SetConstColor(color);
	dev->SetState(state);

    dev->Draw(RenderDevice::EPrimLineList,VBuf.Unlock(),poly.VertexCount);
}

void LineRender::DrawFilledPoly(const ConvexPolygon& poly, DWORD color)
{
	Vector* v=VBuf.Lock(poly.VertexCount);
	for(int i=0; i<poly.VertexCount; ++i)
        v[i]=poly.Verts[i];

    RenderDevice* dev=VBuf.GetDevice();

	SetState(dev);

	RenderState state;
    state.SetConstColor(color);
	dev->SetState(state);

    dev->Draw(RenderDevice::EPrimTriFan,VBuf.Unlock(),poly.VertexCount-2);
}

void LineRender::DrawTranslucentZones(const DynamicArray<Zone*>& zones, DWORD color)
{
    RenderDevice* dev = VBuf.GetDevice();

	SetState(dev);

	RenderState state;
    state.SetConstColor(color);
	state.SetBlendMode(RenderState::EBlendAdd);
	state.SetDepthTest(false);
	state.SetDepthWrite(false);
	dev->SetState(state);

	for( int i = 0; i < zones.Size(); ++i )
	{
		const Zone& z = *zones[i];
		if( !z.OccludeEnteredInLastFrame )
			continue;
		if( z.Triangles.Size() )
		{
			int triangleCount = z.Triangles.Size();
			Vector* v = VBuf.Lock( triangleCount );
			for( int j = 0; j < triangleCount; ++j )
		        v[j] = z.Vertices[z.Triangles[j]];
		    dev->Draw( RenderDevice::EPrimTriList,VBuf.Unlock(),triangleCount/3 );
		}
	}
}

void LineRender::DrawAntiPortals(const DynamicArray<AntiPortal*>& anti, DWORD color)
{
    RenderDevice* dev = VBuf.GetDevice();

	SetState(dev);

	RenderState state;
    state.SetConstColor(color);
	state.SetBlendMode(RenderState::EBlendAdd);
	state.SetDepthTest(false);
	state.SetDepthWrite(false);
	dev->SetState(state);

	for( int i = 0; i < anti.Size(); ++i )
		if( !anti[i]->Disabled )
		{
			const AntiPortal& ap = *anti[i];
			int triangleCount = ap.Triangles.Size();
			Vector* v = VBuf.Lock(triangleCount);
			for( int j = 0; j < triangleCount; ++j )
		        v[j] = ap.Vertices[ap.Triangles[j]];
		    dev->Draw( RenderDevice::EPrimTriList,VBuf.Unlock(),triangleCount/3 );
		}
}

void LineRender::DrawWireframe(const DynamicArray<SimpleVertex>& verts, const Matrix& m, const DWORD color)
{
	RenderDevice* dev=VBuf.GetDevice();

	SetState(dev);

	Vector* v=(Vector*)dev->LockStream(VBuf.GetStream(),0,0);
	int vertexCount=0;

	dev->SetColorMode();
	dev->SetObjectTransform(&m);

	RenderState state;
    state.SetConstColor(color);
	state.SetBlendMode(RenderState::EBlendAdd);
	state.SetDepthWrite(false);
	dev->SetState(state);

	const int triCount=verts.Size()/3;
	for (int i=0; i<triCount; ++i) 
	{
		vertexCount+=6;
		if (vertexCount>=VBuf.GetMaxElems()) 
		{
			dev->UnlockStream(VBuf.GetStream());
			dev->Draw(RenderDevice::EPrimLineList,0,(vertexCount-6)/2);

			v=(Vector*)dev->LockStream(VBuf.GetStream(),0,0);
			vertexCount=6;
		}

		v[0]=verts[i*3].Pos;
		v[1]=verts[i*3+1].Pos;
		v[2]=verts[i*3+1].Pos;
		v[3]=verts[i*3+2].Pos;
		v[4]=verts[i*3+2].Pos;
		v[5]=verts[i*3].Pos;

		v+=6;
	}

	dev->UnlockStream(VBuf.GetStream());
	dev->Draw(RenderDevice::EPrimLineList,0,vertexCount/2);

	dev->SetObjectTransform(GIdentityMatrix);
}

void LineRender::DrawMesh(const WorldMesh* mesh, const DWORD color)
{
    RenderDevice* dev=GEngine->Video->RenderDev;

	SetState(dev);

	RenderState state;
	state.SetConstColor(color);
	state.SetDepthTest(false);
	state.SetDepthWrite(false);
	state.SetAlphaTest(false);
	state.SetLighting(false);
	state.SetBlendMode(RenderState::EBlendAdd);
	//state.SetBlendMode(RenderState::EBlendNone);
	dev->SetState(state);
	dev->SetObjectTransform((mesh->Flags & Entity::EEntMovable) ? &mesh->ObjectToWorld : GIdentityMatrix);

    // lock entire buffer
    Vector* v=(Vector*)dev->LockStream(VBuf.GetStream(),0,0);
    int vertexCount=0;

#ifdef TRANSGAMING_DEINDEX
    if (!mesh->isIndexed){
        int triCount;
        int base;
        
        
        if (mesh->PrimType == RenderDevice::EPrimTriList)
            triCount = mesh->VertList.Size() / 3;

        else
            triCount = mesh->VertList.Size() - 2;


        for (int i=0; i<triCount; ++i) 
	    {
		    vertexCount+=6;
            if (vertexCount >= VBuf.GetMaxElems()) 
		    {
			    dev->UnlockStream(VBuf.GetStream());
                dev->Draw(RenderDevice::EPrimLineList,0,(vertexCount-6)/2);

                v=(Vector*)dev->LockStream(VBuf.GetStream(),0,0);
                vertexCount=6;
		    }


            if (mesh->PrimType == RenderDevice::EPrimTriList)
                base = i * 3;

            else
                base = i;


		    const Vector P[3] = 
		    {
			    mesh->VertList.getVertPos(base + 0),
                mesh->VertList.getVertPos(base + 1),
                mesh->VertList.getVertPos(base + 2),
		    };

		    v[0] = P[0];
		    v[1] = P[1];
		    v[2] = P[1];
		    v[3] = P[2];
		    v[4] = P[2];
		    v[5] = P[0];
                
            v+=6;
	    }
    }

    else{
#endif
        const int triCount=mesh->Triangles.Size()/3;
        for (int i=0; i<triCount; ++i) 
	    {
		    vertexCount+=6;
            if (vertexCount >= VBuf.GetMaxElems()) 
		    {
			    dev->UnlockStream(VBuf.GetStream());
                dev->Draw(RenderDevice::EPrimLineList,0,(vertexCount-6)/2);

                v=(Vector*)dev->LockStream(VBuf.GetStream(),0,0);
                vertexCount=6;
		    }

		    const int T0 = mesh->Triangles[i*3+0];
		    const int T1 = mesh->Triangles[i*3+1];
		    const int T2 = mesh->Triangles[i*3+2];

		    const Vector P[3] = 
		    {
			    mesh->VertList.getVertPos(T0),
			    mesh->VertList.getVertPos(T1),
			    mesh->VertList.getVertPos(T2),
		    };

		    v[0] = P[0];
		    v[1] = P[1];
		    v[2] = P[1];
		    v[3] = P[2];
		    v[4] = P[2];
		    v[5] = P[0];
                
            v+=6;
	    }
#ifdef TRANSGAMING_DEINDEX
    }
#endif

    dev->UnlockStream(VBuf.GetStream());
    dev->Draw(RenderDevice::EPrimLineList,0,vertexCount/2);

    dev->SetObjectTransform(GIdentityMatrix);
}

void LineRender::DrawWaypointSet( const WaypointSet& wps )
{
	int i=0;
	RenderDevice* dev=GEngine->Video->RenderDev;

	SetState(dev);

	dev->SetObjectTransform(GIdentityMatrix);

    // render unselected waypoints for all monsters
    for(int index = 0; index < wps.mWaypoints.Size(); index++ )
	{
		if( wps.mSelected.Test( index ) || wps.mBigMonsters.Test( index ) )
		{
			continue;
		};

		Waypoint& wp = wps.mWaypoints[index];
		BoundingBox b;
		wp.MakeBox( b );
		DrawFilledBox( b, 0xff );
	};

	// render selected waypoints for all monsters
    for(int index = 0; index < wps.mWaypoints.Size(); index++ )
	{
		if( !wps.mSelected.Test( index ) || wps.mBigMonsters.Test( index ) )
		{
			continue;
		};

		Waypoint& wp = wps.mWaypoints[index];
		BoundingBox b;
		wp.MakeBox( b );
		DrawFilledBox( b, 0xff00 );
	};

    // render unselected waypoints for big monsters
    for(int index = 0; index < wps.mWaypoints.Size(); index++ )
	{
		if( wps.mSelected.Test( index ) || (!wps.mBigMonsters.Test( index )) )
		{
			continue;
		};

		Waypoint& wp = wps.mWaypoints[index];
		BoundingBox b;
		wp.MakeBox( b );
		DrawFilledBox( b, 0xa00000 );
	};

	// render selected waypoints for big monsters
    for(int index = 0; index < wps.mWaypoints.Size(); index++ )
	{
		if( !wps.mSelected.Test( index ) || (!wps.mBigMonsters.Test( index )) )
		{
			continue;
		};

		Waypoint& wp = wps.mWaypoints[index];
		BoundingBox b;
		wp.MakeBox( b );
		DrawFilledBox( b, 0xff0000 );
	};

	int size = wps.mWaypoints.Size();

	// render all connections
	RenderState state;
    state.SetConstColor(0xff0000);
	dev->SetState(state);

	if( wps.mDistances )
	{
		if( !wps.mPackedBase )		// Operating on unpacked connections
		{
			int howManyLinePoints = 0;
			for( i = 0; i < size*size; i++ )
			{
				if( wps.mDistances[i] > 0 )
				{
					howManyLinePoints += 2;
				};
			};

			int ind = 0;
			i = 0;
			int j = 0;
			while( howManyLinePoints )
			{
				int currentLinePoints = (howManyLinePoints > 1024)?1024:howManyLinePoints;
				howManyLinePoints -= 1024;
				if( howManyLinePoints < 0 )
				{
					howManyLinePoints = 0;
				};

				Vector* v = VBuf.Lock( currentLinePoints );
				currentLinePoints = 0;

				for( ;i < size; i++ )
				{
					for( ; j < size; j++, ind++ )
					{
						if( wps.mDistances[ind] > 0 )
						{
							v[currentLinePoints++] = wps.mWaypoints[i].mPosition;
							v[currentLinePoints++] = wps.mWaypoints[j].mPosition;
							if( currentLinePoints >= 1024 )
							{
								j++;
								ind++;
								break;
							};
						};
					};
					if( currentLinePoints >= 1024 )
					{
						break;
					};
					j = 0;
				};

				dev->Draw(RenderDevice::EPrimLineList, VBuf.Unlock(), currentLinePoints/2 );
			};
		}
		else	// Operating on packed connections
		{
			int i = 0;
			int j = 0;

			int howManyLeft = wps.mHowManyDistances*2;
			while( howManyLeft )
			{
				int toDrawThisTime = (howManyLeft>1024)?1024:howManyLeft;
				howManyLeft -= toDrawThisTime;

				Vector* v = VBuf.Lock( toDrawThisTime );
				int drawThisTime = 0;

				while( drawThisTime < toDrawThisTime )
				{
					while( j >= wps.mPackedCount[i] )
					{
						j = 0;
						i++;
					};
					int index = wps.mPackedBase[i] + j;
					j++;
					v[drawThisTime++] = wps.mWaypoints[i].mPosition;
					v[drawThisTime++] = wps.mWaypoints[wps.mPackedIndices[index]].mPosition;
				};

				dev->Draw(RenderDevice::EPrimLineList, VBuf.Unlock(), toDrawThisTime/2 );
			};
		};
	};
}

void LineRender::DrawWaypointSet2( void )
{
	RenderDevice* dev=GEngine->Video->RenderDev;
#ifdef	TRANSGAMING
	Pathfinder2& p = GEngine->pcfWorld->Paths2;
#else
	Pathfinder2& p = GEngine->World->Paths2;
#endif	
	short curSet = p.GetCurrentSet();

	SetState(dev);
	dev->SetObjectTransform(GIdentityMatrix);

	int size = p.mWaypoints.Size();

	// determine waypoints to draw
	Vector pos = GEngine->Video->GetActiveView()->mCamera.GetPosition();
	float limit = p.GetViewLimits();
	Vector disp( limit,limit,limit );
	BoundingBox box( pos-disp, pos+disp );
	DynamicArray<int> wpIndices;
	for( int index = 0; index < size; ++index )
		if( box.PointTest( p.mWaypoints[index].Position ) )
			wpIndices.Add(index);
	size = wpIndices.Size();

	// render unselected waypoints
	for( int index = 0; index < size; ++index )
	{
		Waypoint2& wp = p.mWaypoints[wpIndices[index]];
		BoundingBox b;
		wp.MakeBox( b );

		if( wp.Selected )
			DrawFilledBox( b, p.SelectedColor );
		else if( !p.SetEnabled(wp.Set) )
		{
			Assert( wp.Set != curSet );
			if( p.DrawDisabledSets )
				DrawFilledBox( b, p.DisabledColor );
		}
		else if( wp.Set == curSet )
			DrawFilledBox( b, p.ChosenSetColor );
		else
			if (wp.Disabled)
				DrawFilledBox( b, 0 );
			else
				DrawFilledBox( b, p.SetColors[wp.Set%NUM_WPSET_COLORS] );
	}

	// render all connections
	RenderState state;
    state.SetConstColor(p.BorderColor);
	dev->SetState(state);

	int indices[2048];

	// Draw border lines
	int i = 0;
	int j = 0;

	while( i < size )
	{
		int drawThisTime = 0;
		while( drawThisTime < 1024 )
		{
			Waypoint2* wp = &p.mWaypoints[wpIndices[i]];

			while( j >= wp->Count )
			{
				j = 0;
				i++;
				if( i == size ) break;
				wp = &p.mWaypoints[wpIndices[i]];
			}
			if( i == size ) break;
			int index = wp->Base + j;
			if( !( wp->Selected || p.mWaypoints[p.mPackedIndices[index]].Selected ) &&
				( wp->Set != p.mWaypoints[p.mPackedIndices[index]].Set ) )
			{
				indices[drawThisTime++] = wpIndices[i];
				indices[drawThisTime++] = p.mPackedIndices[index];
			}
			j++;
		}

		Vector* v = VBuf.Lock( drawThisTime );
		for( int k = 0; k < drawThisTime; ++k )
		{
			v[k] = p.mWaypoints[indices[k]].Position;
			k++;
			v[k] = p.mWaypoints[indices[k]].Position;
		}
		dev->Draw(RenderDevice::EPrimLineList, VBuf.Unlock(), drawThisTime/2 );
	}

	// Draw nonborder lines
	for( int a = 0; a < NUM_WPSET_COLORS; ++a )
	{
	    state.SetConstColor(p.SetColors[a]);
		dev->SetState(state);

		i = 0;
		j = 0;

		while( i < size )
		{
			int drawThisTime = 0;
			while( drawThisTime < 1024 )
			{
				Waypoint2* wp = &p.mWaypoints[wpIndices[i]];

				while( j >= wp->Count )
				{
					j = 0;
					i++;
					if( i == size ) break;
					wp = &p.mWaypoints[wpIndices[i]];
				}
				if( i == size ) break;
				int index = wp->Base + j;
				if( !( wp->Selected || p.mWaypoints[p.mPackedIndices[index]].Selected ) &&
					( wp->Set == p.mWaypoints[p.mPackedIndices[index]].Set ) &&
					( wp->Set % NUM_WPSET_COLORS == a ) &&
					( p.SetEnabled(wp->Set) ) &&
					( wp->Set != curSet ) )
				{
					indices[drawThisTime++] = wpIndices[i];
					indices[drawThisTime++] = p.mPackedIndices[index];
				}
				j++;
			}

			Vector* v = VBuf.Lock( drawThisTime );
			for( int k = 0; k < drawThisTime; ++k )
			{
				v[k] = p.mWaypoints[indices[k]].Position;
				k++;
				v[k] = p.mWaypoints[indices[k]].Position;
			}
			dev->Draw(RenderDevice::EPrimLineList, VBuf.Unlock(), drawThisTime/2 );
		}
	}

	if( p.DrawDisabledSets )
	{
		// Draw nonborder lines of disabled sets
	    state.SetConstColor(p.DisabledColor);
		dev->SetState(state);

		i = 0;
		j = 0;

		while( i < size )
		{
			int drawThisTime = 0;
			while( drawThisTime < 1024 )
			{
				Waypoint2* wp = &p.mWaypoints[wpIndices[i]];

				while( j >= wp->Count )
				{
					j = 0;
					i++;
					if( i == size ) break;
					wp = &p.mWaypoints[wpIndices[i]];
				}
				if( i == size ) break;
				int index = wp->Base + j;
				if( ( wp->Set == p.mWaypoints[p.mPackedIndices[index]].Set ) &&
					( !p.SetEnabled(wp->Set) ) )
				{
					indices[drawThisTime++] = wpIndices[i];
					indices[drawThisTime++] = p.mPackedIndices[index];
				}
				j++;
			}

			Vector* v = VBuf.Lock( drawThisTime );
			for( int k = 0; k < drawThisTime; ++k )
			{
				v[k] = p.mWaypoints[indices[k]].Position;
				k++;
				v[k] = p.mWaypoints[indices[k]].Position;
			}
			dev->Draw(RenderDevice::EPrimLineList, VBuf.Unlock(), drawThisTime/2 );
		}
	}

	// Draw nonborder lines of current set
    state.SetConstColor(p.ChosenSetColor);
	dev->SetState(state);

	i = 0;
	j = 0;

	while( i < size )
	{
		int drawThisTime = 0;
		while( drawThisTime < 1024 )
		{
			Waypoint2* wp = &p.mWaypoints[wpIndices[i]];

			while( j >= wp->Count )
			{
				j = 0;
				i++;
				if( i == size ) break;
				wp = &p.mWaypoints[wpIndices[i]];
			}
			if( i == size ) break;
			int index = wp->Base + j;
			if( !( wp->Selected || p.mWaypoints[p.mPackedIndices[index]].Selected ) &&
				( wp->Set == p.mWaypoints[p.mPackedIndices[index]].Set ) &&
				( wp->Set == curSet ) )
			{
				indices[drawThisTime++] = wpIndices[i];
				indices[drawThisTime++] = p.mPackedIndices[index];
			}
			j++;
		}

		Vector* v = VBuf.Lock( drawThisTime );
		for( int k = 0; k < drawThisTime; ++k )
		{
			v[k] = p.mWaypoints[indices[k]].Position;
			k++;
			v[k] = p.mWaypoints[indices[k]].Position;
		}
		dev->Draw(RenderDevice::EPrimLineList, VBuf.Unlock(), drawThisTime/2 );
	}

	// Draw selected waypoints' connections
    state.SetConstColor(p.SelectedColor);
	dev->SetState(state);

	i = 0;
	j = 0;

	while( i < size )
	{
		int drawThisTime = 0;
		while( drawThisTime < 1024 )
		{
			Waypoint2* wp = &p.mWaypoints[wpIndices[i]];

			while( j >= wp->Count )
			{
				j = 0;
				i++;
				if( i == size ) break;
				wp = &p.mWaypoints[wpIndices[i]];
			}
			if( i == size ) break;
			int index = wp->Base + j;
			if( wp->Selected || p.mWaypoints[p.mPackedIndices[index]].Selected )
			{
				indices[drawThisTime++] = wpIndices[i];
				indices[drawThisTime++] = p.mPackedIndices[index];
			}
			j++;
		}

		Vector* v = VBuf.Lock( drawThisTime );
		for( int k = 0; k < drawThisTime; ++k )
		{
			v[k] = p.mWaypoints[indices[k]].Position;
			k++;
			v[k] = p.mWaypoints[indices[k]].Position;
		}
		dev->Draw(RenderDevice::EPrimLineList, VBuf.Unlock(), drawThisTime/2 );
	}
}

#define DISPLACEMENT_UP 0.1f

void LineRender::DrawFloor( const PathfinderFloor& floor, DWORD color )
{
	RenderDevice* dev=GEngine->Video->RenderDev;

	SetState(dev);

	RenderState state;
	state.SetDepthTest(true);
	state.SetDepthWrite(true);
	state.SetConstColor(color);
	dev->SetState(state);

	dev->SetObjectTransform(GIdentityMatrix);

    // render all triangles
	int toDraw = floor.mFloorTriangles.Size();
	int drawn = 0;
	while( toDraw )
	{
		int toDrawThisTime = (toDraw<1023)?toDraw:1023;
		toDraw -= toDrawThisTime;
		Vector* v = VBuf.Lock( toDrawThisTime );
	    for( int i = 0; i < toDrawThisTime; i++ )
		{
			v[i] = floor.mFloorTriangles[drawn+i];
			v[i].Y += DISPLACEMENT_UP;
		};
		drawn += toDrawThisTime;
		dev->Draw(RenderDevice::EPrimTriList, VBuf.Unlock(), toDrawThisTime/3 );
	};
}

void LineRender::DrawFloor( const Pathfinder2Floor& floor, DWORD color )
{
#ifdef FLOORS_ENABLED
	RenderDevice* dev=GEngine->Video->RenderDev;

	SetState(dev);

	RenderState state;
	state.SetDepthTest(true);
	state.SetDepthWrite(true);
	state.SetConstColor(color);
	dev->SetState(state);

	dev->SetObjectTransform(GIdentityMatrix);

    // render all triangles
	int toDraw = floor.mFloorTriangles.Size();
	int drawn = 0;
	while( toDraw )
	{
		int toDrawThisTime = (toDraw<1023)?toDraw:1023;
		toDraw -= toDrawThisTime;
		Vector* v = VBuf.Lock( toDrawThisTime );
	    for( int i = 0; i < toDrawThisTime; i++ )
		{
			v[i] = floor.mFloorTriangles[drawn+i];
			v[i].Y += DISPLACEMENT_UP;
		};
		drawn += toDrawThisTime;
		dev->Draw(RenderDevice::EPrimTriList, VBuf.Unlock(), toDrawThisTime/3 );
	};
#endif
}

void LineRender::DrawWaypointPath( const WaypointPath& path )
{
	int size = path.mPath.Size();
	if( size < 2 ) return;

	RenderDevice* dev=GEngine->Video->RenderDev;

	SetState(dev);

	RenderState state;
	state.SetDepthTest(true);
	state.SetDepthWrite(true);
	state.SetConstColor( 0xff00 );
	dev->SetState(state);

	dev->SetObjectTransform(GIdentityMatrix);

	// render all connections
	Vector* v = VBuf.Lock( size );
	for( int i = 0; i < size; i++ )
	{
		v[i] = path.mPath[i]->mPosition;
		v[i].Y += 0.5f;
	};
	dev->Draw(RenderDevice::EPrimLineStrip, VBuf.Unlock(), size-1 );
}

void LineRender::DrawGlobalWaypointPath( const WaypointGPath& path )
{
	int i=0;
	if( !path.mInit )
	{
		return;
	};
	RenderDevice* dev=GEngine->Video->RenderDev;

	SetState(dev);

	if( path.mCalculatedPoints.Size() >= 1 )
	{

		RenderState state;
		state.SetDepthTest(true);
		state.SetDepthWrite(true);
		state.SetConstColor( 0xffff );
		dev->SetState(state);

		dev->SetObjectTransform(GIdentityMatrix);

		// render all connections
		Vector* v = VBuf.Lock( path.mCalculatedPoints.Size()+1 );
		for( i = 0; i < path.mCalculatedPoints.Size(); i++ )
		{
			v[i] = path.mCalculatedPoints[i];
			v[i].Y += 0.5f;
		};
		if( path.mPrecisePath )
		{
			v[i] = path.mPrecisePath->PeekNextFromPath();
			if( v[i].Magnitude() == 0.0f )
				v[i] = v[i-1];
			else
				v[i].Y += 0.5f;
		}
		else
			v[i] = v[i-1];

		dev->Draw(RenderDevice::EPrimLineStrip, VBuf.Unlock(), path.mCalculatedPoints.Size() );
	};

	if( path.mPrecisePath )
	{
		DrawWaypointPath( *path.mPrecisePath );
	};

	int size = path.mPortals.Size();
	if( size < 2 )
	{
		return;
	};

	RenderState state;
	state.SetConstColor( 0xa000 );
	dev->SetState(state);

	dev->SetObjectTransform(GIdentityMatrix);

	// render all connections
	Vector* v = VBuf.Lock( size );
	for( int i = 0; i < size; i++ )
	{
		v[i] = path.mPortals[i]->GetPortal()->Box.GetCenter();
	};
	dev->Draw(RenderDevice::EPrimLineStrip, VBuf.Unlock(), size-1 );
}

// =========================================================================
void LineRender::DrawLine(const Vector& p1, const Vector& p2, DWORD color,bool ztest )
{
	RenderDevice* dev=GEngine->GetRenderDevice();
    Assert(dev);

	SetState(dev);

	RenderState state;
	state.SetDepthTest(ztest);
	state.SetDepthWrite(ztest);
	state.SetConstColor(color);
	dev->SetState(state);

	dev->SetObjectTransform(GIdentityMatrix);

	Vector* v = VBuf.Lock(2);
	v[0] = p1;
	v[1] = p2;
	dev->Draw(RenderDevice::EPrimLineList, VBuf.Unlock(), 1);
}

void LineRender::DrawSphere(float radius, DWORD color, Vector* center, bool ztest )
{
	const int SEGS = 32;
    Vector pt[3][SEGS];

    for (int i=0; i<SEGS; ++i)
	{        
        float val = float(i)/(SEGS/6);
        float sinVal = GSinFloat(val);
        float cosVal = GCosFloat(val);
        pt[0][i] = Vector(0            , sinVal*radius, cosVal*radius);
        pt[1][i] = Vector(sinVal*radius, 0            , cosVal*radius);
        pt[2][i] = Vector(sinVal*radius, cosVal*radius, 0);
		if( center )
		{
			pt[0][i] += *center;
			pt[1][i] += *center;
			pt[2][i] += *center;
		};
    }

	RenderDevice* dev=GEngine->GetRenderDevice();
    Assert(dev);

	const int primNum = 3*SEGS;
    Vector* v = VBuf.Lock(2*primNum);
    Assert(v)
    // joint
    for (int a=0; a<3; a++)
        for (int i=0; i<SEGS; i++)
        {
            *v++ = pt[a][i];
            *v++ = pt[a][(i+1)%SEGS];
        }

	SetState(dev);

	RenderState state;
    state.SetBlendMode(RenderState::EBlendNone);
    state.SetDepthTest(ztest);
    state.SetDepthWrite(ztest);
    state.SetAlphaTest(false);
	state.SetConstColor(color);
	dev->SetState(state);

	dev->Draw(RenderDevice::EPrimLineList, VBuf.Unlock(), primNum);
}

void LineRender::DrawDirLight(DWORD color, const Vector& pos, const Vector& dir)
{
	const int SEGS = 12;

	RenderDevice* dev=GEngine->GetRenderDevice();
    Assert(dev);

	SetState(dev);

	RenderState state;
	state.SetConstColor(color);
	dev->SetState(state);

	const float radius = 1;
	const float width = 0.1f;

	Vector fwd = dir;
	fwd.Normalize();
	fwd = fwd * radius;

	Vector left = (dir^dir.MinimalAxis());
	left.Normalize();
	Vector left_side = left * width;
	left = left * radius;

	Vector up  = (left^dir);
	up.Normalize();
	Vector up_side = up * width;
	up = up * radius;

	const Vector half_fwd = fwd*0.5f;

    for (int i=0; i<SEGS; ++i)  
    {        
		Vector* v = VBuf.Lock(6);
		Assert(v);
        float val = float(i)/SEGS*F_TWOPI;
        float sinVal = GSinFloat(val);
        float cosVal = GCosFloat(val);

		const Vector offset = pos + left*sinVal + up*cosVal;
		const Vector side = left_side*cosVal-up_side*sinVal;

		*v++ = offset-fwd;
		*v++ = offset+half_fwd;
		*v++ = offset+half_fwd+side;
		*v++ = offset+fwd;
		*v++ = offset+half_fwd-side;
		*v++ = offset+half_fwd;

	    dev->Draw(RenderDevice::EPrimLineStrip, VBuf.Unlock(), 5);
    }

}

void LineRender::DrawEllipse(DWORD color, const Vector& pos, const Vector& xAxis, const Vector& yAxis)
{
	const int SEGS = 32;
    Vector pt[SEGS];

    for (int i=0; i<SEGS; ++i)
	{        
        float val = float(i)/(SEGS/6);
        float sinVal = GSinFloat(val);
        float cosVal = GCosFloat(val);
        pt[i] = pos + xAxis*cosVal + yAxis*sinVal;
    }

	RenderDevice* dev=GEngine->GetRenderDevice();
    Assert(dev);

	const int primNum = SEGS;
    Vector* v = VBuf.Lock(2*primNum);
    Assert(v)
    // joint
    for (int i=0; i<SEGS; i++)
    {
        *v++ = pt[i];
        *v++ = pt[(i+1)%SEGS];
    }

	SetState(dev);

	RenderState state;
	state.SetConstColor(color);
	dev->SetState(state);

	dev->Draw(RenderDevice::EPrimLineList, VBuf.Unlock(), primNum);
}

void LineRender::DrawSphereCone(float radius, DWORD color, float angle, Matrix* m)
{
	const int SEGS = 32;
    Vector pt[2][SEGS];

    for (int i=0; i<SEGS; ++i)
	{        
        float val = -angle/2 + (angle)*float(i)/(SEGS-1);
        float sinVal = GSinFloat(val);
        float cosVal = GCosFloat(val);
        pt[0][i] = Vector(0            , sinVal*radius, cosVal*radius);
        pt[1][i] = Vector(sinVal*radius, 0            , cosVal*radius);

		pt[0][i] = m->TransformCoord(pt[0][i]);
		pt[1][i] = m->TransformCoord(pt[1][i]);
    }

	RenderDevice* dev=GEngine->GetRenderDevice();
    Assert(dev);

	const int primNum = 2*(SEGS-1);
    Vector* v = VBuf.Lock(2*primNum);
    Assert(v)
    // joint
    for (int a=0; a<2; a++)
        for (int i=0; i<SEGS-1; i++)
        {
            *v++ = pt[a][i];
            *v++ = pt[a][i+1];
        }

	SetState(dev);

	RenderState state;
	state.SetConstColor(color);
	dev->SetState(state);

    dev->Draw(RenderDevice::EPrimLineList, VBuf.Unlock(), primNum);
}
#endif
