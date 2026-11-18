#include "stdafx.h"
#include "Engine.h"
#include "SimpleGame.h"
#include "MainFrm.h"

namespace { FactoryRegistration<BaseGame,DemoGame> reg("demogame"); }
LRESULT CALLBACK FullScreenWndProc( HWND hWnd, UINT uMsg, WPARAM wParam,LPARAM lParam );
Mesh* pickedMesh=0;

// =======================================================================

class FullScreenWindow : public Window
{
public:
    FullScreenWindow(DemoGame* game) : mGame(game) {}
    BOOL HandleMessages(UINT msg, WPARAM wp, LPARAM lp, LRESULT& result);

private:
    DemoGame*   mGame; 
};

BOOL FullScreenWindow::HandleMessages(UINT msg, WPARAM wp, LPARAM lp, LRESULT& result)
{
    if(msg==WM_CLOSE) {
        mGame->ChangeMode(false);
        return TRUE;
    }

    result=DefWindowProc(mhWnd,msg,wp,lp);
    return TRUE;
}

// =========================================================================
DemoGame::DemoGame()
:   mInputSystem(0), mPhysics(0)
,   LogoTexRef(EInvalidIndex)
,   RenderMode(0) // normal
,   Mode(EModePortals|EModeShowHUD|EModeFlyMove|EModeShowStaticMeshes)
,   CurrentZone(0)
,   hwndFullScreen(0)
,   mUpdateLogic(true)
{
}
// =========================================================================
DemoGame::~DemoGame()
{
    Close();
}
// =========================================================================
void DemoGame::Close()
{
    guard(DemoGame::Close);
    if(mInputSystem) delete mInputSystem, mInputSystem=0;
    if(mPhysics)     delete mPhysics,     mPhysics=0;
    unguard;
}
// =========================================================================
BOOL DemoGame::InitRenderer()
{
    guard(DemoGame::InitRenderer);

	HWND hWnd = GetWindow();	
	GLog.Print("HWND=%d\n",hWnd);
	if(GEngine->InitRenderWindowSimply(RenderDeviceManager::ED3DRenderDevice,hwndFullScreen,0,0,FALSE)) 
	{
        mInputSystem=new Win32InputSystem(hWnd);

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

        mViewport.SetWindow(GetWindow());

        /*
        int renderWidth = GetWindowWidth();
		int renderHeight  = GetWindowHeight();

        ScreenVertex::FX=(float)renderWidth;
        ScreenVertex::FY=(float)renderHeight;

        // setup viewport params
        mViewport.XOffset = 0;
        mViewport.YOffset = 0;
        mViewport.Width   = renderWidth;
        mViewport.Height  = renderHeight;
        mViewport.MinZ    = 1.0f;
        mViewport.MaxZ    = 32768.0f;
        */

        return TRUE;
    }
    return FALSE;
    unguard;
}
// =========================================================================
void DemoGame::LoadScreen()
{
    guard(Demo::LoadScreen);

    RenderDevice* dev=GEngine->GetDevice();
    Assert(dev);

    mCamera.Tick();
    mViewport.Tick(mCamera);

    if(dev->Lock(mViewport,0)) {

        //RenderState state(RenderState::EConstantColor);
        //state.ConstantColor=-1;
        //dev->SetState(state);
        dev->SetConstColor(-1);

        mHUD.SetRenderState();
        int renderWidth = GetWindowWidth();
		int renderHeight = GetWindowHeight();

        int w=256,h=256;
			
		Texture texInfo;
        texInfo.Name="../data/textures/hud/loading_screen.tga";
        LogoTexRef=dev->CreateTexture(texInfo);
        GLog.Print("LogoTexRef:%d\n",LogoTexRef);

        mHUD.DrawQuad(LogoTexRef,
                      (renderWidth -w)/2,
                      (renderHeight-h)/2,
                      w,h);

        dev->Unlock();
		dev->Flip(mViewport);

        // set default font
        mHUD.SetFont(0);
    } 

    unguard;
}
// =========================================================================
BOOL DemoGame::Initialize()
{
    guard(DemoGame::Intialize);

	WNDCLASS wc;
	// Register the Window Class
	wc.lpszClassName = TEXT("FullScreen Window");
	wc.lpfnWndProc   = FullScreenWndProc;
	wc.style         = CS_VREDRAW | CS_HREDRAW;
	wc.hInstance     = NULL;
	wc.hIcon         = LoadIcon( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME) );
	wc.hCursor       = NULL;
	wc.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	
	if( RegisterClass( &wc ) == 0 )
		return false;

    hwndFullScreen = CreateWindowEx(WS_EX_TOPMOST, TEXT("Fullscreen Window"), "Pain Killer",
		                            WS_POPUP, 0,0, 100, 100,
			                        NULL, NULL, NULL, NULL );
    if(!hwndFullScreen) {
        return FALSE;
    }

    if(!InitRenderer()) {
        return FALSE;
    }

    // particle system
    if(!mParticleSystem.Init()) {
        GLog.Print("DemoGame: ERROR particles init failed\n");
        return FALSE;
    }

    // read some stuff from ini
    const char* section="SpawnPoint";
    const char* s;
    s=GConfig.GetString(section,"X"); if(s) mCamera.Position.X=float(atof(s));
    s=GConfig.GetString(section,"Y"); if(s) mCamera.Position.Y=float(atof(s));
    s=GConfig.GetString(section,"Z"); if(s) mCamera.Position.Z=float(atof(s));

    section="General";
    s=GConfig.GetString(section,"FlyMove"); if(s && !stricmp("true",s)) Mode|=EModeFlyMove;

    // setup world
    float scale=1.0f;
    String mapDir("../Data/Maps/");

    section="DemoGame";
    const char* mapName=GConfig.GetString(section,"Map");
    if(mapName) {
        const char* scaleValue=GConfig.GetString(section,"Scale");
        if(scaleValue) {
            scale=float(atof(scaleValue));
        }
    }
    else {
        // some default
        mapName="test.dat";
        scale=3.0f;
    }

	GScript.DoStringA("Game:Init()");	
	GScript.DoStringA("Game:LoadData()");	

	return true; //StartWorld((mapDir+mapName).AsChar(),scale);

    unguard;
}

