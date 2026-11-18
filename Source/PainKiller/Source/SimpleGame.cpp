#include "Engine.h"
#include "Engine/PCFSystem.h"
#include "Engine/BaseGame.h"

#include "Render/HUD.h"
#include "Render/Decal.h"
#include "Render/View.h"
#include "Engine/Mesh.h"
#include "Render/FogVolume.h"
#include "Render/Video.h"
#include "Engine/Model.h"
#include "Engine/World.h"
#include "Render/Material.h"
#include "Physics/PhysicsObject.h"
#include "Physics/PhysicsEngine.h"
#include "Physics/PhysicsWorld.h"

#include "SimpleGame.h"


static DWORD ColorAsDWORD(const float* color)
{
    DWORD r=(DWORD)(color[0]*255.f);
    DWORD g=(DWORD)(color[1]*255.f);
    DWORD b=(DWORD)(color[2]*255.f);
    DWORD a=(DWORD)(color[3]*255.f);

    return (a<<24)|(r<<16)|(g<<8)|b;
}

static float FogColor[4] = { 0.2f,0.2f,0.2f,0 };
static float FogDensity = 0.1f;
static float FogStart = 128;
static float FogEnd   = 1024;
static int   FogMode  = 0;

namespace { FactoryRegistration<BaseGame,DemoGame> reg("demogame"); }

// =========================================================================

DemoGame::DemoGame()
:   mInputSystem(0), mPhysics(0)
,   LogoTexRef(EInvalidIndex)
,   RenderMode(0) // normal
,   Mode(EModePortals/*|EModeShowHUD|EModeFlyMove*/) // XXX B.S. - for Adrian
,   CurrentZone(0)
{

    Contrast   =0.5f;
    Gamma      =1.5f;
    Brightness =0.5f;
}

DemoGame::~DemoGame()
{
    Close();
}

void DemoGame::Close()
{
    guard(DemoGame::Close);
    if(mInputSystem) delete mInputSystem, mInputSystem=0;
    if(mPhysics)     delete mPhysics,     mPhysics=0;
    unguard;
}

BOOL DemoGame::InitRenderer()
{
    guard(DemoGame::InitRenderer);
    if(GEngine->OpenRenderWindow(RenderDeviceManager::ED3DRenderDevice)) {
        RenderWindow* renderWnd=GEngine->GetRenderWindow();
        mInputSystem=new Win32InputSystem(renderWnd->GetHandle());

        if(GEngine->IsFullScreen()) {
            mInputSystem->LockMouse(TRUE);
            Mode|=EModeLockedMouse;
        }

        // contrast,gamma,brightness values
        Contrast  =0.5f;
        Gamma     =1.5f;
        Brightness=0.5f;

        const char* s;
        s=GConfig.GetString("Video","Contrast"  ); if(s) Contrast  =float(atof(s));
        s=GConfig.GetString("Video","Gamma"     ); if(s) Gamma     =float(atof(s));
        s=GConfig.GetString("Video","Brightness"); if(s) Brightness=float(atof(s));

        mCamera.Fov=90.0f;
        mCamera.Position=Vector(0,0,-120);
        mCamera.Rotation=Vector(0,0,0);

        mViewport.SetWindow(renderWnd->GetHandle());

        /*
        Texture texInfo;
        texInfo.Name="../Data/atten.tga";
        LogoTexRef=GEngine->GetDevice()->PrecacheTexture(texInfo);
        */

#if 0
		s=GConfig.GetString("Fog","FogRed"); if(s) FogColor[0] = float(atof(s));
		s=GConfig.GetString("Fog","FogGreen"); if(s) FogColor[1] = float(atof(s));
		s=GConfig.GetString("Fog","FogBlue"); if(s) FogColor[2] = float(atof(s));

		s=GConfig.GetString("Fog","Start"); if(s) FogStart = float(atof(s));
		s=GConfig.GetString("Fog","End"); if(s) FogEnd = float(atof(s));
		s=GConfig.GetString("Fog","Density"); if(s) FogDensity = float(atof(s));
		s=GConfig.GetString("Fog","Mode"); if(s) FogMode= atoi(s);

		switch(FogMode)
		{
		case 0: FogMode=RenderState::FogState::EFogExp; break;
		case 1: FogMode=RenderState::FogState::EFogExp2; break;
		case 2: FogMode=RenderState::FogState::EFogLinear; break;
		}
#endif

        return TRUE;
    }
    return FALSE;
    unguard;
}

void DemoGame::LoadScreen()
{
    guard(Demo::LoadScreen);

    RenderDevice* dev=GEngine->GetDevice();
    Assert(dev);

    mCamera.Tick();
    mViewport.Tick(mCamera);

    if(dev->Lock(mViewport)) {

        /*
        const char* str="Loading...";
        mHUD.SetFont(1);
        int width =mHUD.GetTextWidth(str);
        int height=mHUD.GetTextHeight();

        RenderState state(RenderState::ETexEnv|RenderState::EConstantColor);
        state.TexFlags[0]=RenderState::ETexOpModulate|RenderState::ETexArgColor;
        state.ConstantColor=0xff;
        dev->SetState(state);

        mHUD.SetRenderState();
        RenderWindow* renderWnd=GEngine->GetRenderWindow();
        mHUD.Print((renderWnd->GetWidth()-width)/2,(renderWnd->GetHeight()-height)/2,str);
        */

        mHUD.SetRenderState();
        RenderWindow* renderWnd=GEngine->GetRenderWindow();

        int renderWidth, renderHeight;
        renderWnd->GetSizes(renderWidth,renderHeight);

        int w=256,h=256;
        mHUD.DrawQuad(LogoTexRef,
                      (renderWidth -w)/2,
                      (renderHeight-h)/2,
                      w,h);

        dev->Unlock();
		dev->Flip(mViewport);

        /*
        // set normal state for rendering base+lmap
        state.Flags=RenderState::ETexEnv;
        state.TexFlags[0]=RenderState::ETexOpTexture;
        state.TexFlags[1]=RenderState::ETexOpModulate;  // IMPLICIT: modulate with FB
        dev->SetState(state);
        */

        // set default font
        mHUD.SetFont(0);
    } 

    unguard;
}