BOOL DemoGame::StartWorld(const char* mapFileName, const float scale)
{
    guard(DemoGame::StartWorld);
    RenderDevice* dev=GEngine->GetDevice();
    dev->Flush();

	pickedMesh = 0;

    // setup HUD
    if(!mHUD.Create(dev)) {
        GLog.Print("ERROR: Could not initialize HUD\n");
        return FALSE;
    }

    LoadScreen();

    // vertex buffer for lines (debug zone/wireframe view)
    if(!VBuf.Create(dev, 1024)) {
        GLog.Print("DemoGame: ERROR Failed to create line vertex buffer\n");
        return FALSE;
    }

    // create physics engine
    if(mPhysics) {
        delete mPhysics;
        mPhysics=0;
    }

    // BART: HACK!
    // since there are no spawn points per level yet, after loading a new level the camera
    // may end up in solids, which would make physics unstable from the start
    // for now, always start level in fly mode
    Mode|=EModeFlyMove;

    mPhysics=new PhysicsEngine();
    GEngine->mPhysics = mPhysics;
    mPhysics->Init();

    // load world
    mWorld.ObjectToWorld=ScaleMatrix(scale);
    if(!mWorld.LoadLevel(mapFileName)) {
        GLog.Print("WARNING: couldnt load map: %s\n",mapFileName);
    }



    // init physics world
    mPhysics->InitWorld(mWorld);
    mPhysics->CreatePlayer(mCamera.Position);

    if(!mWorld.Precache(dev)) {
        return FALSE;
    }

    // waypoints

	mWorld.Paths.ClearContents();
	mWorld.Paths.SetViewport( &mViewport );
	mWorld.Paths.Test_InitWithWaypointGrid( 10 );
	mCurrentZoneGPath.Empty();
	mCurrentZoneGPath.Init( &mWorld.Paths, GAStarDistanceHeuristic );

//	mWaypoints.ClearContents();
//	int tempPt = mWaypoints.NumberOfWaypoints()-1;
//	mCurrentZonePath.Empty();
//	mWaypoints.GetShortestPath( 0, &tempPt, 1, GAStarDistanceHeuristic, mCurrentZonePath );

	mCamera.Tick();

	// warm up things
    for(int i=0; i<1000; ++i) {
        mParticleSystem.Tick(1.0f/120.0f);
    }
    mPhysics->Start();	
	
	GEngine->ResetTimer();
    mActive=TRUE;

    return TRUE;
    unguard;
}