static FLOAT WorldScale=1.0f;

BOOL DemoGame::Initialize()
{
    guard(DemoGame::Intialize);

    if(!InitRenderer()) {
        return FALSE;
    }

    // create physics engine
    mPhysics=new PhysicsEngine();
    mPhysics->Init();
    mPhysics->CreatePlayer(Vector(0,1024,0));

    // read some stuff from ini
    const char* section="SpawnPoint";
    const char* s;
    s=GConfig.GetString(section,"X"); if(s) mCamera.Position.X=float(atof(s));
    s=GConfig.GetString(section,"Y"); if(s) mCamera.Position.Y=float(atof(s));
    s=GConfig.GetString(section,"Z"); if(s) mCamera.Position.Z=float(atof(s));

    section="General";
    s=GConfig.GetString(section,"FlyMove"); if(s && !stricmp("true",s)) Mode|=EModeFlyMove;

    // start shit
    RenderDevice* dev=GEngine->GetDevice();

    // setup HUD
    if(!mHUD.Create(dev, 1024)) {
        GLog.Print("ERROR: Could not initialize HUD\n");
        return FALSE;
    }

    LoadScreen();
        
    // setup world
    section="DemoGame";
    const char* mapName=GConfig.GetString(section,"Map");
    if(mapName) {
        const char* scale=GConfig.GetString(section,"Scale");
        if(scale) {
            WorldScale=(float)atof(scale);
            mWorld.ObjectToWorld=ScaleMatrix(WorldScale);
        }
        String tmp(mapName);
        if(!mWorld.LoadLevel((String("../Data/Maps/")+tmp).AsChar())) {
            GLog.Print("WARNING: couldnt load map: %s\n",mapName);
        }
    }

    // init physics world
    mPhysics->InitWorld(mWorld);
    mPhysics->SetPlayerPos(0,mCamera.Position);            

    // vertex buffer for lines (debug zone/wireframe view)
    if(!VBuf.Create(dev)) {
        GLog.Print("DemoGame: ERROR Failed to create line vertex buffer\n");
        return FALSE;
    }

    // particle system
    if(!mParticleSystem.Init()) {
        GLog.Print("DemoGame: ERROR particles init failed\n");
        return FALSE;
    }

    if(!mWorld.Precache(dev)) {
        return FALSE;
    }

    // warm up things
    for(int i=0; i<1000; ++i) {
        mParticleSystem.Tick(1.0f/120.0f);
    }

    mPhysics->Start();

    Vector pos=mCamera.Position;
    mCamera.Position=Vector(0,4096,0);
    mCamera.ViewMatrix=LookAtMatrix(mCamera.Position,Vector(0,0,0),Vector(0,1,0));
    mViewport.ClearColor=0;
    mViewport.Tick(mCamera);

    if(dev->Lock(mViewport)) {
        RenderWorld();
        dev->Unlock();
    }

    mCamera.Position=pos;
    return TRUE;

    unguard;
}

static void AddBox(Vector*& v, const BoundingBox& box)
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

void DemoGame::RenderZones()
{
    guard(DemoGame::RenderZones);

    RenderDevice* dev=GEngine->GetDevice();
    Assert(dev);

    /*
    RenderState state;
    state.Flags=RenderState::ETexEnv
               |RenderState::EConstantColor
               |RenderState::EStreamSource;

    state.TexFlags[0]=RenderState::ETexOpColor;
    state.TexFlags[1]=RenderState::ETexOpDisable;
    state.StreamSource =VBuf.GetStream();
    */

    /*
    TextureState st[2];
    st[0].Set(0,TextureState::ETexOpColor);
    st[1].Set(1,TextureState::ETexOpDisable);
    dev->SetTextureState(st,2);
    */
    TextureState st;
    st.Set(0,TextureState::ETexOpColor);
    dev->SetTextureState(&st,1);

    dev->SetStream(VBuf.GetStream());
    dev->SetObjectTransform(GIdentityMatrix);

    // render all zones
    for(int index=0; index<mWorld.Zones.Size(); ++index) {
        const Zone& zone=mWorld.Zones[index];
        Vector* v=VBuf.Lock(6*8);
        AddBox(v,zone.Box);

        // highlight current zone with different color
        /*
        state.ConstantColor=(index==CurrentZone ? 0xff00 : 0xff);
        dev->SetState(state);
        */
        dev->SetConstColor(index==CurrentZone ? 0xff00 : 0xff);
        dev->Draw(RenderDevice::EPrimLineList,VBuf.Unlock(),(6*8)/2);
    }

    if(Mode & EModeZonePortals) {

        const Zone& curZone=mWorld.Zones[CurrentZone];
        Vector* v=VBuf.Lock(curZone.PortalRefs.Size()*8);

        for(int index=0; index<curZone.PortalRefs.Size(); ++index) {
            int portalIndex=curZone.PortalRefs[index];
            const Portal& portal=mWorld.Portals[portalIndex];

		    //BoundingBox Box=portal.Box;
		    //Box+=portal.Poly.Verts[0]+mWorld.Planes[portal.Poly.PlaneIndex].Normal*mWorld.mPortalThicknessTolerance;
		    //Box+=portal.Poly.Verts[0]-mWorld.Planes[portal.Poly.PlaneIndex].Normal*mWorld.mPortalThicknessTolerance;
            //AddBox(v,Box);

            v[0]=portal.Poly.Verts[0];
            v[1]=portal.Poly.Verts[1];
            v[2]=portal.Poly.Verts[1];
            v[3]=portal.Poly.Verts[2];
            v[4]=portal.Poly.Verts[2];
            v[5]=portal.Poly.Verts[3];
            v[6]=portal.Poly.Verts[3];
            v[7]=portal.Poly.Verts[0];
            v+=8;

        }

        /*
        state.ConstantColor=0xff0000;
        dev->SetState(state);
        */
        dev->SetConstColor(0xff0000);
        dev->Draw(RenderDevice::EPrimLineList,VBuf.Unlock(),8*curZone.PortalRefs.Size()/2);
    }
    else {
        Vector* v=VBuf.Lock(8*mWorld.Portals.Size());

        for(int index=0; index<mWorld.Portals.Size(); ++index) {
            const Portal& portal=mWorld.Portals[index];

            v[0]=portal.Poly.Verts[0];
            v[1]=portal.Poly.Verts[1];
            v[2]=portal.Poly.Verts[1];
            v[3]=portal.Poly.Verts[2];
            v[4]=portal.Poly.Verts[2];
            v[5]=portal.Poly.Verts[3];
            v[6]=portal.Poly.Verts[3];
            v[7]=portal.Poly.Verts[0];
            v+=8;

        }

        /*
        state.ConstantColor=0xff0000;
        dev->SetState(state);
        */
        dev->SetConstColor(0xff0000);
        dev->Draw(RenderDevice::EPrimLineList,VBuf.Unlock(),8*mWorld.Portals.Size()/2);
    }

    unguard;
}

void DemoGame::RenderNormals()
{
    guard(DemoGame::RenderNormals);
    RenderDevice* dev=GEngine->GetDevice();

    BYTE* ptr=dev->LockStream(VBuf.GetStream(),0,0);
    Vector* v=(Vector*)ptr;

    /*
    RenderState state;
    state.Flags=RenderState::ETexEnv|RenderState::EConstantColor|RenderState::EStreamSource;
    state.TexFlags[0]=RenderState::ETexOpColor;
    state.TexFlags[1]=RenderState::ETexOpDisable;
    state.ConstantColor=0xff0000;
    state.StreamSource =VBuf.GetStream();
    dev->SetState(state);
    */

    TextureState st[2];
    st[0].Set(0,TextureState::ETexOpColor);
    st[1].Set(1,TextureState::ETexOpDisable);
    dev->SetTextureState(st,2);

    dev->SetConstColor(0xff0000);
    dev->SetStream(VBuf.GetStream());

    int vertexCount=0;

    for(int index=0; index<mWorld.Meshes.Size(); ++index) {
        const Mesh& mesh=mWorld.Meshes[index];
        for(int i=0; i<mesh.Normals.Size(); ++i) {

            vertexCount+=2;
            if(vertexCount>=VBuf.GetMaxElems()) {
                dev->UnlockStream(VBuf.GetStream());
                dev->Draw(RenderDevice::EPrimLineList,0,(vertexCount-2)/2);

                ptr=dev->LockStream(VBuf.GetStream(),0,0);
                v=(Vector*)ptr;
                vertexCount=2;
            }

            v[0]=mesh.Verts[i].Pos;
            v[1]=mesh.Verts[i].Pos+mesh.Normals[i]*2.0f;

            v+=2;
        }
    }

    dev->UnlockStream(VBuf.GetStream());
    dev->Draw(RenderDevice::EPrimLineList,0,vertexCount/2);
    unguard;
}

void DemoGame::RenderTriangles()
{
    guard(DemoGame::RenderTriangles);
    RenderDevice* dev=GEngine->GetDevice();

    mWorld.Occlude(mViewport,(Mode & EModePortals));

    /*
    RenderState state;
    state.Flags=RenderState::ETexEnv|RenderState::EConstantColor|RenderState::EStreamSource;
    state.TexFlags[0]=RenderState::ETexOpColor;
    state.TexFlags[1]=RenderState::ETexOpDisable;
    state.ConstantColor=0xff0000;
    state.StreamSource =VBuf.GetStream();
    dev->SetState(state);
    */

    /*
    TextureState st[2];
    st[0].Set(0,TextureState::ETexOpColor);
    st[1].Set(1,TextureState::ETexOpDisable);
    dev->SetTextureState(st,2);
    */

    dev->SetColorMode();
    dev->SetConstColor(0xff0000);
    dev->SetStream(VBuf.GetStream());

    // lock entire buffer
    Vector* v=(Vector*)dev->LockStream(VBuf.GetStream(),0,0);

    int vertexCount=0;

    //for(int index=0; index<mWorld.Meshes.GetCount(); ++index) {
    //    const Mesh& mesh=mWorld.Meshes[index];

    bool identityXForm=true;
    dev->SetObjectTransform(GIdentityMatrix);

    for(List<Mesh>::Item* item=mWorld.MeshList.ListFront; item; item=item->Next) {

        Mesh& mesh=*item->Ref;

        if(mesh.Flags & EMeshPhysical) {
            /*
            state.Flags=RenderState::ETransformObject;
            state.ObjectToWorld=mesh.ObjectMatrix;
            dev->SetState(state);
            */
            dev->SetObjectTransform(&mesh.ObjectMatrix);
            identityXForm=false;
        }
        else {
            if(false==identityXForm) {
                /*
                state.Flags=RenderState::ETransformObject;
                state.ObjectToWorld=IdentityMatrix();
                dev->SetState(state);
                */

                dev->SetObjectTransform(GIdentityMatrix);
                identityXForm=true;
            }
        }

        const int triCount=mesh.Triangles.Size()/3;
        for(int i=0; i<triCount; ++i) {

            vertexCount+=6;
            if(vertexCount>=VBuf.GetMaxElems()) {
                dev->UnlockStream(VBuf.GetStream());
                dev->Draw(RenderDevice::EPrimLineList,0,(vertexCount-6)/2);

                v=(Vector*)dev->LockStream(VBuf.GetStream(),0,0);
                vertexCount=6;
            }

            v[0]=mesh.Verts[mesh.Triangles[i*3+0]].Pos;
            v[1]=mesh.Verts[mesh.Triangles[i*3+1]].Pos;
            v[2]=mesh.Verts[mesh.Triangles[i*3+1]].Pos;
            v[3]=mesh.Verts[mesh.Triangles[i*3+2]].Pos;
            v[4]=mesh.Verts[mesh.Triangles[i*3+2]].Pos;
            v[5]=mesh.Verts[mesh.Triangles[i*3+0]].Pos;
            
            v+=6;
        }
    }

    dev->UnlockStream(VBuf.GetStream());
    dev->Draw(RenderDevice::EPrimLineList,0,vertexCount/2);
    unguard;
}