// =========================================================================
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
// =========================================================================
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
    dev->SetColorMode();
    dev->SetStream(VBuf.GetStream());

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
// =========================================================================
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

    dev->SetColorMode();
    dev->SetStream(VBuf.GetStream());
    dev->SetConstColor(0xff0000);

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
// =========================================================================
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


#ifdef TRANSGAMING_DEINDEX
        if (!mesh.isIndexed){
            int triCount;
            int multiplier;


            if (mesh.PrimType == RenderDevice::EPrimTriList){
                triCount = mesh.Verts.Size() / 3;
                multiplier = 3;
            }

            else{
                triCount = mesh.Verts.Size() - 2;
                multiplier = 1;
            }


            for(int i=0; i<triCount; ++i) {

                vertexCount+=6;
                if(vertexCount>=VBuf.GetMaxElems()) {
                    dev->UnlockStream(VBuf.GetStream());
                    dev->Draw(RenderDevice::EPrimLineList,0,(vertexCount-6)/2);

                    v=(Vector*)dev->LockStream(VBuf.GetStream(),0,0);
                    vertexCount=6;
                }

                v[0]=mesh.Verts[i*multiplier+0].Pos;
                v[1]=mesh.Verts[i*multiplier+1].Pos;
                v[2]=mesh.Verts[i*multiplier+1].Pos;
                v[3]=mesh.Verts[i*multiplier+2].Pos;
                v[4]=mesh.Verts[i*multiplier+2].Pos;
                v[5]=mesh.Verts[i*multiplier+0].Pos;
                
                v+=6;
            }
        }

        else{
#endif
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
#ifdef TRANSGAMING_DEINDEX
    }
#endif

    dev->UnlockStream(VBuf.GetStream());
    dev->Draw(RenderDevice::EPrimLineList,0,vertexCount/2);
    unguard;
}

static int TriangleCount=0;
static int PortalCount;
static int ObjectCount;
// =========================================================================
void DemoGame::RenderWorld()
{
    TriangleCount=0;
    PortalCount=0;
    ObjectCount=0;

    RenderDevice* dev=GEngine->GetDevice();
    Assert(dev);

    /*	
    // set rendering state for drawing world
    RenderState state(RenderState::ETexEnv
                     |RenderState::ECullCCW
                     |RenderState::EDepthWrite
                     |RenderState::EDepthTest
                     |RenderState::EBlendNone);

    switch(RenderMode) {
    case 2: // lightmaps only
        state.Flags|=RenderState::EConstantColor;
        state.ConstantColor=0xff0000;

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
    */
     
    TextureState texSt[2];

    switch(RenderMode) {
    case 2: // lightmaps only

        texSt[0].Set(0,TextureState::ETexOpColor);
        texSt[1].Set(1,TextureState::ETexOpTexture);
        break;

    case 1: // textures only
        texSt[0].Set(0,TextureState::ETexOpTexture,0,TextureState::ETexFilterBilinear);
        texSt[1].Set(1,TextureState::ETexOpDisable);
        break;

    case 0: // normal
    default:
        texSt[0].Set(0,TextureState::ETexOpTexture,0,TextureState::ETexFilterBilinear);
        texSt[1].Set(1,TextureState::ETexOpModulate,TextureState::ETexArgFB);
        break;
    }

    dev->SetTextureState(texSt,2);

    RenderState state;
    state.SetCullMode(RenderState::ECullCCW);
    state.SetBlendMode(RenderState::EBlendNone);
    state.SetDepthTest(true);
    state.SetDepthWrite(true);

    dev->SetState(state);


    BOOL outOfWorld=mWorld.Occlude(mViewport,(Mode & EModePortals));

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

    bool identityXForm=true;
    dev->SetObjectTransform(GIdentityMatrix);

	if( Mode & EModeShowStaticMeshes )
	{
		for(List<Mesh>::Item* item=mWorld.MeshList.ListFront; item; item=item->Next) {

	        Mesh& mesh=*item->Ref;

	        if(EInvalidIndex!=mesh.VertexStream) {

	            if(mesh.Flags & EMeshTranslucent) {
	                trans[translucentCount++]=&mesh;
	                continue;
	            }

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
            
	            //dev->SetState(state);

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

	            // stats
	            ++ObjectCount;

#ifdef TRANSGAMING_DEINDEX
                if (mesh.isIndexed){
                    if (mesh.PrimType == RenderDevice::EPrimTriList)
                        TriangleCount += mesh.Verts.Size() / 3;

                    else
                        TriangleCount += mesh.Verts.Size() - 2;
                }

                else
#endif
	                TriangleCount+=mesh.Triangles.Size()/3;
	        }
	    }
	}

    mWorld.RenderModels();

	if( Mode & EModeShowWaypoints ) {
		//mWaypoints.Render( dev, VBuf );
		mCurrentZonePath.Render( dev, VBuf );
		mCurrentZoneGPath.Render( dev, VBuf );
	};
	mWorld.Paths.Render( dev, VBuf, Mode & EModeShowWaypoints, Mode & EModeShowFloors );

    if(translucentCount>0) {

        /*
        RenderState state(RenderState::ETexEnv
                        |RenderState::ECullCCW
                        |RenderState::EDepthWrite
                        |RenderState::EDepthTest
                        |RenderState::EBlendTranslucent);

        switch(RenderMode) {
        case 2: // lightmaps only
            state.Flags|=RenderState::EConstantColor;
            state.ConstantColor=0xff0000;

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

        state.DepthWrite=FALSE;
        state.DepthTest =TRUE;
        dev->SetState(state);
        */

        // set rendering state for drawing world
        dev->SetTextureState(texSt,2);

        RenderState state;
        state.SetDepthWrite(false);
        state.SetDepthTest(true);
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
            dev->SetState(state);
            */

            dev->SetIndexedStream(mesh.VertexStream,mesh.IndexStream);
            dev->SetObjectTransform(mesh.Flags & EMeshPhysical ? &mesh.ObjectMatrix : GIdentityMatrix);

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
        state.SetBlendMode(RenderState::EBlendNone);
        state.SetDepthWrite(true);
    }

    /*
    // avoid weirdness and reset world matrix to known state!
    // also, turn off second stage
    state.Flags=RenderState::ETransformObject|RenderState::ETexEnv;
    state.TexFlags[0]=RenderState::ETexOpModulate;
    state.TexFlags[1]=RenderState::ETexOpDisable;
    state.ObjectToWorld=IdentityMatrix();

    dev->SetState(state);
    */
    dev->SetObjectTransform(GIdentityMatrix);
}

Mesh* DemoGame::PickObject( Vector& hitPoint )
{
    POINT pt;
    GetCursorPos( &pt );
    ScreenToClient( GetWindow(), &pt );
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

    Mesh* pickedMesh=0;
    float dist=FLT_MAX;
	for(List<Mesh>::Item* item=mWorld.MeshList.ListFront; item; item=item->Next) 
    {
        Mesh& mesh=*item->Ref;

        if(ray.Intersect(mesh.Box)) 
        {
#ifdef TRANSGAMING_DEINDEX
            if (!mesh.isIndexed){
                int triCount;
                int multiplier;


                if (mesh.PrimType == RenderDevice::EPrimTriList){
                    triCount = mesh.Verts.Size() / 3;
                    multiplier = 3;
                }

                else{
                    triCount = mesh.Verts.Size() - 2;
                    multiplier = 1;
                }


                for(int i=0; i<triCount; ++i) {
                    const Vector& v0=mesh.Verts[i * multiplier + 0].Pos;
                    const Vector& v1=mesh.Verts[i * multiplier + 1].Pos;
                    const Vector& v2=mesh.Verts[i * multiplier + 2].Pos;
                    float d;
				    Vector point;
                    if(ray.IntersectTriangle(v0,v1,v2,d, &point )) 
                    {
                        if(d<dist) 
                        {
                            dist=d;
                            pickedMesh=&mesh;
						    hitPoint = point;
                        }
                    }
                }
            }

            else{
#endif
                const int triCount=mesh.Triangles.Size()/3;
                for(int i=0; i<triCount; ++i) {
                    const WORD e0=mesh.Triangles[i*3+0];
                    const WORD e1=mesh.Triangles[i*3+1];
                    const WORD e2=mesh.Triangles[i*3+2];

                    const Vector& v0=mesh.Verts[e0].Pos;
                    const Vector& v1=mesh.Verts[e1].Pos;
                    const Vector& v2=mesh.Verts[e2].Pos;
                    float d;
				    Vector point;
                    if(ray.IntersectTriangle(v0,v1,v2,d, &point )) 
                    {
                        if(d<dist) 
                        {
                            dist=d;
                            pickedMesh=&mesh;
						    hitPoint = point;
                        }
                    }
                }
#ifdef TRANSGAMING_DEINDEX
            }
#endif
        }
    }

    return pickedMesh;
}