void DemoGame::RenderTriangles(const Mesh& mesh)
{
    guard(DemoGame::RenderTriangles);
    RenderDevice* dev=GEngine->GetDevice();

    // lock entire buffer
    Vector* v=(Vector*)dev->LockStream(VBuf.GetStream(),0,0);
    int vertexCount=0;

    /*
    RenderState state;
    state.Flags=RenderState::ETexEnv|RenderState::EConstantColor|RenderState::EStreamSource;
    state.TexFlags[0]=RenderState::ETexOpColor;
    state.TexFlags[1]=RenderState::ETexOpDisable;
    state.ConstantColor=0xff0000;
    state.StreamSource =VBuf.GetStream();

    //state.Flags=RenderState::ETransformObject;
    //state.ObjectToWorld=(mesh.Flags & EMeshPhysical) ? mesh.ObjectMatrix : IdentityMatrix();
    dev->SetState(state);
    */


    dev->SetColorMode();
    dev->SetConstColor(0xff0000);
    dev->SetObjectTransform((mesh.Flags & EMeshPhysical) ? &mesh.ObjectMatrix : GIdentityMatrix);
    dev->SetStream(VBuf.GetStream());

    const int triCount=mesh.Triangles.Size()/3;
    for(int i=0; i<triCount; ++i) {

        vertexCount+=6;
        if(vertexCount>=VBuf.GetMaxElems()) {
            dev->UnlockStream(VBuf.GetStream());
            dev->Draw(RenderDevice::EPrimLineList,0,vertexCount/2);

            v=(Vector*)dev->LockStream(VBuf.GetStream(),0,0);
            vertexCount=6;
        }

        v[0]=mesh.Verts[mesh.Triangles[i*3+0]].Pos;
        v[1]=mesh.Verts[mesh.Triangles[i*3+1]].Pos;
        v[2]=mesh.Verts[mesh.Triangles[i*3+1]].Pos;
        v[3]=mesh.Verts[mesh.Triangles[i*3+2]].Pos;
        v[4]=mesh.Verts[mesh.Triangles[i*3+2]].Pos;
        v[5]=mesh.Verts[mesh.Triangles[i*3+0]].Pos;
            
        v+=6;
    }

    dev->UnlockStream(VBuf.GetStream());
    dev->Draw(RenderDevice::EPrimLineList,0,vertexCount/2);
    unguard;
}

static int TriangleCount=0;
static int PortalCount;
static int ObjectCount;