// =========================================================================
void DemoGame::ReadInput()
{
    RenderDevice* dev=GEngine->GetDevice();
    Assert(dev);

    mInputSystem->Tick();

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
                //Mode^=EModeLockedMouse;
                //mInputSystem->LockMouse((Mode & EModeLockedMouse));
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
            case '0':
                ChangeMode(!GEngine->IsFullScreen());
                break;
            case 'X':
                //StartWorld("../Data/Maps/test.dat",3);
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
					mInputSystem->LockMouse(TRUE);
					Mode|=EModeLockedMouse;
                } else {
                    mCamera.Position=mPhysics->GetPlayerPos(0);
                    mPhysics->SetPlayerPos(0,mCamera.Position);            
					mInputSystem->LockMouse(FALSE);
					Mode&=~EModeLockedMouse;
                }

                Mode ^= EModeFlyMove;
                break;
            case 'R':
                ++RenderMode;
                if(RenderMode>=3) {
                    RenderMode=0;
                }
                break;
            }
        }
    }
}
// =========================================================================
void DemoGame::Tick(float timeDelta)
{
    guard(DemoGame::Tick);

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


	Resize();

    RenderDevice* dev=GEngine->GetDevice();
    Assert(dev);

    ReadInput();


    if(Mode & EModeLockedMouse) 
	{
		mCamera.Rotation+=mInputSystem->GetMouseDelta();
		// clamp vertical rotation
		if(mCamera.Rotation.Y>+80.0f) mCamera.Rotation.Y=+80.0f;
		if(mCamera.Rotation.Y<-80.0f) mCamera.Rotation.Y=-80.0f;		
	}
	else
	{
		// tutaj moja obsluga camery w trybie edycji
	}
	
    const DWORD Action=mInputSystem->GetActionStatus();
    if(Mode & EModeFlyMove) {
        /*const float speed=40.0f;
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
        }*/
    }
    else {
        mPhysics->PlayerAction(0,mCamera.ViewMatrix,Action);
    }

	    // tick stuff
    mPhysics->Tick(timeDelta);

    // update camera
    if(!(Mode & EModeFlyMove)) {
        mCamera.Position=mPhysics->GetPlayerPos(0);
    }

	if (mUpdateLogic)
		GScript.DoStringA("Game:Tick(%f)",timeDelta);

    mCamera.Tick();


    mViewport.ClearColor=(Mode & (EModeOnlyZones|EModeShowTris)) ? 0 : 0xff1a1a9a;
    mViewport.Tick(mCamera);


    if(Action & (1<<EInputFire)) {

        HitData hitData;
        BOOL hit=mPhysics->LineTrace(mCamera.Position,mCamera.Position+mCamera.GetForwardVector()*640,hitData);
        if(hit) {

        }
    }

    if(Action & (1<<EInputAltFire)) {
    }

    // render
    if(dev->Lock(mViewport,0)) {

        if(Mode & EModeOnlyZones) {
            RenderZones();
        }
        else {
            if(Mode & EModeShowTris) {
                RenderTriangles();
            }
            else {
				mWorld.Tick(timeDelta);
                RenderWorld();

				/*
                if(Action & (1<<EInputFire)) {
					Vector tempVec1;
                    pickedMesh=PickObject(tempVec1);
                }
				*/

                mParticleSystem.Tick(timeDelta);
                mParticleSystem.Render(mCamera);
				GScript.DoStringA("Game:Render()");
            }

        }

        char tmp[64];

        if(Mode & EModeShowHUD) {

            /*
            RenderState state;
            state.Flags=RenderState::ETexEnv|RenderState::EConstantColor|RenderState::ETransformObject;
            state.TexFlags[0]=RenderState::ETexOpModulate|RenderState::ETexArgColor|RenderState::ETexFilterPoint;
            state.TexFlags[1]=RenderState::ETexOpDisable;
            state.ConstantColor=0xff00;
			state.ObjectToWorld=IdentityMatrix();

            //state.Light.Flags=RenderState::ELightOff;
            dev->SetState(state);
            */

            TextureState st[2];
            st[0].Set(0,TextureState::ETexOpColor,0,TextureState::ETexFilterPoint);
            st[1].Set(1,TextureState::ETexOpDisable);

            dev->SetTextureState(st,ARRAY_COUNT(st));            
            dev->SetConstColor(0xff00);

            mHUD.SetRenderState();
            mHUD.SetFont(0);

            int y=2;

            sprintf(tmp,"FPS: %.2f %s",FPS,(pickedMesh?pickedMesh->Name.AsChar():"none"));
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
    unguard;
}
// =========================================================================
HWND DemoGame::GetWindow()
{
	if (GEngine->IsFullScreen()) return hwndFullScreen;
	CMDIChildWnd * pWnd = ((CMainFrame*)AfxGetMainWnd())->MDIGetActive();
	if (pWnd) return pWnd->m_hWnd;
	return NULL;
}
// =========================================================================
int DemoGame::GetWindowHeight()
{
	RECT rc;
	GetClientRect(GetWindow(),&rc);
	return rc.bottom - rc.top;
}
// =========================================================================
int DemoGame::GetWindowWidth()
{
	RECT rc;
	GetClientRect(GetWindow(),&rc);
    return rc.right - rc.left;
}
// =========================================================================
void DemoGame::Resize()
{
	//mViewport.SetView(0,0, GetWindowWidth(), GetWindowHeight());
    mViewport.SetWindow(GetWindow());
}
// =========================================================================
Win32InputSystem*  DemoGame::GetInputSystem()
{
	return mInputSystem;
}
// =========================================================================
void DemoGame::ChangeMode(bool fullscreen)
{
	mActive = false;
    if(fullscreen) {
        ShowWindow(hwndFullScreen,SW_SHOW);
        GEngine->InitRenderWindowSimply(RenderDeviceManager::ED3DRenderDevice,hwndFullScreen,800,600,TRUE);
    }
    else {
        ShowWindow(hwndFullScreen,SW_HIDE);
        HWND hwnd=((CMainFrame*)AfxGetMainWnd())->MDIGetActive()->m_hWnd;
        GEngine->InitRenderWindowSimply(RenderDeviceManager::ED3DRenderDevice,hwndFullScreen,0,0,FALSE);
    }
}
// =========================================================================
void DemoGame::SetMode(DWORD mode)
{
	Mode = mode;
}
// =========================================================================
DWORD DemoGame::GetMode()
{
	return Mode;
}
// =========================================================================
LRESULT CALLBACK FullScreenWndProc( HWND hWnd, UINT uMsg, WPARAM wParam,LPARAM lParam )
{
	DemoGame* pGame = NULL;
	if (GEngine && GEngine->GetGame())
		pGame= (DemoGame*) GEngine->GetGame();
	

	//if (GEngine->IsFullScreen())
	//{
		//LRESULT res;	
		if (pGame->GetInputSystem())
			//pGame->GetInputSystem()->HandleMessages(uMsg,wParam,lParam,res);
			pGame->GetInputSystem()->HandleMessages(0,uMsg,wParam,lParam);

		switch (uMsg)
		{
			case WM_ACTIVATE:
				pGame->mActive = LOWORD(wParam)==WA_ACTIVE?true:false; 		
			break;

			case WM_CLOSE:
				pGame->ChangeMode(false);
				return 0; // nie mozna zamknac okna, bo jest tworzone tylko raz :)
				//AfxGetApp()->GetMainWnd()->PostMessage(WM_CLOSE); - dla wersji fullscreen
			break;
			
			case WM_SYSCOMMAND:
				if(wParam==SC_SCREENSAVE || wParam==SC_MONITORPOWER)
					return 0;
			break;
		}
	//}

    return DefWindowProc( hWnd, uMsg, wParam, lParam );
}
// =========================================================================
void DemoGame::RenderLine(const Vector& p1, const Vector& p2, DWORD color )
{
	guard(WaypointSet::RenderLine);
	RenderDevice* dev=GEngine->GetDevice();
    Assert(dev);

    /*
    RenderState state;
    state.Flags=RenderState::ETexEnv
               |RenderState::EConstantColor
               |RenderState::EStreamSource
			   |RenderState::ETransformObject
			   |RenderState::EDepthTest
			   |RenderState::EBlendNone;

    state.DepthTest = false;


    state.TexFlags[0]=RenderState::ETexOpColor;
    state.TexFlags[1]=RenderState::ETexOpDisable;
    state.StreamSource = VBuf.GetStream();
	state.ObjectToWorld = IdentityMatrix();

	state.ConstantColor = color;

	dev->SetState(state);
    */

    RenderState state;
    state.SetObjectTransform(GIdentityMatrix);
    state.SetDepthTest(false);
    state.SetBlendMode(RenderState::EBlendNone);
    state.SetConstColor(color);

    dev->SetState(state);
    dev->SetStream(VBuf.GetStream());
    dev->SetColorMode();

	Vector* v = VBuf.Lock(2);
	v[0] = p1;
	v[1] = p2;
	dev->Draw(RenderDevice::EPrimLineList, VBuf.Unlock(), 1);

	unguard;
}
// =========================================================================
void DemoGame::RenderBox(const BoundingBox& box, DWORD color)
{
	guard(WaypointSet::RenderBox);
	RenderDevice* dev=GEngine->GetDevice();
    Assert(dev);

	Vector* v = VBuf.Lock(3*6*2);
	const Vector P1=Vector(box.Min.X,box.Min.Y,box.Min.Z);
	const Vector P2=Vector(box.Max.X,box.Min.Y,box.Min.Z);                       
	const Vector P3=Vector(box.Min.X,box.Max.Y,box.Min.Z);                       
	const Vector P4=Vector(box.Max.X,box.Max.Y,box.Min.Z);                       
	const Vector P5=Vector(box.Min.X,box.Min.Y,box.Max.Z);                       
	const Vector P6=Vector(box.Max.X,box.Min.Y,box.Max.Z);                       
	const Vector P7=Vector(box.Min.X,box.Max.Y,box.Max.Z);                       
	const Vector P8=Vector(box.Max.X,box.Max.Y,box.Max.Z);

	v[0] = P1; v[1] = P3; v[2] = P2;
	v[3] = P2; v[4] = P3; v[5] = P4;
	v[6] = P2; v[7] = P4; v[8] = P6;
	v[9] = P6; v[10] = P4; v[11] = P8;
	v[12] = P6; v[13] = P8; v[14] = P5;
	v[15] = P5; v[16] = P8; v[17] = P7;
	v[18] = P5; v[19] = P7; v[20] = P1;
	v[21] = P1; v[22] = P7; v[23] = P3;
	v[24] = P5; v[25] = P1; v[26] = P6;
	v[27] = P6; v[28] = P1; v[29] = P2;
	v[30] = P3; v[31] = P7; v[32] = P4;
	v[33] = P4; v[34] = P7; v[35] = P8;

    /*
	RenderState state;
    state.Flags=RenderState::ETexEnv
               |RenderState::EConstantColor
               |RenderState::EStreamSource
			   |RenderState::ETransformObject;

    state.TexFlags[0]=RenderState::ETexOpColor;
    state.TexFlags[1]=RenderState::ETexOpDisable;
    state.StreamSource =VBuf.GetStream();
	state.ObjectToWorld=IdentityMatrix();

	state.ConstantColor = color;

	dev->SetState(state);
    */

    RenderState state;
    state.SetObjectTransform(GIdentityMatrix);
    state.SetConstColor(color);
    dev->SetState(state);
    dev->SetColorMode();
    dev->SetStream(VBuf.GetStream());

	dev->Draw(RenderDevice::EPrimTriList, VBuf.Unlock(), 6*2 );
	unguard;
}
// =========================================================================