void DemoGame::RenderWorld()
{
    TriangleCount=0;
    PortalCount=0;
    ObjectCount=0;

    RenderDevice* dev=GEngine->GetDevice();
    Assert(dev);

#if 0
    // set rendering state for drawing world
    RenderState state(RenderState::ETexEnv
                     |RenderState::ECullCCW
                     |RenderState::EDepthWrite
                     |RenderState::EDepthTest
                     |RenderState::EBlendNone);

    switch(RenderMode) {
    case 2: // lightmaps only
        //state.Flags|=RenderState::EConstantColor;
        //state.ConstantColor=0xff0000;

        state.TexFlags[0]=RenderState::ETexOpColor;
        state.TexFlags[1]=RenderState::ETexOpTexture;
        break;

    case 1: // textures only
        state.TexFlags[0]=RenderState::ETexOpTexture|RenderState::ETexFilterBilinear;
        state.TexFlags[1]=RenderState::ETexOpDisable;
        break;

    case 0: // normal
    default:
        state.TexFlags[0]=RenderState::ETexOpTexture|RenderState::ETexFilterBilinear;
        state.TexFlags[1]=RenderState::ETexOpModulate;  // IMPLICIT: modulate with FB
        break;
    }

    state.DepthWrite=TRUE;
    state.DepthTest =TRUE;
    dev->SetState(state);
#endif

    //TextureState st0(0),st1(1);
    TextureState st[2];

    switch(RenderMode) {
    case 2: // lightmaps only
        //state.Flags|=RenderState::EConstantColor;
        //state.ConstantColor=0xff0000;

        dev->SetConstColor(0xff0000);
        st[0].Set(0,TextureState::ETexOpColor);
        st[1].Set(1,TextureState::ETexOpTexture);
        break;

    case 1: // textures only
        st[0].Set(0,TextureState::ETexOpTexture,0,TextureState::ETexFilterBilinear);
        st[1].Set(1,TextureState::ETexOpDisable);
        break;

    case 0: // normal
    default:
        st[0].Set(0,TextureState::ETexOpTexture,0,TextureState::ETexFilterBilinear);
        st[1].Set(1,TextureState::ETexOpModulate,TextureState::ETexArgFB);
        break;
    }

    dev->SetTextureState(st,2);

    RenderState state;
    state.SetCullMode(RenderState::ECullCCW);
    state.SetBlendMode(RenderState::EBlendNone);
    state.SetDepthTest(true);
    state.SetDepthWrite(true);

    dev->SetState(state);

/*
    state.Flags=RenderState::ETexTransform;
    state.DiffuseMap = LogoTexRef;
    state.TextureTransform = TRUE;
    dev->SetState(state);
*/

    mWorld.Occlude(mViewport,(Mode & EModePortals));

    // portal stats
    if(Mode & EModePortals) {
        for(int i=0; i<mWorld.Portals.Size(); ++i) {
            if(mWorld.VisitedPortals.Test(i)) {
                ++PortalCount;
            }
        }
    }

    Mesh* trans[1024];
    int translucentCount=0;

    dev->SetObjectTransform(GIdentityMatrix);
    bool identityXForm=true;

    for(List<Mesh>::Item* item=mWorld.MeshList.ListFront; item; item=item->Next) {

        Mesh& mesh=*item->Ref;

        if(EInvalidIndex!=mesh.VertexStream) {

            if(mesh.Flags & EMeshTranslucent) {
                trans[translucentCount++]=&mesh;
                continue;
            }

            /*
            RenderState state(RenderState::EStreamSource|RenderState::ETransformObject);
            state.StreamSource=mesh.RenderStreamSrc;
            state.ObjectToWorld=mesh.Flags & EMeshPhysical ? mesh.ObjectMatrix : IdentityMatrix();
            */

            
            /*
            RenderState state(RenderState::EStreamSource);
            state.StreamSource=mesh.RenderStreamSrc;
            */

            dev->SetIndexedStream(mesh.VertexStream,mesh.IndexStream);

            if(mesh.Flags & EMeshPhysical) {
                /*
                state.Flags|=RenderState::ETransformObject;
                state.ObjectToWorld=mesh.ObjectMatrix;
                */
                dev->SetObjectTransform(&mesh.ObjectMatrix);
                identityXForm=false;
            }
            else {
                if(false==identityXForm) {
                    /*
                    state.Flags|=RenderState::ETransformObject;
                    state.ObjectToWorld=IdentityMatrix();
                    */
                    dev->SetObjectTransform(GIdentityMatrix);
                    identityXForm=true;
                }
            }
            

            for(int i=0; i<mesh.Sections.Size(); ++i) {
                Mesh::Section& section=mesh.Sections[i];

                /*
                RenderState state(RenderState::EDiffuseMap|RenderState::ELightMap);
                state.DiffuseMap=mWorld.Materials[section.MaterialRef].TexRef;
                state.LightMap  =mesh.LMapIndex==EInvalidIndex ? EInvalidIndex : mWorld.Materials[mesh.LMapIndex].TexRef;
                dev->SetState(state);
                */

                /*
                TextureState st0(0,mWorld.Materials[section.MaterialRef].TexRef);
                TextureState st1(1,mesh.LMapIndex==EInvalidIndex ? 
                                   EInvalidIndex                 : 
                                   mWorld.Materials[mesh.LMapIndex].TexRef);

                dev->SetTextureState(&st0,2);                                   
                */

                TextureState st[2];
                st[0]=TextureState(0,mWorld.Materials[section.MaterialRef].TexRef);
                st[1]=TextureState(1,mesh.LMapIndex==EInvalidIndex ? 
                                     EInvalidIndex                 : 
                                     mWorld.Materials[mesh.LMapIndex].TexRef);

                dev->SetTextureState(st,2);

                dev->Draw(RenderDevice::EPrimTriStrip,section.RenderBase,section.RenderCount);
            }

            // stats
            ++ObjectCount;
            TriangleCount+=mesh.Triangles.Size()/3;
        }
    }

    if(translucentCount>0) {

        /*
        RenderState state(RenderState::EBlendTranslucent|RenderState::EDepthWrite);
        state.DepthWrite=FALSE;
        dev->SetState(state);
        */

        RenderState state;
        state.SetDepthWrite(false);
        state.SetBlendMode(RenderState::EBlendTranslucent);
        dev->SetState(state);

        // reverse order to render back to front
        for(int i=translucentCount-1; i>=0; --i) {
            Mesh& mesh=*trans[i];

            //RenderState state(RenderState::EStreamSource);
            //state.StreamSource=mesh.RenderStreamSrc;
            
            /*
            RenderState state(RenderState::EStreamSource|RenderState::ETransformObject);
            state.StreamSource=mesh.RenderStreamSrc;
            state.ObjectToWorld=mesh.Flags & EMeshPhysical ? mesh.ObjectMatrix : IdentityMatrix();
            */

            dev->SetIndexedStream(mesh.VertexStream,mesh.IndexStream);

            state.Flags=0;
            state.SetObjectTransform(mesh.Flags & EMeshPhysical ? &mesh.ObjectMatrix : GIdentityMatrix);
            dev->SetState(state);

            for(int i=0; i<mesh.Sections.Size(); ++i) {
                Mesh::Section& section=mesh.Sections[i];

                /*
                RenderState state(RenderState::EDiffuseMap|RenderState::ELightMap);
                state.DiffuseMap=mWorld.Materials[section.MaterialRef].TexRef;
                state.LightMap  =mesh.LMapIndex==EInvalidIndex ? EInvalidIndex : mWorld.Materials[mesh.LMapIndex].TexRef;
                dev->SetState(state);
                */

                TextureState st[2];
                st[0]=TextureState(0,mWorld.Materials[section.MaterialRef].TexRef);
                st[1]=TextureState(1,mesh.LMapIndex==EInvalidIndex ? 
                                     EInvalidIndex                 :
                                     mWorld.Materials[mesh.LMapIndex].TexRef);

                dev->SetTextureState(st,2);

                dev->Draw(RenderDevice::EPrimTriStrip,section.RenderBase,section.RenderCount);
            }
        }

        /*
        state.Flags=RenderState::EBlendNone|RenderState::EDepthWrite;
        state.DepthWrite=TRUE;
        dev->SetState(state);
        */
        state.Flags=0;
        state.SetDepthWrite(true);
        state.SetBlendMode(RenderState::EBlendNone);
        dev->SetState(state);
    }

    dev->SetObjectTransform(GIdentityMatrix);

#if 0
    // avoid weirdness and reset world matrix to known state!
    // also, turn off second stage
    state.Flags=RenderState::ETransformObject|RenderState::ETexEnv;
    state.TexFlags[0]=RenderState::ETexOpModulate;
    state.TexFlags[1]=RenderState::ETexOpDisable;
    state.ObjectToWorld=IdentityMatrix();

    dev->SetState(state);
#endif

/////////////////////////
/*
    state.Flags=RenderState::ETexTransform;
    state.DiffuseMap = LogoTexRef;
    state.TextureTransform = TRUE;
    dev->SetState(state);
*/
}

const Mesh* DemoGame::PickObject()
{
    POINT pt;
    GetCursorPos( &pt );
    ScreenToClient( GEngine->GetRenderWindow()->GetHandle(), &pt );
    float x=float(pt.x);
    float y=float(pt.y);

    // Compute the vector of the pick ray in screen space
    Vector v;
    v.X =  ( ( ( 2.0f * x ) / mViewport.Width  ) - 1 ) / mViewport.CameraToScreen.A00;
    v.Y = -( ( ( 2.0f * y ) / mViewport.Height ) - 1 ) / mViewport.CameraToScreen.A11;
    v.Z =  1.0f;

    // Transform the screen space pick ray into 3D space
    RaySegment ray;
    Matrix CameraToWorld=mViewport.WorldToCamera.Inverse();
    ray.Dir    = CameraToWorld.TransformNormal(v);
    ray.Origin = CameraToWorld.GetTranslation();

    float dist=FLT_MAX;
    const Mesh* MPtr=0;

    for(List<Mesh>::Item* item=mWorld.MeshList.ListFront; item; item=item->Next) {

        Mesh& mesh=*item->Ref;

        const int triCount=mesh.Triangles.Size()/3;
        for(int i=0; i<triCount; ++i) {
            const WORD e0=mesh.Triangles[i*3+0];
            const WORD e1=mesh.Triangles[i*3+1];
            const WORD e2=mesh.Triangles[i*3+2];

            const Vector& v0=mesh.Verts[e0].Pos;
            const Vector& v1=mesh.Verts[e1].Pos;
            const Vector& v2=mesh.Verts[e2].Pos;

            float d;
            if(ray.IntersectTriangle(v0,v1,v2,d)) {
                if(!MPtr || d<dist) {
                    dist=d;
                    MPtr=&mesh;
                }
                //return &mesh;
            }
        }
    }

    return MPtr;
    //return NULL;
}

void DemoGame::ReadInput()
{
    RenderDevice* dev=GEngine->GetDevice();
    Assert(dev);

    mInputSystem->Tick();

    /*
    InputSystem::KeyInfoItem* keys=mInputSystem->GetActiveKeyList();
    if(keys)
    {
        for(InputSystem::KeyInfoItem* key=keys; key; key=key->Next)
        {
            GLog.Print("%d=%d ",key->Key,key->Status);
        }
        GLog.Print("\n");
    }
    */

    for(InputSystem::KeyInfoItem* key=mInputSystem->GetActiveKeyList(); key; key=key->Next)
    {
        if(key->Status==EKeyPressed)
        {
            // gamma delta
            const float d=0.05f;

            switch(key->Key)
            {
            case VK_ESCAPE:        
                GEngine->Close();
                break;
            case 'M':
                Mode^=EModeLockedMouse;
                //mInputSystem->LockMouse((Mode & EModeLockedMouse));
				mInputSystem->LockMouse((Mode & EModeLockedMouse));
                break;
            // gamma settings
            case '1':
                Contrast-=d;
                dev->SetGamma(Contrast,Gamma,Brightness);
                break;
            case '2':
                Contrast+=d;
                dev->SetGamma(Contrast,Gamma,Brightness);
                break;
            case '3':
                Gamma-=d;
                dev->SetGamma(Contrast,Gamma,Brightness);
                break;
            case '4':
                Gamma+=d;
                dev->SetGamma(Contrast,Gamma,Brightness);
                break;
            case '5':
                Brightness-=d;
                dev->SetGamma(Contrast,Gamma,Brightness);
                break;
            case '6':
                Brightness+=d;
                dev->SetGamma(Contrast,Gamma,Brightness);
                break;
            // toggles
            case 'Z':
                Mode^=EModeOnlyZones;
                break;
            case 'P':
                Mode^=EModeZonePortals;
                break;
            case 'N':
                ++CurrentZone;
                if(CurrentZone>=mWorld.Zones.Size()) {
                    CurrentZone=0;
                }
                break;
            case 'V':
                Mode^=EModePortals;
                break;
            case 'T':
                Mode^=EModeShowTris;
                break;
            case 'H':
                Mode^=EModeShowHUD;
                break;
            case 'F':
                if(Mode & EModeFlyMove) { // switching to physics
                    mPhysics->SetPlayerPos(0,mCamera.Position);            
                } else {
                    mCamera.Position=mPhysics->GetPlayerPos(0);
                }

                Mode ^= EModeFlyMove;
                break;
            case 'R':
                ++RenderMode;
                if(RenderMode>=3) {
                    RenderMode=0;
                }
                break;
            case '8':
                GEngine->InitRenderWindow
                (
                    RenderDeviceManager::ED3DRenderDevice,
                    GEngine->GetRenderWindow()->GetHandle(),
                    640,480,
                    !GEngine->IsFullScreen()
                );
                break;
            case '9':
                GEngine->InitRenderWindow
                (
                    RenderDeviceManager::ED3DRenderDevice,
                    GEngine->GetRenderWindow()->GetHandle(),
                    800,600,
                    !GEngine->IsFullScreen()
                );
                break;
            case '0':
                GEngine->InitRenderWindow
                (
                    RenderDeviceManager::ED3DRenderDevice,
                    GEngine->GetRenderWindow()->GetHandle(),
                    1024,768,
                    !GEngine->IsFullScreen()
                );
                break;
            }
        }
    }

/*
    if(mInputSystem->KeyQueueSize>0) {
        // gamma delta
        const float d=0.05f;

        for(int i=0; i<mInputSystem->KeyQueueSize; ++i) {
            switch(mInputSystem->KeyQueue[i]) {
            case VK_ESCAPE:        
                GEngine->Close();
                break;
            case 'M':
                Mode^=EModeLockedMouse;
                mInputSystem->LockMouse((Mode & EModeLockedMouse));
                break;
            // gamma settings
            case '1':
                Contrast-=d;
                dev->SetGamma(Contrast,Gamma,Brightness);
                break;
            case '2':
                Contrast+=d;
                dev->SetGamma(Contrast,Gamma,Brightness);
                break;
            case '3':
                Gamma-=d;
                dev->SetGamma(Contrast,Gamma,Brightness);
                break;
            case '4':
                Gamma+=d;
                dev->SetGamma(Contrast,Gamma,Brightness);
                break;
            case '5':
                Brightness-=d;
                dev->SetGamma(Contrast,Gamma,Brightness);
                break;
            case '6':
                Brightness+=d;
                dev->SetGamma(Contrast,Gamma,Brightness);
                break;
            // toggles
            case 'Z':
                Mode^=EModeOnlyZones;
                break;
            case 'P':
                Mode^=EModeZonePortals;
                break;
            case 'N':
                ++CurrentZone;
                if(CurrentZone>=mWorld.Zones.Size()) {
                    CurrentZone=0;
                }
                break;
            case 'V':
                Mode^=EModePortals;
                break;
            case 'T':
                Mode^=EModeShowTris;
                break;
            case 'H':
                Mode^=EModeShowHUD;
                break;
            case 'F':
                if(Mode & EModeFlyMove) { // switching to physics
                    mPhysics->SetPlayerPos(0,mCamera.Position);            
                } else {
                    mCamera.Position=mPhysics->GetPlayerPos(0);
                }

                Mode ^= EModeFlyMove;
                break;
            case 'R':
                ++RenderMode;
                if(RenderMode>=3) {
                    RenderMode=0;
                }
                break;
            case '0':
                GEngine->InitRenderWindow
                (
                    RenderDeviceManager::ED3DRenderDevice,
                    GEngine->GetRenderWindow()->GetHandle(),
                    640,480,
                    !GEngine->IsFullScreen()
                );
                break;
            }
        }

        mInputSystem->KeyQueueSize=0;
    }   
*/
}

void DemoGame::Tick(float timeDelta)
{
    guard(DemoGame::Tick);

#if 0
    mViewport.SetWindow(GEngine->GetRenderWindow()->GetHandle());
    RenderDevice* dev=GEngine->GetDevice();

    mCamera.Tick();

	mViewport.ClearColor=0;
    mViewport.Tick(mCamera);

    mParticleSystem.Tick(timeDelta);

    if(dev->Lock(mViewport)) {

            RenderWorld();
            mParticleSystem.Render(mCamera);

            TextureState st0(0),st1(1);
            st0.Op =TextureState::ETexOpColor;
            //st0.Arg=TextureState::ETexArgColor;
            st0.FilterAddr=TextureState::ETexFilterPoint;

            st1.Op=TextureState::ETexOpDisable;

            dev->SetTextureState(&st0,2);

            /*
            TextureState st[2];
            st[0].Stage=0;
            st[0].Op   =TextureState::ETexOpModulate;
            st[0].Arg  =TextureState::ETexArgColor;
            st[1].Op   =TextureState::ETexOpDisable;

            dev->SetTextureState(st,ARRAY_COUNT(st));            
            */

            mHUD.SetRenderState();
            mHUD.SetFont(0);

            dev->SetConstColor(0xff00);
            mHUD.Print(2,2,"DUPA");
            mHUD.Print(2,32,"Wow this seems to work...");

        dev->Unlock();
		dev->Flip(mViewport);
    }
#endif

//#if 0
    // calc FPS
    static float t=0;
    static float FPS=0;
    static int   frameCount=0;

    ++frameCount;
    t+=timeDelta;
    if(t>=1.0f) { // a second passed
        FPS=float(frameCount)/t;
        frameCount=0;
        t=0;
    }


    //mViewport.Resize(GEngine->GetRenderWindow());
    //mViewport.SetView(0,0,1280,960);
    mViewport.SetWindow(GEngine->GetRenderWindow()->GetHandle());

    RenderDevice* dev=GEngine->GetDevice();
    Assert(dev);

    ReadInput();


    if(Mode & EModeLockedMouse) {
        mCamera.Rotation+=mInputSystem->GetMouseDelta();
        // clamp vertical rotation
        if(mCamera.Rotation.Y>+80.0f) mCamera.Rotation.Y=+80.0f;
        if(mCamera.Rotation.Y<-80.0f) mCamera.Rotation.Y=-80.0f;
    }
    else {
        /*
        if(mInputSystem->GetMouseStatus() & 1) {
            POINT pt=mInputSystem->GetMousePosition();
            PickObject(float(pt.x),float(pt.y));
        }
        */
    }

    static char physDebug[256]="0";
    const DWORD Action=mInputSystem->GetActionStatus();

    if(Mode & EModeFlyMove) {
        const float speed=64.0f*WorldScale;
        Vector move(0,0,0);
        if(Action & (1<<EInputMoveForward)) {
            move+=mCamera.GetForwardVector();
        }
        if(Action & (1<<EInputMoveBackward)) {
            move+=-mCamera.GetForwardVector();
        }
        if(Action & (1<<EInputMoveLeft)) {
            move+=-mCamera.GetRightVector();
        }
        if(Action & (1<<EInputMoveRight)) {
            move+=mCamera.GetRightVector();
        }

        if(!move.AlmostZero()) {
            mCamera.Position+=move*speed*timeDelta;
        }
    }
    else {
        mPhysics->PlayerAction(0,mCamera.ViewMatrix,Action,physDebug);
    }

    // tick stuff
    mPhysics->Tick(timeDelta);

    // update camera
    if(!(Mode & EModeFlyMove)) {
        mCamera.Position=mPhysics->GetPlayerPos(0);

        /*
        static float z=0;
        static float sz1=0;
        static float sz2=0;

        if(Action) {
            
            sz1=GSinFloat(z*18);
            sz2=GSinFloat(z*10);
            z+=timeDelta;                        
        }        
        
        static bool jumpin=false;
        if(Action & (1<<EInputJump)) {
            jumpin=true;
        }
        //else
        if(!mPhysics->IsPlayerFalling(0)) {
            //if(jumpin) z=0;
            jumpin=false;
        }

        if(!jumpin) {
            mCamera.Position.Y+=sz1;
            mCamera.Position+=mCamera.GetRightVector()*sz2;
        }
        else
            z=0;
        */
    }

    mCamera.Tick();
    mParticleSystem.Tick(timeDelta);

    //mViewport.ClearColor=(Mode & (EModeOnlyZones|EModeShowTris)) ? 0 : 0xff1a1a9a;
	mViewport.ClearColor=ColorAsDWORD(FogColor);
    mViewport.Tick(mCamera);

    if(Action & (1<<EInputFire)) {

        sprintf(physDebug,"-");

        HitData hitData;
        BOOL hit=mPhysics->LineTrace(mCamera.Position,mCamera.Position+mCamera.GetForwardVector()*640,hitData);
        if(hit) {
            sprintf(physDebug,"X");
        }
    }

    if(Action & (1<<EInputAltFire)) {
    }

    // debug
    static const Mesh* PickedMesh=0;

    // render
    if(dev->Lock(mViewport)) {

        if(Mode & EModeOnlyZones) {
            RenderZones();
        }
        else {
            if(Mode & EModeShowTris) {
                RenderTriangles();
            }
            else {

                RenderWorld();

                /*
				RenderState state(RenderState::ESetFog);
				state.Fog.Enabled = TRUE;

				state.Fog.Mode = FogMode;
				state.Fog.Density = FogDensity;
				state.Fog.Color = ColorAsDWORD(FogColor);
				state.Fog.Start = FogStart;
				state.Fog.End = FogEnd;

				dev->SetState(state);

                RenderWorld();

				state.Fog.Enabled = FALSE;
				dev->SetState(state);
                */

                if(Action & (1<<EInputFire)) {
                    PickedMesh=PickObject();
                }

                if(PickedMesh) {
                    /*
                    RenderState state(RenderState::EDepthWrite|RenderState::EDepthTest);
                    state.DepthWrite=FALSE;
                    state.DepthTest=FALSE;
                    dev->SetState(state);
                    */
                    RenderState state;
                    state.SetDepthWrite(false);
                    state.SetDepthTest(false);
                    dev->SetState(state);

                    RenderTriangles(*PickedMesh);
                }

                mParticleSystem.Render(mCamera);
            }

        }

        char tmp[64];

        if(Mode & EModeShowHUD) {

            /*
            RenderState state;
            state.Flags=RenderState::ETexEnv|RenderState::EConstantColor;
            state.TexFlags[0]=RenderState::ETexOpModulate|RenderState::ETexArgColor|RenderState::ETexFilterPoint;
            state.TexFlags[1]=RenderState::ETexOpDisable;
            state.ConstantColor=0xff00;
            //state.Light.Flags=RenderState::ELightOff;
            dev->SetState(state);
            */


            TextureState st[2];
            /*
            st[0].Stage=0;
            //st[0].Op   =TextureState::ETexOpModulate;
            //st[0].Arg  =TextureState::ETexArgColor;
            st[0].Op   =TextureState::ETexOpColor;
            st[1].Stage=1;
            st[1].Op   =TextureState::ETexOpDisable;
            */
            st[0].Set(0,TextureState::ETexOpColor,0,TextureState::ETexFilterPoint);
            st[1].Set(1,TextureState::ETexOpDisable);

            dev->SetTextureState(st,ARRAY_COUNT(st));            
            dev->SetConstColor(0xff00);

            mHUD.SetRenderState();
            mHUD.SetFont(0);

            int y=2;

            sprintf(tmp,"FPS: %.2f (%s)",FPS, (PickedMesh ? PickedMesh->Name.AsChar() : "none"));
            mHUD.Print(2,y,tmp); y+=mHUD.GetTextHeight();

            sprintf(tmp,"Eye: %.2f %.2f %.2f",mCamera.Position.X,mCamera.Position.Y,mCamera.Position.Z);
            mHUD.Print(2,y,tmp); y+=mHUD.GetTextHeight();

            sprintf(tmp,"CGB: %.2f %.2f %.2f",Contrast,Gamma,Brightness);
            mHUD.Print(2,y,tmp); y+=mHUD.GetTextHeight();

            sprintf(tmp,"T: %d P: %d O: %d",TriangleCount,PortalCount,ObjectCount);
            mHUD.Print(2,y,tmp); y+=mHUD.GetTextHeight();

            const char* mode;
            if(Mode & EModeOnlyZones) {
                mode="Zones";
            } else {
                switch(RenderMode) {
                case 2:     mode="Lightmaps Only"; break;
                case 1:     mode="Textures Only"; break;
                case 0:     
                default:    mode="Normal"; break;
                }
            }

            sprintf(tmp,"Mode: %s Visiblity: %s",mode,((Mode & EModePortals) ? "On" : "Off"));
            mHUD.Print(2,y,tmp); y+=mHUD.GetTextHeight();

            mHUD.Print(2,y,physDebug);
        }

        dev->Unlock();
		dev->Flip(mViewport);

        // remove blend
        /*
        RenderState state(RenderState::EBlendNone);
        dev->SetState(state);
        */
        dev->SetBlendMode(RenderState::EBlendNone);
    }
//#endif

    unguard;
}
