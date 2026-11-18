//#include "Scripting.h"
#include "Engine.h"

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;

#include "Core/SimpleProfiler.h"
extern ENGINE_API SimpleProfiler GProfiler;

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

#include "Render/HUD.h"
#ifndef TRANSGAMING
# include "Render/RenderWindow.h"
#endif
#include "Engine/PainMenu.h"
#include "Engine/MagicBoard.h"
#include "Engine/MapSelect.h"

#include "Core/FileSystem/FileManagerStandard.h"
extern ENGINE_API GFileManager  	GFileMan;
extern void SaveScreenBuffer(const char *Filename);

#include "Render/Material.h"
#include "Render/ParticleEffect.h"
#include "Render/Billboard.h"
#include "Render/Decal.h"
#include "Render/Trail.h"
#include "Render/View.h"
#include "Net/UdpMessage.h"
#include "Engine/Portal.h"
#include "Engine/Mesh.h"
#include "Render/FogVolume.h"
#include "Engine/InputSystem.h"
#include "Physics/Ragdoll.h"
#include "Physics/PhysicsEngine.h"
#include "Physics/PhysicsWorld.h"

#include "Net/NetworkDevice2.h"
#include "Net/NetworkPeer2.h"

#ifndef TRANSGAMING_NO_MILES
// We currently don't have PSP support for Miles.
#include "Audio/MilesEngine.h"
#endif

#include "Engine/ModelAnimation.h"
#include "Engine/Region.h"
#include "Engine/Sound.h"
#include "Engine/BaseGame.h"
#include "Physics/PhysicsObject.h"
#include "Render/Video.h"
#include "Engine/Model.h"
#include "Engine/World.h"

#include "Game/memoryDump.h"

#ifdef TRANSGAMING_PSP
# include "Engine/Game/PainGame_PSP.h"
#elif defined(TRANSGAMING_PS2)
# include "Engine/Game/PainGame_PS2.h"
#elif defined(TRANSGAMING_WINDOWS)
//# include "../PainEditor/Game/EditorGame.h"
# include "Game/PainGame.h"
#endif

#ifdef TRANSGAMING_PSP
# include "Engine/Engine/simpleTiming.h"
#elif defined(TRANSGAMING_PS2)
#include "Engine/Engine/simpleTiming.h"
#elif defined( TRANSGAMING_WINDOWS )
#include "Engine/simpleTiming.h"
#endif

#ifndef TRANSGAMING_NO_HAVOK
// We currently don't have PSP support for Havok
#include "Physics/Havok/HavokCharacterController.h"
#endif

#ifdef PKTV_ENABLED
# include "Net/PKTVManager.h"
#endif

#ifndef	TRANSGAMING_NO_SAFEDISC
#include <pfnapi.h>
#endif

// SafeDisc Protection
//
// Use like this:
//
//PFNAPI_APPLY_SECURITY( luaMenu_ReturnToGame );
//
// And put the function between:
//
// #pragma optimize( "", off )
// #pragma inline_depth(0)
//
// and
//
// #pragma inline_depth()
// #pragma optimize( "", on )

#ifndef TRANSGAMING
# define CD_CHECK_ON
#endif

#ifdef CD_CHECK_ON
#define PK_CD_LABEL_00 'P'
#define PK_CD_LABEL_01 'K'
#define PK_CD_LABEL_02 '_'
#define PK_CD_LABEL_03 'P'
#define PK_CD_LABEL_04 'L'
#define PK_CD_LABEL_05 '_'
#define PK_CD_LABEL_06 '1'
#define PK_CD_LABEL_07 '\0'
#define PK_CD_LABEL_08 '\0'
#define PK_CD_LABEL_09 '\0'
#define PK_CD_LABEL_10 '\0'
#define PK_CD_LABEL_11 '\0'
#endif

Quaternion EulerToQuat(Vector &ea)
{
	float roll = ea.X;
	float pitch = ea.Y;
	float yaw = ea.Z;

	float cr = cos(roll/2);
	float cp = cos(pitch/2);
	float cy = cos(yaw/2);

	float sr = sin(roll/2);
	float sp = sin(pitch/2);
	float sy = sin(yaw/2);

	float cpcy = cp * cy;
	float spsy = sp * sy;

	Quaternion q;
	q.W = cr * cpcy + sr * spsy;
	q.X = sr * cpcy - cr * spsy;
	q.Y = cr * sp * cy + sr * cp * sy;
	q.Z = cr * cp * sy - sr * sp * cy;
	return q;
}


Vector QuatToEuler(Quaternion &q)
{
	static const float QuatToEulerAccuracy = 0.0000001;

	float sint, cost, sinv, cosv, sinf, cosf;
	float cost_temp;

	sint = (2 * q.W * q.Y) - (2 * q.X * q.Z);
	cost_temp = 1.0 - (sint * sint);

    if (cost_temp > QuatToEulerAccuracy)
		cost = sqrtf(cost_temp);
	else
		cost = 0;


	if (fabs(cost) > QuatToEulerAccuracy)
	{
		sinv = ((2 * q.Y * q.Z) + (2 * q.W * q.X)) / cost;
		cosv = (1 - (2 * q.X * q.X) - (2 * q.Y * q.Y)) / cost;
		sinf = ((2 * q.X * q.Y) + (2 * q.W * q.Z)) / cost;
		cosf = (1 - (2 * q.Y * q.Y) - (2 * q.Z * q.Z)) / cost;
	}
	else
	{
		sinv = (2 * q.W * q.X) - (2 * q.Y * q.Z);
		cosv = 1 - (2 * q.X * q.X) - (2 * q.Z * q.Z);
		sinf = 0;
		cosf = 1;
	}

	float roll = atan2f(sinv, cosv);
	float pitch = atan2f(sint, cost);
	float yaw = atan2f(sinf, cosf);
	return Vector(roll,pitch,yaw);
}

Quaternion RotateQuatByAxisAngle(Quaternion &q, float &angle, Vector &axis)
{
	return q *Quaternion(angle,axis);
}

#pragma warning( disable : 4311 4312 4244 4800)
// =========================================================================
// STANDARD
// =========================================================================
#ifdef TRANSGAMING
static int luaErrorMsg(lua_State *state){
    Script L(state);

    TRACE("!!!!!_ERRORMESSAGE(): ERROR-> lua error: '%s'\n", L.GetString(1));

    return 0;
}
#endif

static int luaMsgBox(lua_State* state)
{
	Script L(state);
#ifdef TRANSGAMING
    TRACE("!!!! ********************************************************************** !!!!\n");
    TRACE("!!!! Script Error!                                                          !!!!\n");
    TRACE("!!!!     %s\n", L.GetString(1));
    TRACE("!!!! ********************************************************************** !!!!\n");

#else
	MessageBox(NULL,L.GetString(1),"Script Message",MB_TOPMOST);
#endif
	if (GEngine)
		GEngine->ResetTimer();
	return 0;
}
// =========================================================================
static int luaExit(lua_State* state)
{
    TRACE("!!!! ********************************************************************** !!!!\n");
    TRACE("!!!! Shutting down engine                                                   !!!!\n");
    TRACE("!!!! ********************************************************************** !!!!\n");

	GEngine->Close();

    TRACE("!!!! ********************************************************************** !!!!\n");
    TRACE("!!!! Done signalling engine to shutdown                                     !!!!\n");
    TRACE("!!!! ********************************************************************** !!!!\n");

	return 0;
}
// =========================================================================
static int luaDoString(lua_State* state)
{
	Script L(state);
	GScript.DoString(L.GetString(1));
	return 0;
}
// =========================================================================
static int luaDoFile(lua_State* state)
{
	Script L(state);
	L.DoFile(L.GetString(1),L.GetBool(2,1));
	return 0;
}
// =========================================================================
static int luaLOG(lua_State* state)
{
    #ifndef FINAL_BUILD
	Script L(state);
	GLog.Print(L.GetString(1));
	//StackTracer::Log(L.GetString(1));
    #endif
	return 0;
}
// =========================================================================
static int luaToLightUD(lua_State* state)
{
	Script L(state);
	L.PushLightUD((void*)L.GetInt(1));
	return 1;
}
// =========================================================================
static int luaToNumber(lua_State* state)
{
	Script L(state);
	L.PushInt((int)L.GetLightUD(1));
	return 1;
}
// =========================================================================
static int luaEulerToQuat(lua_State* state)
{
	Script		L(state);
	Quaternion	q;

	float roll		= L.GetFloat(1);
	float pitch		= L.GetFloat(2);
	float yaw		= L.GetFloat(3);

	float cr		= cos(roll/2);
	float cp		= cos(pitch/2);
	float cy		= cos(yaw/2);

	float sr		= sin(roll/2);
	float sp		= sin(pitch/2);
	float sy		= sin(yaw/2);

	float cpcy		= cp * cy;
	float spsy		= sp * sy;

	q.W				= cr * cpcy + sr * spsy;
	q.X				= sr * cpcy - cr * spsy;
	q.Y				= cr * sp * cy + sr * cp * sy;
	q.Z				= cr * cp * sy - sr * sp * cy;

	L.PushFloat(q.W);
	L.PushFloat(q.X);
	L.PushFloat(q.Y);
	L.PushFloat(q.Z);

	return 4;
}
// =========================================================================
static int luaNormalXToQuat(lua_State* state)
{
	Script L(state);
	Vector nv(L.GetFloat(1),L.GetFloat(2),L.GetFloat(3));

	Matrix m = NormalXRotationMatrix(nv);
	Quaternion q(m);

	L.PushFloat(q.W);
	L.PushFloat(q.X);
	L.PushFloat(q.Y);
	L.PushFloat(q.Z);
	return 4;
}
// =========================================================================
static int luaNormalYToQuat(lua_State* state)
{
	Script L(state);
	Vector nv(L.GetFloat(1),L.GetFloat(2),L.GetFloat(3));

	Matrix m = NormalYRotationMatrix(nv);
	Quaternion q(m);

	L.PushFloat(q.W);
	L.PushFloat(q.X);
	L.PushFloat(q.Y);
	L.PushFloat(q.Z);
	return 4;
}
// =========================================================================
static int luaNormalZToQuat(lua_State* state)
{
	Script L(state);
	Vector nv(L.GetFloat(1),L.GetFloat(2),L.GetFloat(3));

	Matrix m = NormalZRotationMatrix(nv);
	Quaternion q(m);

	L.PushFloat(q.W);
	L.PushFloat(q.X);
	L.PushFloat(q.Y);
	L.PushFloat(q.Z);
	return 4;
}
Vector BezierPoint( DynamicArray <Vector> &p, double mu)
{
   int k,kn,nn,nkn;
   double blend,muk,munk;
   Vector b(0.0,0.0,0.0);
   int n = p.Size()-1;

   if (mu>=1)
       return p[p.Size()-1];

   muk = 1;
   munk = pow(1-mu,(double)n);

   for (k=0;k<=n;k++) {
      nn = n;
      kn = k;
      nkn = n - k;
      blend = muk * munk;
      muk *= mu;
      munk /= (1-mu);
      while (nn >= 1) {
         blend *= nn;
         nn--;
         if (kn > 1) {
            blend /= (double)kn;
            kn--;
         }
         if (nkn > 1) {
            blend /= (double)nkn;
            nkn--;
         }
      }
      b.X += p[k].X * blend;
      b.Y += p[k].Y * blend;
      b.Z += p[k].Z * blend;
   }

   return(b);
}

// =========================================================================
static int luaVArray_Create(lua_State* state)
{
	Script L(state);
    m_setTag("ScriptArray");
    L.PushLightUD(new DynamicArray<Vector>);
    return 1;
}
// =========================================================================
static int luaVArray_Delete(lua_State* state)
{
	Script L(state);
    DynamicArray<Vector> * array = (DynamicArray<Vector> *) L.GetLightUD(1);
    delete array;
    return 0;
}
// =========================================================================
static int luaVArray_AddPoint(lua_State* state)
{
	Script L(state);
    DynamicArray<Vector> * array = (DynamicArray<Vector> *) L.GetLightUD(1);
    array->Add(Vector(L.GetFloat(2),L.GetFloat(3),L.GetFloat(4)));
    return 0;
}
// =========================================================================
static int luaVArray_GetBezierPoint(lua_State* state)
{
	Script L(state);
    DynamicArray<Vector> * array = (DynamicArray<Vector> *) L.GetLightUD(1);
    float a = L.GetFloat(2);
    Vector v = BezierPoint(*array,a);
    L.PushFloat(v.X);
    L.PushFloat(v.Y);
    L.PushFloat(v.Z);
    return 3;
}
// =========================================================================
static const luaL_reg varray_lib[] = {
	{"Create", luaVArray_Create},
	{"Delete", luaVArray_Delete},
	{"AddPoint", luaVArray_AddPoint},
	{"GetBezierPoint", luaVArray_GetBezierPoint},
	{NULL, NULL}
};
// =========================================================================
static int luaQuatToEuler(lua_State* state)
{
	Script L(state);
	Quaternion q(L.GetFloat(1),L.GetFloat(2),L.GetFloat(3),L.GetFloat(4));
	Vector ea = QuatToEuler(q);

	L.PushFloat(ea.X);
	L.PushFloat(ea.Y);
	L.PushFloat(ea.Z);
	return 3;
}
// =========================================================================
static int luaRotateQuatByAxisAngle(lua_State* state)
{
	Script L(state);
	Quaternion q(L.GetFloat(1),L.GetFloat(2),L.GetFloat(3),L.GetFloat(4));
	float angle = L.GetFloat(5);
	Vector axis(L.GetFloat(6),L.GetFloat(7),L.GetFloat(8));

	Quaternion rq = RotateQuatByAxisAngle(q,angle,axis);

	L.PushFloat(rq.W);
	L.PushFloat(rq.X);
	L.PushFloat(rq.Y);
	L.PushFloat(rq.Z);
	return 4;
}

#ifdef CD_CHECK_ON
// =========================================================================
// WARNING: This is a fake CD-Check method
static int luaCdCheck2(lua_State* state)
{
    TRACE("[engineToLua.cpp]::luaCdCheck2(): checking CD label\n");

	char label[13];
	label[11] = PK_CD_LABEL_11;
	label[6]  = PK_CD_LABEL_06;
	label[0]  = PK_CD_LABEL_00;
	char dbits[110];
	label[2]  = PK_CD_LABEL_02;
	label[8]  = PK_CD_LABEL_08;
	DWORD mf, sf, sno;
	label[9]  = PK_CD_LABEL_09;
	label[7]  = PK_CD_LABEL_07;
	label[4]  = PK_CD_LABEL_04;
	GetLogicalDriveStrings( 110, dbits );
	label[10] = PK_CD_LABEL_10;
	label[3]  = PK_CD_LABEL_03;
	Script L(state);
	label[5]  = PK_CD_LABEL_05;
	label[1]  = PK_CD_LABEL_01;

	for( int i = 0; *(dbits+i); i+= 4 )
	{
		char vol[256];
		vol[0] = '\0';
		char drive[4];
		strncpy(drive,dbits+i,4);
		if( GetDriveType( drive ) == DRIVE_CDROM )
		{
			GetVolumeInformation( drive, vol, sizeof(vol), &sno, &mf, &sf, NULL, NULL );
			if( ( vol[0] == 'P' ) &&
				( vol[1] == 'K' ) &&
				( vol[2] == '_' ) &&
				( ( ( vol[3] == '1' ) && ( vol[4] == 0 ) ) ||
				  ( ( vol[5] == '_' ) && ( vol[6] == '1' ) && ( vol[7] == 0 ) ) ) )
			{
				L.PushString( dbits+i );
				return 1;
			}

			if( ( vol[0] == 'P' ) &&
				( vol[1] == 'K' ) &&
				( vol[2] == '_' ) &&
				( vol[3] == 'P' ) &&
				( vol[4] == 'L' ) &&
				( vol[5] == '_' ) &&
				( vol[6] == '1' ) &&
				( vol[7] == 0 ) )
			{
				L.PushString( dbits+i );
				return 1;
			}
/*
			if( stricmp( label, vol ) == 0 )
			{
				L.PushString( dbits+i );
				return 1;
			}
*/
		}
	}
	L.PushNil();
	return 1;
}
#endif

// =========================================================================
// INPUT
// =========================================================================
static int luaInput_Key(lua_State* state)
{
	Script L(state);
    if (GEngine->InputSys)
        L.PushInt(GEngine->InputSys->GetKeyStatus(L.GetInt(1)));
    else
        L.PushInt(0);
	return 1;
}
// =========================================================================
static int luaInput_Action(lua_State* state)
{
	Script L(state);
    if (GEngine->InputSys)
        L.PushBool( GEngine->InputSys->GetActionStatus() & L.GetInt(1) );
    else
        L.PushBool(false);
	return 1;
}
// =========================================================================
static int luaInput_UIAction(lua_State* state)
{
	Script L(state);
    if (GEngine->InputSys)
        L.PushBool( GEngine->InputSys->GetUIActionStatus() & L.GetInt(1) );
    else
        L.PushBool(false);
	return 1;
}
// =========================================================================
static int luaInput_RemoveAction(lua_State* state)
{
	Script L(state);
    DWORD action = GEngine->InputSys->GetActionStatus();
    action &= ~ L.GetInt(1);
	GEngine->InputSys->SetActionStatus(action);
	return 0;
}
// =========================================================================
static int luaInput_RemoveUIAction(lua_State* state)
{
	Script L(state);
    DWORD action = GEngine->InputSys->GetUIActionStatus();
    action &= ~ L.GetInt(1);
	GEngine->InputSys->SetUIActionStatus(action);
	return 0;
}
// =========================================================================
static int luaInput_GetActionStatus(lua_State* state)
{
	Script L(state);
	L.PushInt( GEngine->InputSys->GetActionStatus());
	return 1;
}
// =========================================================================
static int luaInput_GetTime(lua_State* state)
{
	Script L(state);
	L.PushFloat(GEngine?GEngine->SysDriver.GetCurrentTime():0);
	return 1;
}
// =========================================================================
static int luaInput_GetLastTimeStep(lua_State* state)
{
	Script L(state);
	L.PushFloat(GEngine?GEngine->SysDriver.GetLastTimeStep():0);
	return 1;
}
// =========================================================================
#ifndef TRANSGAMING
static int luaInput_GetTimeFromTimerReset(lua_State* state) {
	Script L(state);

#ifdef TRANSGAMING
	float  a = GEngine->SysDriver.GetTimeFromTimerReset() * 1000.0f;
#else
	double a = GEngine->SysDriver.GetTimeFromTimerReset() * 1000.0;
#endif
	L.PushFloat(a);
	return 1;
}
#endif

// =========================================================================
static int luaInput_ResetTimer(lua_State* state)
{
	Script L(state);
	GEngine->ResetTimer();
	return 0;
}
// =========================================================================
static int luaInput_SetTimeMultiplier(lua_State* state)
{
	Script L(state);
	GEngine->SetTimeMultiplier(L.GetFloat(1));
	return 0;
}
// =========================================================================
static int luaInput_GetTimeMultiplier(lua_State* state)
{
	Script L(state);
    L.PushFloat(GEngine->TimeMultiplier);
	return 1;
}
// =========================================================================
static int luaInput_Takescreenshot(lua_State* state)
{
	Script L(state);
	GEngine->TakeScreenshot();
	return 0;
}
// =========================================================================
static int luaInput_LoadBindings(lua_State* state)
{
	Script L(state);
    if (GEngine->InputSys)
        GEngine->InputSys->LoadBindings();
	return 0;
}
// =========================================================================
static int luaInput_Reset(lua_State* state)
{
    if (GEngine->InputSys)
        GEngine->InputSys->Reset();
	return 0;
}
// =========================================================================
static int luaInput_GetTimeDelta(lua_State* state)
{
	Script L(state);
    if (GEngine)
        L.PushFloat(GEngine->LastDelta);
    else
        L.PushFloat(0);
	return 1;
}
// =========================================================================
static int luaInput_Reinit(lua_State* state)
{
	Script L(state);
//	ReinitInputSystem( L.GetBool( 1, false ), GEngine->Video->GetActiveView()->GetActiveWindow() );
	return 0;
}
// =========================================================================
static int luaInput_IsFireSwitched(lua_State *state)
{
	Script L(state);
	L.PushBool( GEngine->InputSys->IsFireSwitched() );
	return 1;
}
// =========================================================================
static int luaInput_GetAsyncKeyState(lua_State* state)
{
    Script L(state);
    L.PushInt( GetAsyncKeyState( L.GetInt(1)) );
    return 1;
}
// =========================================================================
static int luaInput_GetUseDInput(lua_State* state)
{
    Script L(state);
	L.PushBool( GEngine->InputSys->GetUseDInput() );
    return 1;
}
// =========================================================================
static int luaInput_SetUseDInput(lua_State* state)
{
    Script L(state);
	bool use = L.GetBool( 1, true );
	GEngine->InputSys->SetUseDInput( use );
    return 0;
}
// =========================================================================
static int luaInput_GetDIDeltaScale(lua_State* state)
{
    Script L(state);
	L.PushFloat( GEngine->InputSys->GetDIDeltaScale() );
    return 1;
}
// =========================================================================
static int luaInput_SetDIDeltaScale(lua_State* state)
{
    // eric-> this function is never called from the scripts.  All it does is add an extra
    //   sensitivity scaling factor to the mouse.
    Script L(state);
	float s = L.GetFloat( 1, 3.0f );
	GEngine->InputSys->SetDIDeltaScale( s );
    return 0;
}
// =========================================================================
static int luaInput_GetKeyNameByEngName(lua_State* state)
{
	Script L(state);
	const char *eng = L.GetString( 1 );
	L.PushString( GEngine->InputSys->GetKeyNameByEngName( eng ) );
    return 1;
}
// =========================================================================
static int luaInput_GetShortNameByEngName(lua_State* state)
{
	Script L(state);
	const char *eng = L.GetString( 1 );
	L.PushString( GEngine->InputSys->GetShortNameByEngName( eng ) );
    return 1;
}
// =========================================================================
static int luaInput_GetEngNameByShortName(lua_State* state)
{
	Script L(state);
	const char *eng = L.GetString( 1 );
	L.PushString( GEngine->InputSys->GetKeyName( GEngine->InputSys->GetKeyByShortName( eng ) ) );
    return 1;
}
// =========================================================================
#ifndef TRANSGAMING
static int luaInput_GetKBType( lua_State *state )
{
	Script L(state);
	WORD langID = HIWORD( GetKeyboardLayout( 0 ) );
	char txt[255];
	sprintf( txt, "0x%x - 0x%x", LOWORD( GetKeyboardLayout( 0 ) ), langID );
	MessageBox( NULL, txt, "Keyboard layout", MB_OK );
	L.PushInt( 1 );
    return 1;
}
#endif
// =========================================================================
static int luaInput_BindKeyCommand( lua_State *state )
{
	Script L(state);
	const char *kstr = L.GetString( 1 );
	const char *cmd = L.GetString( 2, NULL);
	int         key = GEngine->InputSys->GetKeyByShortName( kstr );


	if( key > 0 )
		GEngine->InputSys->BindKey( 0, key, cmd );
	return 0;
}
// =========================================================================
static int luaInput_getCurrentControlProfile(lua_State *state){
    Script L(state);


    if (GEngine == NULL || GEngine->InputSys == NULL)
        L.PushInt((int)InputSystem::PAD_PROFILE_NONE);

    else
        L.PushInt(GEngine->InputSys->getCurrentControlProfile());

    return 1;
}
// =========================================================================
static int luaInput_setCurrentControlProfile(lua_State *state){
    Script L(state);


    //the engine hasn't been initialized yet => queue the message so that it gets set on the first frame
    if (GEngine == NULL || GEngine->InputSys == NULL)
        DummyMessageHandler::QueueHorribleWindowsMessage(DummyMessageHandler::PKMSG_SETCONTROLPROFILE, L.GetInt(1, (int)InputSystem::PAD_PROFILE_NONE), 0);

    //the engine is initialized => set the control profile immediately
    else{
        GEngine->InputSys->setCurrentControlProfile((InputSystem::ControlProfile)L.GetInt(1, (int)InputSystem::PAD_PROFILE_NONE));
        GEngine->InputSys->LoadBindings();
    }

    return 0;
}
// =========================================================================
static const luaL_reg input_lib[] = {
	{"Key", luaInput_Key},
	{"Action", luaInput_Action},
	{"UIAction", luaInput_UIAction},
	{"GetTime", luaInput_GetTime},
#ifndef TRANSGAMING
	{"GetTimeFromTimerReset", luaInput_GetTimeFromTimerReset},
#endif
	{"ResetTimer", luaInput_ResetTimer},  
	{"SetTimeMultiplier",luaInput_SetTimeMultiplier},
    {"GetTimeMultiplier",luaInput_GetTimeMultiplier},
	{"TakeScreenshot",luaInput_Takescreenshot},
	{"LoadBindings",luaInput_LoadBindings},
    {"GetActionStatus",luaInput_GetActionStatus},
    {"RemoveAction",luaInput_RemoveAction},
	{"RemoveUIAction",luaInput_RemoveUIAction},
    {"Reset",luaInput_Reset},
	{"Reinit",luaInput_Reinit},
	{"IsFireSwitched",luaInput_IsFireSwitched},
    {"GetTimeDelta",luaInput_GetTimeDelta},
    {"GetAsyncKeyState",luaInput_GetAsyncKeyState},
	{"GetUseDInput", luaInput_GetUseDInput},
	{"SetUseDInput", luaInput_SetUseDInput},
	{"GetDIDeltaScale", luaInput_GetDIDeltaScale},
	{"SetDIDeltaScale", luaInput_SetDIDeltaScale},
	{"GetKeyNameByEngName", luaInput_GetKeyNameByEngName},
	{"GetShortNameByEngName", luaInput_GetShortNameByEngName},
	{"GetEngNameByShortName", luaInput_GetEngNameByShortName},
#ifndef TRANSGAMING
	{"GetKBType", luaInput_GetKBType},
#else
    {"GetCurrentProfile", luaInput_getCurrentControlProfile},
    {"SetCurrentProfile", luaInput_setCurrentControlProfile},
#endif
	{"BindKeyCommand", luaInput_BindKeyCommand},
	{"GetLastTimeStep", luaInput_GetLastTimeStep},
	{NULL, NULL}
};


// =========================================================================
// DEBUG
// =========================================================================

static int luaDebug_SaveScreenShot(lua_State *state)
{
	static int ScreenNumber = 0;
	char	temp[256];

#ifdef TRANSGAMING_PS2
	snprintf(temp, 256, "host0:Screens/PainkillerPS2-%2.2d.tga", ScreenNumber++);
#else
	snprintf(temp, 256, "host0:Screens/PainkillerPSP-%2.2d.tga", ScreenNumber++);	//use ms0: for memory stick
#endif
	SaveScreenBuffer(temp);
}

#ifdef TRANSGAMING

static int luaDebug_setInGame(lua_State *state)
{
#if !defined(TRANSGAMING_MASTERING) && defined(MEMORY_TEST)
	Script          L(state);
    const char *    section = L.GetString(1, "");
    MemorySection   ms = MEM_COUNT;


    if (!strcasecmp(section, "MEM_SYSTEM"))
        ms = MEM_SYSTEM;

    else if (!strcasecmp(section, "MEM_MENU"))
        ms = MEM_MENU;

    else if (!strcasecmp(section, "MEM_LEVEL"))
        ms = MEM_LEVEL;


	m_SetInGame(ms);
#endif

    return 0;
}

static int luaDebug_dumpMemoryReport(lua_State *state){
#ifndef TRANSGAMING_MASTERING
	Script          L(state);
	const char *    filename = L.GetString(1);
	bool            overwrite = L.GetBool(2, true);
    bool            dumpAllocations = L.GetBool(3, false);
    bool            humanReadable = L.GetBool(4, true);


    TRACE("dumpMemoryReport(): dumping a memory report %s in %s format\n", dumpAllocations ? "with allocations" : "without allocations", humanReadable ? "human readable" : "CSV");
    m_dumpMemoryReport(filename, dumpAllocations, humanReadable);
#endif

    return 0;
}

static int luaDebug_setMemoryLogState(lua_State *state){
    Script      L(state);
    bool        s = L.GetBool(1);


    m_setLoggingState(s);
    TRACE("setMemoryLogState(): setting memory logging state to %s\n", s ? "TRUE" : "FALSE");


    return 0;
}

static int luaDebug_toggleMemoryLogState(lua_State *){
    static bool logState = m_getLoggingState();


    logState = !logState;
    m_setLoggingState(logState);
    TRACE("toggleMemoryLogState(): toggling memory logging state to %s\n", logState ? "TRUE" : "FALSE");

    return 0;
}

static int luaDebug_setTimingState(lua_State *state){
    Script      L(state);
    bool        s = L.GetBool(1);


    g_gatherTiming = s;
    TRACE("setTimingState(): setting timing state to %s\n", s ? "TRUE" : "FALSE");

    return 0;
}

static int luaDebug_toggleTimingState(lua_State *){

    g_gatherTiming = !g_gatherTiming;
    TRACE("toggleTimingState(): toggling timing state to %s\n", g_gatherTiming ? "TRUE" : "FALSE");

    return 0;
}

static int luaDebug_touch(lua_State *){
    //TRACE("touch(): touching the code from a lua script\n");

    return 0;
}

static int luaDebug_touch2(lua_State *){
    //TRACE("touch2(): touching the code from a lua script\n");

    return 0;
}

static int luaDebug_touch3(lua_State *){
    //TRACE("touch3(): touching the code from a lua script\n");

    return 0;
}

static int luaDebug_touch4(lua_State *){
    //TRACE("touch(): touching the code from a lua script\n");

    return 0;
}

static int luaDebug_touch5(lua_State *){
    //TRACE("touch2(): touching the code from a lua script\n");

    return 0;
}

static int luaDebug_touch6(lua_State *){
    //TRACE("touch3(): touching the code from a lua script\n");

    return 0;
}

static int luaDebug_DDPMsg(lua_State *state)
{
#ifdef TRANSGAMING_PS2
	Script L(state);

	const char *msg = L.GetString(1);
	printf("DDPMSG: %s\n", msg);
#endif

	return 0;
}


#ifdef DEBUG
static int luaDebug_enumerateTextures(lua_State *s){
    GEngine->Video->MaterialSys->EnumerateTextures();
    return 0;
}
#endif

static const luaL_reg debug_lib[] = {
    {"dumpMemoryReport",        luaDebug_dumpMemoryReport},
    {"setInGame",               luaDebug_setInGame},
    {"setMemoryLogState",       luaDebug_setMemoryLogState},
    {"toggleMemoryLogState",    luaDebug_toggleMemoryLogState},
    {"setTimingState",          luaDebug_setTimingState},
    {"toggleTimingState",       luaDebug_toggleTimingState},
    {"SaveScreenShot",          luaDebug_SaveScreenShot},
    {"DDPMsg",                  luaDebug_DDPMsg},
    {"touch",                   luaDebug_touch},
    {"touch2",                  luaDebug_touch2},
    {"touch3",                  luaDebug_touch3},
    {"touch4",                  luaDebug_touch4},
    {"touch5",                  luaDebug_touch5},
    {"touch6",                  luaDebug_touch6},
#ifdef DEBUG
    {"enumerateTextures",       luaDebug_enumerateTextures},
#endif
    {NULL, NULL}
};


// =========================================================================
// EDITOR
// =========================================================================
static int luaEditor_PostMessage(lua_State* state)
{
	Script L(state);

    DummyMessageHandler::QueueHorribleWindowsMessage(DummyMessageHandler::PKMSG_LOADLEVEL, L.GetInt(1),L.GetInt(2));
	return 0;
}

static int luaEditor_PostNextLeveletteMsg(lua_State *state){
    Script L(state);


    DummyMessageHandler::QueueHorribleWindowsMessage(DummyMessageHandler::PKMSG_LOADNEXTLEVELETTE, L.GetInt(1, 0),L.GetInt(2, 0));
	return 0;    
}


static const luaL_reg editor_lib[] = {
    {"PostMessage", luaEditor_PostMessage},
    {"PostNextLeveletteMsg", luaEditor_PostNextLeveletteMsg},
    {NULL, NULL}
};


// =========================================================================
// MEMORY
// =========================================================================
static int luaMemory_saveState(lua_State *state){
    Script L(state);


    Memory::safe.registerStateSave(L.GetString(1, NULL));
    return 0;
}

static int luaMemory_loadState(lua_State *state){
    Script L(state);


    Memory::safe.registerStateLoad(L.GetString(1, NULL));
    return 0;
}

#ifdef DEBUG
static int luaMemory_dumpModule(lua_State *state){
    Memory::safe.dumpModule();
    return 0;    
}
#endif


static const luaL_reg memory_lib[] = {
    {"saveState", luaMemory_saveState},
    {"loadState", luaMemory_loadState},
#ifdef DEBUG
    {"dumpModule", luaMemory_dumpModule},
#endif
    {NULL, NULL}
};

#endif
// =========================================================================

// =========================================================================
// MOUSE
// =========================================================================
static int luaMouse_GetPos(lua_State* state)
{
	Script L(state);
    if (GEngine->InputSys)
    {
        L.PushFloat(GEngine->InputSys->GetMousePosition().x);
        L.PushFloat(GEngine->InputSys->GetMousePosition().y);
    }
    else
    {
        L.PushFloat(0);
        L.PushFloat(0);
    }
	return 2;
}
// =========================================================================
static int luaMouse_GetDelta(lua_State* state)
{
    Script L(state);
    if (GEngine->InputSys)
    {
        L.PushFloat(GEngine->InputSys->GetMouseDelta().X);
        L.PushFloat(GEngine->InputSys->GetMouseDelta().Y);
    }
    else
    {
        L.PushFloat(0);
        L.PushFloat(0);
    }
    return 2;
}
// =========================================================================
static int luaMouse_SetPos(lua_State* state)
{
	Script L(state);
	if (GEngine->InputSys)
		GEngine->InputSys->SetMousePosition(L.GetInt(1),L.GetInt(2));
	return 0;
}
// =========================================================================
static int luaMouse_LB(lua_State* state)
{
	Script L(state);
	L.PushInt(GEngine->InputSys->GetKeyStatus(EMouseKeyLeft));
	return 1;
}
// =========================================================================
static int luaMouse_RB(lua_State* state)
{
	Script L(state);
	L.PushInt(GEngine->InputSys->GetKeyStatus(EMouseKeyRight));
	return 1;
}
// =========================================================================
static int luaMouse_MB(lua_State* state)
{
	Script L(state);
	L.PushInt(GEngine->InputSys->GetKeyStatus(EMouseKeyMiddle));
	return 1;
}
// =========================================================================
static int luaMouse_X(lua_State* state)
{
	Script L(state);
	L.PushFloat(GEngine->InputSys->GetMousePosition().x);
	return 1;
}
// =========================================================================
static int luaMouse_Y(lua_State* state)
{
	Script L(state);
	L.PushFloat(GEngine->InputSys->GetMousePosition().y);
	return 1;
}
// =========================================================================
static int luaMouse_SetSensitivity(lua_State* state)
{
	Script L(state);
    if (GEngine->InputSys)
        GEngine->InputSys->SetMouseSensitivity( L.GetFloat( 1, 0.11f ) );
	return 0;
}
// =========================================================================
static int luaMouse_SetWheelSensitivity(lua_State* state)
{
#ifndef TRANSGAMING
	Script L(state);
    if (GEngine->InputSys)
        GEngine->InputSys->SetWheelSensitivity( L.GetFloat( 1, 1.0f ) );
#endif
	return 0;
}
// =========================================================================
static int luaMouse_SetSmooth(lua_State* state)
{
	Script L(state);
    if (GEngine->InputSys)
        GEngine->InputSys->SetMouseSmooth( L.GetBool( 1, false ) );
	return 0;
}
// =========================================================================
static int luaMouse_Show(lua_State* state)
{
	Script L(state);
    if (GEngine->InputSys)
        GEngine->InputSys->ShowMouse( L.GetBool( 1,true) );
	return 0;
}
// =========================================================================
static int luaMouse_SetInverse(lua_State* state)
{
	Script L(state);
    if (GEngine->InputSys)
        GEngine->InputSys->InvertMouse = L.GetBool( 1, false );
	return 0;
}
// =========================================================================
static int luaMouse_Lock(lua_State* state)
{
    Script L(state);
    if (GEngine->InputSys)
        GEngine->InputSys->LockMouse(L.GetBool(1,true));
    return 0;
}
// =========================================================================
static int luaMouse_IsLocked(lua_State* state)
{
    Script L(state);

    if (!GEngine->InputSys)
        L.PushBool(true);
    else
        L.PushBool(GEngine->InputSys->IsMouseLocked());
    return 1;
}
// =========================================================================
static const luaL_reg mouse_lib[] = {
	{"GetPos",luaMouse_GetPos},
	{"SetPos",luaMouse_SetPos},
	{"X", luaMouse_X},
	{"Y", luaMouse_Y},
	{"LB", luaMouse_LB},
	{"RB", luaMouse_RB},
	{"MB", luaMouse_MB},
	{"SetSensitivity", luaMouse_SetSensitivity},
	{"SetWheelSensitivity", luaMouse_SetWheelSensitivity},
    {"Show", luaMouse_Show},
	{"SetInverse", luaMouse_SetInverse},
	{"SetSmooth", luaMouse_SetSmooth},
    {"Lock", luaMouse_Lock},
    {"IsLocked", luaMouse_IsLocked},
    {"GetDelta", luaMouse_GetDelta},
	{NULL, NULL}
};
// =========================================================================
// MODEL
// =========================================================================
static int luaModel_SetAnim(lua_State* state)
{
	Script L(state);
#ifndef TRANSGAMING
	Model *m = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
	Model *m = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif
	if (m && m->AsModel())
	{
		bool loop = L.GetBool(3,true);
		float speed = L.GetFloat(4,1.0f);
		float blendtime = L.GetFloat(5,0.2f);
		bool startAfterBlend = L.GetBool(8,false);
		int a = m->FindAnimation(L.GetString(2,"???"));
		if (a==-1)
		{
			a = m->LoadAnimation(L.GetString(2,"???"), loop);
		}
		//if (L.GetBool(8,false)) // movement curve X - narazie
		//	m->SetAnimationMovementCurve(a,"ROOOT",ModelAnimation::ETransX | ModelAnimation::ETransZ);
		//else
		//	m->SetAnimationMovementCurve(a,"ROOOT",ModelAnimation::ETransZ);
		int curve = L.GetInt(6, 0);
		if (curve > 0)
			m->SetAnimationMovementCurve(a,L.GetString(7,"ROOOT"),curve);
		m->ActivateAnimation(a,loop,speed,blendtime,startAfterBlend);

		L.PushInt(a);
	}
	else
		L.PushInt(-1);
	return 1;
}
// =========================================================================
static int luaModel_ResetFrame(lua_State* state)
{
	Script L(state);
#ifndef TRANSGAMING
	Model *m = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
	Model *m = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif
	if (m && m->AsModel())
		m->LastAnimateFrame = -1;
	return 0;
}
// =========================================================================
static int luaModel_LoadAnim(lua_State* state) // only for cache
{
	Script L(state);
    int a = -1;
#ifndef TRANSGAMING
	Model *m = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Model *m = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

	if (m && m->AsModel())
        a = m->LoadAnimation(L.GetString(2,"???"), L.GetBool(3,false));
    L.PushInt(a);
	return 1;
}
// =========================================================================
static int luaModel_GetAnim(lua_State* state)
{
	Script L(state);
#ifndef TRANSGAMING
	Model *m = (Model *)GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Model *m = (Model *)GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

	if (m && m->AsModel())
	{
		int a = m->FindAnimation(L.GetString(2));
		L.PushInt(a);
	}
	else
		L.PushInt(-1);
	return 1;
}
// =========================================================================
static int luaModel_EnableRagdoll(lua_State* state)
{
	Script L(state);
#ifndef TRANSGAMING
	Model *m = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Model *m = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

	if( m && m->AsModel())
		m->SetRagdoll(L.GetBool(2), L.GetInt(3,6));
	return 0;
}
// =========================================================================
static int luaModel_MakeGib(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Model *m = (Model*) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Model *m = (Model*) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif
	if (m && m->AsModel()) {
        char gibFile[MAX_PATH];


		//if (L.IsNil(3))
        snprintf(gibFile, MAX_PATH, "%s_gib", m->FileName.AsChar());
		//else
		//	gibFile = L.GetString(3);
		const char *getVelFromJoint = L.GetString(3,"");
#ifndef TRANSGAMING
		Model* gib = GEngine->World->GibModel( m, gibFile, 10, 6, 0, 0, L.GetInt(2), getVelFromJoint );
#else
		Model* gib = GEngine->pcfWorld->GibModel( m, gibFile, 10, 6, 0, 0, L.GetInt(2), getVelFromJoint );
#endif
		if (gib)
			L.PushInt(gib->GIndex);
		else
			L.PushNil();
	}
	else
		L.PushNil();
	return 1;
}
// =========================================================================
static int luaModel_SetRagdollMovedByExplosions(lua_State* state)
{
    Script L(state);
#ifndef TRANSGAMING
	Model *m = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Model *m = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

    if( !m || !m->AsModel() || !m->RagdollPtr ) return 0;
#ifndef TRANSGAMING_NO_HAVOK
    m->RagdollPtr->IsMovedByExplosions(L.GetBool(2));
#endif
    return 0;
}
// =========================================================================
static int luaModel_SetRagdollHardDeactivator(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Model *m = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Model *m = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif
	if( !m || !m->AsModel() || !m->RagdollPtr ) return 0;
#ifndef TRANSGAMING_NO_HAVOK
	m->RagdollPtr->SetHardDeactivator();
#endif
	return 0;
}
// =========================================================================
static int luaModel_SetRagdollBreakablesThreshold(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Model *m = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Model *m = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

	if( !m || !m->AsModel() || !m->RagdollPtr ) return 0;
#ifndef TRANSGAMING_NO_HAVOK
	m->RagdollPtr->SetBreakablesThreshold( L.GetFloat( 2, 10000000.0f) );
#endif
	return 0;
}
// =========================================================================
static int luaModel_SetRagdollFriction(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Model *m = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Model *m = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

	if( !m || !m->AsModel() || !m->RagdollPtr ) return 0;
#ifndef TRANSGAMING_NO_HAVOK
	m->RagdollPtr->SetFriction( L.GetFloat(2,1.0f) );
#endif
	return 0;
}
// =========================================================================
static int luaModel_SetRagdollRestitution(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Model *m = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
	Model *m = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

	if( !m || !m->AsModel() || !m->RagdollPtr ) return 0;
#ifndef TRANSGAMING_NO_HAVOK
	m->RagdollPtr->SetRestitution( L.GetFloat(2,1.0f) );
#endif
	return 0;
}
// =========================================================================
static int luaModel_SetRagdollLinearDamping(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Model *m = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Model *m = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

	if( !m || !m->AsModel() || !m->RagdollPtr ) return 0;
#ifndef TRANSGAMING_NO_HAVOK
	m->RagdollPtr->SetLinearDamping( L.GetFloat(2,0.05f) );
#endif
	return 0;
}
// =========================================================================
/*static int luaModel_SetRagdollGravity(lua_State* state)
{
	Script L(state);
#ifndef TRANSGAMING
	Model *m = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Model *m = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

	if( !m || !m->AsModel() || !m->RagdollPtr ) return 0;
#ifndef TRANSGAMING_NO_HAVOK
	m->RagdollPtr->EnableGravity( L.GetBool(2,false) );
#endif
	return 0;
}*/
// =========================================================================
static int luaModel_SetRagdollAngularDamping(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Model *m = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Model *m = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

	if( !m || !m->AsModel() || !m->RagdollPtr ) return 0;
#ifndef TRANSGAMING_NO_HAVOK
	m->RagdollPtr->SetAngularDamping( L.GetFloat(2,0.05f) );
#endif
	return 0;
}
// =========================================================================
static int luaModel_SetRagdollDeactivationHardness(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Model *m = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Model *m = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

	if( !m || !m->AsModel() || !m->RagdollPtr ) return 0;
#ifndef TRANSGAMING_NO_HAVOK
	m->RagdollPtr->SetDeactivationHardness( L.GetFloat(2) );
#endif
	return 0;
}
// =========================================================================
static int luaModel_ApplyPointImpulseToRagdoll(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Model *m = (Model *)GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Model *m = (Model *)GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

	if( !m || !m->AsModel() || !m->RagdollPtr ) return 0;
	Vector point(L.GetFloat(2),L.GetFloat(3),L.GetFloat(4));
	Vector imp(L.GetFloat(5),L.GetFloat(6),L.GetFloat(7));
#ifndef TRANSGAMING_NO_HAVOK
	m->RagdollPtr->ApplyPointImpulse( point, imp );
#endif
	return 0;
}
// =========================================================================
static int luaModel_IsRagdoll(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Model *m = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Model *m = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

    bool ret = false;
#ifndef TRANSGAMING_NO_HAVOK
	if (m && m->AsModel())
        ret = m->IsRagdoll();
#endif

    L.PushBool(ret);
	return 1;
}
// =========================================================================
static int luaModel_GetRagdollCollisionGroup(lua_State* state)
{
    Script L(state);

#ifndef TRANSGAMING
	Model *m = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Model *m = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

    int ret = 0;
#ifndef TRANSGAMING_NO_HAVOK
    if (m && m->AsModel() && m->RagdollPtr )
        ret = m->RagdollPtr->GetCollisionGroup();
#endif

    L.PushInt(ret);
    return 1;
}
// =========================================================================
static int luaModel_SetRagdollCollisionGroup(lua_State* state)
{
    Script L(state);

#ifndef TRANSGAMING
	Model *m = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Model *m = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

#ifndef TRANSGAMING_NO_HAVOK
    if (m && m->AsModel() && m->RagdollPtr )
        m->RagdollPtr->SetCollisionGroup(L.GetInt(2));
#endif
    return 0;
}
// =========================================================================
static int luaModel_IsRagdollActive(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Model *m = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Model *m = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

	if (m && m->AsModel())
		L.PushBool( m->RagDoll );
	else
		L.PushBool( false );
	return 1;
}
// =========================================================================
static int luaModel_GetAnimLength(lua_State* state)
{
	Script L(state);
	float time = 0.0f;

#ifndef TRANSGAMING
	Model *m = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Model *m = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

	if (m && m->AsModel())
		time = m->GetAnimationTotalTime(L.GetInt(2,-1));
	L.PushFloat(time);
	return 1;
}
// =========================================================================
static int luaModel_GetAnimTime(lua_State* state)
{
	Script L(state);
	float time = 0.0f;

#ifndef TRANSGAMING
	Model *m = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
	Model *m = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

	int a = L.GetInt(2,-1);
	if (m && m->AsModel() && a > -1)
		time = m->GetAnimationFrameTime(a);

	L.PushFloat(time);
	return 1;
}

// =========================================================================
static int luaModel_SetAnimTimeScale(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Model *m = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Model *m = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

	if (m && m->AsModel())
		m->SetAnimationTimeScale(L.GetInt(2,-1),L.GetFloat(3));
	return 0;
}
static int luaModel_SetAnimTime(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Model *m = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Model *m = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

	int a = L.GetInt(2,-1);
	if (m && m->AsModel() && a > -1) {
		float time = L.GetFloat(3,0.0);
		m->SetAnimationFrameTime(a,time);
	}
	return 0;
}


// =========================================================================
static int luaModel_GetAnimTimeScale(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Model *m = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Model *m = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

	if (m && m->AsModel())
		L.PushFloat(m->GetAnimationTimeScale(L.GetInt(2,-1)));
	else
		L.PushFloat(1.0f);
	return 1;
}

// =========================================================================
static int luaModel_SetHeadTrackCurve(lua_State* state)
{
/*	Script L(state);

#ifndef TRANSGAMING
	Model *m = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Model *m = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if (m)
		L.PushBool(m->SetHeadTrackCurve(L.GetInt(2),L.GetString(3)));
	else
		L.PushNil();
*/
	return 0;
}
// =========================================================================
static int luaModel_SetHeadTrackRot(lua_State* state)
{
/*	Script L(state);
#ifndef
    Model *m = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
	Model *m = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif
	if (m)
		m->SetHeadTrackRot(L.GetFloat(2),L.GetFloat(3),L.GetFloat(4));
*/
	return 0;
}
// =========================================================================
static int luaModel_ApplyJointRotation(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Model *m = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Model *m = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

	if (m && m->AsModel()) // ApplyJointRotation(int joint,float heading, float pitch, float yaw);
		m->ApplyJointRotation(L.GetInt(2),L.GetFloat(3),L.GetFloat(4),L.GetFloat(5));
	return 0;
}
// =========================================================================
static int luaModel_SetAnimationMovementCurve(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Model *m = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Model *m = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

	if (m && m->AsModel())
		m->SetAnimationMovementCurve(L.GetInt(2,-1),"ROOOT",ModelAnimation::ETransZ);
	return 0;
}
// =========================================================================
static int luaModel_GetAnimationMovement(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Model *m = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Model *m = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

	Vector v;
	if (m && m->AsModel())
		v = m->GetAnimationMovement(L.GetInt(2,-1),L.GetFloat(3));

	L.PushFloat(v.X);
	L.PushFloat(v.Y);
	L.PushFloat(v.Z);

	return 3;
}
// =========================================================================
static int luaModel_GetJointIndex(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Entity * e = GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Entity * e = GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

	int idx = -1;
	if (e && e->AsModel()) {
        Model *m = (Model *) e;
		const char* joint = L.GetString(2,"");
		idx = m->GetJointIndex(joint);
		if( idx >= 0 )
		{
			bool checkOnlyInragdoll = L.GetBool(3,false);
#ifndef TRANSGAMING_NO_HAVOK
			if( checkOnlyInragdoll && m->RagdollPtr && m->RagDoll && m->RagdollPtr->IsActive() && !m->RagdollPtr->Joint_InRagdoll( idx ) )
				idx = -1;
#endif
		}
	}
	L.PushInt(idx);
	return 1;
}
// =========================================================================
static int luaModel_GetJointName(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Entity * e = GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Entity * e = GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

	if( e && e->AsModel() )
#ifdef TRANSGAMING
		L.PushString(e->AsModel()->GetJointName(L.GetInt(2)));
#else
		L.PushString(e->AsModel()->GetJointName(L.GetInt(2)).AsChar());
#endif
    else
        L.PushString("???");

	return 1;
}
// =========================================================================
static int luaModel_GetRagdollJointPos(lua_State* state)
{
	Script L(state);
	Vector v(0,0,0);

#ifndef TRANSGAMING
	Entity * e = GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
	Entity * e = GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

	if( e &&  e->AsModel() )
	{
        Model *m = (Model *) e;
		int idx = L.GetInt(2);
		if( idx > -1 ) {
#ifndef TRANSGAMING_NO_HAVOK
			if (m->RagDoll && m->RagdollPtr)
				m->RagdollPtr->Joint_GetPosition(idx, v);
#endif
		}

	}
	L.PushFloat(v.X);
	L.PushFloat(v.Y);
	L.PushFloat(v.Z);
	return 3;
}

static int luaModel_GetJointPos(lua_State* state)
{
	Script L(state);
	Vector v(0,0,0);

#ifndef TRANSGAMING
	Entity * e = GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Entity * e = GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

	if( e &&  e->AsModel() )
	{
        Model *m = (Model *) e;
		int idx = L.GetInt(2);
		if( idx > -1 )
		{
			bool checkOnlyInragdoll = L.GetBool(3,false);
#ifndef TRANSGAMING_NO_HAVOK
			if( !checkOnlyInragdoll || !m->RagdollPtr || !m->RagDoll || !m->RagdollPtr->IsActive() || m->RagdollPtr->Joint_InRagdoll( idx ) )
				v = m->GetJointTransform(idx).GetTranslation();
#endif
		}
	}

	L.PushFloat(v.X);
	L.PushFloat(v.Y);
	L.PushFloat(v.Z);
	return 3;
}

static int luaModel_GetRagdollJointRotation(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Entity * e = GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Entity * e = GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

	Quaternion r(1,0,0,0);
	if( e && e->AsModel())
	{
        Model *m = (Model *) e;
		int idx = L.GetInt(2);
#ifndef TRANSGAMING_NO_HAVOK
		if( m && m->AsModel() && m->RagdollPtr )
			m->RagdollPtr->Joint_GetRotation(idx, r);
#endif
	}
	L.PushFloat(r.W);
	L.PushFloat(r.X);
	L.PushFloat(r.Y);
	L.PushFloat(r.Z);
	return 4;
}

// =========================================================================
static int luaModel_GetJointRotation(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Entity * e = GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Entity * e = GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

	Quaternion r(1,0,0,0);
	if( e && e->AsModel())
	{
        Model *m = (Model *) e;
		int idx = L.GetInt(2);

		Matrix mat = m->GetJointTransform(idx).GetRotation();
		r = Quaternion(m->GetJointTransform(idx).GetRotation());
	}
	L.PushFloat(r.W);
	L.PushFloat(r.X);
	L.PushFloat(r.Y);
	L.PushFloat(r.Z);
	return 4;
}
// =========================================================================
static int luaModel_CopyMatrixFromJointToJoint(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Model *ms = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
	int idxs = L.GetInt(2);
    Model *md = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(3),Entity::ETypeModel);
#else
    Model *ms = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
	int idxs = L.GetInt(2);
    Model *md = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(3),Entity::ETypeModel);
#endif

	int idxd = L.GetInt(4);

#ifndef FINAL_BUILD
	if( ms && !ms->AsModel() )
	{
		GLog.Print( "Warning: MODEL.CopyMatrixFromJointToJoint() given other entity 1 than model!!!\n" );
		GLog.Flush();
	}
	if( md && !md->AsModel() )
	{
		GLog.Print( "Warning: MODEL.CopyMatrixFromJointToJoint() given other entity 2 than model!!!\n" );
		GLog.Flush();
	}
#endif

#ifndef TRANSGAMING_NO_HAVOK
	if( ms && md )
	{
		Matrix mat = ms->GetJointTransform( idxs );
		md->RagdollPtr->Joint_SetMatrix( idxd, mat );
	}
#endif
	return 0;
}
// =========================================================================
static int luaModel_TransformPointByJoint(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Model *m = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Model *m = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

	int idx = L.GetInt(2,-1);
	Vector v(L.GetFloat(3,0),L.GetFloat(4,0),L.GetFloat(5,0));
	Quaternion r(1,0,0,0);

	if (m && m->AsModel() && idx >= 0)
	{
		v = m->GetJointTransform(idx).Transform(v);
		if (!(L.IsNil(8)))
		{
			Vector euler(L.GetFloat(6,0),L.GetFloat(7,0),L.GetFloat(8,0));
			r = EulerToQuat(euler) *Quaternion(m->GetJointTransform(idx).GetRotation());
		}
		else
			r = Quaternion(m->GetJointTransform(idx).GetRotation());
	}

	L.PushFloat(v.X);
	L.PushFloat(v.Y);
	L.PushFloat(v.Z);

	L.PushFloat(r.W);
	L.PushFloat(r.X);
	L.PushFloat(r.Y);
	L.PushFloat(r.Z);
	return 7;
}
// =========================================================================
static int luaModel_GetClosestJoint(lua_State* state)
{
    Script L(state);
    int idx = -1;
    float dist = 9999999;

#ifndef TRANSGAMING
	Entity *e = GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Entity *e = GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

    if (e && e->AsModel())
    {
        Model *m = e->AsModel();

        Vector v(L.GetFloat(2),L.GetFloat(3),L.GetFloat(4));
        for (int i=0;i<m->Skeleton.Joints.Size();i++)
            if (m->Skeleton.Joints[i].Global)
            {
                float d = (m->Skeleton.Joints[i].Transform.GetTranslation()-v).Magnitude();
                if (d < dist)
                {
                    dist = d;
                    idx = i;
                }
            }
    }
    L.PushInt(idx);
    L.PushFloat(dist);
    return 2;
}
// =========================================================================
static int luaModel_LineTrace(lua_State* state)
{
	HitData hitData;
	Script L(state);

#ifndef TRANSGAMING
	Model *m = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Model *m = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

	Assert( !m || ( m->Type == Entity::ETypeModel ) );
	if( m && m->RagdollPtr )
	{
		Vector start(L.GetFloat(2),L.GetFloat(3),L.GetFloat(4));
		Vector end(L.GetFloat(5),L.GetFloat(6),L.GetFloat(7));

#ifndef TRANSGAMING_NO_HAVOK
		if( m->RagdollPtr->LineTrace( start, end, hitData ) )
		{
			L.PushBool(true);
			L.PushFloat(hitData.Dist);
			L.PushFloat(hitData.HitPoint.X);
			L.PushFloat(hitData.HitPoint.Y);
			L.PushFloat(hitData.HitPoint.Z);
			L.PushFloat(hitData.Normal.X);
			L.PushFloat(hitData.Normal.Y);
			L.PushFloat(hitData.Normal.Z);
			L.PushLightUD(hitData.HavokEntity);

			if (hitData.UserData)
                L.PushInt(((Entity*)hitData.UserData)->GIndex);
			else
				L.PushNil();
			return 10;
		}
#else

#endif // TRANSGAMING_NO_HAVOK
	}
	L.PushBool(false);
	return 1;
}
// =========================================================================
// =========================================================================
static int luaModel_ApplyVelocitiesToAllJoints(lua_State* state) {
	Script L(state);

#ifndef TRANSGAMING
	Model *m = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Model *m = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

	if( !m || !m->AsModel() || !m->RagdollPtr ) return 0;
#ifndef TRANSGAMING_NO_HAVOK
	Vector linearVel(L.GetFloat(2),L.GetFloat(3),L.GetFloat(4));
	Vector angularVel(L.GetFloat(5),L.GetFloat(6),L.GetFloat(7));
	m->RagdollPtr->SetVelocities( linearVel, angularVel );
#endif
	return 0;
}
// =========================================================================
static int luaModel_MoveAllJoints(lua_State* state) {
	Script L(state);

#ifndef TRANSGAMING
	Model *m = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Model *m = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

	if( !m || !m->AsModel() || !m->RagdollPtr ) return 0;
#ifndef TRANSGAMING_NO_HAVOK
	Vector linearVel(L.GetFloat(2),L.GetFloat(3),L.GetFloat(4));
	m->RagdollPtr->Move( linearVel );
#endif
	return 0;
}
// =========================================================================
static int luaModel_GetJointFromHavokBody(lua_State* state) {
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	void *body = L.GetLightUD(2);
    if( e && e->AsModel() && body )
	{
#ifdef TRANSGAMING_NO_HAVOK
		L.PushInt(-1);
#else
		Model* m = e->AsModel();
		L.PushInt( m->RagdollPtr?m->RagdollPtr->GetJointFromHavokBody( body ):-1 );
#endif
	}
	else
		L.PushInt(-1);
	return 1;
}
// =========================================================================
static int luaModel_ApplyVelocitiesToJoint(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Model *m = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Model *m = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

	if( !m || !m->AsModel() || !m->RagdollPtr ) return 0;
#ifndef TRANSGAMING_NO_HAVOK
	Vector linearVel(L.GetFloat(3),L.GetFloat(4),L.GetFloat(5));
	Vector angularVel(L.GetFloat(6,0),L.GetFloat(7,0),L.GetFloat(8,0));
	m->RagdollPtr->Joint_SetVelocities( L.GetInt(2), linearVel, angularVel );
#endif
	return 0;
}
// =========================================================================
static int luaModel_GetVelocitiesFromJoint(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Model *m = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Model *m = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

	Vector lv(0,0,0);
	Vector av(0,0,0);

	if( !m || !m->AsModel() || !m->RagdollPtr ) {
		L.PushFloat(lv.X); L.PushFloat(lv.Y); L.PushFloat(lv.Z);
		L.PushFloat(0);
		L.PushFloat(av.X); L.PushFloat(av.Y); L.PushFloat(av.Z);
		L.PushFloat(0);
		return 8;
	}

#ifndef TRANSGAMING_NO_HAVOK
	if( m->RagdollPtr->IsActive() )
	{
		lv = m->RagdollPtr->Joint_GetLinearVelocity( L.GetInt(2));
		av = m->RagdollPtr->Joint_GetAngularVelocity( L.GetInt(2));
	}
#endif

	L.PushFloat(lv.X); L.PushFloat(lv.Y); L.PushFloat(lv.Z);
	L.PushFloat(lv.Magnitude());
	L.PushFloat(av.X); L.PushFloat(av.Y); L.PushFloat(av.Z);
	L.PushFloat(av.Magnitude());
	return 8;
}
// =========================================================================
static int luaModel_ApplyVelocitiesToJointLinked(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Model *m = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Model *m = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

	if( !m || !m->AsModel() || !m->RagdollPtr ) return 0;
#ifndef TRANSGAMING_NO_HAVOK
	Vector linearVel(L.GetFloat(3),L.GetFloat(4),L.GetFloat(5));
	Vector angularVel(L.GetFloat(6),L.GetFloat(7),L.GetFloat(8));
	m->RagdollPtr->Joint_SetVelocitiesForLinked( L.GetInt(2), linearVel, angularVel );
#endif
	return 0;
}
// =========================================================================
static int luaModel_ApplyPositionToJoint(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Model *m = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Model *m = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

	if( !m || !m->AsModel() || !m->RagdollPtr ) return 0;
#ifndef TRANSGAMING_NO_HAVOK
	Vector pos(L.GetFloat(3),L.GetFloat(4),L.GetFloat(5));
	m->RagdollPtr->Joint_SetPosition( L.GetInt(2), pos );
#endif
	return 0;
}
// =========================================================================
static int luaModel_SetJointPositionLowLevel(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Model *m = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Model *m = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

	if( !m || !m->AsModel() || !m->RagdollPtr ) return 0;
#ifndef TRANSGAMING_NO_HAVOK
	Vector pos(L.GetFloat(3),L.GetFloat(4),L.GetFloat(5));
	m->RagdollPtr->Joint_SetPositionLL( L.GetInt(2), pos );
#endif
	return 0;
}

// =========================================================================
static int luaModel_ApplyRotationToJoint(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Model *m = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Model *m = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

	if( !m || !m->AsModel() || !m->RagdollPtr ) return 0;
	Quaternion q;
	if (L.IsNil(6)) {
		Vector pos(L.GetFloat(3),L.GetFloat(4),L.GetFloat(5));
		q = EulerToQuat(pos);
	}
	else
		q = Quaternion(L.GetFloat(3),L.GetFloat(4),L.GetFloat(5),L.GetFloat(6));
#ifndef TRANSGAMING_NO_HAVOK
	m->RagdollPtr->Joint_SetRotation( L.GetInt(2), q );
#endif
	return 0;
}

static int luaModel_ApplyRotationQuaternionToJoint(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Model *m = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Model *m = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

	if( !m || !m->AsModel() || !m->RagdollPtr ) return 0;
#ifndef TRANSGAMING_NO_HAVOK
	Quaternion q = Quaternion(L.GetFloat(3), L.GetFloat(4), L.GetFloat(5), L.GetFloat(6));
	m->RagdollPtr->Joint_SetRotation( L.GetInt(2), q );
#endif
	return 0;
}
// =========================================================================
static int luaModel_SetPinned(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Model *m = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Model *m = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

	if( !m || !m->AsModel() || !m->RagdollPtr ) return 0;
#ifndef TRANSGAMING_NO_HAVOK
	m->RagdollPtr->SetPinned( L.GetBool(2) );
#endif
	return 0;
}
// =========================================================================
static int luaModel_IsPinned(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Model *m = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Model *m = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

#ifdef TRANSGAMING_NO_HAVOK
	L.PushBool(false);
#else
	L.PushBool( ( !m || !m->AsModel() || !m->RagdollPtr )?false:m->RagdollPtr->IsPinned() );
#endif
	return 1;
}
// =========================================================================
static int luaModel_SetPinnedJoint(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Model *m = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Model *m = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

	if( !m || !m->AsModel() || !m->RagdollPtr ) return 0;
#ifndef TRANSGAMING_NO_HAVOK
	m->RagdollPtr->Joint_SetPinned( L.GetInt(2), L.GetBool(3) );
#endif
	return 0;
}
// =========================================================================
static int luaModel_IsPinnedJoint(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Model *m = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Model *m = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

#ifdef TRANSGAMING_NO_HAVOK
	L.PushBool(false);
#else
	L.PushBool( ( !m || !m->AsModel() || !m->RagdollPtr )?false:m->RagdollPtr->Joint_IsPinned( L.GetInt(2) ) );
#endif
	return 1;
}
// =========================================================================
static int luaModel_EnableJoint(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Model *m = (Model*) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
	Model *m = (Model*) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

#ifndef TRANSGAMING_NO_HAVOK
	if( m && m->AsModel() && m->RagdollPtr )
		m->RagdollPtr->Joint_Enable( L.GetInt(2), L.GetBool(3,false) );
#endif
	return 0;
}
// =========================================================================
static int luaModel_JointsLinked(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Model *m = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Model *m = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

    int j1 = L.GetInt(2);
    int j2 = L.GetInt(3);

#ifdef TRANSGAMING_NO_HAVOK
    L.PushBool(false);
#else
    L.PushBool( ( !m || !m->AsModel() || !m->RagdollPtr || j1 < 0 || j2 < 0)?false:m->RagdollPtr->Joint_AreLinked( j1, j2 ) );
#endif
	return 1;
}
// =========================================================================
static int luaModel_BreakConstraintsForJoint(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Model *m = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Model *m = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

	if( !m || !m->AsModel() || !m->RagdollPtr ) return 0;
#ifndef TRANSGAMING_NO_HAVOK
	m->RagdollPtr->Joint_BreakConstraints( L.GetInt(2) );
#endif
	return 0;
}
// =========================================================================
static int luaModel_SetMaterialSpecular(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Model *m = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Model *m = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

	if (m && m->AsModel())
	{
		Vector col(L.GetFloat(3)/255.0f,L.GetFloat(4)/255.0f,L.GetFloat(5)/255.0f);
		m->SetMaterialSpecular(L.GetString(2),col,L.GetFloat(6));
	}
	return 0;
}
// =========================================================================
static int luaModel_SetMaterialRefractFresnel(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Model *m = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Model *m = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

	if (m && m->AsModel())
	{
		Vector reflTint(L.GetFloat(5)/255.0f,L.GetFloat(6)/255.0f,L.GetFloat(7)/255.0f);
		Vector refrTint(L.GetFloat(8)/255.0f,L.GetFloat(9)/255.0f,L.GetFloat(10)/255.0f);
		m->SetMaterialRefractFresnel(L.GetString(2),L.GetFloat(3),L.GetFloat(4),reflTint,refrTint);
	}
	return 0;
}
// =========================================================================
static int luaModel_SetMeshVisibility(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Model *m = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Model *m = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

	if (m && m->AsModel())
		m->SetMeshVisibility(L.GetString(2),L.GetBool(3, false));
	return 0;
}

// =========================================================================
static int luaModel_SetMeshLighting(lua_State* state)
{
    Script L(state);

#ifndef TRANSGAMING
	Model *m = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Model *m = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

    if (m && m->AsModel())
	{
		Vector color(1,1,1);
		if (!L.IsNil(6))
			color.Set(L.GetFloat(4)/255.0f,L.GetFloat(5)/255.0f,L.GetFloat(6)/255.0f);
        m->SetMeshLighting(L.GetString(2),L.GetBool(3, true), color);
	}
    return 0;
}
// =========================================================================
static int luaModel_ResetMaterialSpecular(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Model *m = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Model *m = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

	if (m && m->AsModel())
		m->ResetMaterialSpecular();
	return 0;
}
// =========================================================================
static int luaModel_SetTexture(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Model *m = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Model *m = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

	if (m && m->AsModel())
		m->SetTexture(L.GetString(2),L.GetString(3));
	return 0;
}
// =========================================================================
static int luaModel_SetMaterial(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Model *m = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Model *m = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

	if (m && m->AsModel())
		m->SetMaterial(L.GetString(2));
	return 0;
}
// =========================================================================
static int luaModel_SetBlendAlpha(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Model *m = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
	Model *m = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

	if (m && m->AsModel())
		m->SetBlendAlpha(L.GetFloat(2));
	return 0;
}
// =========================================================================
static int luaModel_EnableNormalMaps(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Model *m = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Model *m = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

	bool useNormalMaps = L.GetBool(2);

	if (m && m->AsModel() && m->Mesh && (useNormalMaps != m->Mesh->UseNormalMaps)) {
		m->Mesh->UseNormalMaps = useNormalMaps;
		m->Mesh->RenderCleanup();
		m->Mesh->RenderInitialize();
	}
	return 0;
}
// =========================================================================
static int luaModel_SetWaterImpact(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Model *m = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Model *m = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

	if (m && m->AsModel())
	{
		Vector epicenter(L.GetFloat(2),L.GetFloat(3),L.GetFloat(4));

		// domyslne parametry dobrane do skali swamp bossa 20!
		float amplitude = L.GetFloat(5,1.0f);
		float period    = L.GetFloat(6,1.0f);
		float range     = L.GetFloat(7,20.0f);
		float speed     = L.GetFloat(8,6.0f);
		int no          = L.GetInt(9,0);

		m->SetWaterImpact(no,epicenter,amplitude,period,range,speed);
	}

	return 0;
}
// =========================================================================
static int luaModel_UpdateWaterImpact(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Model *m = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Model *m = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

	if (m && m->AsModel())
	{
		Vector epicenter(L.GetFloat(2),L.GetFloat(3),L.GetFloat(4));

		// domyslne parametry dobrane do skali swamp bossa 20!
		float amplitude = L.GetFloat(5,1.0f);
		float period    = L.GetFloat(6,1.0f);
		float range     = L.GetFloat(7,20.0f);
		float speed     = L.GetFloat(8,6.0f);
		int no          = L.GetInt(9,0);

		m->UpdateWaterImpact(no,epicenter,amplitude,period,range,speed);
	}
	return 0;
}
// =========================================================================
static int luaModel_DrawJointNames(lua_State* state)
{
#ifdef TRANSGAMING_USE_EDITOR
	Script L(state);

# ifndef TRANSGAMING
	Model *m = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
# else
    Model *m = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
# endif

# ifdef	TRANSGAMING
	if (m && m->AsModel())
		m->DrawJointNames(Color_TG(255,255,255,255));
# else
	if (m && m->AsModel())
		m->DrawJointNames(Color(255,255,255,255));
# endif
#endif
	return 0;
}
// =========================================================================
static int luaModel_CreateShadowMap(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Model *m = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Model *m = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

	if (m && m->AsModel())
        m->CreateShadowMap(L.GetInt(2,128)?true:false);
	return 0;
}
// =========================================================================
static int luaModel_SetTexturesPriority(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Model *m = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Model *m = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

	DWORD priority = L.GetInt(2,0);

	if (m && m->AsModel() && m->Mesh)
	{
		DynamicArray<MeshShape> &meshes = m->Mesh->Meshes;

		for (int i=0; i<meshes.Size(); ++i)
			meshes[i].SetTexturesPriority(priority);
	}
	return 0;
}
// =========================================================================
static int luaModel_EternallyFreeze(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Model *m = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Model *m = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

	if( !m || !m->AsModel() || !m->RagdollPtr ) return 0;
#ifndef TRANSGAMING_NO_HAVOK
	m->RagdollPtr->EternalFreeze();
#endif
	return 0;
}
// =========================================================================
static int luaModel_RagdollSelfExplosion(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Model *m = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Model *m = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

	if( !m || !m->AsModel() || !m->RagdollPtr ) return 0;

#ifndef TRANSGAMING_NO_HAVOK
	Vector center( L.GetFloat(2),L.GetFloat(3),L.GetFloat(4) );
	float strength = L.GetFloat(5);
	float range = L.GetFloat(6);
	m->RagdollPtr->SelfExplosion( center, strength, range );
#endif
	return 0;
}
// =========================================================================
static const luaL_reg model_lib[] =
{
  {"SetAnim", luaModel_SetAnim},
  {"LoadAnim", luaModel_LoadAnim},
  {"GetAnim", luaModel_GetAnim},
  {"EnableRagdoll", luaModel_EnableRagdoll},
  {"MakeGib", luaModel_MakeGib},
  {"IsRagdoll", luaModel_IsRagdoll},
  {"IsRagdollActive", luaModel_IsRagdollActive},
  {"GetRagdollCollisionGroup", luaModel_GetRagdollCollisionGroup},
  {"SetRagdollCollisionGroup", luaModel_SetRagdollCollisionGroup},
  {"ApplyPointImpulseToRagdoll", luaModel_ApplyPointImpulseToRagdoll},
  {"GetAnimLength", luaModel_GetAnimLength},
  {"GetAnimTime", luaModel_GetAnimTime},
  {"SetAnimTime", luaModel_SetAnimTime},
  {"SetAnimTimeScale", luaModel_SetAnimTimeScale},
  {"GetAnimTimeScale", luaModel_GetAnimTimeScale},
  {"GetJointIndex", luaModel_GetJointIndex},
  {"GetRagdollJointPos", luaModel_GetRagdollJointPos},
  {"GetJointPos", luaModel_GetJointPos},
  {"GetRagdollJointRotation",luaModel_GetRagdollJointRotation},
  {"GetJointRotation", luaModel_GetJointRotation},
  {"CopyMatrixFromJointToJoint", luaModel_CopyMatrixFromJointToJoint},
  {"TransformPointByJoint", luaModel_TransformPointByJoint},
  {"BreakConstraintsForJoint", luaModel_BreakConstraintsForJoint},
  {"ApplyVelocitiesToAllJoints", luaModel_ApplyVelocitiesToAllJoints},
  {"MoveAllJoints", luaModel_MoveAllJoints},
  {"GetJointFromHavokBody", luaModel_GetJointFromHavokBody},
  {"ApplyVelocitiesToJoint", luaModel_ApplyVelocitiesToJoint},
  {"ApplyVelocitiesToJointLinked", luaModel_ApplyVelocitiesToJointLinked},
  {"SetJointPositionLowLevel", luaModel_SetJointPositionLowLevel},
  {"ApplyPositionToJoint", luaModel_ApplyPositionToJoint},
  {"ApplyRotationToJoint", luaModel_ApplyRotationToJoint},
  {"ApplyRotationQuaternionToJoint", luaModel_ApplyRotationQuaternionToJoint},
  {"GetVelocitiesFromJoint", luaModel_GetVelocitiesFromJoint},
  {"SetAnimMovementCurve",luaModel_SetAnimationMovementCurve},
  {"GetAnimMovement",luaModel_GetAnimationMovement},
  {"SetMaterialSpecular",luaModel_SetMaterialSpecular},
  {"SetBlendAlpha",luaModel_SetBlendAlpha},
  {"SetMaterialRefractFresnel",luaModel_SetMaterialRefractFresnel},
  {"SetMeshVisibility",luaModel_SetMeshVisibility},
  {"SetMeshLighting",luaModel_SetMeshLighting},
  {"SetTexture",luaModel_SetTexture},
  {"SetTexturesPriority",luaModel_SetTexturesPriority},
  {"EnableNormalMaps",luaModel_EnableNormalMaps},
  {"SetMaterial",luaModel_SetMaterial},
  {"SetWaterImpact",luaModel_SetWaterImpact},
  {"UpdateWaterImpact",luaModel_UpdateWaterImpact},
  {"SetRagdollMovedByExplosions",luaModel_SetRagdollMovedByExplosions},
  {"SetRagdollHardDeactivator",luaModel_SetRagdollHardDeactivator},
  {"ResetMaterialSpecular",luaModel_ResetMaterialSpecular},
  {"DrawJointNames",luaModel_DrawJointNames},
  {"SetHeadTrackCurve",luaModel_SetHeadTrackCurve},
  {"SetHeadTrackRot",luaModel_SetHeadTrackRot},
  {"ApplyJointRotation",luaModel_ApplyJointRotation},
  {"SetPinned", luaModel_SetPinned},
  {"IsPinned", luaModel_IsPinned},
  {"SetPinnedJoint", luaModel_SetPinnedJoint},
  {"IsPinnedJoint", luaModel_IsPinnedJoint},
  {"EnableJoint", luaModel_EnableJoint},
  {"JointsLinked", luaModel_JointsLinked},
  {"EternallyFreeze", luaModel_EternallyFreeze},
  {"ResetFrame", luaModel_ResetFrame},

  {"SetRagdollBreakablesThreshold", luaModel_SetRagdollBreakablesThreshold},
  {"SetRagdollFriction", luaModel_SetRagdollFriction},
  {"SetRagdollRestitution", luaModel_SetRagdollRestitution},
  {"SetRagdollLinearDamping", luaModel_SetRagdollLinearDamping},
  {"SetRagdollAngularDamping", luaModel_SetRagdollAngularDamping},
  {"SetRagdollDeactivationHardness", luaModel_SetRagdollDeactivationHardness},
  {"GetClosestJoint", luaModel_GetClosestJoint},
  {"GetJointName", luaModel_GetJointName},
  {"LineTrace",luaModel_LineTrace},
  {"RagdollSelfExplosion",luaModel_RagdollSelfExplosion},
  {"CreateShadowMap", luaModel_CreateShadowMap},
  {NULL, NULL}
};

// =========================================================================
// MESH
// =========================================================================

static int luaMesh_SetMeshGroup(lua_State* state)
{
    Script L(state);

#ifndef TRANSGAMING
	WorldMesh *m = (WorldMesh *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeMesh);
#else
	WorldMesh *m = (WorldMesh *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeMesh);
#endif

	if( m && m->AsMesh() ) {
		m->ActiveGroup = L.GetInt(2);
	}
    return 0;
}

// =========================================================================
static int luaMesh_SetLighting(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	WorldMesh *m = (WorldMesh *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeMesh);
#else
    WorldMesh *m = (WorldMesh *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeMesh);
#endif

#ifndef FINAL_BUILD
	if( m && !m->AsMesh() )
	{
		GLog.Print( "Warning: MESH.SetLighting() given other entity than mesh!!!\n" );
		GLog.Flush();
	}
#endif

	if (m)
	{
		if (L.GetBool(2)) {
			m->Flags &= ~SimpleMesh::ENoLight;
		}
		else {
			m->Flags |= SimpleMesh::ENoLight;
		}
	}
	return 0;
}
// =========================================================================
static int luaMesh_SetDefaultDetailMaps(lua_State* state)
{
#ifndef TRANSGAMING
	Script L(state);

#ifndef TRANSGAMING
	GEngine->World->MeshesSetDefaultDetailMaps(L.GetString(1,NULL),L.GetFloat(2,8.2f),L.GetFloat(3,7.1f));
#else
	GEngine->pcfWorld->MeshesSetDefaultDetailMaps(L.GetString(1,NULL),L.GetFloat(2,8.2f),L.GetFloat(3,7.1f));
#endif
#endif

	return 0;
}
// =========================================================================
#ifndef TRANSGAMING
static int luaMesh_SetDetailMap(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	WorldMesh *m = (WorldMesh *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeMesh);
#else
    WorldMesh *m = (WorldMesh *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeMesh);
#endif

#ifndef FINAL_BUILD
	if( m && !m->AsMesh() )
	{
		GLog.Print( "Warning: MESH.SetDetailMap() given other entity than mesh!!!\n" );
		GLog.Flush();
	}
#endif

	if (m)
	{
		const char* name = L.GetString(2,NULL);
        if (!m->DetailMap.Name.EqualsNoCase(name)) {
			m->SetTextureName(SimpleMesh::ETexDetailMap, name);
			m->ReloadTextures();
		}
		m->DetailMap.Tile.U = L.GetFloat(3,8.2f);
		m->DetailMap.Tile.V = L.GetFloat(4,7.1f);
	}
	return 0;
}
// =========================================================================
static int luaMesh_GetDetailMap(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	WorldMesh *m = (WorldMesh *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeMesh);
#else
    WorldMesh *m = (WorldMesh *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeMesh);
#endif

#ifndef FINAL_BUILD
	if( m && !m->AsMesh() )
	{
		GLog.Print( "Warning: MESH.GetDetailMap() given other entity than mesh!!!\n" );
		GLog.Flush();
	}
#endif

    if (m && m->CubeMapName.Size()>0)
        GScript.PushString(m->DetailMap.Name.AsChar());
    else
        GScript.PushString("");

	GScript.PushFloat(m->DetailMap.Tile.U);
	GScript.PushFloat(m->DetailMap.Tile.V);

    return 3;
}
#endif
// =========================================================================
static int luaMesh_SetDefaultCubeMaps(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	GEngine->World->MeshesSetDefaultCubeMaps(L.GetString(1,NULL));
#else
	GEngine->pcfWorld->MeshesSetDefaultCubeMaps(L.GetString(1,NULL));
#endif

	return 0;
}
// =========================================================================
static int luaMesh_SetCubeMap(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	WorldMesh *m = (WorldMesh *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeMesh);
#else
    WorldMesh *m = (WorldMesh *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeMesh);
#endif

#ifndef FINAL_BUILD
	if( m && !m->AsMesh() )
	{
		GLog.Print( "Warning: MESH.SetCubeMap() given other entity than mesh!!!\n" );
		GLog.Flush();
	}
#endif

	if (m)
	{
		const char* name = L.GetString(2,NULL);
        if (!m->CubeMapName.EqualsNoCase(name)) {
			m->SetTextureName(SimpleMesh::ETexCubeMap, name);
			m->ReloadTextures();
		}
	}
	return 0;
}
// =========================================================================
static int luaMesh_GetCubeMap(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	WorldMesh *m = (WorldMesh *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeMesh);
#else
    WorldMesh *m = (WorldMesh *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeMesh);
#endif

    if (m && m->CubeMapName.Size()>0)
        GScript.PushString(m->CubeMapName.AsChar());
    else
        GScript.PushString("");

    return 1;
}
// =========================================================================
#ifndef TRANSGAMING
static int luaMesh_SetDefaultNormalMaps(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	GEngine->World->MeshesSetDefaultNormalMaps(L.GetString(1,NULL));
#else
	GEngine->pcfWorld->MeshesSetDefaultNormalMaps(L.GetString(1,NULL));
#endif

	return 0;
}
// =========================================================================
static int luaMesh_SetNormalMap(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	WorldMesh *m = (WorldMesh *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeMesh);
#else
    WorldMesh *m = (WorldMesh *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeMesh);
#endif

	if (m)
	{
		const char* name = L.GetString(2,NULL);
        if (!m->NormalMapName.EqualsNoCase(name)) {
			m->SetTextureName(SimpleMesh::ETexNormalMap, name);
			m->ReloadTextures();
		}
	}
	return 0;
}
// =========================================================================
static int luaMesh_GetNormalMap(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	WorldMesh *m = (WorldMesh *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeMesh);
#else
    WorldMesh *m = (WorldMesh *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeMesh);
#endif

    if (m && m->NormalMapName.Size()>0)
        GScript.PushString(m->NormalMapName.AsChar());
    else
        GScript.PushString("");

    return 1;
}
#endif
// =========================================================================
static int luaMesh_GetLightMap(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	WorldMesh *m = (WorldMesh *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeMesh);
#else
	WorldMesh *m = (WorldMesh *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeMesh);
#endif

	if (m && m->LightMapName.Size()>0)
		GScript.PushString(m->LightMapName.AsChar());
	else
		GScript.PushString("");

	return 1;
}
// =========================================================================
static int luaMesh_GetTextures(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	WorldMesh *mesh = (WorldMesh *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeMesh);
#else
	WorldMesh *mesh = (WorldMesh *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeMesh);
#endif

	L.NewTable();
	for (int i=0; i < mesh->Data->Groups.Size(); i++)
	{
		{
			L.PushInt(i+1);
			L.PushString(mesh->Data->Groups[i].TexName.Size()?mesh->Data->Groups[i].TexName.AsChar():"");
			L.SetTable(-3);
		}
	}

	return 1;
}
// =========================================================================
static int luaMesh_GetRandomPoint(lua_State* state)
{
	Script L(state);
    Vector point(0,0,0);

#ifndef TRANSGAMING
	WorldMesh *m = (WorldMesh *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeMesh);
#else
    WorldMesh *m = (WorldMesh *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeMesh);
#endif

    if (m)
        point = m->GetRandomPoint();

    GScript.PushFloat(point.X);
    GScript.PushFloat(point.Y);
    GScript.PushFloat(point.Z);
    return 3;
}
// =========================================================================
static int luaMesh_SetDefaultMaterial(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	WorldMesh *m = (WorldMesh *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeMesh);
#else
    WorldMesh *m = (WorldMesh *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeMesh);
#endif

	if (m)
	{
		const char* name = L.GetString(2,NULL);
		m->SetDefaultMaterial(name);
	}
	return 0;
}

// =========================================================================
static int luaMesh_SetSpecular(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	WorldMesh *m = (WorldMesh *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeMesh);
#else
    WorldMesh *m = (WorldMesh *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeMesh);
#endif

	if (m)
	{
		float Power = L.GetFloat(2, 8.0F);
		m->SetSpecular(Power);
	}

	return 0;
}

static int luaMesh_AddSpecularLight(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	WorldMesh *m = (WorldMesh *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeMesh);
#else
    WorldMesh *m = (WorldMesh *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeMesh);
#endif

	if (m)
	{
#ifndef TRANSGAMING
		Light* light = (Light*) GEngine->World->GetEntityByIndex(L.GetInt(2), Entity::ETypeLight);
#else
	    Light* light = (Light*) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(2), Entity::ETypeLight);
#endif
		Assert(light);
		m->AddSpecularLight(light);
	}
	return 0;
}

static int luaMesh_ResetSpecularLights(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	WorldMesh *m = (WorldMesh *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeMesh);
#else
    WorldMesh *m = (WorldMesh *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeMesh);
#endif

	if (m)
	{
		m->SpecularLights[0] = NULL;
		m->SpecularLights[1] = NULL;
	}
	return 0;
}

// =========================================================================
static const luaL_reg mesh_lib[] = {
  {"SetLighting", luaMesh_SetLighting},
  {"SetDefaultDetailMaps", luaMesh_SetDefaultDetailMaps},
  {"SetDefaultCubeMaps", luaMesh_SetDefaultCubeMaps},
  {"SetCubeMap", luaMesh_SetCubeMap},
  {"GetCubeMap", luaMesh_GetCubeMap},
#ifndef TRANSGAMING
  {"SetDefaultNormalMaps", luaMesh_SetDefaultNormalMaps},
  {"SetDetailMap", luaMesh_SetDetailMap},
  {"GetDetailMap", luaMesh_GetDetailMap},
  {"SetNormalMap", luaMesh_SetNormalMap},
  {"GetNormalMap", luaMesh_GetNormalMap},
#endif
  {"GetLightMap", luaMesh_GetLightMap},
  {"GetRandomPoint", luaMesh_GetRandomPoint},
  {"SetMeshGroup", luaMesh_SetMeshGroup},
  {"SetDefaultMaterial", luaMesh_SetDefaultMaterial},
  {"SetSpecular", luaMesh_SetSpecular},
  {"AddSpecularLight", luaMesh_AddSpecularLight},
  {"ResetSpecularLights",luaMesh_ResetSpecularLights},
  {"GetTextures",luaMesh_GetTextures},
  {NULL, NULL}
};

// =========================================================================
// ENTITY
// =========================================================================
static int luaEntity_Create(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Entity * pe = GEngine->World->CreateEntity(L.GetInt(1),L.GetString(2),L.GetString(3),L.GetFloat(4,1),L.GetBool(5,false) );
#else
    Entity * pe = GEngine->pcfWorld->CreateEntity(L.GetInt(1),L.GetString(2),L.GetString(3),L.GetFloat(4,1),L.GetBool(5,false) );
#endif
    //if( !pe && GEngine->Net && !GEngine->Net->IsServer() )		-- wywalalo box-a gdy nie ma karty dzwiekowej, Marek
    //    MessageBox(NULL,L.GetString(2),"Cannot create entity:",MB_TOPMOST);
    L.PushInt(pe?pe->GIndex:0);
    L.PushInt(pe?pe->GIndex:0);
	return 2;
}
// =========================================================================
static int luaEntity_GetIndex(lua_State* state)
{
    Script L(state);
    int idx = 0;
#ifndef TRANSGAMING
	Entity *e = GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

    if( e)
        idx = e->GIndex;

    L.PushInt(idx);
    return 1;
}
// =========================================================================
static int luaEntity_GetPtrByIndex(lua_State* state)
{
    Script L(state);

#ifndef TRANSGAMING
	Entity *e = GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if (e)
        L.PushInt(e->GIndex);
    else
        L.PushNil();
    return 1;
}
// =========================================================================
static int luaEntity_Exist(lua_State* state)
{
    Script L(state);

#ifndef TRANSGAMING
	Entity *e = GEngine->World->GetEntityByIndex(L.GetInt(1,0));
#else
	Entity *e = GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1,0));
#endif

    L.PushBool(e?true:false);
    return 1;
}
// =========================================================================
static int luaEntity_SetTimeToDie(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if( e)
		e->TimeToDelete = L.GetFloat(2);
	return 1;
}
// =========================================================================
static int luaEntity_EnableNetworkSynchronization(lua_State* state)
{
    Script L(state);

#ifndef TRANSGAMING
	Entity *e = GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
	Entity *e = GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

    if( e)
    {
        if (L.GetBool(2,true))
            e->SetSynchroState( L.GetBool(3,false) ? Entity::ESynchroEntity|Entity::ESynchroAnimation : Entity::ESynchroEntity ,L.GetInt(4,0),L.GetInt(5,SERVER_ID),L.GetInt(6,0));
        else
            e->SetSynchroState( 0, 0, SERVER_ID, 0 );
    }
    return 0;
}
// =========================================================================
static int luaEntity_SetSynchroString(lua_State* state)
{
    Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

    if( e)
        e->SetSynchroString(L.GetString(2));
    return 0;
}
// =========================================================================
static int luaEntity_GetSynchroString(lua_State* state)
{
    Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
	Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

    if( e && e->GetSynchroString())
        L.PushString(e->GetSynchroString());
    else
        L.PushString("");
    return 1;
}
// =========================================================================
static int luaEntity_RegisterChild(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Entity * e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
    Entity * child =  (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(2));
#else
    Entity * e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
    Entity * child =  (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(2));
#endif

	if( e && child)
	{
		int joint = -1;
		if (!L.IsNil(4))
		{
			if (L.GetType(4) == LUA_TNUMBER)
				joint = L.GetInt(4);
			else if (e && e->AsModel())
				joint = e->AsModel()->GetJointIndex(L.GetString(4));
		}
		e->RegisterChild(child,L.GetBool(3,true),joint);
        child->KillByParent = L.GetBool(5,true);
		e->UpdateTransform(true);
	}
	return 0;
}
// =========================================================================
static int luaEntity_SetKillByParent(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Entity * e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity * e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if( e)
        e->KillByParent = L.GetBool(2,true);
	return 0;
}
// =========================================================================
static int luaEntity_ComputeChildMatrix(lua_State* state)
{
    Script L(state);

#ifndef TRANSGAMING
	Entity * child =  (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
    Entity * parent = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(2));
#else
    Entity * child =  (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
    Entity * parent = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(2));
#endif

    if( parent && child)
    {
        Matrix m;

        int joint = L.GetInt(3,-1);
        if (joint>-1 && parent->AsModel())
            m = child->ObjectToWorld * parent->AsModel()->GetJointTransform(joint).Inverse();
        else
            m = child->ObjectToWorld * parent->ObjectToWorld.Inverse();

        child->SetObjectTransform(m);
    }
    return 0;
}
// =========================================================================
static int luaEntity_ComputeLocalPoint(lua_State* state)
{
    Script L(state);
    Vector nv(0,0,0);

#ifndef TRANSGAMING
	Entity * e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity * e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

    if( e)
    {
        Vector v(L.GetFloat(2),L.GetFloat(3),L.GetFloat(4));
        nv = e->ObjectToWorld.Inverse().Transform(v);
    }

    L.PushFloat( nv.X );
    L.PushFloat( nv.Y );
    L.PushFloat( nv.Z );
    return 3;
}
// =========================================================================
static int luaEntity_TransformLocalPointToWorld(lua_State* state)
{
    Script L(state);
    Vector nv(0,0,0);

#ifndef TRANSGAMING
	Entity * e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity * e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

    if( e)
    {
        Vector v(L.GetFloat(2),L.GetFloat(3),L.GetFloat(4));
        nv = e->ObjectToWorld.Transform(v);
    }

    L.PushFloat( nv.X );
    L.PushFloat( nv.Y );
    L.PushFloat( nv.Z );
    return 3;
}
// =========================================================================
static int luaEntity_UnregisterChild(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Entity * e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
    Entity * child =  (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(2));
#else
    Entity * e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
    Entity * child =  (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(2));
#endif

	if( e && child)
		e->UnregisterChild(child);
	return 0;
}
// =========================================================================
static int luaEntity_UnregisterAllChildren(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Entity * e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity * e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

    int type = L.GetInt(2,-1);

#ifndef TRANSGAMING_NO_HAVOK
    if (e)
        e->UnregisterAllChildren(type);
#endif

    return 0;
}
// =========================================================================
static int luaEntity_KillAllChildren(lua_State* state)
{
	Script L(state);
#ifndef TRANSGAMING_NO_HAVOK

#ifndef TRANSGAMING
	Entity * e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity * e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

    int type = L.GetInt(2,-1);
    if (e)
        e->KillAllChildren(type);
#endif

    return 0;
}

// =========================================================================
static int luaEntity_KillAllChildrenByName(lua_State* state)
{
    Script L(state);
    bool ret = false;
#ifndef TRANSGAMING_NO_HAVOK

#ifndef TRANSGAMING
	Entity * e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity * e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

    if (e)
        ret = e->KillAllChildrenByName(L.GetString(2));
#endif
    L.PushBool(ret);
    return 1;
}
// =========================================================================
static int luaEntity_GetChildByName(lua_State* state)
{
    Script L(state);
    int idx = 0;
#ifndef TRANSGAMING_NO_HAVOK

#ifndef TRANSGAMING
	Entity * e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity * e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

    if (e)
    {
        Entity *child = e->GetChildByName(L.GetString(2));
        if (child)
            idx = child->GIndex;
    }
#endif
    L.PushInt(idx);
    return 1;
}
// =========================================================================
static int luaEntity_EnableCollisions(lua_State* state)
{
	Script L(state);
#ifndef TRANSGAMING_NO_HAVOK

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if( e && e->PO )
		//bool enable, float minimalDelay, float minimalStrength, float minimalOtherBodyMass
		e->PO->SetCollisionCallbacks( L.GetBool(2,true) ,L.GetFloat(3,0.4f), L.GetFloat(4,0.6f) );
#endif
	return 0;
}
// =========================================================================
static int luaEntity_EnableCollisionsToAll(lua_State* state)			// dla Thora
{
	Script L(state);
	//bool enable, float minimalDelay, float minimalStrength, float minimalMass, float maximalMass, float minProbability, int activeMeshGroup
#ifdef TRANSGAMING_NO_HAVOK
	int count = 0;
#else

# ifndef TRANSGAMING
	int count = GEngine->World->Physics->SetCollisionToAll( L.GetBool(1,true) ,L.GetFloat(2,0.5f),
		L.GetFloat(3,1.0f), L.GetFloat(4,0.1f), L.GetFloat(5,10.0f), L.GetFloat(6,10.0f), L.GetInt(7,-1.0f) );
# else
    /*TRACE("luaEntity_EnableCollisionsToAll(): state = %p, GEngine = %p, GEngine->pcfWorld = %p, GEngine->Physics = %p, GEngine->pcfWorld->Physics = %p\n",
                state, GEngine, GEngine->pcfWorld, GEngine->Physics, GEngine->pcfWorld->Physics);//*/

	int count = GEngine->pcfWorld->Physics->SetCollisionToAll( L.GetBool(1,true) ,L.GetFloat(2,0.5f),
		L.GetFloat(3,1.0f), L.GetFloat(4,0.1f), L.GetFloat(5,10.0f), L.GetFloat(6,10.0f), L.GetInt(7,-1) );
# endif

#endif
	L.PushInt(count);
	return 1;
}

// =========================================================================
static int luaEntity_EnableCollisionsToMesh(lua_State* state)			// dla Thora
{
	Script L(state);
	//bool enable, float minimalDelay, float minimalStrength, float minimalMass, float maximalMass, float minProbability
#ifdef TRANSGAMING_NO_HAVOK
	bool ok = 0;
#else

#ifndef TRANSGAMING
	bool ok = GEngine->World->Physics->EnableCollisionToMesh( L.GetBool(1,true), L.GetString(2), L.GetFloat(3,0.5f), L.GetFloat(4, 5.0f));
#else
    //TRACE("luaEntity_EnableCollisionsToMesh(): enabling collision to mesh using physics engine %p\n", GEngine->pcfWorld->Physics);
	bool ok = GEngine->pcfWorld->Physics->EnableCollisionToMesh( L.GetBool(1,true), L.GetString(2), L.GetFloat(3,0.5f), L.GetFloat(4, 5.0f));
#endif

#endif
	L.PushBool(ok);
	return 1;
}

// =========================
static int luaEntity_EnableCollisionsToRagdoll(lua_State* state) {
	Script L(state);

#ifndef TRANSGAMING
	Model *m = (Model *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Model *m = (Model *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if( !m || !m->RagdollPtr ) return 0;
#ifndef TRANSGAMING_NO_HAVOK
	//bool enable, float minimalDelay, float minimalStrength, float minimalOtherBodyMass
	m->RagdollPtr->Joint_SetCollisionCallbacks( L.GetInt(2,-1), L.GetFloat(3,0.4f), L.GetFloat(4,1.0f) );
#endif
	return 0;
}

// =========================================================================
static int luaEntity_EnableLineTraceCollisions(lua_State* state)
{
	Script L(state);
#ifndef TRANSGAMING_NO_HAVOK

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if( e && e->PO )
		e->PO->EnableLineTraceCollision( L.GetBool(2) );
#endif
	return 0;
}

// =========================================================================
static int luaEntity_SetPlayerSpeed(lua_State* state) {
	Script L(state);
	GEngine->Physics->TWPlayerSpeed = L.GetFloat(1);
	GEngine->Physics->TWPlayerJumpStrength = L.GetFloat( 2, GEngine->Physics->TWPlayerJumpStrength );
	GEngine->Physics->canForgetOPS = false;
	return 0;
}
// =========================================================================
static int luaEntity_GetPlayerSpeed(lua_State* state) {
	Script L(state);
	L.PushFloat(GEngine->Physics->TWPlayerSpeed);
	L.PushFloat(GEngine->Physics->TWPlayerJumpStrength);
	return 2;
}

// =========================================================================
static int luaEntity_PO_Create(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	DWORD type = (DWORD)L.GetInt(2, 0);
	float scale = L.GetFloat(3, -1.0f);
	int collisionGroup = L.GetInt( 4, -1 );
	bool dontAddToWorld = L.GetBool( 5, false );
	if (e)
	{
		e->CreatePhysicsObject( type, scale, collisionGroup, dontAddToWorld );
		if (e->PO) {
			e->PO->SetMonsterVisionProperties( 22, F_PI/2, 6 );

#ifndef TRANSGAMING_NO_HAVOK
			e->PO->Activate(true);
#endif
		}
		else
			GLog.Print( "PO_Create: ERROR!\n" );
	}

	return 0;
}

// =========================================================================
static int luaEntity_PO_GetMaxSphereRay(lua_State* state)
{
	Script L(state);
#ifndef TRANSGAMING_NO_HAVOK

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if( e && e->PO )
		L.PushFloat( e->PO->GetMaxSphereRay() );
	else
		L.PushFloat( 0.0f );
#else
	L.PushFloat(0.0f);
#endif
	return 1;
}
// =========================================================================
static int luaEntity_PO_GetPhysicsBody(lua_State* state)
{
    Script L(state);
#ifndef TRANSGAMING_NO_HAVOK

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

    if( e && e->PO )
        L.PushLightUD( e->PO->GetHavokBody());
    else
        L.PushNil();
#else
        L.PushNil();
#endif
    return 1;
}
// =========================================================================
static int luaEntity_PO_SetMonsterMovementConst(lua_State* state) {
#ifndef TRANSGAMING_NO_HAVOK
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if (e && e->PO ) {
		e->PO->havokInfluenceInMonsterMovement = L.GetFloat(2, 0.5f);
		e->PO->doNotCheckFloors = L.GetBool(3, false);
	}
#endif
	return 0;
}
// =========================================================================
static int luaEntity_PO_AddAction(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
    Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

    if (e && e->PO )
    {
        DWORD action = (DWORD)L.GetInt(2);
        e->PO->Action |= action;
    }
#endif
    return 0;
}
// =========================================================================
static int luaEntity_PO_RemoveAction(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
    Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

    if (e && e->PO )
    {
        DWORD action = (DWORD)L.GetInt(2);
        e->PO->Action &= ~action;
    }
#endif
    return 0;
}
// =========================================================================
static int luaEntity_PO_GetAction(lua_State* state)
{
    Script L(state);
    DWORD a = 0;
#ifndef TRANSGAMING_NO_HAVOK

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

    if (e && e->PO )
        a = e->PO->Action;
#endif
    L.PushInt(a);
    return 1;
}
// =========================================================================
static int luaEntity_PO_SetAction(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
    Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

    if (e && e->PO )
        e->PO->Action = (DWORD)L.GetInt(2);
#endif
    return 0;
}
// =========================================================================
static int luaEntity_PO_IsActionState(lua_State* state)
{
    Script L(state);
#ifndef TRANSGAMING_NO_HAVOK
#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

    if (e && e->PO ){
        unsigned int mask = L.GetInt(2);
        unsigned int action = e->PO->Action;

        //TRACE("ENTITY.PO_IsActionState(): returning 0x%X (action = 0x%X, mask = 0x%X)\n", action & mask, action, mask);
        L.PushBool( (action & mask) != 0);
    }
    else
        L.PushBool(false);
#else
    L.PushBool(false);
#endif
    return 1;
}
// =========================================================================
static int luaEntity_PO_Move(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if (e && e->PO )
	{
		Vector v(L.GetFloat(2),L.GetFloat(3),L.GetFloat(4));
		e->PO->SetMoveVector(v);
	}
#endif
	return 0;
}
// =========================================================================
static int luaEntity_PO_Activate(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if( e && e->PO )
		e->PO->Activate(L.GetBool(2));
#endif
	return 0;
}
// =========================================================================
static int luaEntity_PO_IsActive(lua_State* state)
{
	Script L(state);
#ifndef TRANSGAMING_NO_HAVOK

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if( e && e->PO )
	{
		L.PushBool(e->PO->IsActive());
		return 1;
	}
#endif
	L.PushBool(false);
	return 1;
}
// =========================================================================
static int luaEntity_PO_Enable(lua_State* state)
{
	Script L(state);
#ifndef TRANSGAMING_NO_HAVOK

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if( e && e->PO )
	{
		bool enable = L.GetBool(2,true);
		if (e->PO->IsEnabled() == enable) return 0;
		e->PO->Enable(enable);
	}
#endif
	return 0;
}
// =========================================================================
static int luaEntity_PO_IsEnabled(lua_State* state)
{
	Script L(state);
#ifndef TRANSGAMING_NO_HAVOK

# ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
# else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
# endif

	if( e && e->PO )
	{
        //TRACE("luaEntity_PO_IsEnabled(): the physics object for entity '%d' is %senabled\n", L.GetInt(1), e->PO->IsEnabled() ? "" : "not ");
		L.PushBool(e->PO->IsEnabled());
		return 1;
	}
#endif
    //TRACE("luaEntity_PO_IsEnabled(): the physics object for entity '%d' is missing\n", L.GetInt(1));
	L.PushBool(false);
	return 1;
}
// =========================================================================
static int luaEntity_PO_Rotate(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if( e && e->PO )
	{
		Vector v(L.GetFloat(2),L.GetFloat(3),L.GetFloat(4));
		e->PO->Rotate(v);
	}
#endif
	return 0;
};

// =========================================================================
static int luaEntity_PO_SetSightParams(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if( e && e->PO )
		e->PO->SetMonsterVisionProperties( L.GetFloat(2, 20.0f), L.GetFloat(3, 2.0f), L.GetFloat(4, 180.0f) * F_PI/360, L.GetFloat(5, 180.0f) * F_PI/180 );
#endif
	return 0;
}

// =========================================================================
static int luaEntity_PO_Exist(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if( e )
		L.PushBool( (e->PO != NULL) );
	return 1;
}
// =========================================================================
static int luaEntity_PO_SetMonsterType(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if (e && e->PO)
		e->PO->State |= EPOIsMonster;
#endif
	return 0;
}
// =========================================================================
static int luaEntity_Release(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if (e)
#ifndef TRANSGAMING
		GEngine->World->DeleteEntityDelayed(e);
#else
		GEngine->pcfWorld->DeleteEntityDelayed(e);
#endif
	return 0;
}
// =========================================================================
static int luaEntity_PO_SetRestitution(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if( e && e->PO )
		e->PO->SetRestitution(L.GetFloat(2));
#endif
	return 0;
}
// =========================================================================
static int luaEntity_PO_SetMass(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if (e)
	{
		if ( !strcmp( e->Name.AsChar(), "pCubeShape11") )
			TRACE("##### Entity_PO_SetMass - pCubeShape11 - Before");
		if( e->PO )
			e->PO->SetMass(L.GetFloat(2));
		if( e->AsModel() && ((Model*)e)->RagdollPtr )
			((Model*)e)->RagdollPtr->SetMass( L.GetFloat(2) );
		if ( !strcmp( e->Name.AsChar(), "pCubeShape11") )
			TRACE("##### Entity_PO_SetMass - pCubeShape11 - After");
	}
#endif
	return 0;
}
// =========================================================================
static int luaEntity_PO_ScaleInertiaTensor(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
	Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if( e )
	{
		if( e->PO )
			e->PO->ScaleInertiaTensor( L.GetFloat(2) );
		if( e->AsModel() && ((Model*)e)->RagdollPtr )
			((Model*)e)->RagdollPtr->ScaleInertiaTensor( L.GetFloat(2) );
	}
#endif
	return 0;
}
// =========================================================================
static int luaEntity_PO_SetHardDeactivator(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if( e && e->PO )
		e->PO->SetHardDeactivator();
#endif
	return 0;
}
// =========================================================================
static int luaEntity_PO_GetMass(lua_State* state)
{
	Script L(state);
#ifndef TRANSGAMING_NO_HAVOK

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if(e && e->PO )
		L.PushFloat( e->PO->GetMass() );
	else
		L.PushFloat(0.0f);
#else
	L.PushFloat(0.0f);
#endif
	return 1;
}
// =========================================================================
static int luaEntity_PO_LineTrace(lua_State* state)
{
	HitData hitData;
	Script L(state);
#ifndef TRANSGAMING_NO_HAVOK

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if( e )
	{
		if( e->AsMesh() && e->AsMesh()->FixedMeshIndex != 65535 && ( !e->PO || !e->PO->IsEnabled() ) )
		{
			Vector start(L.GetFloat(2),L.GetFloat(3),L.GetFloat(4));
			Vector end(L.GetFloat(5),L.GetFloat(6),L.GetFloat(7));

#ifndef TRANSGAMING
			if( GEngine->World->Physics->LineTraceStaticMesh( e->AsMesh(), start, end, hitData ) )
#else
			if( GEngine->pcfWorld->Physics->LineTraceStaticMesh( e->AsMesh(), start, end, hitData ) )
#endif
			{
				L.PushBool(true);
				L.PushFloat(hitData.Dist);
				L.PushFloat(hitData.HitPoint.X);
				L.PushFloat(hitData.HitPoint.Y);
				L.PushFloat(hitData.HitPoint.Z);
				L.PushFloat(hitData.Normal.X);
				L.PushFloat(hitData.Normal.Y);
				L.PushFloat(hitData.Normal.Z);
				L.PushLightUD(hitData.HavokEntity);

				if (hitData.UserData)
                    L.PushInt(((Entity*)hitData.UserData)->GIndex);
				else
					L.PushNil();
				return 10;
			}
		}
		else if( e->PO )
		{
			Vector start(L.GetFloat(2),L.GetFloat(3),L.GetFloat(4));
			Vector end(L.GetFloat(5),L.GetFloat(6),L.GetFloat(7));

			if( e->PO->LineTrace( start, end, hitData ) )
			{
				L.PushBool(true);
				L.PushFloat(hitData.Dist);
				L.PushFloat(hitData.HitPoint.X);
				L.PushFloat(hitData.HitPoint.Y);
				L.PushFloat(hitData.HitPoint.Z);
				L.PushFloat(hitData.Normal.X);
				L.PushFloat(hitData.Normal.Y);
				L.PushFloat(hitData.Normal.Z);
				L.PushLightUD(hitData.HavokEntity);

				if (hitData.UserData)
                    L.PushInt(((Entity*)hitData.UserData)->GIndex);
				else
					L.PushNil();
				return 10;
			}
		}
	}
#endif
	L.PushBool(false);
	return 1;
}
// =========================================================================
static int luaEntity_SetPosition(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if (e)
		e->SetPosition(Vector(L.GetFloat(2),L.GetFloat(3),L.GetFloat(4)));
#ifndef TRANSGAMING_NO_HAVOK
	if( e && e->PO )
		e->PO->SetPosition( Vector(L.GetFloat(2),L.GetFloat(3),L.GetFloat(4)) );
#endif
	return 0;
}
// =========================================================================
static int luaEntity_SetLocalBBox(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if (e)
	{
		e->LocalBox.Min = Vector(L.GetFloat(2),L.GetFloat(3),L.GetFloat(4));
		e->LocalBox.Max = Vector(L.GetFloat(5),L.GetFloat(6),L.GetFloat(7));
	}
	return 0;
}
// =========================================================================
static int luaEntity_GetDimensions(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if (e)
	{
        L.PushFloat((fabs(e->LocalBox.Min.X) + fabs(e->LocalBox.Max.X)) * e->GetScale()); // width
        L.PushFloat((fabs(e->LocalBox.Min.Y) + fabs(e->LocalBox.Max.Y)) * e->GetScale()); // height
        L.PushFloat((fabs(e->LocalBox.Min.Z) + fabs(e->LocalBox.Max.Z)) * e->GetScale()); // depth
	}
	else
	{
		L.PushFloat(0);
		L.PushFloat(0);
		L.PushFloat(0);
	}

	return 3;
}
// =========================================================================
static int luaEntity_GetVelocity(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

#ifndef TRANSGAMING_NO_HAVOK
	if( e && e->PO )
	{
		Vector vel = e->PO->GetVel();
		L.PushFloat( vel.X );
		L.PushFloat( vel.Y );
		L.PushFloat( vel.Z );
		L.PushFloat( sqrt(vel.X * vel.X + vel.Y * vel.Y + vel.Z * vel.Z));
		return 4;
	}
#endif
	L.PushFloat( 0.0f );
	L.PushFloat( 0.0f );
	L.PushFloat( 0.0f );
	L.PushFloat( 0.0f );
	return 4;
}


// =========================================================================
static int luaEntity_SetVelocity(lua_State* state)
{
	Script L(state);
#ifndef TRANSGAMING_NO_HAVOK

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if( e && e->PO ) {
		Vector v = Vector(L.GetFloat(2),L.GetFloat(3),L.GetFloat(4));
		float size = v.Magnitude();
		if (size < 10000.0)
			e->PO->SetVel( v );
	}
#endif
	return 0;
}
// =========================================================================
static int luaEntity_SetAngularVelocity(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if( e && e->PO )
		e->PO->SetAngularVel( Vector(L.GetFloat(2),L.GetFloat(3),L.GetFloat(4)) );
#endif
	return 0;
}
// =========================================================================
static int luaEntity_SetOrientation(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if (e)
		e->SetOrientation(L.GetFloat(2));
#ifndef TRANSGAMING_NO_HAVOK
	if( e && e->PO )
		e->PO->SetOrientation( L.GetFloat(2) );
#endif
	return 0;
}
// =========================================================================
static int luaEntity_GetOrientation(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if (e)
	{
#ifndef TRANSGAMING_NO_HAVOK
		if( e && e->PO && e->PO->IsEnabled())
			L.PushFloat(e->PO->GetOrientation());
		else
			L.PushFloat(e->GetOrientation());
#else
		L.PushFloat(e->GetOrientation());
#endif
	}
	else
		L.PushFloat(0);
	return 1;
}
// =========================================================================
static int luaEntity_SetRotation(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if (e)
	{
		Vector rot(L.GetFloat(2),L.GetFloat(3),L.GetFloat(4));

		Quaternion q = EulerToQuat(rot);
		e->SetRotation(q);
#ifndef TRANSGAMING_NO_HAVOK
		if (e->PO)
			e->PO->SetRotQuaternion(q);
#endif
	}
	return 0;
}
// =========================================================================
static int luaEntity_GetRotation(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if (e)
	{
		Quaternion rot;
#ifndef TRANSGAMING_NO_HAVOK
		if( e && e->PO)
			rot = e->PO->GetRotQuaternion();
		else
			rot = e->GetRotation();
#else
		rot = e->GetRotation();
#endif
		Vector ea = QuatToEuler(rot);

		L.PushFloat(ea.X);
		L.PushFloat(ea.Y);
		L.PushFloat(ea.Z);
	}
	else
	{
		L.PushFloat(0);
		L.PushFloat(0);
		L.PushFloat(0);
	}
	return 3;
}
// =========================================================================
static int luaEntity_SetRotationQ(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if (e)
	{
		Quaternion q(L.GetFloat(2),L.GetFloat(3),L.GetFloat(4),L.GetFloat(5));
		q.Normalize();
		e->SetRotation(q);
#ifndef TRANSGAMING_NO_HAVOK
		if (e->PO)
			e->PO->SetRotQuaternion(q);
#endif
	}
	return 0;
}
// =========================================================================
static int luaEntity_GetRotationQ(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if (e)
	{
		Quaternion rot;
#ifndef TRANSGAMING_NO_HAVOK
		if( e && e->PO)
			rot = e->PO->GetRotQuaternion();
		else
			rot = e->GetRotation();
#else
		rot = e->GetRotation();
#endif
		rot.Normalize();

		L.PushFloat(rot.W);
		L.PushFloat(rot.X);
		L.PushFloat(rot.Y);
		L.PushFloat(rot.Z);
	}
	else
	{
		L.PushFloat(0);
		L.PushFloat(0);
		L.PushFloat(0);
		L.PushFloat(0);
	}
	return 4;
}
// =========================================================================
static int luaEntity_SetScale(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if (e)
		e->SetScale(L.GetFloat(2));
	return 0;
}
// =========================================================================
static int luaEntity_SetAmbient(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if (e)
	{
#ifdef	TRANSGAMING
		e->SetEnvironmentAmbient(L.GetBool(2), Color_TG(L.GetFloat(3)/255.0f,L.GetFloat(4)/255.0f,L.GetFloat(5)/255.0f));
#else
		e->SetEnvironmentAmbient(L.GetBool(2), Color(L.GetFloat(3)/255.0f,L.GetFloat(4)/255.0f,L.GetFloat(5)/255.0f));
#endif
	}
	return 0;
}
// =========================================================================
static int luaEntity_GetPosition(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if (e)
	{
		Vector v;
#ifndef TRANSGAMING_NO_HAVOK
		if (e->PO)
			v = e->PO->GetPosition();
		else
			v = e->GetPosition();
#else
			v = e->GetPosition();
#endif

		L.PushFloat(v.X);
		L.PushFloat(v.Y);
		L.PushFloat(v.Z);
		return 3;
	}

	L.PushFloat(0);
	L.PushFloat(0);
	L.PushFloat(0);
	return 3;
}
// =========================================================================
static int luaEntity_GetWorldPosition(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if (e)
	{
		Vector v;
		if (e->Flags & Entity::EEntCenteredGeometry)
			v = e->ObjectToWorld.GetTranslation();
		else {
			v = e->Box.GetCenter();
		}
		L.PushFloat(v.X);
		L.PushFloat(v.Y);
		L.PushFloat(v.Z);
		return 3;
	}

	L.PushFloat(0);
	L.PushFloat(0);
	L.PushFloat(0);
	return 3;
}
// =========================================================================
static int luaEntity_GetCenter(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if (e)
	{
		Vector v = e->GetCenter();

		L.PushFloat(v.X);
		L.PushFloat(v.Y);
		L.PushFloat(v.Z);
		return 3;
	}

	L.PushFloat(0);
	L.PushFloat(0);
	L.PushFloat(0);
	return 3;
}
// =========================================================================
static int luaEntity_GetType(lua_State* state)
{
    Script L(state);

    int type = 0;

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

    if (e)
        type = e->Type;

    L.PushInt(type);
    return 1;
}
// =========================================================================
static int luaEntity_GetName(lua_State* state)
{
    Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if (e && e->Name.Size())
        L.PushString(e->Name.AsChar());
    else
        L.PushString("");

    return 1;
}
// =========================================================================
static int luaEntity_GetFileName(lua_State* state)
{
    Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

    if (e && e->FileName.Size())
        L.PushString(e->FileName.AsChar());
    else
        L.PushString("");

    return 1;
}
// =========================================================================
static int luaEntity_PO_GetPawnFloorPos(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if (e) {
		Vector v;
#ifndef TRANSGAMING_NO_HAVOK
		if (e->PO)
			v = e->PO->GetPawnFloorPos();
		else
			v = e->GetPosition();
#else
			v = e->GetPosition();
#endif
		L.PushFloat(v.X);
		L.PushFloat(v.Y);
		L.PushFloat(v.Z);
		return 3;
	}

	L.PushFloat(0);
	L.PushFloat(0);
	L.PushFloat(0);
	return 3;
}
// =========================================================================
static int luaEntity_PO_GetPawnHeadPos(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

#ifndef TRANSGAMING_NO_HAVOK
	if (e && e->PO)
	{
		Vector v1 = e->PO->GetPawnHeadPos();
		L.PushFloat(v1.X);
		L.PushFloat(v1.Y);
		L.PushFloat(v1.Z);
		return 3;
	}
#endif

	L.PushFloat(0);
	L.PushFloat(0);
	L.PushFloat(0);
	return 3;
}

// =========================================================================
static int luaEntity_PO_GetType(lua_State* state)
{
    Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

    int type = -1;
#ifndef TRANSGAMING_NO_HAVOK
    if (e && e->PO)
        type = e->PO->GetType();
#endif
    L.PushInt(type);
    return 1;
}
// =========================================================================
static int luaEntity_PO_SetPawnHeadPos(lua_State* state)
{
    Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

#ifndef TRANSGAMING_NO_HAVOK
    if (e && e->PO)
    {
        Vector v(L.GetFloat(2),L.GetFloat(3),L.GetFloat(4));
        e->PO->SetPawnHeadPos(v);
    }
#endif

    return 0;
}
// =========================================================================
static int luaEntity_SeesPoint(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if (e)
	{
		Vector v(L.GetFloat(2),L.GetFloat(3),L.GetFloat(4));
		L.PushBool(e->SeesPoint(v));
	}
	return 1;

}
// =========================================================================
static int luaEntity_SeesEntity(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING_NO_HAVOK

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
    Entity *de = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(2));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
    Entity *de = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(2));
#endif

	if( e && de )
	{
		Ragdoll* r = ((Model*)e)->RagdollPtr;
		if( r ) r->EnableLineTraceCollision( false );
		if( !e->PO )
		{
			if( de->PO )
			{
				HitData hitData;
				Vector start(e->GetPosition());
				Vector end(de->GetPosition());
				if (de->PO->GetType() == EPawnBodyPlayer) {		// zeby trace-owal wyzej dla playera, a nie pod nogi
					end.Y += 1.2f;
				}
#ifndef TRANSGAMING
				bool col = GEngine->World->Physics->LineTrace(start,end,hitData);
#else
                //TRACE("luaEntity_SeesEntity(): checking visibility using physics engine %p\n", GEngine->pcfWorld->Physics);
				bool col = GEngine->pcfWorld->Physics->LineTrace(start,end,hitData);
#endif
				L.PushBool(!col);
			}
			else
				L.PushBool(false);
		}
		else
			L.PushBool(e->SeesEntity(*de));
		if( r )
			r->EnableLineTraceCollision( true );
	}
#else
    L.PushBool(false);
#endif
	return 1;
}
// =========================================================================
/*static int luaEntity_SeesPlayer(lua_State* state)
{
	Script L(state);
#ifndef TRANSGAMING_NO_HAVOK

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
    Entity *pe = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(2));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
    Entity *pe = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(2));
#endif

	if( e && e->PO && pe && pe->PO )
	{
		Ragdoll* r = ((Model*)e)->RagdollPtr;
		if( r ) r->EnableLineTraceCollision( false );
#ifndef TRANSGAMING
		L.PushBool( GEngine->World->Physics->CalculatePawnToEntityVisibility(e->PO,pe->PO ));
#else
		L.PushBool( GEngine->pcfWorld->Physics->CalculatePawnToEntityVisibility(e->PO,pe->PO ));
#endif
		r->EnableLineTraceCollision( true );
	}
	else
		L.PushBool( false );
#else
	L.PushBool( false );
#endif
	return 1;
}*/

// =========================================================================
static int luaEntity_SetPosAndRotRelativeToCamera(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if (e)
	{
		View * view = GEngine->Video->GetActiveView();
		Camera &c = view->mCamera;

		Matrix m = ScaleMatrix(e->GetScale());

		Vector trans(L.GetFloat(2),L.GetFloat(3),L.GetFloat(4));
		Vector rot(L.GetFloat(5),L.GetFloat(6),L.GetFloat(7));


		m *= XRotationMatrix(rot.X) * ZRotationMatrix(rot.Z) * YRotationMatrix(rot.Y)  * TranslationMatrix(trans) * c.WorldToCamera();


		//e->SetPosition(trans);
		e->SetObjectTransform(m);
	}
	return 1;
}
// =========================================================================
static int luaEntity_PO_Hit(lua_State* state)
{
	Script L(state);
#ifndef TRANSGAMING_NO_HAVOK

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if( e && e->PO )
	{
		Vector point(L.GetFloat(2),L.GetFloat(3),L.GetFloat(4));
		Vector vect(L.GetFloat(5),L.GetFloat(6),L.GetFloat(7));
		float size = vect.Magnitude();
		if (size > 0.01 && size < 10000.0)
			e->PO->Hit( point, vect );
	}
#endif
	return 0;
}
// =========================================================================
static int luaEntity_PO_AccumulateRotation(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity*) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity*) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if( e && e->PO )
	{
		Vector point(L.GetFloat(2),L.GetFloat(3),L.GetFloat(4));
		Vector vect(L.GetFloat(5),L.GetFloat(6),L.GetFloat(7));
		float size = vect.Magnitude();
		if (size > 0.01 && size < 10000.0)
			e->PO->AddRotateActor( point, vect );
	}
#endif
	return 0;
}
// =========================================================================
static int luaEntity_PO_Impulse(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if( e && e->PO )
	{
		Vector point(L.GetFloat(2),L.GetFloat(3),L.GetFloat(4));
		Vector vect(L.GetFloat(5),L.GetFloat(6),L.GetFloat(7));
		float size = vect.Magnitude();
		if (size > 0.01 && size < 10000.0)
			e->PO->Impulse( point, vect );
	}
#endif
	return 0;
}
// =========================================================================
static int luaEntity_PO_SetFlying(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if( e && e->PO )
	{
		e->PO->SetFlying( L.GetBool(2) );
	}
#endif
	return 0;
}
// =========================================================================
static int luaEntity_PO_SetPlayerFlying(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if( e && e->PO )
		e->PO->SetPlayerShocked( L.GetFloat(2) );
#endif
	return 0;
}
// =========================================================================
static int luaEntity_PO_SetEntitySteered(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if( e && e->PO )
		e->PO->SetEntitySteered( L.GetBool(2, true) );
#endif
	return 0;
}


// =========================================================================
static int luaEntity_PO_SetMovedByExplosions(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if( e && e->PO )
		e->PO->IsMovedByExplosions( L.GetBool(2) );
#endif
	return 0;
}
// =========================================================================
static int luaEntity_PO_SetPinned(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if( e && e->PO )
		e->PO->SetPinned( L.GetBool(2) );
#endif
	return 0;
}
// =========================================================================
static int luaEntity_PO_IsPinned(lua_State* state)
{
	Script L(state);
#ifndef TRANSGAMING_NO_HAVOK

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if( e && e->PO )
		L.PushBool( e->PO->IsPinned() );
	else
		L.PushBool( false );
#else
		L.PushBool( false );
#endif
	return 1;
}
// =========================================================================
static int luaEntity_PO_IsFixed(lua_State* state)
{
    Script L(state);
#ifndef TRANSGAMING_NO_HAVOK

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

    if( e && e->PO )
        L.PushBool( e->PO->IsFixed() );
    else
        L.PushBool( true );
#else
        L.PushBool( true );
#endif
    return 1;
}
// =========================================================================
static int luaEntity_PO_IsFlying(lua_State* state)
{
	Script L(state);
#ifndef TRANSGAMING_NO_HAVOK

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if( e && e->PO )
		L.PushBool( !e->PO->MonsterFloorCheck() );
	else
		L.PushBool( false );
#else
		L.PushBool( false );
#endif
	return 1;
}
// =========================================================================
static int luaEntity_PO_IsOnFloor(lua_State* state)
{
	Script L(state);
#ifndef TRANSGAMING_NO_HAVOK

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if( e && e->PO ) {
		L.PushBool(e->PO->onFloor);
		L.PushFloat(e->PO->floorNormal.X);
		L.PushFloat(e->PO->floorNormal.Y);
		L.PushFloat(e->PO->floorNormal.Z);
	}
	else {
		L.PushBool( false );
		L.PushFloat(0.0f);
		L.PushFloat(1.0f);
		L.PushFloat(0.0f);
	}
#else
		L.PushBool( false );
		L.PushFloat(0.0f);
		L.PushFloat(1.0f);
		L.PushFloat(0.0f);
#endif
	return 4;
}
// =========================================================================
static int luaEntity_AddLight(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Entity *entity = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
	Light *light = (Light *) GEngine->World->GetEntityByIndex(L.GetInt(2));
#else
	Entity *entity = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
	Light *light = (Light *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(2));
#endif

	if( entity && light )
		entity->AddLight( light );
	return 0;
}
// =========================================================================
static int luaEntity_ResetLights(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Entity *entity = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
	Entity *entity = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if( entity )
	{
		entity->LightCount=0;
		for (int i=0; i<Entity::MaxLights; ++i)
			entity->LightIntensity[i] = -1;
	}
	return 0;
}
// =========================================================================
static int luaEntity_Draw(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if( !e ) return 0;

	View * view = GEngine->Video->GetActiveView();
	Viewport &pViewport = view->LocalViewport;
	RenderDevice *dev = GEngine->Video->RenderDev;
    dev->SetScreenTransform(&pViewport.CameraToScreen);
    dev->SetCameraTransform(&pViewport.WorldToCamera);
    dev->SetObjectTransform(GIdentityMatrix);

	e->Draw(
		GEngine->Video->RenderDev,View::EModeShowTextures|View::EModeShowMeshes|View::EModeShowModels|View::EModeZPass);
	e->Draw(GEngine->Video->RenderDev,View::EModeShowTextures|View::EModeShowMeshes|View::EModeShowModels|View::EModeMultiPass);
	return 0;
}
// =========================================================================
static int luaEntity_Tick(lua_State* state)
{
    Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

    if (e)
        e->Tick(L.GetFloat(2,0.001f));
    return 0;
}
// =========================================================================
static int luaEntity_RemoveRagdollFromIntersectionSolver(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if( e && e->AsModel() )
	{
		Ragdoll* r = ((Model*)e)->RagdollPtr;
		if( r ) r->EnableLineTraceCollision( false );
	}
#endif
	return 0;
}
// =========================================================================
static int luaEntity_RemoveRagdoll(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

	if( e && e->AsModel() )
	{
		Ragdoll* r = ((Model*)e)->RagdollPtr;
		if( r ) delete r;
	}
#endif

	return 0;
}
// =========================================================================
static int luaEntity_RecreateRagdollIfNone(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

	if( e && e->AsModel() )
		e->AsModel()->CreateRagdollIfNone();

	return 0;
}
// =========================================================================
static int luaEntity_AddRagdollToIntersectionSolver(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

	if( e && e->AsModel() )
	{
		Ragdoll* r = ((Model*)e)->RagdollPtr;
		if( r ) r->EnableLineTraceCollision( true );
	}
#endif
	return 0;
}
// =========================================================================
static int luaEntity_RemoveFromIntersectionSolver(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if( e )
	{
		if( e->PO )
			e->PO->EnableLineTraceCollision( false );

		if( e->AsModel() )
		{
			Ragdoll* r = ((Model*)e)->RagdollPtr;
			if( r ) r->EnableLineTraceCollision( false );
		}
	}
#endif

	return 0;
}
// =========================================================================
static int luaEntity_AddToIntersectionSolver(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if( e )
	{
		if( e->PO )
			e->PO->EnableLineTraceCollision( true );

		if( e->AsModel() )
		{
			Ragdoll* r = ((Model*)e)->RagdollPtr;
			if( r ) r->EnableLineTraceCollision( true );
		}
	}
#endif

	return 0;
}
// =========================================================================
static int luaEntity_CanLineTraceCollision(lua_State* state)
{
    Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

    bool is = false;
#ifndef TRANSGAMING_NO_HAVOK
    if( e  && e->PO)
        is = e->PO->CanLineTraceCollision();
#endif

    L.PushBool(is);
    return 1;
}
// =========================================================================
static int luaEntity_PO_EnableGravity(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if( e && e->PO )
		e->PO->EnableGravity( L.GetBool(2,true) );
#endif
	return 0;
}
// =========================================================================
static int luaEntity_PO_SetGravity(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if( e && e->PO )
		e->PO->SetGravity( Vector(L.GetFloat(2,0.0f),L.GetFloat(3,0.0f),L.GetFloat(4,0.0f)) );
#endif
	return 0;
}

// =========================================================================
static int luaEntity_PO_EnableSpeedDamping(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if(e && e->PO)
		e->PO->EnableSpeedDamping( L.GetBool(2,true), L.GetFloat(3,70.0f), L.GetFloat(4,50.0f), L.GetFloat(5,0.01f) );
#endif
	return 0;
}
// =========================================================================
static int luaEntity_PO_MaintainLinearMovement(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if( e && e->PO )
		e->PO->MaintainLinearMovement( L.GetBool(2,true), Vector( L.GetFloat(3,0.0f), L.GetFloat(4,1.0f), L.GetFloat(5,0.0f) ), L.GetBool( 6, false ) );
#endif
	return 0;
}
// =========================================================================
static int luaEntity_PO_MaintainPosition(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if( e && e->PO )
		e->PO->MaintainPosition( L.GetBool(2,true), Vector( L.GetFloat(3,0.0f), L.GetFloat(4,0.0f), L.GetFloat(5,0.0f) ), L.GetFloat(6,1.0f) );
#endif
	return 0;
}
// =========================================================================
static int luaEntity_PO_MaintainVelocity(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if( e && e->PO )
		e->PO->MaintainVelocity( L.GetBool(2,true), Vector( L.GetFloat(3,0.0f), L.GetFloat(4,0.0f), L.GetFloat(5,0.0f) ), L.GetFloat(6,1.0f) );
#endif
	return 0;
}
// =========================================================================
static int luaEntity_PO_MaintainAngularVelocity(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if( e && e->PO )
		e->PO->MaintainAngularVelocity( L.GetBool(2,true), Vector( L.GetFloat(3,0.0f), L.GetFloat(4,0.0f), L.GetFloat(5,0.0f) ), L.GetFloat(6,1.0f) );
#endif
	return 0;
}
// =========================================================================
static int luaEntity_PO_SetAsTransporter(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if( e && e->PO )
		e->PO->SetAsTransporter( L.GetBool(2,true), Vector( L.GetFloat(3,0.0f), L.GetFloat(4,0.0f), L.GetFloat(5,0.0f) ), L.GetFloat(6,0.1f) );
#endif
	return 0;
}
// =========================================================================
static int luaEntity_PO_SetFriction(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if( e && e->PO )
		e->PO->SetFriction(L.GetFloat(2));
#endif
	return 0;
}
// =========================================================================
static int luaEntity_PO_SetLinearDamping(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if( e && e->PO )
		e->PO->SetLinearDamping( L.GetFloat(2,0.001f) );
#endif
	return 0;
}
// =========================================================================
static int luaEntity_PO_SetAngularDamping(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if( e && e->PO )
		e->PO->SetAngularDamping( L.GetFloat(2,0.001f) );
#endif
	return 0;
}
// =========================================================================
static int luaEntity_PO_GetLinearDamping(lua_State* state)
{
	Script L(state);
#ifndef TRANSGAMING_NO_HAVOK

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if( e && e->PO )
		L.PushFloat( e->PO->GetLinearDamping() );
	else
		L.PushFloat(0.0f);
#else
		L.PushFloat(0.0f);
#endif
	return 1;
}
// =========================================================================
static int luaEntity_PO_GetAngularDamping(lua_State* state)
{
	Script L(state);
#ifndef TRANSGAMING_NO_HAVOK

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if( e && e->PO )
		L.PushFloat( e->PO->GetAngularDamping() );
	else
		L.PushFloat(0.0f);
#else
		L.PushFloat(0.0f);
#endif
	return 1;
}
// =========================================================================
static int luaEntity_PO_SetFreedomOfRotation(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if(e && e->PO )
		e->PO->SetFreedomOfRotation( L.GetInt(2), L.GetFloat(3,1.0f) );
#endif
	return 0;
}
// =========================================================================
static int luaEntity_PO_Remove(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if (e)
		e->RemovePhysicsObject();
	return 0;
}
// =========================================================================
static int luaEntity_PO_GetCollisionGroup(lua_State* state)
{
	Script L(state);
#ifndef TRANSGAMING_NO_HAVOK

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if(e && e->PO)
	{
		L.PushInt( e->PO->GetCollisionGroup() );
		return 1;
	}
#endif
	L.PushInt(0);
	return 1;
}
// =========================================================================
static int luaEntity_PO_SetCollisionGroup(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	int collGr = L.GetInt(2,0);
	if (collGr != 0) {
#ifndef TRANSGAMING_NO_HAVOK
		if(e && e->PO)
			e->PO->SetCollisionGroup( collGr );
#endif
	}
	else
		GLog.Print("ERROR: PO_SetCollisionGroup 0\n");

	return 0;
}
// =========================================================================
static int luaEntity_PO_MoveCenterOfMass(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
    Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

    if(e && e->PO)
    {
        Vector vect(L.GetFloat(2),L.GetFloat(3),L.GetFloat(4));
        e->PO->MoveCenterOfMass(vect);
    };
#endif
    return 0;
}
// =========================================================================
static int luaEntity_SpawnDecal(lua_State* state)
{
	Script L(state);
    if (!GEngine->Video) {L.PushNil();return 1;}

#ifdef TRANSGAMING_HACK
    L.PushInt(0);
    return 0;
#endif

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if (e)
	{
#ifndef TRANSGAMING
		Decal* decal = (Decal*)GEngine->World->CreateEntity(Entity::ETypeDecal,L.GetString(2),"",L.GetFloat(9,1.0f));
#else
		Decal* decal = (Decal*)GEngine->pcfWorld->CreateEntity(Entity::ETypeDecal,L.GetString(2),"",L.GetFloat(9,1.0f));
#endif
		Vector pos = Vector(L.GetFloat(3),L.GetFloat(4),L.GetFloat(5));
		Vector dir = Vector(L.GetFloat(6),L.GetFloat(7),L.GetFloat(8));
		decal->Spawn(e, pos, dir);

#ifndef TRANSGAMING
		GEngine->World->AddEntity(decal);
#else
		GEngine->pcfWorld->AddEntity(decal);
#endif
        L.PushInt(decal->GIndex);
	}
	else {
		L.PushInt(0);		//////
	}
	return 1;
}
// =========================================================================
static int luaEntity_SpawnOrientedDecal(lua_State* state)
{
	Script L(state);
    if (!GEngine->Video) {L.PushNil();return 1;}

#ifdef TRANSGAMING_HACK
    L.PushInt(0);
    return 0;
#endif

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if (e)
	{
#ifndef TRANSGAMING
		Decal* decal = (Decal*)GEngine->World->CreateEntity(Entity::ETypeDecal,L.GetString(2),"",L.GetFloat(12,1.0f));
#else
		Decal* decal = (Decal*)GEngine->pcfWorld->CreateEntity(Entity::ETypeDecal,L.GetString(2),"",L.GetFloat(12,1.0f));
#endif

		Vector pos = Vector(L.GetFloat(3),L.GetFloat(4),L.GetFloat(5));
		Vector dir = Vector(L.GetFloat(6),L.GetFloat(7),L.GetFloat(8));
		Vector tan = Vector(L.GetFloat(9),L.GetFloat(10),L.GetFloat(11));
		Vector bin = dir^tan; bin.Normalize();
		decal->Spawn(e, pos, dir, tan, bin);

#ifndef TRANSGAMING
		GEngine->World->AddEntity(decal);
#else
		GEngine->pcfWorld->AddEntity(decal);
#endif
        L.PushInt(decal->GIndex);
	}
	else {
		L.PushInt(0);		//////
	}
	return 1;
}
// =========================================================================
static int luaEntity_UpdateDecal(lua_State* state) {
	Script L(state);


#ifdef TRANSGAMING_HACK
    L.PushInt(0);
    return 0;
#endif
#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if (e) {

#ifndef TRANSGAMING
		Decal* decal = (Decal *)GEngine->World->GetEntityByIndex(L.GetInt(2),Entity::ETypeDecal);
#else
        Decal* decal = (Decal *)GEngine->pcfWorld->GetEntityByIndex(L.GetInt(2),Entity::ETypeDecal);
#endif
		Vector pos = Vector(L.GetFloat(3),L.GetFloat(4),L.GetFloat(5));
		Vector dir = Vector(L.GetFloat(6),L.GetFloat(7),L.GetFloat(8));
		decal->Spawn(e, pos, dir);
	}
	return 0;
}
// =========================================================================
static int luaEntity_SpawnStaticDecal(lua_State* state)
{
	Script L(state);

#ifdef TRANSGAMING_HACK
    L.PushInt(0);
    return 0;
#endif

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if (e)
	{
#ifndef TRANSGAMING
		Decal* decal = (Decal*)GEngine->World->CreateEntity(Entity::ETypeDecal,"static","",1.0f);
#else
		Decal* decal = (Decal*)GEngine->pcfWorld->CreateEntity(Entity::ETypeDecal,"static","",1.0f);
#endif
		const char *texname = L.GetString(2);
		Vector pos = Vector(L.GetFloat(3),L.GetFloat(4),L.GetFloat(5));
		Vector nor = Vector(L.GetFloat(6),L.GetFloat(7),L.GetFloat(8));
		Vector tan = Vector(L.GetFloat(9),L.GetFloat(10),L.GetFloat(11));
		Vector bin = Vector(L.GetFloat(12),L.GetFloat(13),L.GetFloat(14));
		decal->Spawn(e, pos, nor, tan, bin);
		decal->SetTexture(texname);
#ifndef TRANSGAMING
		GEngine->World->AddEntity(decal);
#else
		GEngine->pcfWorld->AddEntity(decal);
#endif
		L.PushInt(decal->GIndex);
	}
	else {
		L.PushInt(0);		//////
	}
	return 1;
}
// =========================================================================
static int luaEntity_ReloadDecalSystem(lua_State* state)
{
	Script L(state);
    if (GEngine->Video)
    {
	    GEngine->Video->DecalSys.Reload();
	    GEngine->Video->TrailSys.Reload();
    }
	return 0;
}
// =========================================================================
static int luaEntity_AttachTrailToBones(lua_State* state)
{
	Script L(state);
#ifndef TRANSGAMING
	Entity *parent = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
    Entity *trail  = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(2));
#else
    Entity *parent = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
    Entity *trail  = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(2));
#endif

	if (trail && parent && trail->AsTrail()) {
        //TRACE("luaEntity_AttachTrailToBones(): attaching the trail '%s' to the entity '%s' and these bones:\n", trail->Name.AsChar(), parent->Name.AsChar());
		DynamicArray<String> bones;
		for (int i=3; !L.IsNil(i); i++)
		{
            const char *bone = L.GetString(i);
            //TRACE("        %d) '%s'\n", i - 2, bone);
			bones.Add(bone);
		}
		trail->AsTrail()->AttachToBones(parent,bones);
        trail->KillByParent = false;
	}
	return 0;
}
// =========================================================================
static int luaEntity_ExplodeItem(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	const char *filename = L.GetString(2,"");
	float strength = L.GetFloat(3,0.0f);
	float radius = L.GetFloat(4,0.0f);
	float lifetimeAfterExplosion = L.GetFloat(5,30.0f);
    float ascale = L.GetFloat(6,1.0f);
	Vector offset(L.GetFloat(9,0.0f),L.GetFloat(10,0.0f),L.GetFloat(11,0.0f));
	const char* meshName = NULL;


    if (!(L.IsNil(7)))
		meshName = L.GetString(7, "");


	bool nonCollidable = L.GetBool(8,false);

	if( e && e->PO )
#ifndef TRANSGAMING
		L.PushInt(GEngine->World->ExplodeItem( e, filename, strength, radius, lifetimeAfterExplosion,ascale,nonCollidable,meshName,offset ));
#else
        L.PushInt(GEngine->pcfWorld->ExplodeItem( e, filename, strength, radius, lifetimeAfterExplosion,ascale,nonCollidable,meshName,offset ));
#endif
	else
		L.PushNil();
	return 1;
}

// =========================================================================
static int luaEntity_DamageItem(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeMesh);
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeMesh);
#endif

	Assert( e->Type == Entity::ETypeMesh );

	const char *filename = L.GetString(2, "");
	const char *meshname = L.GetString(3, "");
	if( e && e->PO  )
#ifndef TRANSGAMING
		L.PushInt( GEngine->World->DamageItem( (WorldMesh*)e, filename, meshname )->GIndex );
#else
		L.PushInt( GEngine->pcfWorld->DamageItem( (WorldMesh*)e, filename, meshname )->GIndex );
#endif

	else
		L.PushInt( 0 );
	return 1;
}
// =========================================================================
static int luaEntity_RenderBBox(lua_State* state)
{
#ifdef TRANSGAMING_USE_EDITOR
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if (e)
		e->RenderLocalBoundingBox();
#endif
	return 0;
}
/*
// =========================================================================
static int luaEntity_GetBBox(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if (e) {
		L.PushFloat(e->Box.Min.X);
		L.PushFloat(e->Box.Min.Y);
		L.PushFloat(e->Box.Min.Z);
		L.PushFloat(e->Box.Max.X);
		L.PushFloat(e->Box.Max.Y);
		L.PushFloat(e->Box.Max.Z);
		return 6;
	}
	return 0;
}
// =========================================================================
static int luaEntity_GetBRadius(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if (e) {
		Vector radius = 0.5f*((e->Box.Max.X - e->Box.Min.X),
						  (e->Box.Max.Y - e->Box.Min.Y),
						  (e->Box.Max.Z - e->Box.Min.Z));
		L.PushFloat(radius.Magnitude());
		return 1;
	}
	return 0;
}
*/
// =========================================================================
static int luaEntity_Pick(lua_State* state)
{
	Script L(state);
	int sx = L.GetInt(1);
	int sy = L.GetInt(2);
	int UseHavok = L.GetBool(3) ? 1 : 0;

	View * view = GEngine->Video->GetActiveView();
#ifndef TRANSGAMING
	Entity * e = GEngine->World->PickEntity(view->LocalViewport,sx,sy,UseHavok);
#else
    Entity * e = GEngine->pcfWorld->PickEntity(view->LocalViewport,sx,sy,UseHavok);
#endif
    L.PushInt(e?e->GIndex:0);

	return 1;
}
// =========================================================================
static int luaEntity_IsWater(lua_State* state)
{
	Script L(state);
#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif
	if (e && e->AsMesh())
		L.PushBool(e->AsMesh()->Flags & WorldMesh::EWater);
	else
		L.PushBool(false);
	return 1;
}
// =========================================================================
static int luaEntity_IsFixedMesh(lua_State* state)
{
    Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

    if (e && e->AsMesh())
        L.PushBool(e->AsMesh()->FixedMeshIndex != 65535);
    else
        L.PushBool(false);
    return 1;
}
// =========================================================================
static int luaEntity_EnableDeathZoneTest(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if (e)
		e->TestDeathZone = L.GetBool(2,true);;
	return 0;
}
// =========================================================================
static int luaEntity_EnableDraw(lua_State* state)
{
    Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

    if (e)
    {
        bool draw = L.GetBool(2,false);
        bool alsoChildren = L.GetBool(3,false);

		e->EnableDraw(draw);

		if (alsoChildren)
			for (int i=0;i<e->Children.Size();i++)
				e->Children[i]->EnableDraw(draw);

        e->UpdateTransform(true);
    }
    return 0;
}
// =========================================================================
static int luaEntity_EnableTick(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
	Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if (e)
	{
		bool draw = L.GetBool(2,false);
		bool alsoChildren = L.GetBool(3,false);
		if (draw)
		{
			e->Flags &= ~Entity::EEntNoTick;
			if (alsoChildren)
				for (int i=0;i<e->Children.Size();i++) {
					e->Children[i]->Flags &= ~Entity::EEntNoTick;
				}
		}
		else
		{
			e->Flags |= Entity::EEntNoTick;
			if (alsoChildren)
				for (int i=0;i<e->Children.Size();i++)
					e->Children[i]->Flags |= Entity::EEntNoTick;
		}
	}
	return 0;
}
// =========================================================================
static int luaEntity_EnableDemonic(lua_State* state)
{
    Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

    if (e)
    {
        bool enable = L.GetBool(2,false);
        bool alsoChildren = L.GetBool(3,false);

        e->Demonic = enable;

        if (alsoChildren)
            for (int i=0;i<e->Children.Size();i++)
                e->Children[i]->Demonic = enable;
    }
    return 0;
}
// =========================================================================
static int luaEntity_EnableGunPass(lua_State* state)
{
    Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

    if (e)
    {
        bool alsoChildren = L.GetBool(2,false);
        e->Sort = Entity::ESortGun;
        if (alsoChildren)
            for (int i=0;i<e->Children.Size();i++)
                e->Children[i]->Sort = Entity::ESortGun;
    }
    return 0;
}
// =========================================================================
static int luaEntity_IsDrawEnabled(lua_State* state) {
    Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if (e) {
		DWORD f = e->Flags;
		if (f &= Entity::EEntNoDraw) {
			L.PushBool(false);
			return 1;
		}
	}
	L.PushBool(true);
	return 1;
}

// =========================================================================
static int luaEntity_PortalVisibilityTest(lua_State* state)
{
	Script L( state );

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
	Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif
	if( e )
	{
		Vector eye = GEngine->Video->GetActiveView()->LocalViewport.EyePosition;
		L.PushBool( SceneRender::EntityVisible( e, eye ) );
    }
	else
		L.PushBool( false );
    return 1;
}
// =========================================================================
static int luaEntity_PO_SetPlayerShocked(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if( e && e->PO )
		e->PO->SetPlayerShocked();
#endif
	return 0;
}
// =========================================================================
static int luaEntity_PO_IsOnLadder(lua_State* state)
{
    Script L(state);
    bool ladder = false;
#ifndef TRANSGAMING_NO_HAVOK

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

    if( e && e->PO )
        ladder = e->PO->onLadder;
#endif
    L.PushBool(ladder);
    return 1;
}
// =========================================================================
static int luaEntity_PO_JumpedInLastAction( lua_State* state )
{
    Script L(state);
    bool jumped = false;
#ifndef TRANSGAMING_NO_HAVOK

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

    if( e && e->PO )
        jumped = e->PO->JumpedInLastAction();
#endif
    L.PushBool( jumped );
    return 1;
}
// =========================================================================
static int luaEntity_PO_SetGrenade( lua_State* state )
{
#ifndef TRANSGAMING_NO_HAVOK
    Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity*)GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
	Entity *e = (Entity*)GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if( e && e->PO )
		e->PO->SetGrenade( L.GetBool(2,true) );
#endif
    return 0;
}
// =========================================================================
static int luaEntity_PO_SetMissile( lua_State* state )
{
#ifndef TRANSGAMING_NO_HAVOK
    Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity*)GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
	Entity *e = (Entity*)GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if( GEngine && GEngine->Net && e && e->PO )
		e->PO->SetMPMissileController( L.GetInt(2,0) );
#endif
	return 0;
}
// =========================================================================
static int luaEntity_UpdatePart( lua_State* state )
{
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity*)GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
	Entity *e = (Entity*)GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if( e )
		for (int i=0;i<e->Children.Size();i++) {
			if (e->Children[i]->Type == Entity::ETypeParticleFx) {
				((ParticleEffect *)e->Children[i])->updateOnce = true;
			}
		}
	return 0;
}
// =========================================================================
static int luaEntity_PO_HideFromPrediction( lua_State* state )
{
#ifndef TRANSGAMING_NO_HAVOK
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity*)GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
	Entity *e = (Entity*)GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if( e && e->PO )
		e->PO->SetNotPredictionAffected();
#endif
    return 0;
}
// =========================================================================
static const luaL_reg entity_lib[] = {
	{"Create", luaEntity_Create},
	{"Release", luaEntity_Release},
    {"GetPtrByIndex", luaEntity_GetPtrByIndex},
    {"GetIndex", luaEntity_GetIndex},
    {"Exist", luaEntity_Exist},
    {"GetType", luaEntity_GetType},
    {"GetName", luaEntity_GetName},
    {"GetFileName", luaEntity_GetFileName},
	{"SetTimeToDie", luaEntity_SetTimeToDie},
    {"GetWorldPosition", luaEntity_GetWorldPosition},
	{"GetPosition", luaEntity_GetPosition},
	{"SetPosition", luaEntity_SetPosition},
    {"GetCenter", luaEntity_GetCenter},
	{"SetLocalBBox", luaEntity_SetLocalBBox},
    {"GetDimensions", luaEntity_GetDimensions},
	{"SetPosAndRotRelativeToCamera", luaEntity_SetPosAndRotRelativeToCamera},
	{"SetRotation", luaEntity_SetRotation},
	{"GetRotation", luaEntity_GetRotation},
	{"SetRotationQ", luaEntity_SetRotationQ},
	{"GetRotationQ", luaEntity_GetRotationQ},
	{"SetOrientation", luaEntity_SetOrientation},
	{"GetOrientation", luaEntity_GetOrientation},
	{"GetVelocity", luaEntity_GetVelocity},
    {"GetName", luaEntity_GetName},
	{"SetVelocity", luaEntity_SetVelocity},
	{"SetAngularVelocity", luaEntity_SetAngularVelocity},
	{"SetScale", luaEntity_SetScale},
	{"SetAmbient", luaEntity_SetAmbient},
	{"SeesPoint", luaEntity_SeesPoint},
	{"SeesEntity", luaEntity_SeesEntity},
	/*{"SeesPlayer", luaEntity_SeesPlayer},*/
	{"UnregisterChild", luaEntity_UnregisterChild},
	{"RegisterChild", luaEntity_RegisterChild},
	{"UnregisterAllChildren", luaEntity_UnregisterAllChildren},
    {"KillAllChildren", luaEntity_KillAllChildren},
    {"KillAllChildrenByName", luaEntity_KillAllChildrenByName},
    {"GetChildByName", luaEntity_GetChildByName},
    {"ComputeChildMatrix", luaEntity_ComputeChildMatrix},
	{"AddLight", luaEntity_AddLight},
	{"ResetLights", luaEntity_ResetLights},
	{"Draw", luaEntity_Draw},
    {"Tick", luaEntity_Tick},
	{"EnableCollisions", luaEntity_EnableCollisions},
	{"EnableCollisionsToAll", luaEntity_EnableCollisionsToAll},
	{"EnableCollisionsToMesh", luaEntity_EnableCollisionsToMesh},
	{"EnableCollisionsToRagdoll", luaEntity_EnableCollisionsToRagdoll},
	{"EnableLineTraceCollisions", luaEntity_EnableLineTraceCollisions},
    {"CanLineTraceCollision", luaEntity_CanLineTraceCollision},
    {"EnableDraw", luaEntity_EnableDraw},
	{"EnableTick", luaEntity_EnableTick},
	{"EnableDemonic", luaEntity_EnableDemonic},
	{"EnableGunPass", luaEntity_EnableGunPass},
	{"IsDrawEnabled", luaEntity_IsDrawEnabled},
	{"RemoveFromIntersectionSolver", luaEntity_RemoveFromIntersectionSolver},
	{"AddToIntersectionSolver", luaEntity_AddToIntersectionSolver},
	{"RemoveRagdollFromIntersectionSolver", luaEntity_RemoveRagdollFromIntersectionSolver},
	{"RemoveRagdoll", luaEntity_RemoveRagdoll},
	{"RecreateRagdollIfNone", luaEntity_RecreateRagdollIfNone},
	{"AddRagdollToIntersectionSolver", luaEntity_AddRagdollToIntersectionSolver},
	{"SpawnStaticDecal", luaEntity_SpawnStaticDecal},
	{"SpawnDecal", luaEntity_SpawnDecal},
	{"SpawnOrientedDecal", luaEntity_SpawnOrientedDecal},
	{"UpdateDecal", luaEntity_UpdateDecal},
	{"ReloadDecalSystem", luaEntity_ReloadDecalSystem},
	{"AttachTrailToBones", luaEntity_AttachTrailToBones},
	{"ExplodeItem", luaEntity_ExplodeItem},
	{"DamageItem", luaEntity_DamageItem},
	{"RenderBBox", luaEntity_RenderBBox},
    {"SetKillByParent", luaEntity_SetKillByParent},
	/*{"GetBBox", luaEntity_GetBBox},
	{"GetBRadius", luaEntity_GetBRadius},*/
	{"Pick", luaEntity_Pick},
	{"IsWater", luaEntity_IsWater},
    {"IsFixedMesh", luaEntity_IsFixedMesh},
	{"EnableDeathZoneTest",luaEntity_EnableDeathZoneTest},
    {"ComputeLocalPoint",luaEntity_ComputeLocalPoint},
    {"TransformLocalPointToWorld",luaEntity_TransformLocalPointToWorld},
    {"EnableNetworkSynchronization",luaEntity_EnableNetworkSynchronization},
    {"SetSynchroString",luaEntity_SetSynchroString},
    {"GetSynchroString",luaEntity_GetSynchroString},

	{"PortalVisibilityTest",luaEntity_PortalVisibilityTest},

	{"PO_Exist", luaEntity_PO_Exist},
	{"PO_Create", luaEntity_PO_Create},
	{"PO_GetMaxSphereRay", luaEntity_PO_GetMaxSphereRay},
	{"PO_Remove", luaEntity_PO_Remove},
	{"PO_Activate", luaEntity_PO_Activate},
	{"PO_IsActive", luaEntity_PO_IsActive},
	{"PO_Enable", luaEntity_PO_Enable},
	{"PO_IsEnabled", luaEntity_PO_IsEnabled},
	{"PO_Move", luaEntity_PO_Move},
	{"PO_SetMonsterMovementConst", luaEntity_PO_SetMonsterMovementConst},
	{"PO_Rotate", luaEntity_PO_Rotate},
	{"PO_SetSightParams", luaEntity_PO_SetSightParams},
	{"PO_GetPawnFloorPos", luaEntity_PO_GetPawnFloorPos},
	{"PO_GetPawnHeadPos", luaEntity_PO_GetPawnHeadPos},
    {"PO_SetPawnHeadPos", luaEntity_PO_SetPawnHeadPos},
	{"PO_Hit", luaEntity_PO_Hit},
	{"PO_AccumulateRotation", luaEntity_PO_AccumulateRotation},
	{"PO_Impulse", luaEntity_PO_Impulse},
	{"PO_EnableGravity", luaEntity_PO_EnableGravity},
	{"PO_SetGravity", luaEntity_PO_SetGravity},
	{"PO_EnableSpeedDamping", luaEntity_PO_EnableSpeedDamping},
	{"PO_MaintainLinearMovement", luaEntity_PO_MaintainLinearMovement},
	{"PO_MaintainPosition", luaEntity_PO_MaintainPosition},
	{"PO_MaintainVelocity", luaEntity_PO_MaintainVelocity},
	{"PO_MaintainAngularVelocity", luaEntity_PO_MaintainAngularVelocity},
	{"PO_SetAsTransporter", luaEntity_PO_SetAsTransporter},
	{"PO_SetFreedomOfRotation", luaEntity_PO_SetFreedomOfRotation},
	{"PO_SetFriction", luaEntity_PO_SetFriction},
	{"PO_SetMass", luaEntity_PO_SetMass},
	{"PO_GetMass", luaEntity_PO_GetMass},
	{"PO_ScaleInertiaTensor", luaEntity_PO_ScaleInertiaTensor},
	{"PO_SetHardDeactivator", luaEntity_PO_SetHardDeactivator},
	{"PO_SetRestitution", luaEntity_PO_SetRestitution},
	{"PO_SetMonsterType", luaEntity_PO_SetMonsterType},
	{"PO_GetCollisionGroup", luaEntity_PO_GetCollisionGroup},
	{"PO_SetCollisionGroup", luaEntity_PO_SetCollisionGroup},
	{"PO_SetLinearDamping", luaEntity_PO_SetLinearDamping},
	{"PO_SetAngularDamping", luaEntity_PO_SetAngularDamping},
	{"PO_GetLinearDamping", luaEntity_PO_GetLinearDamping},
	{"PO_GetAngularDamping", luaEntity_PO_GetAngularDamping},
	{"PO_SetFlying", luaEntity_PO_SetFlying},
	{"PO_SetPlayerFlying", luaEntity_PO_SetPlayerFlying},
	{"PO_SetEntitySteered", luaEntity_PO_SetEntitySteered},
	{"PO_SetMovedByExplosions", luaEntity_PO_SetMovedByExplosions},
	{"PO_SetPinned", luaEntity_PO_SetPinned},
	{"PO_IsPinned", luaEntity_PO_IsPinned},
	{"PO_IsFlying", luaEntity_PO_IsFlying},	// watch out, this checks if char is in the air, nothing to do with SetFlying()
	{"PO_IsOnFloor", luaEntity_PO_IsOnFloor},	// checks only variable (set by moster movement) FAST
    {"PO_AddAction", luaEntity_PO_AddAction},
    {"PO_RemoveAction", luaEntity_PO_RemoveAction},
    {"PO_GetAction", luaEntity_PO_GetAction},
    {"PO_SetAction", luaEntity_PO_SetAction},
    {"PO_IsActionState", luaEntity_PO_IsActionState},
    {"PO_MoveCenterOfMass", luaEntity_PO_MoveCenterOfMass},
    {"PO_MoveCenterOfMass", luaEntity_PO_MoveCenterOfMass},
    {"PO_GetPhysicsBody", luaEntity_PO_GetPhysicsBody},
	{"PO_LineTrace", luaEntity_PO_LineTrace},
    {"PO_SetPlayerShocked", luaEntity_PO_SetPlayerShocked},
    {"PO_GetType", luaEntity_PO_GetType},
    {"PO_JumpedInLastAction", luaEntity_PO_JumpedInLastAction},
    {"PO_IsFixed", luaEntity_PO_IsFixed},
    {"PO_SetGrenade", luaEntity_PO_SetGrenade},
    {"PO_SetMissile", luaEntity_PO_SetMissile},
    {"PO_IsOnLadder", luaEntity_PO_IsOnLadder},
	{"PO_HideFromPrediction", luaEntity_PO_HideFromPrediction},
	{"UpdatePart",luaEntity_UpdatePart},
	{NULL, NULL}
};
// =========================================================================
// World
// =========================================================================
static int luaWorld_LoadMap(lua_State* state)
{
	Script L(state);

	const char *name = L.GetString(1);
    const char *texName = L.GetString(2);
	const char *levname = L.GetString(3);

	#ifdef MPDEMO
		if (!(strstr(levname,"DM_Sacred") ||  strstr(levname,"DM_Cursed")))
			return 0;
	#endif

#ifndef TRANSGAMING
	GEngine->World->LevelName = levname;
    GEngine->World->Name = String::StripExtension(String::BaseName(name));
    GEngine->World->FileName = String::BaseName(name);
#else
    //char tmpBuf[MAX_PATH];

    //TRACE("luaWorld_LoadMap(): assigning the level name to '%s'\n", levname);
	GEngine->pcfWorld->LevelName = levname;
    //TRACE("luaWorld_LoadMap(): stripping off the basename from '%s'\n", name);
    GEngine->pcfWorld->Name = texName; //String::StripBaseName(name, tmpBuf, MAX_PATH);
    //TRACE("luaWorld_LoadMap(): getting the base filename from '%s' (tmpBuf = '%s')\n", name, tmpBuf);
    GEngine->pcfWorld->FileName = String::GetFilename(name);

    TRACE("luaWorld_LoadMap(): loading the map '%s' from '%s' (name = '%s', levName = '%s', texName = '%s')\n", GEngine->pcfWorld->Name.AsChar(), GEngine->pcfWorld->FileName.AsChar(), name, levname, texName);
#endif

#ifdef TRANSGAMING_PSP
	if (strstr(name,".dat") || strstr(name,".mpk"))
	//if (strstr(name,".dat") || strstr(name,".mpp"))
#elif defined(TRANSGAMING_PS2)
	if (strstr(name,".dat") || strstr(name,".mpk"))
	//if (strstr(name,".dat") || strstr(name,".mp2"))
#else
	if (strstr(name,".dat") || strstr(name,".mpk"))
#endif
	{
        //TRACE("luaWorld_LoadMap(): activating the loading screen\n");
        if (GEngine->Video)
        {
			GEngine->Video->PMenu->LoadScreen.Activate( true );
        }

#ifndef TRANSGAMING
		GEngine->World->Overbright = L.GetBool(4,false); // use overbright on lightmaps (modulate2x)
		GEngine->World->RTCubeMap = L.GetBool(5,false);
		GEngine->World->AnimMeshManager.SetShadowMapParams(L.GetInt(6,128),L.GetInt(7,24));
//		GEngine->World->RenderToCubemap = GEngine->World->RTCubeMap;
		GEngine->World->LoadMeshPak(name);
        GEngine->World->Scale = L.GetFloat(3);
#else
        TRACE("luaWorld_LoadMap(): loading map parameters and the mesh pack '%s'\n", name);
		//GEngine->World->Overbright = L.GetBool(5,false); // use overbright on lightmaps (modulate2x)
		GEngine->pcfWorld->RTCubeMap = L.GetBool(6,false);
		GEngine->pcfWorld->AnimMeshManager.SetShadowMapParams(L.GetInt(7,64),L.GetInt(8,8));
//		GEngine->pcfWorld->RenderToCubemap = GEngine->pcfWorld->RTCubeMap;
		GEngine->pcfWorld->LoadMeshPak(name);
        GEngine->pcfWorld->Scale = L.GetFloat(4);
#endif
 	}
	return 0;
}
// =========================================================================
static int luaWorld_Init(lua_State* state)
{
	Script L(state);

    //TRACE("luaWorld_Init(): initializing the world using physics engine %p\n", GEngine->pcfWorld->Physics);
#ifndef TRANSGAMING
	Assert( !GEngine->World->Physics );
#else
	Assert( !GEngine->pcfWorld->Physics );
#endif

	float deactDelay = L.GetFloat( 4, 5.0f );
	float deactMaxPosDiff = L.GetFloat( 5, 0.3f );
#ifndef TRANSGAMING_NO_HAVOK

#ifndef TRANSGAMING
	GEngine->World->Physics = GEngine->Physics->MakePhysicsWorld( GEngine->World, deactDelay, deactMaxPosDiff );
    GEngine->World->Physics->ActiveMeshesMassScale = L.GetFloat( 1, 1.0f );
    GEngine->World->Physics->DefaultMeshFriction = L.GetFloat( 2, GEngine->World->Physics->DefaultMeshFriction);
    GEngine->World->Physics->DefaultMeshRestitution = L.GetFloat( 3, GEngine->World->Physics->DefaultMeshRestitution);
#else
    //TRACE("luaWorld_Init(): initializing physics world\n");
	GEngine->pcfWorld->Physics = GEngine->Physics->MakePhysicsWorld( GEngine->pcfWorld, deactDelay, deactMaxPosDiff );
    //TRACE("luaWorld_Init(): physics world initialized to %p\n", GEngine->pcfWorld->Physics);
    GEngine->pcfWorld->Physics->ActiveMeshesMassScale = L.GetFloat( 1, 1.0f );
    GEngine->pcfWorld->Physics->DefaultMeshFriction = L.GetFloat( 2, GEngine->pcfWorld->Physics->DefaultMeshFriction);
    GEngine->pcfWorld->Physics->DefaultMeshRestitution = L.GetFloat( 3, GEngine->pcfWorld->Physics->DefaultMeshRestitution);
#endif // TRANSGAMING

#endif // TRANSGAMING_NO_HAVOK

    //TRACE("luaWorld_Init(): initializing the world object\n");
#ifndef TRANSGAMING
	GEngine->World->Init(GEngine->World->Scale);
#else
	GEngine->pcfWorld->Init(GEngine->pcfWorld->Scale);
#endif

	bool fromSave = L.GetBool( 6, false);
    if (!fromSave)
#ifndef TRANSGAMING
		GEngine->World->BuildGlasses();
#else
        GEngine->pcfWorld->BuildGlasses();
#endif

#ifndef TRANSGAMING_NO_HAVOK

    //TRACE("luaWorld_Init(): starting up the physics\n");
#ifndef TRANSGAMING
	GEngine->World->Physics->Start( fromSave );
#else
    GEngine->pcfWorld->Physics->Start( fromSave );
#endif

#endif
    return 0;
}
// =========================================================================
static int luaWorld_DemoInit(lua_State* state)
{
	Script L(state);
	Assert( !GEngine->Physics );

#ifndef TRANSGAMING
	Assert( !GEngine->World->Physics );
	GEngine->World->Init(GEngine->World->Scale);
#else
	Assert( !GEngine->pcfWorld->Physics );
	GEngine->pcfWorld->Init(GEngine->pcfWorld->Scale);
#endif

//	GEngine->pcfWorld->BuildGlasses();

    return 0;
}
// =========================================================================
static int luaWorld_LateVBsBegin(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	GEngine->World->LateVBs = true;
#else
	GEngine->pcfWorld->LateVBs = true;
#endif

    return 1;
}
// =========================================================================
static int luaWorld_LateVBsEnd(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	GEngine->World->LateVBs = false;
	GEngine->World->ProcessLateVBs();
#else
	GEngine->pcfWorld->LateVBs = false;
	GEngine->pcfWorld->ProcessLateVBs();
#endif

    return 1;
}
// =========================================================================
static int luaWorld_GetFileName(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	L.PushString(GEngine->World->FileName.AsChar());
#else
    L.PushString(GEngine->pcfWorld->FileName.AsChar());
#endif

    return 1;
}
// =========================================================================
static int luaWorld_ApplyTweaks(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
	Script L(state);
	if (GEngine && GEngine->Physics)
        GEngine->Physics->GetTweaksFromScript();
#endif
	return 0;
}
// =========================================================================
static int luaWorld_GetFrameCounter(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	L.PushInt(GEngine->World->Frame);
#else
	L.PushInt(GEngine->pcfWorld->Frame);
#endif

	return 0;
}
// =========================================================================
static int luaWorld_AdvanceFrameCounter(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	GEngine->World->Frame++;
#else
	GEngine->pcfWorld->Frame++;
#endif

	return 0;
}
// =========================================================================
static int luaWorld_AddEntity(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if (e)
	{
/*#ifdef TRANSGAMING
        TRACE("luaWorld_AddEntity(): adding entity to world: e = 0x%X (id = %d, name = '%s')\n", e, e->GIndex, e->FileName);
#endif//*/

		bool novis = L.GetBool(2,false);

#ifndef TRANSGAMING
		GEngine->World->AddEntity(e);
#else
		GEngine->pcfWorld->AddEntity(e);
#endif

		if (novis)
		{
			e->Flags |= Entity::EEntAlwaysVisible;
			e->EnableDraw(false);
		}

/*#ifdef TRANSGAMING
        TRACE("luaWorld_AddEntity(): done adding entity to world\n");
#endif//*/
	}
	return 0;
}
// =========================================================================
static int luaWorld_RemoveEntity(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
    Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif

	if (e)
#ifndef TRANSGAMING
		GEngine->World->RemoveEntity(e);
#else
		GEngine->pcfWorld->RemoveEntity(e);
#endif
	return 0;
}
// =========================================================================
static int luaWorld_LineTrace(lua_State* state)
{
	HitData hitData;
	Script L(state);
	Vector start(L.GetFloat(1),L.GetFloat(2),L.GetFloat(3));
	Vector end(L.GetFloat(4),L.GetFloat(5),L.GetFloat(6));

	/// XXX
	//GLog.Print("GEngine->Physics->TickPrecomputeCD\n");

#ifndef TRANSGAMING_NO_HAVOK
#ifndef TRANSGAMING
	if (GEngine->World->Physics->LineTrace(start,end,hitData))
#else
    if (GEngine->pcfWorld->Physics->LineTrace(start,end,hitData))
#endif
	{
		L.PushBool(true);
		L.PushFloat(hitData.Dist);
		L.PushFloat(hitData.HitPoint.X);
		L.PushFloat(hitData.HitPoint.Y);
		L.PushFloat(hitData.HitPoint.Z);
		L.PushFloat(hitData.Normal.X);
		L.PushFloat(hitData.Normal.Y);
		L.PushFloat(hitData.Normal.Z);
		L.PushLightUD(hitData.HavokEntity);

		if (hitData.UserData)
			L.PushInt(((Entity*)hitData.UserData)->GIndex);
		else
			L.PushNil();
		return 10;
	}
#endif
	L.PushBool(false);
	return 1;
}
// =========================================================================
static int luaWorld_FindEntityByName(lua_State* state)
{
    Script L(state);
    const char * name = L.GetString(1);

    Entity* next;
#ifndef TRANSGAMING
	for (Entity* entity = GEngine->World->EntityList; entity; entity=next)
#else
    for (Entity* entity = GEngine->pcfWorld->EntityList; entity; entity=next)
#endif
    {
        next = entity->Next;
        if (entity->Name.Len() && entity->Name.EqualsNoCase(name))
        {
            L.PushInt(entity->GIndex);
            return 1;
        }
    }

    L.PushInt(0);
    return 1;
}
// =========================================================================
static int luaWorld_GetEntityList(lua_State* state)
{
	Script L(state);

	int etype = L.GetInt(1);

	L.NewTable();
	Entity* next;
#ifndef TRANSGAMING
	for (Entity* entity = GEngine->World->EntityList; entity; entity=next)
#else
	for (Entity* entity = GEngine->pcfWorld->EntityList; entity; entity=next)
#endif
	{
		next = entity->Next;
		if (entity->Type == etype)
		{
			L.PushInt(entity->GIndex);
			L.PushString(entity->Name.Size()?entity->Name.AsChar():"");
			L.SetTable(-3);
		}
	}
	return 1;
}
// =========================================================================
static int luaWorld_LineTraceHitPlayerBalls(lua_State* state)
{
	HitData hitData;
	Script L(state);
	Vector start(L.GetFloat(1),L.GetFloat(2),L.GetFloat(3));
	Vector end(L.GetFloat(4),L.GetFloat(5),L.GetFloat(6));

	/// XXX
	//GLog.Print("GEngine->Physics->TickPrecomputeCD\n");

#ifndef TRANSGAMING_NO_HAVOK
#ifndef TRANSGAMING
	if (GEngine->World->Physics->LineTraceHitPlayer(start,end,hitData))
#else
    if (GEngine->pcfWorld->Physics->LineTraceHitPlayer(start,end,hitData))
#endif
	{
		L.PushBool(true);
		L.PushFloat(hitData.Dist);
		L.PushFloat(hitData.HitPoint.X);
		L.PushFloat(hitData.HitPoint.Y);
		L.PushFloat(hitData.HitPoint.Z);
		L.PushFloat(hitData.Normal.X);
		L.PushFloat(hitData.Normal.Y);
		L.PushFloat(hitData.Normal.Z);
		L.PushLightUD(hitData.HavokEntity);

		if (hitData.UserData)
            L.PushInt(((Entity*)hitData.UserData)->GIndex);
		else
			L.PushNil();
		return 10;
	}
#endif
	L.PushBool(false);
	return 1;
}
// =========================================================================
static int luaWorld_LineTraceFixedGeom(lua_State* state)
{
	HitData hitData;
	Script L(state);
	Vector start(L.GetFloat(1),L.GetFloat(2),L.GetFloat(3));
	Vector end(L.GetFloat(4),L.GetFloat(5),L.GetFloat(6));

	/// XXX
	//GLog.Print("GEngine->Physics->TickPrecomputeCD\n");

#ifndef TRANSGAMING_NO_HAVOK
#ifndef TRANSGAMING
	if (GEngine->World->Physics->LineTraceFixedGeom(start,end,hitData))
#else
    if (GEngine->pcfWorld->Physics->LineTraceFixedGeom(start,end,hitData))
#endif
	{
		L.PushBool(true);
		L.PushFloat(hitData.Dist);
		L.PushFloat(hitData.HitPoint.X);
		L.PushFloat(hitData.HitPoint.Y);
		L.PushFloat(hitData.HitPoint.Z);
		L.PushFloat(hitData.Normal.X);
		L.PushFloat(hitData.Normal.Y);
		L.PushFloat(hitData.Normal.Z);
		L.PushLightUD(hitData.HavokEntity);

		if (hitData.UserData)
            L.PushInt(((Entity*)hitData.UserData)->GIndex);
		else
			L.PushNil();
		return 10;
	}
#endif
	L.PushBool(false);
	return 1;
}

// =========================================================================
static int luaWorld_LineTraceFirstHit(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING_NO_HAVOK
	Vector start(L.GetFloat(1),L.GetFloat(2),L.GetFloat(3));
	Vector end(L.GetFloat(4),L.GetFloat(5),L.GetFloat(6));

#ifndef TRANSGAMING
	L.PushBool( GEngine->World->Physics->LineTraceFirstHit(start,end) );
#else
    L.PushBool( GEngine->pcfWorld->Physics->LineTraceFirstHit(start,end) );
#endif

#else
    L.PushBool(false);
#endif
	return 1;
}

// =========================================================================
static int luaWorld_HitPhysicObject(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
	Script L(state);
    void *e = (void *) L.GetLightUD(1);
	if (e)
	{
		Vector point(L.GetFloat(2),L.GetFloat(3),L.GetFloat(4));
		Vector vect(L.GetFloat(5),L.GetFloat(6),L.GetFloat(7));
#ifndef TRANSGAMING
		GEngine->World->Physics->HitEntity(e,point,vect);
#else
		GEngine->pcfWorld->Physics->HitEntity(e,point,vect);
#endif
	}
#endif
	return 0;
}
// =========================================================================
static int luaWorld_Explosion2(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
	Script L(state);
	Vector center(L.GetFloat(1),L.GetFloat(2),L.GetFloat(3));
#ifndef TRANSGAMING
	GEngine->World->Physics->Explosion(center,L.GetFloat(4),L.GetFloat(5),L.GetInt(6,-1),L.GetInt(7),L.GetFloat(8));
#else
    GEngine->pcfWorld->Physics->Explosion(center,L.GetFloat(4),L.GetFloat(5),L.GetInt(6,-1),L.GetInt(7),L.GetFloat(8));
#endif

#endif
	return 0;
}
// =========================================================================
static int luaWorld_MultiplayerExplosion(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
    Script L(state);
    Vector center(L.GetFloat(1),L.GetFloat(2),L.GetFloat(3));
#ifndef TRANSGAMING
	GEngine->World->Physics->MultiplayerExplosion(center,L.GetFloat(4),L.GetFloat(5),L.GetInt(6,-1),L.GetInt(7),L.GetFloat(8),L.GetFloat(9,1.0f));
#else
    GEngine->pcfWorld->Physics->MultiplayerExplosion(center,L.GetFloat(4),L.GetFloat(5),L.GetInt(6,-1),L.GetInt(7),L.GetFloat(8),L.GetFloat(9,1.0f));
#endif

#endif
    return 0;
}
// =========================================================================
static int luaWorld_ExplosionUp(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
	Script L(state);
	Vector center(L.GetFloat(1),L.GetFloat(2),L.GetFloat(3));
#ifndef TRANSGAMING
	GEngine->World->Physics->ExplosionUp(center,L.GetFloat(4),L.GetFloat(5),L.GetFloat(6),L.GetFloat(7));
#else
	GEngine->pcfWorld->Physics->ExplosionUp(center,L.GetFloat(4),L.GetFloat(5),L.GetFloat(6),L.GetFloat(7));
#endif

#endif
	return 0;
}
// =========================================================================
static int luaWorld_ExplosionParabolic(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
	Script L(state);
	Vector center(L.GetFloat(1),L.GetFloat(2),L.GetFloat(3));
	Vector target(L.GetFloat(6),L.GetFloat(7),L.GetFloat(8));
	float flightTime = L.GetFloat(4);
	float radius = L.GetFloat(5);
#ifndef TRANSGAMING
	GEngine->World->Physics->ExplosionParabolic(center,flightTime,radius,target);
#else
	GEngine->pcfWorld->Physics->ExplosionParabolic(center,flightTime,radius,target);
#endif

#endif
	return 0;
}


// =========================================================================

static int luaWorld_SetupFog(lua_State* state)
{
	Script L(state);

	if (!GEngine->Video)
		return 0;

	int Mode = L.GetInt(1);

	if (Mode)
	{
#ifndef TRANSGAMING
		GEngine->World->UseFog = true;
#else
		GEngine->pcfWorld->UseFog = true;
#endif
		if (Mode>0)
			Mode = FogState::EFogLinear;
#ifndef TRANSGAMING
		GEngine->World->Env.FogParams.Mode = Mode;
		GEngine->World->Env.FogParams.Start = L.GetFloat(2);
		GEngine->World->Env.FogParams.End = L.GetFloat(3);
		GEngine->World->Env.FogParams.Density = L.GetFloat(4);
		GEngine->World->Env.FogParams.Color = L.GetInt(5);

		if (GEngine->Video->BloomEnabled && (GEngine->World->BloomFX.Multiplier > 0.0F))
#else
		GEngine->pcfWorld->Env.FogParams.Mode = Mode;
		GEngine->pcfWorld->Env.FogParams.Start = L.GetFloat(2);
		GEngine->pcfWorld->Env.FogParams.End = L.GetFloat(3);
		GEngine->pcfWorld->Env.FogParams.Density = L.GetFloat(4);
		GEngine->pcfWorld->Env.FogParams.Color = L.GetInt(5);

		if (GEngine->Video->BloomEnabled && (GEngine->pcfWorld->BloomFX.Multiplier > 0.0F))
#endif
		{
#ifdef	TRANSGAMING
			Plane color = ::Color_TG(GEngine->pcfWorld->Env.FogParams.Color);
			GEngine->pcfWorld->Env.FogParams.Color = ::Color_TG(&color.X).dwVal;
#else
			Plane color = ::Color(GEngine->World->Env.FogParams.Color);
			GEngine->World->Env.FogParams.Color = ::Color(&color.X).dwVal;
#endif

		}

	}
	else
#ifndef TRANSGAMING
		GEngine->World->UseFog = false;
#else
		GEngine->pcfWorld->UseFog = false;
#endif
	return 0;
}
// =========================================================================
static int luaWorld_SetDirLight(lua_State* state)
{
	Script L(state);

	Vector to;
#ifndef TRANSGAMING
	GEngine->World->Env.DirLight.SetDirection(Vector( L.GetFloat(1), L.GetFloat(2), L.GetFloat(3)));
	GEngine->World->Env.DirLight.Color  = L.GetInt(4);
	GEngine->World->Env.DirLight.Intensity = L.GetFloat(5);
#else
	GEngine->pcfWorld->Env.DirLight.SetDirection(Vector( L.GetFloat(1), L.GetFloat(2), L.GetFloat(3)));
	GEngine->pcfWorld->Env.DirLight.Color  = L.GetInt(4);
	GEngine->pcfWorld->Env.DirLight.Intensity = L.GetFloat(5);
#endif
	return 0;
}
// =========================================================================
static int luaWorld_SetGamePaused(lua_State* state)
{
	Script L(state);
	bool pause = L.GetBool( 1, false );
	GEngine->Game->mPaused = pause;
	return 0;
}
// =========================================================================
static int luaWorld_SetGameVisible(lua_State* state)
{
    if (!GEngine->Video) return 0;

	Script L(state);
	bool visible = L.GetBool( 1, false );
	View * view = GEngine->Video->GetActiveView();
	if( visible )
		view->Mode &= ~View::EModeDoNotRenderWorld;
	else
		view->Mode |= View::EModeDoNotRenderWorld;
	return 0;
}
// =========================================================================
static int luaWorld_IsGamePaused(lua_State* state)
{
	Script L(state);
	L.PushBool( GEngine->Game->mPaused );
	return 1;
}
// =========================================================================
static int luaWorld_SetupWater(lua_State* state)
{
	Script L(state);
#ifndef TRANSGAMING
	GEngine->World->Water.FresnelBias		= L.GetFloat(1);
	GEngine->World->Water.FresnelExponent	= L.GetFloat(2);
	GEngine->World->Water.BumpHeight		= L.GetFloat(3);
	GEngine->World->Water.WaveAmplitude		= L.GetFloat(4);
	GEngine->World->Water.WaveFreq			= L.GetFloat(5);
#else
	GEngine->pcfWorld->Water.FresnelBias		= L.GetFloat(1);
	GEngine->pcfWorld->Water.FresnelExponent	= L.GetFloat(2);
	GEngine->pcfWorld->Water.BumpHeight		= L.GetFloat(3);
	GEngine->pcfWorld->Water.WaveAmplitude		= L.GetFloat(4);
	GEngine->pcfWorld->Water.WaveFreq			= L.GetFloat(5);
#endif

#ifdef	TRANSGAMING
	GEngine->pcfWorld->Water.DeepWaterColor	= Color_TG(L.GetFloat(6)/255.0f,L.GetFloat(7)/255.0f,L.GetFloat(8)/255.0f);
	GEngine->pcfWorld->Water.ShallowWaterColor	= Color_TG(L.GetFloat(9)/255.0f,L.GetFloat(10)/255.0f,L.GetFloat(11)/255.0f);
#else
	GEngine->World->Water.DeepWaterColor	= Color(L.GetFloat(6)/255.0f,L.GetFloat(7)/255.0f,L.GetFloat(8)/255.0f);
	GEngine->World->Water.ShallowWaterColor	= Color(L.GetFloat(9)/255.0f,L.GetFloat(10)/255.0f,L.GetFloat(11)/255.0f);
#endif

#ifndef TRANSGAMING
	GEngine->World->Water.WaveSpeed			= L.GetFloat(12);
	GEngine->World->Water.WaterAmount		= L.GetFloat(13);
	GEngine->World->Water.ReflectionAmount	= L.GetFloat(14);
	GEngine->World->Water.WaterLevel		= L.GetFloat(15);

	bool reflect = L.GetBool(16);
	bool refract = L.GetBool(17);

	GEngine->World->Water.Pan[0].X		= L.GetFloat(18);
	GEngine->World->Water.Pan[0].Y		= L.GetFloat(19);
	GEngine->World->Water.Tile[0].X		= L.GetFloat(20);
	GEngine->World->Water.Tile[0].Y		= L.GetFloat(21);

	GEngine->World->Water.ReflectFlags = 0;
	if (reflect)
		GEngine->World->Water.ReflectFlags |= Entity::EReflectScene;
	if (refract)
		GEngine->World->Water.ReflectFlags |= Entity::ERefractScene;

	if (GEngine->World->WaterMesh)
		GEngine->World->WaterMesh->SetupWaterShader();
#else
	GEngine->pcfWorld->Water.WaveSpeed			= L.GetFloat(12);
	GEngine->pcfWorld->Water.WaterAmount		= L.GetFloat(13);
	GEngine->pcfWorld->Water.ReflectionAmount	= L.GetFloat(14);
	GEngine->pcfWorld->Water.WaterLevel		= L.GetFloat(15);

	bool reflect = L.GetBool(16);
	bool refract = L.GetBool(17);

	GEngine->pcfWorld->Water.Pan[0].X		= L.GetFloat(18);
	GEngine->pcfWorld->Water.Pan[0].Y		= L.GetFloat(19);
	GEngine->pcfWorld->Water.Tile[0].X		= L.GetFloat(20);
	GEngine->pcfWorld->Water.Tile[0].Y		= L.GetFloat(21);

	GEngine->pcfWorld->Water.ReflectFlags = 0;
	if (reflect)
		GEngine->pcfWorld->Water.ReflectFlags |= Entity::EReflectScene;
	if (refract)
		GEngine->pcfWorld->Water.ReflectFlags |= Entity::ERefractScene;

	if (GEngine->pcfWorld->WaterMesh)
		GEngine->pcfWorld->WaterMesh->SetupWaterShader();
#endif

	return 0;
}
// =========================================================================
static int luaWorld_SetupSky(lua_State* state)
{
	Script L(state);

	int Mode = L.GetInt(1);
	if (Mode)
	{
		View * view = GEngine->Video->GetActiveView();
		view->Mode |= View::EModeShowSky;

#ifndef TRANSGAMING
/*		GEngine->World->mSky.HasFog = L.GetBool(2);
		GEngine->World->mSky.Scale = L.GetFloat(3);
		GEngine->World->mSky.YOffset  = L.GetFloat(4);

		GEngine->World->mSky.RotSpeed = L.GetFloat(5);
		GEngine->World->mSky.Pan1Speed  = L.GetFloat(6);
		GEngine->World->mSky.Pan2Speed  = L.GetFloat(7);
		GEngine->World->mSky.Tile1 = L.GetFloat(8);
		GEngine->World->mSky.Tile2 = L.GetFloat(9);

		GEngine->World->mSky.BackTex = L.GetString(10);
		GEngine->World->mSky.Cloud1Tex = L.GetString(11);
		GEngine->World->mSky.Cloud2Tex = L.GetString(12);
		GEngine->World->mSky.MaskTex = L.GetString(13);
		GEngine->World->mSky.Reset();
*/
#else
/*		GEngine->pcfWorld->mSky.HasFog = L.GetBool(2);
		GEngine->pcfWorld->mSky.Scale = L.GetFloat(3);
		GEngine->pcfWorld->mSky.YOffset  = L.GetFloat(4);

		GEngine->pcfWorld->mSky.RotSpeed = L.GetFloat(5);
		GEngine->pcfWorld->mSky.Pan1Speed  = L.GetFloat(6);
		GEngine->pcfWorld->mSky.Pan2Speed  = L.GetFloat(7);
		GEngine->pcfWorld->mSky.Tile1 = L.GetFloat(8);
		GEngine->pcfWorld->mSky.Tile2 = L.GetFloat(9);

		GEngine->pcfWorld->mSky.BackTex = L.GetString(10);
		GEngine->pcfWorld->mSky.Cloud1Tex = L.GetString(11);
		GEngine->pcfWorld->mSky.Cloud2Tex = L.GetString(12);
		GEngine->pcfWorld->mSky.MaskTex = L.GetString(13);
		GEngine->pcfWorld->mSky.Reset();
*/
#endif
	}
	else
	{
		View * view = GEngine->Video->GetActiveView();
		view->Mode &= ~View::EModeShowSky;
	}
	return 0;
}
// =========================================================================
static int luaWorld_LoadSky(lua_State* state)
{
	Script L(state);
#ifdef TRANSGAMING_NON_TNL
    if (!GEngine->Video)
    {
        L.PushInt(0);
        return 1;
    }

	RenderDevice *dev = GEngine->Video->RenderDev;
	View * view = GEngine->Video->GetActiveView();

	int numLayers = 0;

	if (dev->HWClass != RenderDevice::EHWClassGenericTnL)
	{
#ifndef TRANSGAMING
		numLayers = GEngine->World->mSky.Load(L.GetString(1));
		GEngine->World->mSky.LowQuality = false;
		GEngine->World->mSky.Height = L.GetFloat(2,0);
		GEngine->World->mSky.Angle  = FDeg2Rad(L.GetFloat(3,0));
#else
        TRACE("luaWorld_LoadSky(): loading a sky from '%s'\n", L.GetString(1, "<none>"));
		numLayers = GEngine->pcfWorld->mSky.Load(L.GetString(1));
		GEngine->pcfWorld->mSky.LowQuality = false;
		GEngine->pcfWorld->mSky.Height = L.GetFloat(2,0);
		GEngine->pcfWorld->mSky.Angle  = FDeg2Rad(L.GetFloat(3,0));
#endif
		if (numLayers)	view->Mode |= View::EModeShowSky;
		else			view->Mode &= ~View::EModeShowSky;
	}

    TRACE("luaWorld_LoadSky(): loaded %d sky layers\n", numLayers);
	L.PushInt(numLayers);
#else
	L.PushInt(0);
#endif
	return 1;
}
// =========================================================================
static int luaWorld_LoadLowQualitySky(lua_State* state)
{
	Script L(state);
    if (!GEngine->Video)
    {
        L.PushInt(0);
        return 1;
    }

	View * view = GEngine->Video->GetActiveView();

#ifndef TRANSGAMING
	int numLayers = GEngine->World->mSky.Load(L.GetString(1));
	GEngine->World->mSky.LowQuality = true;
	GEngine->World->mSky.Height = L.GetFloat(2,0);
	GEngine->World->mSky.Angle  = FDeg2Rad(L.GetFloat(3,0));
#else
    TRACE("luaWorld_LoadLowQualitySky(): loading a low quality sky from '%s'\n", L.GetString(1, "<none>"));
	int numLayers = GEngine->pcfWorld->mSky.Load(L.GetString(1));
	GEngine->pcfWorld->mSky.LowQuality = true;
	GEngine->pcfWorld->mSky.Height = L.GetFloat(2,0);
	GEngine->pcfWorld->mSky.Angle  = FDeg2Rad(L.GetFloat(3,0));
#endif

	if (numLayers)	view->Mode |= View::EModeShowSky;
	else			view->Mode &= ~View::EModeShowSky;

    TRACE("luaWorld_LoadLowQualitySky(): loaded %d sky layers\n", numLayers);
	L.PushInt(numLayers);

	return 1;
}
// =========================================================================
static int luaWorld_SetupSkyLayer(lua_State* state)
{
	Script L(state);
	int layerNum = L.GetInt(1);

#ifndef TRANSGAMING
	SkyLayer* layer = GEngine->World->mSky.GetLayer(layerNum);
#else
	SkyLayer* layer = GEngine->pcfWorld->mSky.GetLayer(layerNum);
#endif
    TRACE("luaWorld_SetupSkyLayer(): setting up sky layer %d\n", layerNum);

	if (layer)
	{
		layer->Mask   = L.GetString(2,"");
		layer->LMap   = L.GetString(3,"");

		layer->Tex1   = L.GetString(4,"");
		layer->Rot1   = L.GetFloat(5,0.0f);
		layer->Pan1U  = L.GetFloat(6,0.0f);
		layer->Pan1V  = L.GetFloat(7,0.0f);
		layer->Tile1U = L.GetFloat(8,1.0f);
		layer->Tile1V = L.GetFloat(9,1.0f);

		layer->Tex2   = L.GetString(10,"");
		layer->Rot2   = L.GetFloat(11,0.0f);
		layer->Pan2U  = L.GetFloat(12,0.0f);
		layer->Pan2V  = L.GetFloat(13,0.0f);
		layer->Tile2U = L.GetFloat(14,1.0f);
		layer->Tile2V = L.GetFloat(15,1.0f);

		layer->Setup();
	}
#ifdef DEBUG
    else
        TRACE("luaWorld_SetupSkyLayer(): ERROR-> the current sky does not have layer %d\n", layerNum);
#endif
	return 0;
}
// =========================================================================
static int luaWorld_InitSky(lua_State* state)
{
	Script L(state);
	if (GEngine->Video)
	{
#ifndef TRANSGAMING
		GEngine->World->mSky.RenderInitialize(GEngine->Video->RenderDev,GEngine->Video->MaterialSys);
#else
		GEngine->pcfWorld->mSky.RenderInitialize(GEngine->Video->RenderDev,GEngine->Video->MaterialSys);
#endif
	}
	return 0;
}
// =========================================================================
static int luaWorld_AmbientColor(lua_State* state)
{
	Script L(state);
#ifdef	TRANSGAMING
	GEngine->pcfWorld->Env.AmbientColor = Color_TG(L.GetFloat(1)/255.0f,L.GetFloat(2)/255.0f,L.GetFloat(3)/255.0f);
	GEngine->pcfWorld->PixelLightingMultiplier = L.GetFloat(4,1.0f);
#else
	GEngine->World->Env.AmbientColor = Color(L.GetFloat(1)/255.0f,L.GetFloat(2)/255.0f,L.GetFloat(3)/255.0f);
	GEngine->World->PixelLightingMultiplier = L.GetFloat(4,1.0f);
#endif
	return 0;
}
// =========================================================================
static int luaWorld_GetAmbientColor(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	L.PushInt( GEngine->World->Env.AmbientColor.R );
	L.PushInt( GEngine->World->Env.AmbientColor.G );
	L.PushInt( GEngine->World->Env.AmbientColor.B );
	L.PushFloat( GEngine->World->PixelLightingMultiplier );
#else
	L.PushInt( GEngine->pcfWorld->Env.AmbientColor.R );
	L.PushInt( GEngine->pcfWorld->Env.AmbientColor.G );
	L.PushInt( GEngine->pcfWorld->Env.AmbientColor.B );
	L.PushFloat( GEngine->pcfWorld->PixelLightingMultiplier );
#endif

	return 4;
}
// =========================================================================
static int luaWorld_EnableDeathZone(lua_State* state)
{
	Script L(state);
    const char * name = L.GetString(1);
#ifndef TRANSGAMING
	for( int i=0; i<GEngine->World->DeathZones.Size(); ++i )
    {
        if (GEngine->World->DeathZones[i]->Name == name)
        {
            GEngine->World->DeathZones[i]->Enabled = L.GetBool(2);
            break;
        }
    }
#else
    for( int i=0; i<GEngine->pcfWorld->DeathZones.Size(); ++i )
    {
        if (GEngine->pcfWorld->DeathZones[i]->Name == name)
        {
            GEngine->pcfWorld->DeathZones[i]->Enabled = L.GetBool(2);
            break;
        }
    }
#endif
	return 0;
}
// =========================================================================
static int luaWorld_SetFarClipDist(lua_State* state)
{
	Script L(state);
#ifndef TRANSGAMING
	GEngine->World->FarClipDist = L.GetFloat(1);
#else
	GEngine->pcfWorld->FarClipDist = L.GetFloat(1);
#endif
	return 0;
}
// =========================================================================
static int luaWorld_SetInLoadingScreen(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	GEngine->World->InLoadingScreen = L.GetBool(1,false);
#else
	GEngine->pcfWorld->InLoadingScreen = L.GetBool(1,false);
#endif

	// Reset input system so mouse down from new game won't cause a weapon shot
#ifndef TRANSGAMING
	if( !GEngine->World->InLoadingScreen )
#else
	if( !GEngine->pcfWorld->InLoadingScreen )
#endif
	{
		GEngine->InputSys->Reset();
		GEngine->InputSys->UpdateMousePosition();
		GEngine->InputSys->CenterMouse();
	}

	return 0;
}
// =========================================================================
static int luaWorld_IsPortalEnabled(lua_State* state)
{
	Script L(state);
	const char *portalName = L.GetString(1);
#ifndef TRANSGAMING
	L.PushBool( portalName[0]?GEngine->World->IsPortalEnabled( portalName ):false );
#else
	L.PushBool( portalName[0] ? GEngine->pcfWorld->IsPortalEnabled( portalName ):false );
#endif
	return 1;
}
// =========================================================================
static int luaWorld_UseSwitchZones(lua_State* state)
{
	Script L(state);
	bool use = L.GetBool(1,true);
#ifndef TRANSGAMING
	if( GEngine->World )
		GEngine->World->UseSwitchZones( use );
#else
	if( GEngine->pcfWorld )
		GEngine->pcfWorld->UseSwitchZones( use );
#endif
	return 0;
}
// =========================================================================
static int luaWorld_EnablePortal(lua_State* state)
{
	Script L(state);
	const char *portalName = L.GetString(1);
	if( portalName[0] )
#ifndef TRANSGAMING
		GEngine->World->EnablePortal( portalName, L.GetBool(2) );
#else
		GEngine->pcfWorld->EnablePortal( portalName, L.GetBool(2) );
#endif
	return 0;
}
// =========================================================================
static int luaWorld_IsAntiPortalEnabled(lua_State* state)
{
	Script L(state);
	const char *antiportalName = L.GetString(1);
#ifndef TRANSGAMING
	L.PushBool( antiportalName[0]?GEngine->World->IsAntiPortalEnabled( antiportalName ):false );
#else
	L.PushBool( antiportalName[0] ? GEngine->pcfWorld->IsAntiPortalEnabled( antiportalName ):false );
#endif
	return 1;
}
// =========================================================================
static int luaWorld_EnableAntiPortal(lua_State* state)
{
	Script L(state);
	const char *antiportalName = L.GetString(1);
	if( antiportalName && antiportalName[0] )
#ifndef TRANSGAMING
		GEngine->World->EnableAntiPortal( antiportalName, L.GetBool(2) );
#else
		GEngine->pcfWorld->EnableAntiPortal( antiportalName, L.GetBool(2) );
#endif
	return 0;
}
// =========================================================================
static int luaWorld_DeleteAntiPortal(lua_State* state)
{
	Script L(state);
	const char *antiportalName = L.GetString(1);
	if( antiportalName && antiportalName[0] )
#ifndef TRANSGAMING
		GEngine->World->DeleteAntiPortal( antiportalName);
#else
		GEngine->pcfWorld->DeleteAntiPortal(antiportalName);
#endif
	return 0;
}
// =========================================================================
static int luaWorld_EnableOcclude(lua_State* state) {
	Script L(state);
	bool enable = L.GetBool(1, true);
	if (GEngine && GEngine->Video) {
		if( enable )
			GEngine->Video->GetActiveView()->Mode |= View::EModeVisibility;
		else
			GEngine->Video->GetActiveView()->Mode &= ~View::EModeVisibility;
	}
	return 0;
}
// =========================================================================
static int luaWorld_EnableOccludeAntiPortals(lua_State* state) {
	Script L(state);
	bool enable = L.GetBool(1, true);
	if (GEngine && GEngine->Video) {
		if( enable )
			GEngine->Video->GetActiveView()->Mode &= ~View::EModeNoAntiportals;
		else
			GEngine->Video->GetActiveView()->Mode |= View::EModeNoAntiportals;
	}
	return 0;
}

// =========================================================================
static int luaWorld_CreateEnabledAntiPortalFromClosedConvexMesh(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	WorldMesh* mesh = (WorldMesh*)GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeMesh);
#else
    WorldMesh* mesh = (WorldMesh*)GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeMesh);
#endif

	if( !mesh ) { L.PushString( "" ); return 1; };

    m_setTag("ScriptPortalAnti");
	AntiPortal* antiportal = new AntiPortal( mesh );
	if( !antiportal->BuildTriangleAdjacencies() )
	{
		GLog.Print("WARNING: Antiportal %s is not a closed shape, ignoring!\n", antiportal->Name.AsChar() );
		delete antiportal;
		L.PushString( "" );
	}
	else
	{

#ifndef TRANSGAMING
		GEngine->World->AntiPortals.Add( antiportal );
#else
		GEngine->pcfWorld->AntiPortals.Add( antiportal );
#endif

#ifndef TRANSGAMING
		for( int zoneIndex=0; zoneIndex<GEngine->World->Zones.Size(); ++zoneIndex )
        {
            Zone* zone = GEngine->World->Zones[zoneIndex];
#else
        for( int zoneIndex=0; zoneIndex<GEngine->pcfWorld->Zones.Size(); ++zoneIndex )
        {
            Zone* zone = GEngine->pcfWorld->Zones[zoneIndex];
#endif
            if( zone->BoxTest( antiportal->Box ) )
                zone->AntiPortals.Add( antiportal );
        }

		GLog.Print("Antiportal %s, Box\t:",antiportal->Name.AsChar());
		GLog.Print(antiportal->Box);
		GLog.Print("\n");
		L.PushString( antiportal->Name.AsChar() );
	}

	return 1;
}
// =========================================================================
static int luaWorld_IsUnderwater(lua_State* state)
{
	Script L(state);
#ifndef TRANSGAMING_NO_HAVOK

#ifndef TRANSGAMING
	L.PushBool( GEngine->World->Physics->IsUnderwaterWorld() );
#else
	L.PushBool( GEngine->pcfWorld->Physics->IsUnderwaterWorld() );
#endif

#else
    L.PushBool(false);
#endif
	return 1;
}
// =========================================================================
static int luaWorld_MakeUnderwater(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
	Script L(state);

#ifndef TRANSGAMING
	if( GEngine->World->Physics )
		GEngine->World->Physics->EnableUnderwaterWorld( L.GetBool(1,false) );
#else
	if( GEngine->pcfWorld->Physics )
		GEngine->pcfWorld->Physics->EnableUnderwaterWorld( L.GetBool(1,false) );
#endif

#endif
	return 0;
}
// =========================================================================
static int luaWorld_UpdateAllEntities(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	GEngine->World->UpdateAllEntities();
#else
	GEngine->pcfWorld->UpdateAllEntities();
#endif

	return 0;
}
// =========================================================================
static int luaWorld_CheckStartGlass(lua_State* state)
{
	Script L(state);
#ifndef TRANSGAMING_NO_HAVOK
    void *body = L.GetLightUD(1);
	if( body != NULL )
	{
		Vector pos( L.GetFloat(2), L.GetFloat(3), L.GetFloat(4));
        float radius = L.GetFloat(5,0.5f);
        Vector vel(L.GetFloat(6,0.0f), L.GetFloat(7,0.0f), L.GetFloat(8,0.0f));

#ifndef TRANSGAMING
		L.PushBool(GEngine->World->Physics->CheckStartGlass( body, pos, radius, vel ));
#else
        L.PushBool(GEngine->pcfWorld->Physics->CheckStartGlass( body, pos, radius, vel ));
#endif

	}
	else
		L.PushBool(false);
#else
	L.PushBool(false);
#endif
	return 1;
}
// =========================================================================
static int luaWorld_FlashSkyTexture(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	GEngine->World->mSky.FlashTexture(L.GetFloat(1),L.GetInt(2));
#else
	GEngine->pcfWorld->mSky.FlashTexture(L.GetFloat(1),L.GetInt(2));
#endif

	return 0;
}
// =========================================================================
static int luaWorld_SetDynamicSpecular(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	GEngine->World->DynamicSpecular = Plane(L.GetFloat(1)/255.f,L.GetFloat(2)/255.f,L.GetFloat(3)/255.f,L.GetFloat(4));
#else
	GEngine->pcfWorld->DynamicSpecular = Plane(L.GetFloat(1)/255.f,L.GetFloat(2)/255.f,L.GetFloat(3)/255.f,L.GetFloat(4));
#endif

	return 0;
}
// =========================================================================
static int luaWorld_SetWorldSpeed( lua_State* state )
{
	Script L(state);
	float speed = L.GetFloat(1,1.0f);
//	GLog.Print( "SetWorldSpeed: %f\n", speed );
	bool wasNormal = ( GEngine->TimeMultiplier == 1.0f );
	GEngine->SetTimeMultiplier( speed );

#ifndef TRANSGAMING_NO_MILES
    if (GEngine->Audio)
    {
		float speed2 = (GEngine->TimeMultiplier-1.0f)*0.5f + 1.0f;
	    GEngine->Audio->SetSpeed( speed2 );
	    GEngine->Audio->SetLowPass( sqrt( speed2 ) );

		if( ( GEngine->TimeMultiplier == 1.0f ) && !wasNormal )
		{
			GEngine->Audio->Reset();
			GLog.Print( "Sound Reset\n" );
		}
    }
#endif

	return 0;
}
// =========================================================================
static int luaWorld_EnumeratePortals(lua_State* state)
{
    Script L(state);

    L.NewTable();

#ifndef TRANSGAMING
	for (int i=0;i<GEngine->World->Portals.Size();i++)
    {
        L.PushInt(i+1);
        L.PushString(GEngine->World->Portals[i]->Name.AsChar());
        L.SetTable(-3);
    }
#else
    for (int i=0;i<GEngine->pcfWorld->Portals.Size();i++)
    {
        L.PushInt(i+1);
        L.PushString(GEngine->pcfWorld->Portals[i]->Name.AsChar());
        L.SetTable(-3);
    }
#endif

    return 1;
}
// =========================================================================
static int luaWorld_EnumerateAntiPortals(lua_State* state)
{
	Script L(state);

	L.NewTable();

#ifndef TRANSGAMING
	for (int i=0;i<GEngine->World->AntiPortals.Size();i++)
	{
		L.PushInt(i+1);
		L.PushString(GEngine->World->AntiPortals[i]->Name.AsChar());
		L.SetTable(-3);
	}
#else
	for (int i=0;i<GEngine->pcfWorld->AntiPortals.Size();i++)
	{
		L.PushInt(i+1);
		L.PushString(GEngine->pcfWorld->AntiPortals[i]->Name.AsChar());
		L.SetTable(-3);
	}
#endif

	return 1;
}
// =========================================================================
static int luaWorld_SetRenderTarget(lua_State* state)
{
	View *view = GEngine->Video->GetActiveView();

    Script L(state);

	int texture = L.GetInt(1,0);

	if (texture>0 && texture<=View::TEX_MBLUR) {

#ifndef TRANSGAMING
		view->RenderTargetTex     = GEngine->World->TexSceneMotionBlur[texture-1];
#else
		view->RenderTargetTex     = GEngine->pcfWorld->TexSceneMotionBlur[texture-1];
#endif

		view->PostProcess.PassNum = L.GetInt(2);

		for (int i=0; i<view->PostProcess.PassNum; i++) {

#ifndef TRANSGAMING
			view->PostProcess.Tex[i]   = GEngine->World->TexSceneMotionBlur[L.GetInt(3+i*2)-1];
#else
			view->PostProcess.Tex[i]   = GEngine->pcfWorld->TexSceneMotionBlur[L.GetInt(3+i*2)-1];
#endif
			view->PostProcess.Alpha[i] = L.GetFloat(4+i*2);
		}

	}
	else {
		view->RenderTargetTex     = NULL;
		view->PostProcess.PassNum = 0;
	}


    return 0;
}
// =========================================================================
static int luaWorld_EnableGhostsFX(lua_State* state)
{
    Script L(state);

#ifndef TRANSGAMING
	GEngine->World->GhostsFX = L.GetInt(1,0);
#else
    GEngine->pcfWorld->GhostsFX = L.GetInt(1,0);
#endif

    return 0;
}
// =========================================================================
static int luaWorld_EnableDemonFX(lua_State* state)
{
    Script L(state);

#ifndef TRANSGAMING
	if (GEngine->World)
        GEngine->World->DemonFX.Enabled = L.GetBool(1,0);
#else
	if (GEngine->pcfWorld)
        GEngine->pcfWorld->DemonFX.Enabled = L.GetBool(1,0);
#endif
    return 0;
}
// =========================================================================
static int luaWorld_EnableSuperDemonFX(lua_State* state)
{
    Script L(state);
#ifndef TRANSGAMING
	if (GEngine->World)
        GEngine->World->DemonFX.Super = L.GetBool(1,0);
#else
	if (GEngine->pcfWorld)
        GEngine->pcfWorld->DemonFX.Super = L.GetBool(1,0);
#endif
    return 0;
}
// =========================================================================
static int luaWorld_DemonFXParams(lua_State* state)
{
    Script L(state);
#ifndef TRANSGAMING
	GEngine->World->DemonFX.Scale = L.GetFloat(1,1.0f);
    GEngine->World->DemonFX.Bias  = L.GetFloat(2,0.0f);
    GEngine->World->DemonFX.MBlur0 = L.GetFloat(3,0.3f);
    GEngine->World->DemonFX.MBlur1 = L.GetFloat(4,0.7f);
#else
    GEngine->pcfWorld->DemonFX.Scale = L.GetFloat(1,1.0f);
    GEngine->pcfWorld->DemonFX.Bias  = L.GetFloat(2,0.0f);
    GEngine->pcfWorld->DemonFX.MBlur0 = L.GetFloat(3,0.3f);
    GEngine->pcfWorld->DemonFX.MBlur1 = L.GetFloat(4,0.7f);
#endif
    return 0;
}
// =========================================================================
static int luaWorld_BloomFXParams(lua_State* state)
{
    Script L(state);
#ifndef TRANSGAMING
	GEngine->World->BloomFX.LuminanceThreshold	= L.GetFloat(1,0.25F);
    GEngine->World->BloomFX.Multiplier			= L.GetFloat(2,0.0F);
    GEngine->World->BloomFX.OverlayColor		= L.GetInt(3);
	GEngine->World->BloomFX.DimScale			= L.GetFloat(4,0.8F);
#else
    GEngine->pcfWorld->BloomFX.LuminanceThreshold	= L.GetFloat(1,0.25F);
    GEngine->pcfWorld->BloomFX.Multiplier			= L.GetFloat(2,0.0F);
    GEngine->pcfWorld->BloomFX.OverlayColor		= L.GetInt(3);
	GEngine->pcfWorld->BloomFX.DimScale			= L.GetFloat(4,0.8F);
#endif
    return 0;
}
// =========================================================================
static int luaWorld_DemonFXWarp(lua_State* state)
{
    Script L(state);
#ifndef TRANSGAMING
	GEngine->World->DemonFX.Warp = L.GetFloat(1,0.0f);
#else
    GEngine->pcfWorld->DemonFX.Warp = L.GetFloat(1,0.0f);
#endif
    return 0;
}
// =========================================================================
static int luaWorld_SetMaxFPS(lua_State* state)
{
    Script L(state);
    float maxfps = L.GetFloat(1,0);
#ifdef TRANSGAMING
    GEngine->MinSecondsPerFrame = maxfps ? (1.0f / maxfps) : 0.0f;
#else
    GEngine->MinSecondsPerFrame = maxfps?((double)1.0)/maxfps:0.0;
#endif
    return 0;
}
// =========================================================================
static int luaWorld_SetPhysicsFPS(lua_State* state)
{
    Script L(state);
	if( GEngine->Physics )
	{
		int maxfps = L.GetInt(1,45);
		if( maxfps > 120 ) maxfps = 120;
		else if( maxfps < 30 ) maxfps = 30;
		GEngine->Physics->PhysicsFPS = maxfps;
	}
    return 0;
}
// =========================================================================
static int luaWorld_EnableDrawMeshGroup(lua_State* state)
{
    Script L(state);
#ifndef TRANSGAMING
	GEngine->World->MeshesActiveGroupEnableDraw(L.GetInt(1),L.GetBool(2,true));
#else
	GEngine->pcfWorld->MeshesActiveGroupEnableDraw(L.GetInt(1),L.GetBool(2,true));
#endif
    return 0;
}
// =========================================================================

// WARNING: This function never deletes entites! Only removes them from world.

static int luaWorld_SetTimeToDeleteMeshGroup(lua_State* state)
{
    Script L(state);
	float ttd = L.GetFloat(2);

#ifndef TRANSGAMING
	if( ttd != 0.0f )
		GEngine->World->MeshesActiveGroupSetTimeToRemove(L.GetInt(1),ttd,L.GetFloat(3,0.0f) );
	else
		GEngine->World->MeshesActiveGroupRemove( L.GetInt(1) );
#else
	if( ttd != 0.0f )
		GEngine->pcfWorld->MeshesActiveGroupSetTimeToRemove(L.GetInt(1),ttd,L.GetFloat(3,0.0f) );
	else
		GEngine->pcfWorld->MeshesActiveGroupRemove( L.GetInt(1) );
#endif

    return 0;
}
// =========================================================================
static int luaWorld_SetCollisionGroupMeshGroup(lua_State* state)
{
	Script L(state);
#ifndef TRANSGAMING
	GEngine->World->MeshesActiveGroupSetCollisionGroup( L.GetInt(1),L.GetInt(2) );
#else
	GEngine->pcfWorld->MeshesActiveGroupSetCollisionGroup( L.GetInt(1),L.GetInt(2) );
#endif
	return 0;
}
// =========================================================================
static int luaWorld_Release(lua_State* state)
{
	Script L(state);
    bool full = L.GetBool(1,true);
#ifndef TRANSGAMING
	if (full) {
		const char* NewMap = L.GetString(2,NULL);
        GEngine->World->Release(NewMap);
	} else {
        GEngine->World->ReleaseWithoutMap();
	}
#else
	if (full) {
        TRACE("luaWorld_Release(): releasing the lua world\n");
		const char* NewMap = L.GetString(2,NULL);
        GEngine->pcfWorld->Release(NewMap);
	} else {
        TRACE("luaWorld_Release(): releasing the lua world without map\n");
        GEngine->pcfWorld->ReleaseWithoutMap();
	}
#endif

	return 0;
}
// =========================================================================
static int luaWorld_SaveGame(lua_State* state)
{
	Script L(state);
    const char * fname = L.GetString(1);

    FileObject *fo = GFileMan.CreateFileWriter(fname,"wp");
    if (!GEngine->SaveGame(fo))
#ifdef TRANSGAMING
        TRACE("[]::luaWorld_SaveGame(): ERROR-> could not save game to file '%s'\n", fname);
#else
        MessageBox(NULL,"SaveGame failed!!!","Error",MB_TOPMOST);
#endif
	GFileMan.Close(fo);

    return 0;
}
// =========================================================================
static int luaWorld_LoadGame(lua_State* state)
{
    Script L(state);
    const char *fname = L.GetString(1);

    FileObject *fo = GFileMan.CreateFileReader(fname);
    if (!fo || !GEngine->LoadGame(fo))
#ifdef TRANSGAMING
        TRACE("[]::luaWorld_LoadGame(): ERROR-> could not load saved game from '%s'\n", fname);
#else
        MessageBox(NULL,"LoadGame failed!!!","Error",MB_TOPMOST);
#endif
	GFileMan.Close(fo);
    return 0;
}
// =========================================================================
static int luaWorld_SetDrawDynLights(lua_State* state)
{
	Script L(state);
#ifndef TRANSGAMING
	GEngine->World->DrawDynLights = L.GetInt( 1, 0 );
	if (GEngine->GetRenderDevice()->HWClass >= RenderDevice::EHWClassNV30 && GEngine->World->DrawDynLights == 2)
		GEngine->World->HighQualityLights = true;
	else
		GEngine->World->HighQualityLights = false;
#else
	GEngine->pcfWorld->DrawDynLights = L.GetInt( 1, 0 );
	if (GEngine->GetRenderDevice()->HWClass >= RenderDevice::EHWClassNV30 && GEngine->pcfWorld->DrawDynLights == 2)
		GEngine->pcfWorld->HighQualityLights = true;
	else
		GEngine->pcfWorld->HighQualityLights = false;
#endif

	return 0;
}
// =========================================================================
static int luaWorld_GetLastExplodedEntities(lua_State* state)
{
	Script L(state);
    L.NewTable();

#ifndef TRANSGAMING
	for (int i=0; i<GEngine->World->LastExplodedEntities.Size(); i++)
    {
        L.PushInt(i+1);
        L.PushInt(GEngine->World->LastExplodedEntities[i]->GIndex);
        L.SetTable(-3);
    }
#else
    for (int i=0; i<GEngine->pcfWorld->LastExplodedEntities.Size(); i++)
    {
        L.PushInt(i+1);
        L.PushInt(GEngine->pcfWorld->LastExplodedEntities[i]->GIndex);
        L.SetTable(-3);
    }
#endif

    return 1;
}
// =========================================================================
static int luaWorld_SwitchToState(lua_State* state)
{
    Script L(state);
    GEngine->SwitchToState( L.GetInt(1));
    return 0;
}
// =========================================================================
static int luaWorld_DeleteDelayedEntities(lua_State* state)
{
    Script L(state);
#ifndef TRANSGAMING
	GEngine->World->DeleteDelayedEntities();
#else
    GEngine->pcfWorld->DeleteDelayedEntities();
#endif
    return 0;
}
// =========================================================================
static int luaWorld_DeleteDyingEntities(lua_State* state)
{
    Script L(state);

#ifndef TRANSGAMING
	GEngine->World->DeleteDyingEntities();
#else
    GEngine->pcfWorld->DeleteDyingEntities();
#endif

    return 0;
}
// =========================================================================
static int luaWorld_FindEnvironmentAtPoint(lua_State* state)
{
	Script L(state);
#ifndef TRANSGAMING
	if (GEngine->World)
	{
		Vector pt(L.GetFloat(1),L.GetFloat(2),L.GetFloat(3));
		for (int i=0; i<GEngine->World->Environments.Size(); ++i)
		{
			Environment* env = GEngine->World->Environments[i];
			if (env && env->PointTest(pt))
			{
				L.PushInt(env->GIndex);
				return 1;
			}
		}
	}
#else
	if (GEngine->pcfWorld)
	{
		Vector pt(L.GetFloat(1),L.GetFloat(2),L.GetFloat(3));
		for (int i=0; i<GEngine->pcfWorld->Environments.Size(); ++i)
		{
			Environment* env = GEngine->pcfWorld->Environments[i];
			if (env && env->PointTest(pt))
			{
				L.PushInt(env->GIndex);
				return 1;
			}
		}
	}
#endif
	L.PushNil();
	return 1;
}
// =========================================================================
static const luaL_reg world_lib[] = {
  {"LoadMap", luaWorld_LoadMap},
  {"ApplyTweaks", luaWorld_ApplyTweaks},
  {"SetGamePaused", luaWorld_SetGamePaused},
  {"IsGamePaused", luaWorld_IsGamePaused},
  {"SetGameVisible", luaWorld_SetGameVisible},
  {"LineTrace", luaWorld_LineTrace},
  {"MultiplayerExplosion", luaWorld_MultiplayerExplosion},
  {"LineTraceHitPlayerBalls", luaWorld_LineTraceHitPlayerBalls},
  {"LineTraceFixedGeom", luaWorld_LineTraceFixedGeom},
  {"LineTraceFirstHit", luaWorld_LineTraceFirstHit},
  {"HitPhysicObject", luaWorld_HitPhysicObject},
  {"AddEntity", luaWorld_AddEntity},
  {"RemoveEntity", luaWorld_RemoveEntity},
  {"Explosion2", luaWorld_Explosion2},
  {"ExplosionUp", luaWorld_ExplosionUp},
  {"ExplosionParabolic", luaWorld_ExplosionParabolic},
  {"LateVBsBegin", luaWorld_LateVBsBegin},
  {"LateVBsEnd", luaWorld_LateVBsEnd},
  {"SetupFog", luaWorld_SetupFog},
  {"SetupWater", luaWorld_SetupWater},
  {"SetupSky", luaWorld_SetupSky},
  {"LoadSky", luaWorld_LoadSky},
  {"LoadLowQualitySky", luaWorld_LoadLowQualitySky},
  {"SetupSkyLayer", luaWorld_SetupSkyLayer},
  {"InitSky", luaWorld_InitSky},
  {"AmbientColor", luaWorld_AmbientColor},
  {"GetAmbientColor", luaWorld_GetAmbientColor},
  {"EnableDeathZone", luaWorld_EnableDeathZone},
  {"SetFarClipDist", luaWorld_SetFarClipDist},
  {"SetInLoadingScreen", luaWorld_SetInLoadingScreen},
  {"IsPortalEnabled", luaWorld_IsPortalEnabled},
  {"UseSwitchZones", luaWorld_UseSwitchZones},
  {"EnableOcclude",luaWorld_EnableOcclude},
  {"EnablePortal", luaWorld_EnablePortal},
  {"IsAntiPortalEnabled", luaWorld_IsAntiPortalEnabled},
  {"EnableAntiPortal", luaWorld_EnableAntiPortal},
  {"DeleteAntiPortal", luaWorld_DeleteAntiPortal},
  {"CreateEnabledAntiPortalFromClosedConvexMesh", luaWorld_CreateEnabledAntiPortalFromClosedConvexMesh},
  {"IsUnderwater", luaWorld_IsUnderwater},
  {"MakeUnderwater", luaWorld_MakeUnderwater},
  {"SetDirLight", luaWorld_SetDirLight},
  {"UpdateAllEntities", luaWorld_UpdateAllEntities},
  {"CheckStartGlass", luaWorld_CheckStartGlass},
  {"FlashSkyTexture", luaWorld_FlashSkyTexture},
  {"SetDynamicSpecular", luaWorld_SetDynamicSpecular},
  {"SetWorldSpeed", luaWorld_SetWorldSpeed},
  {"FindEntityByName", luaWorld_FindEntityByName},
  {"EnumeratePortals", luaWorld_EnumeratePortals},
  {"EnumerateAntiPortals", luaWorld_EnumerateAntiPortals},
  {"EnableGhostsFX", luaWorld_EnableGhostsFX},
  {"EnableDemonFX", luaWorld_EnableDemonFX},
  {"EnableSuperDemonFX", luaWorld_EnableSuperDemonFX},
  {"SetRenderTarget", luaWorld_SetRenderTarget},
  {"DemonFXParams", luaWorld_DemonFXParams},
  {"BloomFXParams", luaWorld_BloomFXParams},
  {"DemonFXWarp", luaWorld_DemonFXWarp},
  {"GetFrameCounter", luaWorld_GetFrameCounter},
  {"AdvanceFrameCounter", luaWorld_AdvanceFrameCounter},
  {"SetMaxFPS", luaWorld_SetMaxFPS},
  {"SetPhysicsFPS", luaWorld_SetPhysicsFPS},
  {"EnableDrawMeshGroup", luaWorld_EnableDrawMeshGroup},
  {"SetTimeToDeleteMeshGroup", luaWorld_SetTimeToDeleteMeshGroup},
  {"SetCollisionGroupMeshGroup", luaWorld_SetCollisionGroupMeshGroup},
  {"Release", luaWorld_Release},
  {"GetFileName", luaWorld_GetFileName},
  {"Init", luaWorld_Init},
  {"DemoInit", luaWorld_DemoInit},
  {"SaveGame", luaWorld_SaveGame},
  {"LoadGame", luaWorld_LoadGame},
  {"SetDrawDynLights", luaWorld_SetDrawDynLights},
  {"GetLastExplodedEntities", luaWorld_GetLastExplodedEntities},
  {"SwitchToState", luaWorld_SwitchToState},
  {"EnableOccludeAntiPortals", luaWorld_EnableOccludeAntiPortals},
  {"DeleteDelayedEntities", luaWorld_DeleteDelayedEntities},
  {"DeleteDyingEntities", luaWorld_DeleteDyingEntities},
  {"FindEnvironmentAtPoint", luaWorld_FindEnvironmentAtPoint},
  {"GetEntityList", luaWorld_GetEntityList},
  {NULL, NULL}
};

#ifdef CD_CHECK_ON

// =========================================================================
// WARNING: This is a fake CD-Check method
static int luaCdCheck3(lua_State* state)
{
    TRACE("[engineToLua.cpp]::luaCdCheck3(): checking CD label\n");

    char dbits[110];
	DWORD d = GetLogicalDriveStrings( 110, dbits );
	Script L(state);
	if( !d )
	{
		L.PushNil();
		return 1;
	}

	char label[13];
	label[2]  = PK_CD_LABEL_02;
	label[8]  = PK_CD_LABEL_08;
	DWORD mf, sf, sno;
	label[9]  = PK_CD_LABEL_09;
	label[7]  = PK_CD_LABEL_07;
	label[4]  = PK_CD_LABEL_04;
	int i = 0;
	label[11] = PK_CD_LABEL_11;
	label[5]  = PK_CD_LABEL_05;
	label[1]  = PK_CD_LABEL_01;

	while( *(dbits+i) )
	{
		char vol[256];
		char drive[4];
		vol[0] = '\0';

		label[6]  = PK_CD_LABEL_06;
		label[0]  = PK_CD_LABEL_00;

		strncpy(drive,dbits+i,4);

		label[10] = PK_CD_LABEL_10;
		label[3]  = PK_CD_LABEL_03;

		if( GetDriveType( drive ) == DRIVE_CDROM )
		{
			GetVolumeInformation( drive, vol, sizeof(vol), &sno, &mf, &sf, NULL, NULL );
			if( ( vol[0] == 'P' ) &&
				( vol[1] == 'K' ) &&
				( vol[2] == '_' ) &&
				( ( ( vol[3] == '1' ) && ( vol[4] == 0 ) ) ||
				  ( ( vol[5] == '_' ) && ( vol[6] == '1' ) && ( vol[7] == 0 ) ) ) )
			{
				L.PushString( dbits+i );
				return 1;
			}

			if( ( vol[0] == 'P' ) &&
				( vol[1] == 'K' ) &&
				( vol[2] == '_' ) &&
				( vol[3] == 'P' ) &&
				( vol[4] == 'L' ) &&
				( vol[5] == '_' ) &&
				( vol[6] == '1' ) &&
				( vol[7] == 0 ) )
			{
				L.PushString( dbits+i );
				return 1;
			}
/*
			if( stricmp( label, vol ) == 0 )
			{
				L.PushString( dbits+i );
				return 1;
			}
*/
		}
		i+= 4;
	}
	L.PushNil();
	return 1;
}

#endif

// =========================================================================
#ifndef TRANSGAMING
static int luaCDGetLabel( lua_State *state )
{
    TRACE("[engineToLua.cpp]::luaCDGetLabel(): checking CD label\n");

	char dbits[112];
	DWORD d = GetLogicalDriveStrings( 112, dbits );
	Script L(state);
	if( !d )
	{
		L.PushNil();
		return 1;
	}

	DWORD mf, sf, sno;
	int i = 0;
	while( *(dbits+i) )
	{
		char vol[256];
		char drive[4];
		vol[0] = '\0';

		strncpy(drive,dbits+i,4);

		if( GetDriveType( drive ) == DRIVE_CDROM )
		{
			GetVolumeInformation( drive, vol, sizeof(vol), &sno, &mf, &sf, NULL, NULL );
			if( ( vol[0] == 'P' ) &&
				( vol[1] == 'K' ) &&
				( vol[2] == '_' ) &&
				( ( ( vol[3] == '1' ) && ( vol[4] == 0 ) ) ||
				  ( ( vol[5] == '_' ) && ( vol[6] == '1' ) && ( vol[7] == 0 ) ) ) )
			{
				L.PushString( vol );
				return 1;
			}

			if( ( vol[0] == 'P' ) &&
				( vol[1] == 'K' ) &&
				( vol[2] == '_' ) &&
				( vol[3] == 'P' ) &&
				( vol[4] == 'L' ) &&
				( vol[5] == '_' ) &&
				( vol[6] == '1' ) &&
				( vol[7] == 0 ) )
			{
				L.PushString( vol );
				return 1;
			}
		}
		i+= 4;
	}

	L.PushNil();
	return 1;
}
#endif

// =========================================================================
// NETWORK
// =========================================================================
static int luaNet_MsgCreate(lua_State* state)
{
	Script L(state);
    m_setTag("ScriptNet");
	UdpMessage *msg = new UdpMessage( 65536 );	// big one, just in case.
	L.PushLightUD(msg);
	return 1;
}
// =========================================================================
static int luaNet_MsgWriteVar(lua_State* state)
{
	Script L(state);

	UdpMessage *msg = (UdpMessage *) L.GetLightUD(1);
	if (msg)
	{
		const char *type = L.GetString(2);
		switch (type[0])
		{
		case 'e': // - entity (by index)
			{
#ifndef TRANSGAMING
				Entity *entity = GEngine->World->GetEntityByIndex(L.GetInt(3,0));
#else
                Entity *entity = GEngine->pcfWorld->GetEntityByIndex(L.GetInt(3,0));
#endif
                msg->WriteEntityPtr(entity);
			}
			break;
		case 's': // - string with '\0'
			{
				const char * var =  L.GetString(3);
				msg->WriteString(var);
			}
			break;
		case 'z': // - string with trailing '\0x0A'
			{
				const char * var =  L.GetString(3);
				msg->WriteStringNL(var);
			}
			break;
		case 'c': // - char
			{
				const char * var =  L.GetString(3);
				msg->WriteChar(var[0]);
			}
			break;
		case 'b': // - byte
			{
				BYTE var =  L.GetInt(3);
				msg->WriteByte(var);
			}
			break;
        case 'B': // - BOOL
			{
				bool var =  L.GetBool(3,false);
				msg->WriteBool(var);
			}
			break;
		case 'u': // - unsigned short
			{
				unsigned short var =  L.GetInt(3);
				msg->WriteUShort(var);
			}
			break;
		case 'i': // - int
			{
				int var =  L.GetInt(3);
				msg->WriteLong(var);
			}
			break;
		case 'f': // - float
			{
				float var =  L.GetFloat(3,0);
				msg->WriteFloat(var);
			}
			break;
		case 't': // - current time
			{
				DWORD start = 0;
				DWORD playTime = 0;
				msg->WriteULong( start?playTime:0L );
			}
			break;
		}
	}

	return 0;
}
// =========================================================================
static int luaNet_MsgReadVar(lua_State* state)
{
	Script L(state);

	UdpMessage *msg = (UdpMessage *) L.GetLightUD(1);
	if (msg)
	{
		const char *type = L.GetString(2);
		switch (type[0])
		{
		case 'e': // - entity (by index)
			{
                Entity * ptr = msg->ReadEntityPtr();
                if (ptr)
                    L.PushInt( ptr->GIndex);
                else
                    L.PushNil();
			}
			break;
		case 's': // - string with '\0'
			{
				String str = msg->ReadString();
				L.PushString(str.AsChar());
			}
			break;
		case 'z': // - string with trailing '\0x0A'
			{
				String str = msg->ReadStringNL();
				L.PushString(str.AsChar());
			}
			break;
		case 'c': // - char
			{
				char var[] = "\0\0";
				var[0] = msg->ReadChar();
				L.PushString(var);
			}
			break;
		case 'b': // - byte
			{
				BYTE var = msg->ReadByte();
				L.PushInt(var);
			}
			break;
        case 'B': // - BOOL
            {
                bool var = msg->ReadBool();
                L.PushBool(var);
            }
            break;
		case 'u': // - unsigned short
			{
				unsigned short var = msg->ReadUShort();
				L.PushInt(var);
			}
			break;
		case 'i': // - int
			{
				int var = msg->ReadLong();
				L.PushInt(var);
			}
			break;
		case 'f': // - float
			{
				float var = msg->ReadFloat();
				L.PushFloat(var);
			}
			break;
		case 't': // - current time
			{
				msg->ReadULong();
				L.PushNil();
			}
			break;
		}
	}
	return 1;
}
// =========================================================================
static int luaNet_MsgSend(lua_State* state)
{
	static int DeliveryReceipts = 0;
	Script L(state);
	UdpMessage *msg = (UdpMessage *) L.GetLightUD(1);
	bool reliable = L.GetBool(2,true);
	bool oneClientOnly = L.GetBool(3,false);
	unsigned short clientIndex = (unsigned short)L.GetInt(4,SERVER_ID);
	if( msg && GEngine->Net )
	{
#ifdef UDP_END_WRITING_CRASH_LOG
		EndWritingCrashNumber = 23;
#endif
		msg->EndWriting();
		GEngine->Net->SendScriptMessage( msg, reliable, oneClientOnly, clientIndex );
		int dr = L.GetBool(5,false)?DeliveryReceipts++:-1;
		if( dr != -1 )
		{
			L.PushInt(dr);
			return 1;
		}
	}
	return 0;
}
// =========================================================================
static int luaNet_UpdatePlayerData(lua_State* state)
{
//	if( !GEngine->Net ) return 0;
	Script L(state);
#ifndef TRANSGAMING
//	Entity *e = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1));
#else
//	Entity *e = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
#endif
//	NetworkClient* nc = GEngine->Net->GetNetworkClientForPlayer( e );
//	if( nc )
//	{
//		nc->Score = L.GetInt(2,0);
//		nc->Frags = L.GetInt(3,0);
//		nc->Deaths = L.GetInt(4,0);
//	}
	return 0;
}
// =========================================================================
static int luaNet_GetClientPing(lua_State* state)
{
	Script L(state);
	L.PushInt(GEngine->Net?(unsigned int)floor(GEngine->Net->Ping( (unsigned short)L.GetInt(1,SERVER_ID) )):0);
	return 1;
}
// =========================================================================
static int luaNet_ClientPingReset(lua_State* state)
{
	Script L(state);
	if( GEngine->Net )
		GEngine->Net->PingReset( (unsigned short)L.GetInt(1,SERVER_ID) );
	return 0;
}
// =========================================================================
static int luaNet_IsSpectator(lua_State* state)
{
	Script L(state);
	L.PushBool(GEngine->Net?GEngine->Net->IsSpectator( (unsigned short)L.GetInt(1,SERVER_ID) ):false);
	return 1;
}
// =========================================================================
static int luaNet_SetSpectator(lua_State* state)
{
	Script L(state);
	if( !GEngine->Net )
		return 0;
	GEngine->Net->SetSpectator( (unsigned short)L.GetInt(1,SERVER_ID), L.GetInt(2) );
	return 0;
}
// =========================================================================
static int luaNet_GetPushLatency(lua_State* state)
{
	Script L(state);
	L.PushInt(0);
	return 1;
}
// =========================================================================
static int luaNet_SetPushLatency(lua_State* state)
{
	return 0;
}
// =========================================================================
static int luaNet_GetSimulatedLatency(lua_State* state)
{
	Script L(state);
	L.PushInt(0);
	return 1;
}
// =========================================================================
static int luaNet_SetSimulatedLatency(lua_State* state)
{
	return 0;
}
// =========================================================================
static int luaNet_CanConnectNewClient(lua_State* state)
{
	Script L(state);
	bool can = false;
	if( GEngine->Net && GEngine->Net->IsServer() )
        can = GEngine->Net->CanConnectNewClient( L.GetInt(1) );
	L.PushBool( can );
	return 1;
}
// =========================================================================
static int luaNet_SendEmptyPacket(lua_State* state)
{
	Script L(state);
	return 0;
}
// =========================================================================
static int luaNet_GetClientPacketLoss(lua_State* state)
{
	Script L(state);
	L.PushInt(GEngine->Net?(unsigned int)floor(GEngine->Net->PacketLoss( (unsigned short)L.GetInt(1,SERVER_ID) )*100.0):0);
	return 1;
}
// =========================================================================
static int luaNet_WriteMesgToFile( lua_State* state )
{
	Script L(state);
	UdpMessage *msg = (UdpMessage *) L.GetLightUD(1);
	if( !msg ) { L.PushBool( false ); return 1; };
	const char *filename = L.GetString(2,"menustate.ini");


	if( msg->WriteToFile( filename ) )
		L.PushBool( true );
	else
		L.PushBool( false );
	delete msg;
	return 1;
}
// =========================================================================
static int luaNet_ReadMesgFromFile( lua_State* state )
{
	Script      L(state);
	const char *filename = L.GetString(1,"menustate.ini");
    m_setTag("ScriptNet");
	UdpMessage *msg = new UdpMessage();


	if( msg->ReadFromFile( filename ) )
		L.PushLightUD( msg );
	else
	{
		delete msg;
		L.PushNil();
	}
	return 1;
}
// =========================================================================
static int luaNet_GetClientID( lua_State* state )
{
    Script L(state);
    if (GEngine->Net)
        L.PushInt(GEngine->Net->GetClientID());
    else
        L.PushInt(-1);
    return 1;
}
// =========================================================================
static int luaNet_LoadMapOnServer( lua_State* state )
{
    Script L(state);
    GEngine->Net->LoadNewMap(L.GetString(1));
    return 0;
}
// =========================================================================
static int luaNet_Disconnect( lua_State* state )
{
	Script L(state);
	if( !GEngine->Net || GEngine->Net->IsServer() ) return 0;
	GEngine->Net->NetworkClose();
	return 0;
}
// =========================================================================
static int luaNet_DisconnectClient( lua_State* state )
{
	Script L(state);
	if( !GEngine->Net || !GEngine->Net->IsServer() ) return 0;
	GEngine->Net->DisconnectClient( L.GetInt(1) );
	return 0;
}
// =========================================================================
static int luaNet_BanClient( lua_State* state )
{
	Script L(state);
	if( !GEngine->Net || !GEngine->Net->IsServer() ) return 0;
	GEngine->Net->BanClient( L.GetInt(1) );
	return 0;
}
// =========================================================================
static int luaNet_ClientConnected( lua_State* state )
{
	Script L(state);
	L.PushBool( ( GEngine->Net && GEngine->Net->IsClient() )?true:false );
	return 0;
}
// =========================================================================
static int luaNet_RecordPlayersInfo( lua_State* state )
{
    return 0;
}
// =========================================================================
static int luaNet_IsPlayingRecording( lua_State* state )
{
	Script L(state);
	L.PushBool( false );
    return 1;
}
// =========================================================================
static int luaNet_DemoPlay( lua_State* state )
{
    return 0;
}
// =========================================================================
static int luaNet_DemoRecord( lua_State* state )
{
    return 0;
}
// =========================================================================
static int luaNet_DemoStop( lua_State* state )
{
    return 0;
}
// =========================================================================
static int luaNet_PredictionOn( lua_State* state )
{
    Script L(state);
	L.PushBool(true);
    return 1;
}
// =========================================================================
static int luaNet_UpdateFramerateLock( lua_State* state )
{
    return 0;
}
// =========================================================================
static int luaNet_SetStatsShow( lua_State* state )
{
    Script L(state);
	View * view = GEngine->Video->GetActiveView();
	bool show = L.GetBool(1,false);
	if( show )
		view->HUDMode |= View::EModeHUD_ShowNetstat;
	else
		view->HUDMode &= ~View::EModeHUD_ShowNetstat;
    return 0;
}
// =========================================================================
static int luaNet_SetStatsNrToAvg( lua_State* state )
{
    Script L(state);
	if( GEngine->Net )
		GEngine->Net->SetStatsNumberToAverage( L.GetInt(1,1) );
    return 0;
}
// =========================================================================
static int luaNet_SetStatsUpdateDelay( lua_State* state )
{
    Script L(state);
	if( GEngine->Net )
	{
		int delay = L.GetInt( 1, 1000 );
		if( ( delay < 1 ) || ( delay > 65535 ) )
			return 0;
		GEngine->Net->SetStatsUpdateDelay( delay );
		GScript.DoString( "Cfg.NetcodeStatsUpdateDelay = %d", delay );
	}
    return 0;
}
// =========================================================================
static int luaNet_SetServerFramerate( lua_State* state )
{
    Script L(state);
	if( GEngine->Net )
	{
		int serverFramerate = L.GetInt( 1, 25 );
		if( serverFramerate > 0 )
		{
			GEngine->Net->SetServerFramerate( serverFramerate );
			GScript.DoString( "Cfg.NetcodeServerFramerate = %d", serverFramerate );
		}
	}
    return 0;
}
// =========================================================================
static int luaNet_SetClientBandwidth( lua_State* state )
{
    Script L(state);
	if( GEngine->Net && GEngine->Net->IsClient() )
	{
		int clientBandwidth = L.GetInt( 1, -1 );
		GEngine->Net->SetBandwidthControl( clientBandwidth );
		GScript.DoString( "Cfg.NetcodeClientMaxBytesPerSecond = %d", clientBandwidth );
	}
    return 0;
}
// =========================================================================
static int luaNet_GetClientBandwidth( lua_State* state )
{
    Script L(state);
	L.PushInt( ( GEngine->Net && GEngine->Net->IsClient() )?GEngine->Net->GetBandwidthControl():-1 );
    return 1;
}
// =========================================================================
static int luaNet_SetupGameSpyVariable( lua_State* state )
{
#ifndef TRANSGAMING_NO_GAMESPY
	Script L(state);
	if( GEngine->Net && GEngine->Net->IsServer() )
		GEngine->Net->GameSpyInfo.RegisterCustomKey( L.GetString(1), L.GetBool(2), L.GetString(3,"") );
#endif
    return 0;
}
// =========================================================================
static int luaNet_SetGameSpyVariable( lua_State* state )
{
#ifndef TRANSGAMING_NO_GAMESPY
	Script L(state);
	if( GEngine->Net && GEngine->Net->IsServer() )
		GEngine->Net->GameSpyInfo.SetCustomKey( L.GetString(1), L.GetString(2,"") );
#endif
    return 0;
}
// =========================================================================
static int luaNet_GetLastFrameLatency( lua_State* state )
{
	Script L(state);
	if( GEngine->Net )
		L.PushInt( GEngine->Net->GetLastFrameLatency() );
	return 1;
}
// =========================================================================
static int luaNet_SetEnemyPredictionInterpolation( lua_State* state )
{
	Script L(state);
	if( GEngine->Physics )
		GEngine->Physics->MPPredictionInterpolation = L.GetBool( 1, false );
	return 0;
}
// =========================================================================
static int luaNet_SendVariable( lua_State* state )
{
	Script L(state);
	if( GEngine->Net )
	{
		String name = L.GetString(2);
		String value = L.GetString(3);
		GEngine->Net->TransferVariable( L.GetInt(1), name, value );
	}
	return 0;
}
// =========================================================================
static const luaL_reg net_lib[] = {
	{"MsgCreate", luaNet_MsgCreate},
	{"MsgWriteVar", luaNet_MsgWriteVar},
	{"MsgReadVar", luaNet_MsgReadVar},
	{"MsgSend", luaNet_MsgSend},
	{"UpdatePlayerData", luaNet_UpdatePlayerData},
	{"GetClientPing", luaNet_GetClientPing},
	{"GetClientPacketLoss", luaNet_GetClientPacketLoss},
	{"ClientPingReset", luaNet_ClientPingReset},
	{"MsgWriteToFile", luaNet_WriteMesgToFile},
	{"MsgReadFromFile", luaNet_ReadMesgFromFile},
    {"GetClientID", luaNet_GetClientID},
    {"LoadMapOnServer", luaNet_LoadMapOnServer},
    {"SendEmptyPacket", luaNet_SendEmptyPacket},
	{"IsSpectator", luaNet_IsSpectator},
	{"SetSpectator", luaNet_SetSpectator},
	{"GetPushLatency", luaNet_GetPushLatency},
	{"SetPushLatency", luaNet_SetPushLatency},
	{"GetSimulatedLatency", luaNet_GetSimulatedLatency},
	{"SetSimulatedLatency", luaNet_SetSimulatedLatency},
	{"CanConnectNewClient", luaNet_CanConnectNewClient},
	{"Disconnect", luaNet_Disconnect},
	{"DisconnectClient", luaNet_DisconnectClient},
	{"BanClient", luaNet_BanClient},
	{"ClientConnected", luaNet_ClientConnected},
	{"PredictionOn", luaNet_PredictionOn},
	{"UpdateFramerateLock", luaNet_UpdateFramerateLock },
	{"GetLastFrameLatency", luaNet_GetLastFrameLatency },

	{"SetStatsShow", luaNet_SetStatsShow },
	{"SetStatsNrToAvg", luaNet_SetStatsNrToAvg },
	{"SetStatsUpdateDelay", luaNet_SetStatsUpdateDelay },
	{"SetServerFramerate", luaNet_SetServerFramerate },
	{"SetClientBandwidth", luaNet_SetClientBandwidth },
	{"GetClientBandwidth", luaNet_GetClientBandwidth },

	{"SetupGameSpyVariable", luaNet_SetupGameSpyVariable },
	{"SetGameSpyVariable", luaNet_SetGameSpyVariable },
	{"SetEnemyPredictionInterpolation", luaNet_SetEnemyPredictionInterpolation },
	{"SendVariable", luaNet_SendVariable },

	{"RecordPlayersInfo", luaNet_RecordPlayersInfo},
	{"IsPlayingRecording", luaNet_IsPlayingRecording},
	{"DemoPlay", luaNet_DemoPlay},
	{"DemoRecord", luaNet_DemoRecord},
	{"DemoStop", luaNet_DemoStop},
	{NULL, NULL}
};
// =========================================================================
// LIGHT
// =========================================================================
static int luaLight_Setup(lua_State* state)
{
	Script L(state);
#ifndef TRANSGAMING
	Light *l = (Light *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeLight);
#else
    Light *l = (Light *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeLight);
#endif
	if (l)
	{
		l->SetType(L.GetInt(2));
		l->Color = L.GetInt(3);
		l->SetDirection(Vector(L.GetFloat(4),L.GetFloat(5),L.GetFloat(6)));
		l->UpdateProj();
		l->Intensity = L.GetFloat(7);
	}
	return 0;
}
// =========================================================================
static int luaLight_SetFalloff(lua_State* state)
{
	Script L(state);
#ifndef TRANSGAMING
	Light *l = (Light *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeLight);
#else
    Light *l = (Light *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeLight);
#endif
	if (l)
	{
		l->SetFalloff(L.GetFloat(2));
		l->SetRadius(L.GetFloat(3));
		l->SetConeAngleCos(cosf(FDeg2Rad(L.GetFloat(4))));
		l->FalloffAngleCos = cosf(0.8*FDeg2Rad(L.GetFloat(4)));
	}
	return 0;
}
// =========================================================================
static int luaLight_SetIntensity(lua_State* state)
{
	Script L(state);
#ifndef TRANSGAMING
	Light *l = (Light *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeLight);
#else
    Light *l = (Light *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeLight);
#endif
	if (l)
		l->Intensity = L.GetFloat(2);
	return 0;
}
// =========================================================================
static int luaLight_SetDynamicFlag(lua_State* state)
{
	Script L(state);
#ifndef TRANSGAMING
	Light *l = (Light *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeLight);
#else
    Light *l = (Light *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeLight);
#endif
    if (!l) return 0;

	bool b = L.GetBool(2,true);
	l->EnableDynamic(b);
	return 0;
}
// =========================================================================
static int luaLight_SetFakeSpecularFlag(lua_State* state)
{
	Script L(state);
#ifndef TRANSGAMING
	Light *l = (Light *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeLight);
#else
    Light *l = (Light *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeLight);
#endif
    if (!l) return 0;

	bool b = L.GetBool(2,true);
	l->EnableFakeSpecular(b);
	return 0;
}
// =========================================================================
static int luaLight_SetLitParentFlag(lua_State* state)
{
	Script L(state);
#ifndef TRANSGAMING
	Light *l = (Light *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeLight);
#else
    Light *l = (Light *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeLight);
#endif
    if (!l) return 0;

	if (L.GetBool(2,true))
		l->Flags |= Light::ELightLitParent;
	else
		l->Flags &= ~Light::ELightLitParent;
	return 0;
}
// =========================================================================
static int luaLight_SetProjector(lua_State* state)
{
	Script L(state);
#ifndef TRANSGAMING
	Light *l = (Light *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeLight);
#else
    Light *l = (Light *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeLight);
#endif
    if (!l) return 0;

	const char *s = L.GetString(2);


    if (s)
	    l->SetProjectorTexture(s);

	return 0;
}
// =========================================================================
static int luaLight_SetImportant(lua_State* state)
{
	Script L(state);
#ifndef TRANSGAMING
	Light *l = (Light *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeLight);
#else
	Light *l = (Light *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeLight);
#endif
	if (!l) return 0;

	l->ImportantDynamic = L.GetBool(2,true);
	return 0;
}
// =========================================================================
static const luaL_reg light_lib[] = {
  {"Setup", luaLight_Setup},
  {"SetFalloff", luaLight_SetFalloff},
  {"SetIntensity", luaLight_SetIntensity},
  {"SetDynamicFlag", luaLight_SetDynamicFlag},
  {"SetFakeSpecularFlag", luaLight_SetFakeSpecularFlag},
  {"SetLitParentFlag", luaLight_SetLitParentFlag},
  {"SetProjector", luaLight_SetProjector},
  {"SetImportant", luaLight_SetImportant},
  {NULL, NULL}
};
// =========================================================================
static int luaBillboard_SetupCorona(lua_State* state)
{
	Script L(state);
#ifndef TRANSGAMING
	Billboard *b = (Billboard *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeBillboard);
#else
    Billboard *b = (Billboard *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeBillboard);
#endif
	if (b)
	{
		b->AlphaMax = L.GetFloat(2);
		b->FadeInTime = L.GetFloat(3);
		b->FadeOutTime = L.GetFloat(4);
		b->MinRadius = L.GetFloat(5);
		b->MinDistance = L.GetFloat(6);
		b->MaxRadius = L.GetFloat(7);
		b->MaxDistance = L.GetFloat(8);
		b->OffDistance = L.GetFloat(9);
		b->TraceMargin = L.GetFloat(10);
		b->SetTexture(L.GetString(11), L.GetBool(15) ? Texture::ETexNoLOD : 0);
		b->Color = L.GetInt(12);

		int bmode = L.GetInt(13);
		switch (bmode)
		{
			case 1:  b->BlendMode = RenderState::EBlendAlpha;       break;
			case 2:  b->BlendMode = RenderState::EBlendAdd;         break;
			case 3:  b->BlendMode = RenderState::EBlendFilter;      break;
			case 4:  b->BlendMode = RenderState::EBlendTranslucent; break;
			default: b->BlendMode = 0;
		}

		bool billboard = L.GetBool(14,false);

		b->SetCorona(!billboard);

		b->Sprite.Size = b->MaxRadius;
		b->UpdateBBox();
	}
	return 0;
}
// =========================================================================
static const luaL_reg billboard_lib[] = {
	{"SetupCorona", luaBillboard_SetupCorona},
	{NULL, NULL}
};
// =========================================================================
// RENDERER
// =========================================================================
#ifndef TRANSGAMING
static int luaRender3D_ScreenTo3D(lua_State* state)
{
	Script L(state);
	float x = L.GetFloat(1);
	float y = L.GetFloat(2);
	float z = L.GetFloat(3,0.5f);

	Vector v;
	View * view = GEngine->Video->GetActiveView();
	Viewport *pViewport = &view->LocalViewport;
	v.X =  ( ( ( 2.0f * x ) / pViewport->Width  ) - 1 );
	v.Y = -( ( ( 2.0f * y ) / pViewport->Height ) - 1 );
	v.Z =  z;

	Matrix CameraToScreen = pViewport->WorldToScreen.Inverse();
	v = CameraToScreen.Transform(v);

	L.PushFloat(v.X);
	L.PushFloat(v.Y);
	L.PushFloat(v.Z);
	return 3;
}
#endif
// =========================================================================
#ifdef TRANSGAMING_USE_EDITOR
static int luaRender3D_DistToCamera(lua_State* state)
{
	Script L(state);
	Vector v2(L.GetFloat(1),L.GetFloat(2),L.GetFloat(3));
	View * view = GEngine->Video->GetActiveView();
	Vector v1 = view->mCamera.GetPosition();

	float distance =  (v1 - v2).Magnitude();
	L.PushFloat(distance);
	return 1;
}
#endif
// =========================================================================
Vector ClosestPointOnLine(Vector p, Vector a, Vector b)
{
	Vector v1 = p - a;
	Vector v2 = b - a;
	v2.Normalize();

#ifdef TRANSGAMING
    //**// NOTE: this could be reimplemented to avoid the v2.Normalize() call above as well
	float d = (a - b).MagnitudeSquared();
	float t = v2.Dot(v1);

	if (t <= 0)
		return a;

	if (t * t >= d)
		return b;

#else
	float d = (a - b).Magnitude();
	float t = v2.Dot(v1);

	if (t <= 0)
		return a;

	if (t >= d)
		return b;
#endif


	Vector v3 = v2 * t;
	return a + v3;
}
// =========================================================================
#ifdef TRANSGAMING_PS2
static int luaRender3D_polygonMode(lua_State *state){
    Script L(state);

#ifdef TRANSGAMING_PS2
    const char *mode = L.GetString(1, "Fill");

    if (!strcasecmp(mode, "fill"))
        GEngine->Video->RenderDev->SetPolygonMode(RenderState::EFillSolid);

    else if (!strcasecmp(mode, "line"))
        GEngine->Video->RenderDev->SetPolygonMode(RenderState::EFillLines);

    else if (!strcasecmp(mode, "point"))
        GEngine->Video->RenderDev->SetPolygonMode(RenderState::EFillPoints);
#endif

    return 0;
}
#endif
// =========================================================================
float DistanceToLine(Vector& p, Vector& a, Vector& b)
{
	Vector v = ClosestPointOnLine(p,a,b);
	float distance =  (v - p).Magnitude();
	return  distance;
}
// =========================================================================
static int luaRender3D_DistToLine(lua_State* state)
{
	Script L(state);
	Vector P;
    Vector A,B;

	P.X = L.GetFloat(1); P.Y = L.GetFloat(2); P.Z = L.GetFloat(3);
	A.X = L.GetFloat(4); A.Y = L.GetFloat(5); A.Z = L.GetFloat(6);
	B.X = L.GetFloat(7); B.Y = L.GetFloat(8); B.Z = L.GetFloat(9);

	L.PushFloat(DistanceToLine(P,A,B));
	return 1;
}
// =========================================================================
#ifndef TRANSGAMING
static int luaRender3D_ClosestPointOnLine(lua_State* state)
{
    Script L(state);

    Vector P;
    Vector A,B;

    P.X = L.GetFloat(1); P.Y = L.GetFloat(2); P.Z = L.GetFloat(3);
    A.X = L.GetFloat(4); A.Y = L.GetFloat(5); A.Z = L.GetFloat(6);
    B.X = L.GetFloat(7); B.Y = L.GetFloat(8); B.Z = L.GetFloat(9);

    Vector v = ClosestPointOnLine(P,A,B);
    L.PushFloat(v.X);
    L.PushFloat(v.Y);
    L.PushFloat(v.Z);
    return 3;
}
#endif
// =========================================================================
static int luaRender3D_DistToLine2D(lua_State* state)
{
	Script L(state);
	Vector P;
	Vector A,B;

	P.X = L.GetFloat(1); P.Y = L.GetFloat(2); P.Z = 0;
	A.X = L.GetFloat(3); A.Y = L.GetFloat(4); A.Z = 0;
	B.X = L.GetFloat(5); B.Y = L.GetFloat(6); B.Z = 0;

	L.PushFloat(DistanceToLine(P,A,B));
	return 1;
}
// =========================================================================
#ifndef TRANSGAMING
static int luaRender3D_ComputeSZ(lua_State* state)
{
	Script L(state);
	Vector v(L.GetFloat(1),L.GetFloat(2),L.GetFloat(3));
	View * view = GEngine->Video->GetActiveView();
	v = view->LocalViewport.WorldToScreen.Transform(v);
	L.PushFloat(v.Z);
	return 1;
}
#endif
// =========================================================================
#ifndef TRANSGAMING
static int luaRender3D_VectorToScreen(lua_State* state)
{
	Script L(state);
	Vector v(L.GetFloat(1),L.GetFloat(2),L.GetFloat(3));
	View * view = GEngine->Video->GetActiveView();
	Viewport &pViewport = view->LocalViewport;
	v = pViewport.WorldToScreen.Transform(v);
	v.X =  (v.X + 1) * pViewport.Width /2;
	v.Y =  (-v.Y + 1) * pViewport.Height /2;

	L.PushFloat(v.X);
	L.PushFloat(v.Y);
	L.PushFloat(v.Z);
	return 3;
}
#endif
// =========================================================================
static int luaRender3D_RGB(lua_State* state)
{
	Script L(state);
#ifdef	TRANSGAMING
	L.PushInt(Color_TG(L.GetInt(1),L.GetInt(2),L.GetInt(3),255).dwVal);
#else
	L.PushInt(Color(L.GetInt(1),L.GetInt(2),L.GetInt(3),255).dwVal);
#endif
	return 1;
}
// =========================================================================
static int luaRender3D_RGBA(lua_State* state)
{
	Script L(state);
#ifdef	TRANSGAMING
    Color_TG color(L.GetInt(1),L.GetInt(2),L.GetInt(3),L.GetInt(4));
	L.PushInt(color.dwVal);
#else
	L.PushInt(Color(L.GetInt(1),L.GetInt(2),L.GetInt(3),L.GetInt(4)).dwVal);
#endif
	return 1;
}
// =========================================================================
#ifdef TRANSGAMING_USE_EDITOR
static int luaRender3D_RenderLine(lua_State* state)
{
    if (!GEngine->Video) return 0;
	Script L(state);
	Vector v1(L.GetFloat(1),L.GetFloat(2),L.GetFloat(3));
	Vector v2(L.GetFloat(4),L.GetFloat(5),L.GetFloat(6));
	bool ztest = L.GetBool(8,true);

	View * view = GEngine->Video->GetActiveView();
	Viewport &pViewport = view->LocalViewport;

	RenderDevice *dev = GEngine->Video->RenderDev;
    dev->SetScreenTransform(&pViewport.CameraToScreen);
    dev->SetCameraTransform(&pViewport.WorldToCamera);
    dev->SetObjectTransform(GIdentityMatrix);

	RenderState st;
	st.SetDepthTest(ztest);
	st.SetDepthWrite(ztest);
	st.SetAlphaTest(false);
	st.SetCullMode(RenderState::ECullNone);
	st.SetBlendMode(RenderState::EBlendNone);
	dev->SetState(st);


	GEngine->Video->LineBuf.DrawLine(v1,v2,L.GetInt(7),ztest);
	return 0;
}
#endif
// =========================================================================
#ifdef TRANSGAMING_USE_EDITOR
static int luaRender3D_RenderBox(lua_State* state)
{
    if (!GEngine->Video) return 0;
	Script L(state);
	BoundingBox bb;
	bb.Min = Vector(L.GetFloat(1),L.GetFloat(2),L.GetFloat(3));
	bb.Max = Vector(L.GetFloat(4),L.GetFloat(5),L.GetFloat(6));
	DWORD color = L.GetInt(7);
	bool ztest = L.GetBool(8,true);

	View * view = GEngine->Video->GetActiveView();
	Viewport &pViewport = view->LocalViewport;
	RenderDevice *dev = GEngine->Video->RenderDev;
    dev->SetScreenTransform(&pViewport.CameraToScreen);
    dev->SetCameraTransform(&pViewport.WorldToCamera);
    dev->SetObjectTransform(GIdentityMatrix);
	GEngine->Video->LineBuf.DrawFilledBox(bb,color,ztest);
	return 0;
}
#endif
// =========================================================================
#ifdef TRANSGAMING_USE_EDITOR
static int luaRender3D_RenderTranslucentBox(lua_State* state)
{
    if (!GEngine->Video) return 0;
	Script L(state);
	BoundingBox bb;
	bb.Min = Vector(L.GetFloat(1),L.GetFloat(2),L.GetFloat(3));
	bb.Max = Vector(L.GetFloat(4),L.GetFloat(5),L.GetFloat(6));
#ifdef	TRANSGAMING
    DWORD color = Color_TG(L.GetInt(7),L.GetInt(8),L.GetInt(9)).dwVal;
#else
    DWORD color = Color(L.GetInt(7),L.GetInt(8),L.GetInt(9)).dwVal;
#endif
    Vector pos(L.GetFloat(10,0),L.GetFloat(11,0),L.GetFloat(12,0));
    Quaternion q(L.GetFloat(13),L.GetFloat(14),L.GetFloat(15),L.GetFloat(16));

	View * view = GEngine->Video->GetActiveView();
	Viewport &pViewport = view->LocalViewport;
	RenderDevice *dev = GEngine->Video->RenderDev;
#ifdef TRANSGAMING
    Matrix tmpMat(q.ToMatrix() * TranslationMatrix(pos));

    dev->SetScreenTransform(&pViewport.CameraToScreen);
    dev->SetCameraTransform(&pViewport.WorldToCamera);
    dev->SetObjectTransform(&tmpMat);
#else
    dev->SetScreenTransform(&pViewport.CameraToScreen);
    dev->SetCameraTransform(&pViewport.WorldToCamera);
    dev->SetObjectTransform(&(q.ToMatrix()*TranslationMatrix(pos)));
#endif

	GEngine->Video->LineBuf.DrawTranslucentBox(bb,color);
	return 0;
}
#endif
// =========================================================================
static int luaRender3D_ScreenSize(lua_State* state)
{
	Script L(state);

    if (GEngine->Video)
    {
	    View * view = GEngine->Video->GetActiveView();
	    Viewport *pViewport = &view->LocalViewport;
	    L.PushInt(pViewport->Width);
	    L.PushInt(pViewport->Height);
    }
    else
    {
        L.PushInt(640);
        L.PushInt(480);
    }
	return 2;
}
// =========================================================================
#ifdef TRANSGAMING_USE_EDITOR
static int luaRender3D_DrawPath(lua_State* state)
{
    if (!GEngine->Video) return 0;
	Script L(state);
	WaypointGPath * path = (WaypointGPath*) L.GetLightUD(1);
	if (path)
	{
		LineRender& lineBuf = GEngine->Video->LineBuf;
		lineBuf.DrawGlobalWaypointPath( *path );
	}
	return 0;
}
#endif
// =========================================================================
static int luaRender3D_DrawSprite(lua_State* state)
{
    if (!GEngine->Video) return 0;
	Script L(state);
#ifdef	TRANSGAMING
	Sprite_TG spr;
#else
	Sprite spr;
#endif
	spr.Position.Set(L.GetFloat(1),L.GetFloat(2),L.GetFloat(3));
	spr.Size = L.GetFloat(4);
	spr.Rotation = L.GetFloat(5);
	spr.Color = L.GetInt(6);
	Texture *pTex = GEngine->Video->MaterialSys->CreateTexture(L.GetString(7));
	spr.TexRef = pTex ? pTex->DeviceId : EInvalidIndex;
	spr.BlendMode = RenderState::EBlendAlpha;
	View * view = GEngine->Video->GetActiveView();
    bool dontFacing = L.GetBool(8,false);
    spr.QRotation = Quaternion(L.GetFloat(9,1),L.GetFloat(10,0),L.GetFloat(11,0),L.GetFloat(12,0));
    GEngine->Video->ParticleSys.RenderSprites(view->mCamera,spr,dontFacing);
	return 0;
}
// =========================================================================
// draw quad stretched between 2 vectors and facing camera
static int luaRender3D_DrawSprite1DOF(lua_State* state)
{
    if (!GEngine->Video) return 0;
	Script L(state);
	Sprite1DOF spr;

    spr.Position.GrowArray(2);
	spr.Position[0].Set(L.GetFloat(1),L.GetFloat(2),L.GetFloat(3));
	spr.Position[1].Set(L.GetFloat(4),L.GetFloat(5),L.GetFloat(6));
	spr.Size = L.GetFloat(7);
	spr.Color = L.GetInt(8);
	Texture *pTex = GEngine->Video->MaterialSys->CreateTexture(L.GetString(9));
	spr.TexRef = pTex ? pTex->DeviceId : EInvalidIndex;
	spr.BlendMode = RenderState::EBlendAlpha;
	View * view = GEngine->Video->GetActiveView();
    spr.DrawMode = L.GetInt(10,0);
	GEngine->Video->ParticleSys.RenderSprites(view->mCamera,spr);
	return 0;
}
// =========================================================================
static int luaRender3D_Spr_Create(lua_State* state)
{
    if (!GEngine->Video) return 0;
    Script L(state);

    m_setTag("ScriptSprite");
    Sprite1DOF *pSpr = new Sprite1DOF();
    pSpr->Size = L.GetFloat(1);
    pSpr->Color = L.GetInt(2);
    Texture *pTex = GEngine->Video->MaterialSys->CreateTexture(L.GetString(3));
    pSpr->TexRef = pTex ? pTex->DeviceId : EInvalidIndex;
    pSpr->DrawMode = L.GetInt(4);
    pSpr->BlendMode = RenderState::EBlendAlpha;
    L.PushLightUD(pSpr);
    return 1;
}
// =========================================================================
static int luaRender3D_Spr_AddPoint(lua_State* state)
{
    if (!GEngine->Video) return 0;
    Script L(state);
    Sprite1DOF *pSpr = (Sprite1DOF *) L.GetLightUD(1);
    if (pSpr)
        pSpr->Position.Add(Vector(L.GetFloat(2),L.GetFloat(3),L.GetFloat(4)));
    return 0;
}
// =========================================================================
static int luaRender3D_Spr_Render(lua_State* state)
{
    if (!GEngine->Video) return 0;
    Script L(state);
    Sprite1DOF *pSpr = (Sprite1DOF *) L.GetLightUD(1);
    if (pSpr)
    {
        View * view = GEngine->Video->GetActiveView();
        GEngine->Video->ParticleSys.RenderSprites(view->mCamera,*pSpr);
        delete pSpr;
    }
    return 0;
}
// =========================================================================
// Especially for modding sniper freaks ;)))))
static int luaRender3D_SetCameraFOV(lua_State* state)
{
    if (!GEngine->Video) return 0;
	Script L(state);
	float degrees = L.GetFloat(1,90);
	if( degrees < 1.0f ) degrees = 1.0f;
	if( degrees > 179.0f ) degrees = 179.0f;
	View * view = GEngine->Video->GetActiveView();
	if( GEngine && view )
		view->mCamera.SetFOV(degrees);
	return 0;
}
// =========================================================================
// Especially for modding sniper freaks ;)))))
static int luaRender3D_GetCameraFOV(lua_State* state)
{
	Script L(state);
    if( GEngine->Video)
    {
	    View * view = GEngine->Video->GetActiveView();
        if(view )
	    {
		    L.PushFloat( view->mCamera.GetFOV() );
		    return 1;
	    }
    }
	L.PushFloat( 90.0f );
	return 1;
}
// =========================================================================
#ifdef TRANSGAMING_USE_EDITOR
static int luaRender3D_DrawSphere(lua_State* state)
{
    if (!GEngine->Video) return 0;
	Script L(state);

	View * view = GEngine->Video->GetActiveView();
	Viewport &pViewport = view->LocalViewport;
	RenderDevice *dev = GEngine->Video->RenderDev;
#ifdef TRANSGAMING
    Vector tmpVec(L.GetFloat(3),L.GetFloat(4),L.GetFloat(5));
#endif

    dev->SetScreenTransform(&pViewport.CameraToScreen);
    dev->SetCameraTransform(&pViewport.WorldToCamera);
    dev->SetObjectTransform(GIdentityMatrix);
#ifdef TRANSGAMING
    GEngine->Video->LineBuf.DrawSphere(L.GetFloat(1),L.GetInt(2),&tmpVec,L.GetBool(6,true));
#else
    GEngine->Video->LineBuf.DrawSphere(L.GetFloat(1),L.GetInt(2),&Vector(L.GetFloat(3),L.GetFloat(4),L.GetFloat(5)),L.GetBool(6,true));
#endif
	return 0;
}
#endif
// =========================================================================
#ifdef TRANSGAMING_USE_EDITOR
static int luaRender3D_DrawDirLight(lua_State* state)
{
    if (!GEngine->Video) return 0;
	Script L(state);

	View * view = GEngine->Video->GetActiveView();
	Viewport &pViewport = view->LocalViewport;
	RenderDevice *dev = GEngine->Video->RenderDev;
    dev->SetObjectTransform(GIdentityMatrix);
    dev->SetCameraTransform(&pViewport.WorldToCamera);
    dev->SetScreenTransform(&pViewport.CameraToScreen);
	RenderState st;
	st.SetDepthTest(true);
	st.SetDepthWrite(true);
	st.SetAlphaTest(false);
	st.SetBlendMode(RenderState::EBlendNone);
	GEngine->Video->RenderDev->SetState(st);

	GEngine->Video->LineBuf.DrawDirLight(
		L.GetInt(1),
		Vector(L.GetFloat(2),L.GetFloat(3),L.GetFloat(4)),
		Vector(L.GetFloat(5),L.GetFloat(6),L.GetFloat(7)));
	return 0;
}
#endif
// =========================================================================
#ifdef TRANSGAMING_USE_EDITOR
static int luaRender3D_DrawSpotLight(lua_State* state)
{
    if (!GEngine->Video) return 0;
	Script L(state);

	View * view = GEngine->Video->GetActiveView();
	Viewport &pViewport = view->LocalViewport;
	RenderDevice *dev = GEngine->Video->RenderDev;
    dev->SetScreenTransform(&pViewport.CameraToScreen);
    dev->SetCameraTransform(&pViewport.WorldToCamera);
    dev->SetObjectTransform(GIdentityMatrix);
	RenderState st;
	st.SetDepthTest(true);
	st.SetDepthWrite(true);
	st.SetAlphaTest(false);
	st.SetBlendMode(RenderState::EBlendNone);
	GEngine->Video->RenderDev->SetState(st);

	DWORD color    = L.GetInt(1);
	Vector pos     = Vector(L.GetFloat(2),L.GetFloat(3),L.GetFloat(4));
	Vector dir     = Vector(L.GetFloat(5),L.GetFloat(6),L.GetFloat(7));
	float  ran     = L.GetFloat(8);
	float  fal	   = L.GetFloat(9);
	float  cone    = FDeg2Rad(L.GetFloat(10));
	dir.Normalize();

//	GEngine->Video->LineBuf.DrawSphere(ran, color, &pos);
//	GEngine->Video->LineBuf.DrawSphere(fal, color, &pos);

	Vector xAxis = dir.MinimalAxis()^dir; xAxis.Normalize();
	Vector yAxis = dir^xAxis;
	float s = GSinFloat(cone);
	float c = GCosFloat(cone);
#ifdef TRANSGAMING
    LookAtMatrix tmpMat(pos,pos+dir,yAxis);
#endif


	xAxis *= s;
	yAxis *= s;
	GEngine->Video->LineBuf.DrawEllipse(color, pos+dir*ran*c, xAxis*ran, yAxis*ran);
	GEngine->Video->LineBuf.DrawEllipse(color, pos+dir*fal*c, xAxis*fal, yAxis*fal);
	GEngine->Video->LineBuf.DrawLine(pos, pos+dir*ran*c + xAxis*ran, color, true);
	GEngine->Video->LineBuf.DrawLine(pos, pos+dir*ran*c - xAxis*ran, color, true);
	GEngine->Video->LineBuf.DrawLine(pos, pos+dir*ran*c + yAxis*ran, color, true);
	GEngine->Video->LineBuf.DrawLine(pos, pos+dir*ran*c - yAxis*ran, color, true);
#ifdef TRANSGAMING
	GEngine->Video->LineBuf.DrawSphereCone(ran, color, 2*cone, &tmpMat);
#else
	GEngine->Video->LineBuf.DrawSphereCone(ran, color, 2*cone, &LookAtMatrix(pos,pos+dir,yAxis));
#endif
	return 0;
}
#endif
// =========================================================================
static int luaRender3D_ClearZBuffer(lua_State* state)
{
    if (!GEngine->Video) return 0;
	GEngine->Video->RenderDev->ClearZ();
	return 0;
}
// =========================================================================
static int luaRender3D_ClearScreen(lua_State* state)
{
    if (!GEngine->Video) return 0;
	GEngine->Video->RenderDev->ClearFB();
	return 0;
}
// =========================================================================
static int luaRender3D_SetContrastGammaAndBrightness(lua_State* state)
{
    if (!GEngine->Video) return 0;
	Script L(state);
	float contrast = L.GetFloat( 1, 1.0f );
	float gamma = L.GetFloat( 2, 0.2f );
	float brightness = L.GetFloat( 3, 1.0f );
	GEngine->Video->RenderDev->SetGamma( contrast, gamma, brightness );
	return 0;
}
// =========================================================================
#ifndef TRANSGAMING
static int luaRender3D_GetAvailableResolutions(lua_State* state)
{
	Script  L(state);
    char    buf[16];


    L.NewTable();
	RenderDevice *dev = GEngine->GetRenderDevice();

	for (int i=0; i<dev->DisplayModeNum; i++)
    {
        L.PushInt(i+1);
        snprintf(buf, sizeof(buf) / sizeof(buf[0]), "%dx%d", dev->DisplayModes[i].XRes,dev->DisplayModes[i].YRes);

        L.PushString(buf);
        L.SetTable(-3);
    }

    return 1;
}
#endif
// =========================================================================
static int luaRender3D_ApplyVideoSettings(lua_State* state)
{
    if (!GEngine->Video) return 0;
	Script L(state);

	String resol = L.GetString( 1, "640X480" );
	bool fullscreen = L.GetBool( 2, false );


#ifdef TRANSGAMING
	//GScript.Call("R3D.SetResolution", 0, "%s, %s", resol.AsChar(), fullscreen?"true":"false" );
	GScript.DoString( "R3D.SetResolution(\"%s\",%s)", resol.AsChar(), fullscreen?"true":"false" );
#else
	GScript.DoString( "R3D.SetResolution(\"%s\",%s)", resol.AsChar(), fullscreen?"true":"false" );
#endif
	View * view = GEngine->Video->GetActiveView();
	RenderDevice* dev = GEngine->Video->RenderDev;

	SetForegroundWindow(view->GetActiveWindow());
	SetFocus(view->GetActiveWindow());
#ifndef TRANSGAMING
	GEngine->InputSys->SetWindow(view->GetActiveWindow());
#endif
	GEngine->InputSys->Reset();
	GEngine->InputSys->UpdateMousePosition();
	GEngine->InputSys->CenterMouse();
	view->LocalViewport.Resize(view->GetActiveWindow());
	view->LocalViewport.SetCameraToScreen();
//	dev->Lock(view->LocalViewport);
	dev->ClearFB(0);
//	dev->Unlock();
	dev->Flip(view->GetActiveWindow());

	// Set gamma, brightness and contrast
	float g = 1.0f;
	float b = 0.5f;
	float c = 0.5f;
    ScriptObject  cfg = GScript.Globals()["Cfg"];
    if( !cfg["Gamma"].IsNil() )
        g = cfg["Gamma"].GetFloat();
    if( !cfg["Brightness"].IsNil() )
        b = cfg["Brightness"].GetFloat();
    if( !cfg["Contrast"].IsNil() )
        c = cfg["Contrast"].GetFloat();

	dev->SetGamma( c, g, b );

	// Set texture quality (MUST match enum ETexLODx enum in render device!!!)
	bool applyTextureLOD = false;
	char *names[] = {"TextureQualityArchitecture","TextureQualityWeapons","TextureQualityCharacters","TextureQualitySkies"};
	cfg = GScript.Globals()["Cfg"];
	for (int i=0; i<RenderDevice::ETexLODNumEntries; ++i)
	{
		int texQ = 0;
		if( !cfg[names[i]].IsNil() )
			texQ = cfg[names[i]].GetInt();
		if (dev->TexLOD[i] != texQ) {
			applyTextureLOD = true;
			dev->TexLOD[i] = texQ;
		}
	}

	GEngine->Video->ShadowsEnabled = cfg["Shadows"].GetInt();

	GEngine->Video->ParticleSys.ParticlesDetail = cfg["ParticlesDetail"].GetInt();

	GEngine->Video->DecalSys.DecalsStay = cfg["DecalsStayTime"].GetFloat();

	GEngine->Video->BloomEnabled		= cfg["Bloom"].GetBool() ? 1 : 0;
	GEngine->Video->WarpEffectsEnabled	= cfg["WarpEffects"].GetBool() ? 1 : 0;
#ifndef TRANSGAMING
	GEngine->World->WaterQuality		= cfg["WaterFX"].GetInt();
#else
	GEngine->pcfWorld->WaterQuality		= cfg["WaterFX"].GetInt();
#endif

	// sanity check
	if (GEngine->GetRenderDevice()->HWClass < RenderDevice::EHWClassNV30)
		GEngine->Video->WarpEffectsEnabled = 0;

#ifndef TRANSGAMING
	if (applyTextureLOD)
 		GEngine->Video->MaterialSys->SetTextureLOD();
#endif

	if (!(GEngine->Video->RenderDev->DevCaps & RenderDevice::EDevCaps_BumpEnvProj))
#ifndef TRANSGAMING
		GEngine->World->WaterQuality = 0;

    dev->SetShaderMemory(4,&view->LocalViewport.WorldToScreen.GetTranspose()[0],4);
	dev->SetShaderMemory(8,&Plane(27.5,255*3,1,0.5),1);
#else
		GEngine->pcfWorld->WaterQuality = 0;

    Plane tmpPlane(27.5,255*3,1,0.5);


    dev->SetShaderMemory(4,&view->LocalViewport.WorldToScreen.GetTranspose()[0],4);
	dev->SetShaderMemory(8,&tmpPlane,1);
#endif

	dev->SetFog( false );

#ifndef TRANSGAMING
	GEngine->World->OnSettingsChanged();
#else
	GEngine->pcfWorld->OnSettingsChanged();
#endif

    /*
	int shadows = L.GetInt( 6, 0 );
	bool texQuality = L.GetBool( 7 );
	bool weather = L.GetBool( 8 );
	bool viewWeapon = L.GetBool( 9 );

	String texFilter = L.GetString( 10 );
	bool dynLights = L.GetBool( 11 );
	bool projectors = L.GetBool( 12 );
	bool coronas = L.GetBool( 13 );
	bool decals = L.GetBool( 14 );
	int decalsStay = L.GetInt( 15 );
*/
	return 0;
}
// =========================================================================
static int luaRender3D_SetResolution( lua_State *state )
{
    if (!GEngine->Video) return 0;
	Script L( state );

	// Set Resolution
#ifdef TRANSGAMING
# ifdef TRANSGAMING_PSP
    const char *    resol = L.GetString(1, "480x272");
# else
    const char *    resol = L.GetString(1, "640x480");
# endif
    char            tmpRes[32];
    char *          tok;
    int             x = 0;
    int             y = 0;


    strncpy(tmpRes, resol, sizeof(tmpRes) / sizeof(tmpRes[0]));
    tok = strtok(tmpRes, "Xx");

    if (tok)
        x = atoi(tok);


    tok = strtok(NULL, "Xx");

    if (tok)
        y = atoi(tok);
#else
	DynamicArray<String> xy;
	char delims[] = "Xx";

	String resol = L.GetString( 1, "640X480" );
	String::Tokenize( resol.AsChar(), delims, xy );

	Assert( xy.Size() == 2 );
	int x = atoi( xy[0].AsChar() );
	int y = atoi( xy[1].AsChar() );
#endif
#ifdef TRANSGAMING_PSP
	if( ( x == 0 ) || ( y == 0 ) ) { x = 480; y = 272; }	// Clamp if not valid
#else
	if( ( x == 0 ) || ( y == 0 ) ) { x = 640; y = 480; }	// Clamp if not valid
#endif
	bool fullscreen = L.GetBool( 2, false );

	int aa = 0;
	if( !GScript.Globals()["Cfg"]["Multisample"].IsNil() )
	{
		LPCSTR multi = GScript.Globals()["Cfg"]["Multisample"].GetString();
		sscanf(multi, "x%d",&aa);
	}
	if( !GScript.Globals()["Cfg"]["Coronas"].IsNil() )
	{
		GEngine->Video->CoronasEnabled = GScript.Globals()["Cfg"]["Coronas"].GetBool();
	}

    TRACE("luaRender3D_SetResolution(): setting the screen resolution to %dx%d\n", x, y);
#if defined(TRANSGAMING_PSP) || defined(TRANSGAMING_PS2)
    // eric-> skip making this call since it's set to the correct resolution when the
    //   graphics system starts up and the resolution can't change on these systems.
	//GEngine->Video->SetRes(RenderDeviceManager::ED3DRenderDevice,x,y,fullscreen,aa);
#else
	GEngine->Video->SetRes(RenderDeviceManager::ED3DRenderDevice,x,y,fullscreen,aa);
#endif
#ifndef TRANSGAMING
	if( GEngine->Video->IsFullScreen )
		GEngine->Video->RenderWnd->Show();
	else
		GEngine->Video->RenderWnd->Hide();
#endif

#ifdef TRANSGAMING
    GScript.Call("Game_ResolutionChanged", 0, "%d, %d", x, y);
#endif
/*
	if( GEngine->InputSys )
	{
		if( !GEngine->Video->IsFullScreen )
			ReinitInputSystem( GEngine->InputSys->IsUsingDInput(), GEngine->Video->GetActiveView()->GetActiveWindow() );
		else
			ReinitInputSystem( true, GEngine->Video->GetActiveView()->GetActiveWindow() );
	}
*/
	return 0;
}
// =========================================================================
static int luaRender3D_EnableShadows(lua_State* state)
{
    if (!GEngine->Video) return 0;
    Script L(state);
	GEngine->Video->ShadowsEnabled = L.GetInt(1);
    return 0;
}
// =========================================================================
static int luaRender3D_EnableBloom(lua_State* state)
{
    if (!GEngine->Video) return 0;
    Script L(state);
	GEngine->Video->BloomEnabled = L.GetBool(1) ? 1 : 0;
    return 0;
}
// =========================================================================
static int luaRender3D_EnableWarpEffects(lua_State* state)
{
    if (!GEngine->Video) return 0;
    Script L(state);
	GEngine->Video->WarpEffectsEnabled = L.GetBool(1) ? 1 : 0;
	// sanity check
	if (GEngine->GetRenderDevice()->HWClass < RenderDevice::EHWClassNV30)
		GEngine->Video->WarpEffectsEnabled = 0;

#ifndef TRANSGAMING
	World* W = GEngine->World;
#else
	World* W = GEngine->pcfWorld;
#endif

	for (int i=0; i<W->Meshes.Size(); ++i) {
		if (W->Meshes[i]) {
			W->Meshes[i]->SetupGlassShader();
		}
	}

    return 0;
}
// =========================================================================
static int luaRender3D_SetWaterQuality(lua_State* state)
{
    if (!GEngine->Video) return 0;
    Script L(state);
#ifndef TRANSGAMING
	World* W = GEngine->World;
#else
	World* W = GEngine->pcfWorld;
#endif
	W->WaterQuality = L.GetInt(1, 0);
    return 0;
}
// =========================================================================
#ifndef TRANSGAMING
static int luaRender3D_IsFullscreen(lua_State* state)
{
    Script L(state);
#ifdef TRANSGAMING
    L.PushBool(true);
#else
	L.PushBool( GEngine->Video->IsFullScreen );
#endif
    return 1;
}
#endif
// =========================================================================
static int luaRender3D_GetHWClass( lua_State *state )
{
	Script L( state );
	L.PushInt( GEngine->Video->RenderDev->HWClass );
	return 1;
}
// =========================================================================
static int luaRender3D_SetTexFiltering(lua_State* state)
{
	Script L(state);
	GEngine->Video->MaterialSys->SetTexFiltering();
	return 0;
}
// =========================================================================
#ifndef TRANSGAMING
static int luaRender3D_SetParticlesDetail(lua_State* state)
{
	Script L(state);
    GEngine->Video->ParticleSys.ParticlesDetail = L.GetInt(1);
	return 0;
}
#endif
// =========================================================================
static int luaRender3D_Flip(lua_State* state)
{
	Script L(state);
	RenderDevice* dev = GEngine->GetRenderDevice();
	View *view = GEngine->Video->GetActiveView();
    dev->Flip( view->GetActiveWindow() );
	return 0;
}
// =========================================================================
#ifndef TRANSGAMING
static int luaRender3D_KeepDecals( lua_State *state )
{
	if (!GEngine->Video) return 0;
	Script L( state );

	GEngine->Video->DecalSys.DecalsAlwaysStay = L.GetBool( 1, false );

	return 0;
}
#endif
// =========================================================================
#ifndef TRANSGAMING
static int luaRender3D_GetFPS( lua_State *state )
{
	Script L( state );
	L.PushFloat( GEngine->avgFPS );
	return 1;
}
#endif
// =========================================================================
static const luaL_reg render_lib[] = {
	{"DistToLine", luaRender3D_DistToLine},
	{"DistToLine2D", luaRender3D_DistToLine2D},
#ifdef TRANSGAMING_USE_EDITOR
	{"DistToCamera", luaRender3D_DistToCamera},
	{"RenderLine", luaRender3D_RenderLine},
	{"RenderBox", luaRender3D_RenderBox},
    {"RenderTranslucentBox", luaRender3D_RenderTranslucentBox},
	{"ScreenTo3D", luaRender3D_ScreenTo3D},
    {"ClosestPointOnLine", luaRender3D_ClosestPointOnLine},
	{"DrawPath", luaRender3D_DrawPath},
	{"DrawSphere", luaRender3D_DrawSphere},
	{"DrawDirLight", luaRender3D_DrawDirLight},
	{"DrawSpotLight", luaRender3D_DrawSpotLight},
#endif
	{"RGB", luaRender3D_RGB},
	{"RGBA", luaRender3D_RGBA},
	{"ScreenSize", luaRender3D_ScreenSize},
	{"ClearZBuffer", luaRender3D_ClearZBuffer},
    {"ClearScreen", luaRender3D_ClearScreen},
	{"ApplyVideoSettings", luaRender3D_ApplyVideoSettings},
	{"SetResolution", luaRender3D_SetResolution},
	{"SetContrastGammaAndBrightness", luaRender3D_SetContrastGammaAndBrightness},
	{"SetCameraFOV", luaRender3D_SetCameraFOV},
	{"GetCameraFOV", luaRender3D_GetCameraFOV},
	{"DrawSprite", luaRender3D_DrawSprite},
	{"DrawSprite1DOF", luaRender3D_DrawSprite1DOF},
    {"EnableShadows", luaRender3D_EnableShadows},
	{"EnableBloom", luaRender3D_EnableBloom},
	{"EnableWarpEffects", luaRender3D_EnableWarpEffects},
	{"SetWaterQuality", luaRender3D_SetWaterQuality},
    {"Spr_Create", luaRender3D_Spr_Create},
    {"Spr_AddPoint", luaRender3D_Spr_AddPoint},
    {"Spr_Render", luaRender3D_Spr_Render},
	{"GetHWClass", luaRender3D_GetHWClass},
	{"SetTexFiltering", luaRender3D_SetTexFiltering},
#ifdef TRANSGAMING_PS2
    {"polygonMode", luaRender3D_polygonMode},
#endif
#ifndef TRANSGAMING
	{"GetAvailableResolutions", luaRender3D_GetAvailableResolutions},
	{"VectorToScreen", luaRender3D_VectorToScreen},
	{"ComputeSZ", luaRender3D_ComputeSZ},
	{"IsFullscreen", luaRender3D_IsFullscreen},
    {"SetParticlesDetail", luaRender3D_SetParticlesDetail},
	{"KeepDecals", luaRender3D_KeepDecals},
	{"GetFPS", luaRender3D_GetFPS},
#endif
	{"Flip", luaRender3D_Flip},
	{NULL, NULL}
};
// =========================================================================
// FILE SYSTEM
// =========================================================================
static int luaFileSystem_CreateDirectory(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	L.PushInt(CreateDirectory(L.GetString(1),NULL));
#endif
	return 1;
}
// =========================================================================
static int luaFileSystem_FindFiles(lua_State* state)
{
	Script L(state);

	const char * mask = L.GetString(1);
	int bfiles = L.GetInt(2);
	int bdirs = L.GetInt(3);

	DWORD mode = 0;
	if (bfiles)	mode|=EFindFiles;
	if (bdirs)	mode|=EFindDirs;

	DynamicArray<String> files;
	GFileMan.FindFiles(mask,files,mode);

	L.NewTable();
	for (int i=0;i<files.Size();i++)
	{
		L.PushInt(i+1);
		L.PushString(files[i].AsChar());
		L.SetTable(-3);
	}
	return 1;
}

#ifdef TRANSGAMING
static int luaFileSystem_FindFirstFile(lua_State *state){
    Script L(state);

    const char *mask = L.GetString(1);
    char        buf[_MAX_PATH];


    if (GFileMan.FindAFile(mask, buf, _MAX_PATH)){
        L.PushBool(true);
        L.PushString(buf);

        return 2;
    }

    else{
        L.PushBool(false);

        return 1;
    }

    return 0;
}
#endif
// =========================================================================
static int luaFileSystem_DeleteFiles(lua_State* state)
{
	Script L(state);

	const char *            mask = L.GetString(1);
	DynamicArray<String>    files;
    char                    buf[MAX_PATH];


    snprintf(buf, MAX_PATH, "%s*.*", mask);
	GFileMan.FindFiles(buf, files); // XXX


    for (int i=0;i<files.Size();i++){
        snprintf(buf, MAX_PATH, "%s%s", mask, files[i].AsChar());

		DeleteFile(buf);
    }

	return 0;
}
// =========================================================================
static int luaFileSystem_RemoveDirectory(lua_State* state)
{
    Script L(state);
    RemoveDirectory(L.GetString(1));
    return 0;
}
// =========================================================================
static int luaFileSystem_GetBaseObjInfo(lua_State* state)
{
	Script L(state);

	GFile* f=GFileMan.CreateFileReader(L.GetString(1));
	if(f)
	{
		char buf[1024];
		while(f->GetS(buf,sizeof(buf)))
		{
			if (strstr(buf,"BaseObj"))
			{
				L.DoString(buf);
				break;
			}
		}
		GFileMan.Close(f);

	}
	return 0;
}

// =========================================================================
#ifndef TRANSGAMING
static int luaFileSystem_CreatePack(lua_State* state)
{
    Script L(state);
    GPack::Make(L.GetString(1),L.GetString(2),L.GetBool(3,false));
    return 0;
}
// =========================================================================
static int luaFileSystem_ExtractPack(lua_State* state)
{
	Script L(state);
	GPack::Extract(L.GetString(1),L.GetString(2));
	return 0;
}
#endif
// =========================================================================
#ifndef TRANSGAMING
static int luaFileSystem_CreateZIP(lua_State* state)
{
    Script L(state);
    GFileMan.CreateZIP(L.GetString(1));
    return 0;
}
// =========================================================================
static int luaFileSystem_CloseZIP(lua_State* state)
{
    GFileMan.CloseZIP();
    return 0;
}
#endif
// =========================================================================
static int luaFileSystem_CreatePAK(lua_State* state)
{
    Script L(state);
#ifndef TRANSGAMING_USE_PHYSFS
    GFileMan.CreatePAK(L.GetString(1));
#endif
    return 0;
}
// =========================================================================
static int luaFileSystem_ClosePAK(lua_State* state)
{
#ifndef TRANSGAMING_USE_PHYSFS
    GFileMan.ClosePAK();
#endif
    return 0;
}
// =========================================================================
static int luaFileSystem_File_Open(lua_State* state)
{
    Script L(state);
#ifndef TRANSGAMING
    FileObject *fo = (FileObject *) GFileMan.CreateFileWriter(L.GetString(1),"wp");
    L.PushLightUD(fo);
#else
    GScript.DumpError(LUA_ERRRUN, __FILE__, __FUNCTION__);
#endif
    return 1;
}

// =========================================================================
static int luaFileSystem_File_Exist(lua_State* state)
{
    Script L(state);
    L.PushBool(GFileMan.FileExist(L.GetString(1)));
    return 1;
}
// =========================================================================
static int luaFileSystem_File_GetSize(lua_State* state)
{
	Script L(state);
	L.PushInt(GFileMan.FileSize(L.GetString(1)));
	return 1;
}
// =========================================================================
static int luaFileSystem_File_Write(lua_State* state)
{
    Script L(state);
#ifndef TRANSGAMING
    FileObject *fo = (FileObject *)L.GetLightUD(1);
    if (fo)
    {
        bool addRN = false;
        LPCSTR buf = L.GetString(2);
        int l = strlen(buf);
        if (l>0 && (buf[l-1]=='\n' || buf[l-1]=='\r'))
        {
            l = l - 1;
            addRN = true;
        }

        fo->Write(buf,l);

        if (addRN)
            fo->Write("\r\n",2);
    }
#else
    GScript.DumpError(LUA_ERRRUN, __FILE__, __FUNCTION__);
#endif
    return 0;
}
// =========================================================================
static int luaFileSystem_File_Close(lua_State* state)
{
    Script L(state);
#ifndef TRANSGAMING
    FileObject *fo = (FileObject *)L.GetLightUD(1);
    if (fo)
        GFileMan.Close(fo);
#else
    GScript.DumpError(LUA_ERRRUN, __FILE__, __FUNCTION__);
#endif
    return 0;
}
// =========================================================================
static int luaFileSystem_RegisterPack(lua_State* state)
{
    Script L(state);
#ifndef TRANSGAMING_USE_PHYSFS
    m_setTag("ScriptPack");
    GPack * pPack = (GZipPack *) GFileMan.RegisterPack(new GPack(), L.GetString(1),L.GetString(2),false);
    if (pPack)
        pPack->UsingSharedMemory = false;
    L.PushLightUD(pPack);
    return 1;
#else
    return 0;
#endif
}
// =========================================================================
static int luaFileSystem_UnregisterPack(lua_State* state)
{
    Script L(state);


#ifndef TRANSGAMING_USE_PHYSFS
    GFileMan.UnregisterPack((GPack*)L.GetLightUD(1));
#endif

    return 0;
}

// =========================================================================
static const luaL_reg file_system_lib[] = {
	{"CreateDirectory", luaFileSystem_CreateDirectory},
	{"FindFiles", luaFileSystem_FindFiles},
    {"FindAFile", luaFileSystem_FindFirstFile},
	{"DeleteFiles", luaFileSystem_DeleteFiles},
	{"GetBaseObjInfo", luaFileSystem_GetBaseObjInfo},
#ifndef TRANSGAMING
    {"CreatePack", luaFileSystem_CreatePack},
	{"ExtractPack", luaFileSystem_ExtractPack},
#endif

    {"RemoveDirectory", luaFileSystem_RemoveDirectory},
#ifndef TRANSGAMING
    {"CreateZIP", luaFileSystem_CreateZIP},
    {"CloseZIP", luaFileSystem_CloseZIP},
#endif
    {"CreatePAK", luaFileSystem_CreatePAK},
    {"ClosePAK", luaFileSystem_ClosePAK},

    {"File_Open", luaFileSystem_File_Open},
    {"File_Write", luaFileSystem_File_Write},
    {"File_Close", luaFileSystem_File_Close},
    {"File_Exist", luaFileSystem_File_Exist},
	{"File_GetSize", luaFileSystem_File_GetSize},
    {"RegisterPack", luaFileSystem_RegisterPack},
    {"UnregisterPack", luaFileSystem_UnregisterPack},

	{NULL, NULL}
};
// =========================================================================
// LANGUAGES
// =========================================================================
static int luaLang_ParseLangFile( lua_State *state )
{
	Script L( state );

	GFile *f = GFileMan.CreateFileReader( L.GetString( 1 ) );
	if( f )
	{
		char buf[1024];
		while( f->GetS( buf, sizeof( buf ) ) )
		{
			int len = strlen( buf );
			if( buf[len-1] == '\n' )
				buf[len-1] = 0;
//			GLog.Print( "aaa: %s\n", buf );
			GScript.Call( "Languages_ParseLangLine", 0, "%s", buf );
		}
		GFileMan.Close( f );
	}

	return 0;
}
// =========================================================================
static const luaL_reg languages_lib[] = {
	{"ParseLangFile", luaLang_ParseLangFile},
	{NULL, NULL}
};
// =========================================================================
// PLAYER
// =========================================================================
static int luaPlayer_ExecAction(lua_State* state)
{
    Script L(state);

#ifndef TRANSGAMING
	Entity * pe = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Entity * pe = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

    if(pe && pe->PO)
	{
//		float delta = L.GetFloat(2);
        Vector fv(L.GetFloat(3),L.GetFloat(4),L.GetFloat(5));
        Vector rv(L.GetFloat(6),L.GetFloat(7),L.GetFloat(8));
		float angle = atan2(fv.X,fv.Z);
		pe->SetOrientation(angle);
#ifndef TRANSGAMING_NO_HAVOK
		pe->PO->SetOrientation(angle);
        int action = pe->PO->Action;
		pe->PO->PlayerAction( fv, rv );
        pe->PO->Action = action;
#endif
	}

	return 0;
}
// =========================================================================
static int luaPlayer_ExecMultiPlayerAction(lua_State* state)
{
    Script L(state);

#ifndef TRANSGAMING
	Entity * pe = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Entity * pe = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

    if(pe && pe->PO)
    {
//		float delta = L.GetFloat(2);
        Vector fv(L.GetFloat(3),L.GetFloat(4),L.GetFloat(5));
        Vector rv(L.GetFloat(6),L.GetFloat(7),L.GetFloat(8));
        float angle = atan2(fv.X,fv.Z);
        pe->SetOrientation(angle);
#ifndef TRANSGAMING_NO_HAVOK
        pe->PO->SetOrientation(angle);
#endif
    }

    return 0;
}
// =========================================================================
static int luaPlayer_BotAction(lua_State* state)
{
    Script L(state);

#ifndef TRANSGAMING
	Entity * pe = (Entity *)GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Entity * pe = (Entity *)GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

	if( pe && pe->PO && ( !GEngine->Net || ( pe != GEngine->Net->GetLocalPlayer() ) ) )
    {
		DWORD action = L.GetInt( 2, 0 );
		float yaw = L.GetFloat(3,0.f);
		float pitch = L.GetFloat(4,0.f);
		float timeDelta = L.GetFloat(5,0.f);
		yaw = fmod( F_PI + fmod(yaw,2*F_PI), 2*F_PI );
		if( yaw > F_PI ) yaw -= 2*F_PI;
		pe->PO->Action = action;
#ifndef TRANSGAMING_NO_HAVOK
		pe->PO->MultiPlayerActionNonPhysic( yaw, pitch, timeDelta );
#endif
    }

    return 0;
}
// =========================================================================
static int luaPlayer_FloorCheck(lua_State* state)
{
    Script L(state);

#ifndef TRANSGAMING_NO_HAVOK
#ifndef TRANSGAMING
	Entity * pe = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Entity * pe = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif
	L.PushBool( (pe && pe->PO)?pe->PO->FloorCheck(-8.5):false );
#else
	L.PushBool(false);
#endif
	return 1;
}
// =========================================================================
static int luaPlayer_FloorCheckMP(lua_State* state)
{
    Script L(state);

#ifndef TRANSGAMING_NO_HAVOK

#ifndef TRANSGAMING
	Entity * pe = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Entity * pe = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

	L.PushBool( (pe && pe->PO)?pe->PO->FloorCheckMP( L.GetFloat(2,0.65f) ):false );
#else
	L.PushBool(false);
#endif
	return 1;
}
// =========================================================================
static int luaPlayer_AttachToUnderBody(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
    Script L(state);

#ifndef TRANSGAMING
	Entity * pe = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Entity * pe = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

    if( pe && pe->PO ) pe->PO->AttachPlayerToUnderbody();
#endif
    return 0;
}
// =========================================================================
static int luaPlayer_DetachFromUnderBody(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
    Script L(state);
#ifndef TRANSGAMING
	Entity * pe = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
    Entity * pe = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif
    if( pe && pe->PO ) pe->PO->DetachPlayerFromUnderbody();
#endif
    return 0;
}
// =========================================================================
static int luaPlayer_GetPitch(lua_State* state)
{
	Script L(state);
#ifndef TRANSGAMING_NO_HAVOK
#ifndef TRANSGAMING
	Entity* pe = (Entity *)GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
	Entity* pe = (Entity *)GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif
	if( pe && pe->PO )
		L.PushInt( pe->PO->GetPlayerPitch() );
	else
		L.PushInt( 0 );
#else
	L.PushInt( 0 );
#endif
    return 1;
}
// =========================================================================
static int luaPlayer_GetMPByte(lua_State* state)
{
	Script L(state);
#ifndef TRANSGAMING_NO_HAVOK
#ifndef TRANSGAMING
	Entity* pe = (Entity *)GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
	Entity* pe = (Entity *)GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif
	if( pe && pe->PO )
		L.PushInt( pe->PO->GetMPByte() );
	else
		L.PushInt( 0 );
#else
	L.PushInt( 0 );
#endif
    return 1;
}
// =========================================================================
static int luaPlayer_SetMPByte(lua_State* state)
{
	Script L(state);
#ifndef TRANSGAMING_NO_HAVOK
#ifndef TRANSGAMING
	Entity* pe = (Entity *)GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
	Entity* pe = (Entity *)GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif
	if( pe && pe->PO )
		pe->PO->SetMPByte( L.GetInt(2,0) );
#endif
    return 0;
}

#ifdef PLAYER_CONTROLLER_TEST
// =========================================================================
static int luaPlayer_RecordMovement(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
    Script L(state);

#ifndef TRANSGAMING
	Entity* pe = (Entity *)GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
	Entity* pe = (Entity *)GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

	if(pe && pe->PO && pe->PO->mPlayerVariables && pe->PO->mPlayerVariables->Controller )
		pe->PO->mPlayerVariables->Controller->StartRecording( "movement.bin" );
#endif

	return 0;
}
// =========================================================================
static int luaPlayer_TestMovement(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
    Script L(state);

#ifndef TRANSGAMING
	Entity* pe = (Entity *)GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
	Entity* pe = (Entity *)GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif

	if(pe && pe->PO && pe->PO->mPlayerVariables && pe->PO->mPlayerVariables->Controller )
		pe->PO->mPlayerVariables->Controller->TestSavedData( "movement.bin", pe->PO );
#endif

	return 0;
}
#endif // PLAYER_CONTROLLER_TEST

// =========================================================================
static int luaPlayer_GetDistanceFromPoint(lua_State* state)
{
	Script L(state);
#ifndef TRANSGAMING_NO_HAVOK
#ifndef TRANSGAMING
	Entity * pe = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
	Entity * pe = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif
	if( pe && pe->PO )
		L.PushFloat( pe->PO->GetDistanceFromPoint( Vector( L.GetFloat(2), L.GetFloat(3), L.GetFloat(4) ) ) );
	else
		L.PushFloat( 10000000.f );
#else
	L.PushFloat( 10000000.f );
#endif
	return 1;
}
// =========================================================================
static int luaPlayer_GetCameraFix(lua_State* state)
{
	Script L(state);
#ifndef TRANSGAMING_NO_HAVOK
#ifndef TRANSGAMING
	Entity * pe = (Entity *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#else
	Entity * pe = (Entity *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeModel);
#endif
	if( pe && pe->PO )
		L.PushFloat( pe->PO->GetCameraFix() );
	else
		L.PushFloat( 0.f );
#else
	L.PushFloat( 0.f );
#endif
	return 1;
}
// =========================================================================
static const luaL_reg player_lib[] = {
	{"ExecAction", luaPlayer_ExecAction},
	{"FloorCheck", luaPlayer_FloorCheck},
	{"FloorCheckMP", luaPlayer_FloorCheckMP},
    {"ExecMultiPlayerAction", luaPlayer_ExecMultiPlayerAction},
	{"BotAction", luaPlayer_BotAction},
	{"AttachToUnderBody", luaPlayer_AttachToUnderBody},
	{"DetachFromUnderBody", luaPlayer_DetachFromUnderBody},
#ifdef PLAYER_CONTROLLER_TEST
	{"RecordMovement", luaPlayer_RecordMovement},
	{"TestMovement", luaPlayer_TestMovement},
#endif
	{"GetPitch", luaPlayer_GetPitch},
	{"GetMPByte", luaPlayer_GetMPByte},
	{"SetMPByte", luaPlayer_SetMPByte},
	{"GetDistanceFromPoint", luaPlayer_GetDistanceFromPoint},
	{"GetCameraFix", luaPlayer_GetCameraFix},
	{NULL, NULL}
};
// =========================================================================
static char soundTmpBuf[MAX_PATH];

static int luaSound_Play2D(lua_State* state)
{
	Script L(state);

#ifdef TRANSGAMING_NO_MILES
	L.PushInt(-1);
#else
    if (!GEngine->Audio)
	{
		L.PushInt(-1);
		return 1;
	}

# ifdef TRANSGAMING
	snprintf(soundTmpBuf, MAX_PATH, soundFileFormatStr, L.GetString(1));
	int sound = GEngine->Audio->Sound2D_CreateEx( soundTmpBuf, 0, L.GetFloat(2,80.0f)/100.0f, L.GetBool(3,false), L.GetBool(4,false), true, 1.0f, L.GetBool(5,false) );
# else
	static char buf[MAX_PATH];

	sprintf(buf, soundFileFormatStr,L.GetString(1));
	int sound = GEngine->Audio->Sound2D_CreateEx( buf, 0, L.GetFloat(2,80.0f)/100.0f, L.GetBool(3,false), L.GetBool(4,false), true, 1.0f, L.GetBool(5,false) );
#endif

	L.PushInt(sound);
#endif
	return 1;
}
// =========================================================================
static int luaSound_Play3D(lua_State* state)
{
	Script L(state);

#ifdef TRANSGAMING_NO_MILES
	L.PushInt(-1);
#else
    if (!GEngine->Audio)
	{
		L.PushInt(-1);
		return 1;
	}
# ifdef TRANSGAMING
	snprintf(soundTmpBuf, MAX_PATH, soundFileFormatStr, L.GetString(1));

	Vector pos(L.GetFloat(2),L.GetFloat(3),L.GetFloat(4));
	float mindist = L.GetFloat(5,6);
	float maxdist = L.GetFloat(6,mindist+24);

	int sound = GEngine->Audio->Sound3D_CreateEx( soundTmpBuf, 0, true, 1, pos, mindist, maxdist, 1.0f, L.GetBool(7,false),
		false, GEngine->Audio->GetDefaultObstruction() );
# else
	static char buf[MAX_PATH];

	sprintf(buf, soundFileFormatStr,L.GetString(1));

	Vector pos(L.GetFloat(2),L.GetFloat(3),L.GetFloat(4));
	float mindist = L.GetFloat(5,6);
	float maxdist = L.GetFloat(6,mindist+24);

	int sound = GEngine->Audio->Sound3D_CreateEx( buf, 0, true, 1, pos, mindist, maxdist, 1.0f, L.GetBool(7,false),
		false, GEngine->Audio->GetDefaultObstruction() );
# endif

	if( sound != -1 )
		GEngine->Audio->Sound3D_Forget( sound );
/*
	int sound = GEngine->Audio->Sound3D_Create( buf, 0 );
	if( sound != -1 )
	{
		GEngine->Audio->Sound3D_SetLoopCount( sound, 1 );
		GEngine->Audio->Sound3D_SetPosition( sound, pos );
		GEngine->Audio->Sound3D_SetHearingDistance( sound, mindist, maxdist );
		GEngine->Audio->Sound3D_SetVolume( sound, 1.0f );
		if( L.GetBool(7,false) )
			GEngine->Audio->Sound3D_SetNoSpeedRandomize( sound );
		GEngine->Audio->Sound3D_PlayAndForget( sound );
	}
*/
	L.PushInt(sound);
#endif
	return 1;
}

// This is saved/loaded in MilesEngine::LoadAudio()
int channels[4] = {-1,-1,-1,-1};

// =========================================================================
static int luaSound_StreamLoad(lua_State* state)
{
#ifndef TRANSGAMING_NO_MILES
    if (!GEngine->Audio) return 0;

# ifdef TRANSGAMING
	Script  L(state);
	int     ch = L.GetInt(1);


    if( channels[ch] != -1 )
		GEngine->Audio->StreamingSound_Delete( channels[ch] );

	snprintf(soundTmpBuf, MAX_PATH,"../Data/Music/%s.mp3",L.GetString(2));
	channels[ch] = GEngine->Audio->StreamingSound_Create(soundTmpBuf);
# else
	static char buf[MAX_PATH];
	Script L(state);

	int ch = L.GetInt(1);
	if( channels[ch] != -1 )
		GEngine->Audio->StreamingSound_Delete( channels[ch] );

	sprintf(buf,"../Data/Music/%s.mp3",L.GetString(2));
	channels[ch] = GEngine->Audio->StreamingSound_Create(buf);
# endif
#endif
	return 0;
}
// =========================================================================
static int luaSound_StreamPlay(lua_State* state)
{
#ifndef TRANSGAMING_NO_MILES
    if (!GEngine->Audio) return 0;
	Script L(state);
	int ch = L.GetInt(1);
	bool loop = L.GetBool(2, true);
	if( channels[ch] != -1 )
	{
		GEngine->Audio->StreamingSound_SetVolume( channels[ch],0 );
		if( loop )
			GEngine->Audio->StreamingSound_SetLoopCount( channels[ch], 0 );
		GEngine->Audio->StreamingSound_Play( channels[ch] );
		GEngine->Audio->StreamingSound_SetVolume( channels[ch],0 );
	}
#endif
	return 0;
}
// =========================================================================
static int luaSound_StreamDelete(lua_State* state)
{
#ifndef TRANSGAMING_NO_MILES
    if (!GEngine->Audio) return 0;
    Script L(state);
    int ch = L.GetInt(1);

    if( channels[ch] != -1 )
        GEngine->Audio->StreamingSound_Delete( channels[ch] );

    channels[ch] = -1;
#endif
    return 0;
}
// =========================================================================
static int luaSound_StreamSetVolume(lua_State* state)
{
#ifndef TRANSGAMING_NO_MILES
    if (!GEngine->Audio) return 0;
	Script L(state);
	int ch = L.GetInt(1);
	if( channels[ch] != -1 )
		GEngine->Audio->StreamingSound_SetVolume( channels[ch], L.GetFloat(2)/100.0f );
#endif
	return 0;
}
// =========================================================================
static int luaSound_StreamGetVolume(lua_State* state)
{
	Script L(state);
#ifdef TRANSGAMING_NO_MILES
	L.PushFloat(0.0f);
#else
    if (!GEngine->Audio)
	{
		L.PushFloat(0.0f);
		return 0;
	}
	int ch = L.GetInt(1);
	if( channels[ch] != -1 )
	{
		float vol;
		GEngine->Audio->StreamingSound_GetVolume( channels[ch], vol );
		L.PushFloat(vol*100.0f);
	}
	else
		L.PushFloat(0.0f);
#endif
	return 1;
}
// =========================================================================
static int luaSound_StreamSetLowPass(lua_State* state)
{
#ifndef TRANSGAMING_NO_MILES
    if (!GEngine->Audio) return 0;
	Script L(state);
	int ch = L.GetInt(1);
	if( channels[ch] != -1 )
		GEngine->Audio->StreamingSound_SetLowPass( channels[ch], L.GetFloat(2) );
#endif
	return 0;
}
// =========================================================================
static int luaSound_StreamGetLowPass(lua_State* state)
{
#ifdef TRANSGAMING_NO_MILES
	return 0;
#else
    if (!GEngine->Audio) return 0;

	Script L(state);
	int ch = L.GetInt(1);
	if( channels[ch] != -1 )
		L.PushFloat( GEngine->Audio->StreamingSound_GetLowPass( channels[ch] ) );
	else
		L.PushFloat(1.0f);
	return 1;
#endif
}
// =========================================================================
static int luaSound_StreamPause(lua_State* state)
{
#ifndef TRANSGAMING_NO_MILES
    if (!GEngine->Audio) return 0;
	Script L(state);
	int ch = L.GetInt(1);
	if( channels[ch] != -1 )
		GEngine->Audio->StreamingSound_Stop( channels[ch] );
#endif
	return 0;
}
// =========================================================================
static int luaSound_StreamResume(lua_State* state)
{
#ifndef TRANSGAMING_NO_MILES
	if( !GEngine->Audio ) return 0;
	Script L(state);
	int ch = L.GetInt(1);
	if( channels[ch] != -1 )
    {
        GEngine->Audio->StreamingSound_SetLoopCount( channels[ch], 0 );
		GEngine->Audio->StreamingSound_Resume( channels[ch] );
    }
#endif
	return 0;
}
// =========================================================================
static int luaSound_SetPlayerPos(lua_State* state)
{
#ifndef TRANSGAMING_NO_MILES
    if (!GEngine->Audio) return 0;
	Script L(state);
	Vector pos(L.GetFloat(1),L.GetFloat(2),L.GetFloat(3));
	GEngine->Audio->SetPlayerPosition(pos);
    Vector vel(L.GetFloat(4,0),L.GetFloat(5,0),L.GetFloat(6,0));
	GEngine->Audio->SetPlayerVelocity(vel);
#endif
	return 0;
}
// =========================================================================
static int luaSound_SetPlayerOrientation(lua_State* state)
{
#ifndef TRANSGAMING_NO_MILES
    if (!GEngine->Audio) return 0;
	Script L(state);
	Vector ang(L.GetFloat(1),L.GetFloat(2),L.GetFloat(3));
	Vector up(0,1,0);
	GEngine->Audio->SetPlayerOrientation(ang , up);
#endif
	return 0;
}
// =========================================================================
static int luaSound_SetMasterVolume(lua_State* state)
{
#ifndef TRANSGAMING_NO_MILES
    if (!GEngine->Audio) return 0;
	Script L(state);
	GEngine->Audio->SetMasterVolumeLevel(L.GetFloat(1)/100.0f);
#endif
	return 0;
}
// =========================================================================
static int luaSound_GetNumOfProviders(lua_State *state)
{
	Script L(state);
#ifdef TRANSGAMING_NO_MILES
	L.PushInt( 0 );
#else
	L.PushInt( GEngine->Audio?GEngine->Audio->NumOfProviders():0 );
#endif
	return 1;
}
// =========================================================================
static int luaSound_SetNextSuccesful3DSoundProvider(lua_State* state)
{
	Script L(state);
#ifdef TRANSGAMING_NO_MILES
	L.PushString( "None" );
#else
	if( GEngine->Audio )
	{
		GEngine->Audio->SetFirstSuccesfulProvider( GEngine->Audio->GetCurrentSoundProvider() );
		L.PushString( GEngine->Audio->GetProviderName(GEngine->Audio->GetCurrentSoundProvider()).AsChar() );
	}
	else
		L.PushString( "None" );
#endif
	return 1;
}
// =========================================================================
static int luaSound_Set3DSoundProvider(lua_State* state)
{
	Script L(state);
#ifdef TRANSGAMING_NO_MILES
	L.PushBool( false );
#else
	L.PushBool( GEngine->Audio?GEngine->Audio->Set3DSoundProvider( L.GetString( 1, "None" ) ):false );
#endif
	return 1;
}
// =========================================================================
static int luaSound_Get3DSoundProviderName(lua_State* state)
{
	Script L(state);
#ifdef TRANSGAMING_NO_MILES
	L.PushString("No Audio");
#else
	if (!GEngine->Audio) {L.PushString("No Audio"); return 1;}
	L.PushString( GEngine->Audio->GetProviderName(L.GetInt(1)).AsChar() );
#endif
	return 1;
}
// =========================================================================
static int luaSound_GetCurrent3DSoundProviderName(lua_State* state)
{
	Script L(state);
#ifdef TRANSGAMING_NO_MILES
	L.PushString("No Audio");
#else
	if (!GEngine->Audio) {L.PushString("No Audio"); return 1;}
	L.PushString( GEngine->Audio->GetProviderName(GEngine->Audio->GetCurrentSoundProvider()).AsChar() );
#endif
	return 1;
}
// =========================================================================
static int luaSound_Set3DSoundProviderByName(lua_State* state)
{
#ifndef TRANSGAMING_NO_MILES
    if (!GEngine->Audio) return 0;
	Script L(state);
	const char *name = L.GetString( 1, "" );
	GEngine->Audio->Set3DSoundProvider(name);
#endif
	return 0;
}
// =========================================================================
static int luaSound_Set3DSoundFalloff(lua_State* state)
{
#ifndef TRANSGAMING_NO_MILES
    if (!GEngine->Audio) return 0;
	Script L(state);
	GEngine->Audio->SetFalloffSpeed( L.GetFloat( 1, 1.0f ) );
#endif
	return 0;
}
// =========================================================================
static int luaSound_Get3DSoundFalloff(lua_State* state)
{
#ifdef TRANSGAMING_NO_MILES
	return 0;
#else
    if (!GEngine->Audio) return 0;
	Script L(state);
	L.PushFloat( GEngine->Audio->GetFalloffSpeed() );
	return 1;
#endif
}
// =========================================================================
static int luaSound_ApplySoundSettings(lua_State* state)
{
#ifndef TRANSGAMING_NO_MILES
    if (!GEngine->Audio) return 0;
	Script L(state);

	// Set volumes
	GEngine->Audio->SetMasterVolumeLevel( L.GetFloat(1,100.0f)/100.0f );
	//GEngine->Audio->SetStreamingVolume( L.GetFloat(2,100.0f)/100.0f );
    // juz nieaktualne - streamy ustawiamy niezalezni
	GEngine->Audio->Set3DDigitalEffectsVolume( L.GetFloat(3,100.0f)/100.0f );

	// Set speaker type
	String speakers = L.GetString( 4, "Two Speakers" );
	if( speakers == "Two Speakers" )
		GEngine->Audio->SetSpeakerType( Speakers2 );
	else if( speakers == "Headphones" )
		GEngine->Audio->SetSpeakerType( Headphones );
	else if( speakers == "Surround" )
		GEngine->Audio->SetSpeakerType( Surround );
	else if( speakers == "Four Speakers" )
		GEngine->Audio->SetSpeakerType( Speakers4 );
	else if( speakers == "Five-One" )
		GEngine->Audio->SetSpeakerType( Speakers51 );
	else if( speakers == "Seven-One" )
		GEngine->Audio->SetSpeakerType( Speakers71 );
	else
		GEngine->Audio->SetSpeakerType( Speakers2 );

	bool reverse = L.GetBool( 6, false );
	GEngine->Audio->SetSpeakersFlip( reverse );

	String provider = L.GetString( 7, "None" );
	int oldProv = GEngine->Audio->GetCurrentSoundProvider();
	if( oldProv != -1 )
	{
		String oldProvName = GEngine->Audio->GetProviderName(oldProv);
		if( oldProvName != provider )
		{
			if( !GEngine->Audio->Set3DSoundProvider( provider ) )
				GEngine->Audio->Set3DSoundProvider( oldProv );
		}
	}
	else
		GEngine->Audio->Set3DSoundProvider( provider );
#endif
	return 0;
}
// =========================================================================
static int luaSound_SetRoomType(lua_State* state)
{
#ifndef TRANSGAMING_NO_MILES
    if (!GEngine->Audio) return 0;
	Script L(state);
	GEngine->Audio->SetRoom(L.GetInt(1));
    GEngine->Audio->SetRoomIntensity2D(L.GetFloat(2,1.0f),L.GetFloat(3,2.0f));
    GEngine->Audio->SetRoomIntensity3D(L.GetFloat(2,1.0f),L.GetFloat(3,2.0f));
#endif
	return 0;
}
// =========================================================================
static int luaSound_SetSoundSpeedRandomizer(lua_State* state)
{
#ifndef TRANSGAMING_NO_MILES
    if (!GEngine->Audio) return 0;
	Script L(state);
	GEngine->Audio->SetSoundSpeedRandomizer(L.GetFloat(1));
#endif
	return 0;
}
// =========================================================================
static int luaSound_SetSoundProperties(lua_State* state)
{
#ifndef TRANSGAMING_NO_MILES
    if (!GEngine->Audio) return 0;
	Script L(state);

# ifdef TRANSGAMING
	const char *name = L.GetString(1);


	if (strcmp(name, "default"))
		snprintf(soundTmpBuf, MAX_PATH, soundFileFormatStr, name);

    else
		snprintf(soundTmpBuf, MAX_PATH, "default");


	GEngine->Audio->SetSoundProperties(soundTmpBuf, L.GetInt(2), L.GetInt(3));
# else
	static char buf[MAX_PATH];
	String name = L.GetString(1);
	if( name != "default" )
		sprintf(buf, soundFileFormatStr,name.AsChar());
	else
		sprintf(buf,"default");
	GEngine->Audio->SetSoundProperties(String(buf),L.GetInt(2),L.GetInt(3));
# endif
#endif
	return 0;
}
// =========================================================================
static int luaSound_GlobalSetLowPass(lua_State* state)
{
#ifndef TRANSGAMING_NO_MILES
    if (!GEngine->Audio) return 0;
	Script L(state);
	GEngine->Audio->SetAllSoundsToLowPass( L.GetFloat(1) );
#endif
	return 0;
}
// =========================================================================
static int luaSound_PreloadFile(lua_State* state)
{
#ifndef TRANSGAMING_NO_MILES
    if (!GEngine->Audio) return 0;
    Script L(state);

    GEngine->Audio->PreloadFile(L.GetString(1));
#endif
    return 0;
}
// =========================================================================
static int luaSound_Reset(lua_State* state)
{
#ifndef TRANSGAMING_NO_MILES
	if( !GEngine->Audio ) return 0;
	Script L(state);

	GLog.Print( "Sound Reset\n" );
	GEngine->Audio->Reset();
#endif
	return 0;
}
// =========================================================================
static int luaSound_StopAllSounds(lua_State* state)
{
#ifndef TRANSGAMING_NO_MILES
	if( !GEngine->Audio ) return 0;
	Script L(state);

	GLog.Print( "Stop All Sounds\n" );
	GEngine->Audio->StopAllSounds();
#endif
	return 0;
}
// =========================================================================
static int luaSound_SaveGame_ResumeSounds(lua_State* state)
{
#ifndef TRANSGAMING_NO_MILES
    if( !GEngine->Audio ) return 0;
    Script L(state);

    GEngine->Audio->SaveGame_ResumeSounds();
#endif
    return 0;
}
// =========================================================================
static int luaSound_SetObstructionProperties(lua_State* state)
{
#ifndef TRANSGAMING_NO_MILES
	if( !GEngine->Audio ) return 0;
	Script L(state);

	float heightenDelay = L.GetFloat( 1, 4.0f );
	float lowerDelay = L.GetFloat( 2, 0.2f );
	float minObstruction = L.GetFloat( 3, 0.0f );
	float maxObstructionLow = L.GetFloat( 4, 0.4f );
	float maxObstructionHigh = L.GetFloat( 5, 0.7f );
	float minObstructionDistance = L.GetFloat( 6, 14.0f );
	float maxObstructionDistance = L.GetFloat( 7, 30.0f );

	GLog.Print( "Sound Set Obstruction Properties\n" );
	GLog.Print( "Heighten Obstruction Delay: %f\n", heightenDelay );
	GLog.Print( "Lower Obstruction Delay: %f\n", lowerDelay );
	GLog.Print( "Minimal Obstruction (when not obstructed): %f\n", minObstruction );
	GLog.Print( "Lowest Obstruction (when obstructed, close): %f\n", maxObstructionLow );
	GLog.Print( "Highest Obstruction (when obstructed, far away): %f\n", maxObstructionHigh );
	GLog.Print( "Maximal Distance with lowest obstruction: %f\n", minObstructionDistance );
	GLog.Print( "Distance beyond which obstruction is highest: %f\n", maxObstructionDistance );

	GEngine->Audio->SetObstructionProperties( heightenDelay, lowerDelay,
		minObstruction, maxObstructionLow, maxObstructionHigh, minObstructionDistance, maxObstructionDistance );
#endif
	return 0;
}
// =========================================================================
static const luaL_reg sound_lib[] = {
	{"Play2D", luaSound_Play2D},
	{"Play3D", luaSound_Play3D},
	{"SetPlayerPos", luaSound_SetPlayerPos},
	{"SetPlayerOrientation", luaSound_SetPlayerOrientation},
	{"StreamLoad", luaSound_StreamLoad},
	{"StreamPlay", luaSound_StreamPlay},
    {"StreamDelete", luaSound_StreamDelete},
	{"StreamPause", luaSound_StreamPause},
	{"StreamResume", luaSound_StreamResume},
	{"StreamSetVolume", luaSound_StreamSetVolume},
	{"StreamGetVolume", luaSound_StreamGetVolume},
	{"StreamSetLowPass", luaSound_StreamSetLowPass},
	{"StreamGetLowPass", luaSound_StreamGetLowPass},
	{"SetMasterVolume", luaSound_SetMasterVolume},
	{"ApplySoundSettings", luaSound_ApplySoundSettings},
	{"SetObstructionProperties", luaSound_SetObstructionProperties},
	{"GetNumOfProviders", luaSound_GetNumOfProviders},
	{"SetNextSuccesful3DSoundProvider", luaSound_SetNextSuccesful3DSoundProvider},
	{"Set3DSoundProvider", luaSound_Set3DSoundProvider},
	{"GetCurrent3DSoundProviderName", luaSound_GetCurrent3DSoundProviderName},
	{"Get3DSoundProviderName", luaSound_Get3DSoundProviderName},
	{"Set3DSoundProviderByName", luaSound_Set3DSoundProviderByName},
	{"Set3DSoundFalloff", luaSound_Set3DSoundFalloff},
	{"Get3DSoundFalloff", luaSound_Get3DSoundFalloff},
	{"SetRoomType", luaSound_SetRoomType},
	{"SetSoundSpeedRandomizer", luaSound_SetSoundSpeedRandomizer},
	{"SetSoundProperties", luaSound_SetSoundProperties},
	{"GlobalSetLowPass", luaSound_GlobalSetLowPass},
    {"PreloadFile", luaSound_PreloadFile},
	{"Reset", luaSound_Reset},
    {"SaveGame_ResumeSounds", luaSound_SaveGame_ResumeSounds},
	{"StopAllSounds", luaSound_StopAllSounds},
	{NULL, NULL}
};
// =========================================================================
// Sound3D
// =========================================================================
static int luaSound3D_Create(lua_State* state)
{
	Script L(state);

#ifdef TRANSGAMING_NO_MILES
    L.PushInt(-1);
#else
    if (!GEngine->Audio) {L.PushInt(-1); return 1;}

# ifdef TRANSGAMING
	snprintf(soundTmpBuf, MAX_PATH, soundFileFormatStr, L.GetString(1));
	int sound = GEngine->Audio->Sound3D_Create( soundTmpBuf, 0 );
# else
	static char buf[MAX_PATH];

    sprintf(buf, soundFileFormatStr,L.GetString(1));
	int sound = GEngine->Audio->Sound3D_Create( buf, 0 );
# endif
	L.PushInt( sound );
#endif
	return 1;
}
// =========================================================================
static int luaSound3D_Delete(lua_State* state)
{
#ifndef TRANSGAMING_NO_MILES
	Script L(state);

	int sound = L.GetInt(1,-1);
	if( GEngine->Audio && ( sound != -1 ) )
		GEngine->Audio->Sound3D_Delete( sound );
#endif
	return 0;
}
// =========================================================================
static int luaSound3D_Play(lua_State* state)
{
#ifndef TRANSGAMING_NO_MILES
	Script L(state);

	int sound = L.GetInt(1,-1);
	if( GEngine->Audio && ( sound != -1 ) )
		GEngine->Audio->Sound3D_Play( sound );
#endif
	return 0;
}
// =========================================================================
static int luaSound3D_DisableRandomize(lua_State* state)
{
#ifndef TRANSGAMING_NO_MILES
	Script L(state);

	int sound = L.GetInt(1,-1);
	if( GEngine->Audio && ( sound != -1 ) )
		GEngine->Audio->Sound3D_SetNoSpeedRandomize( sound );
#endif
	return 0;
}

// =========================================================================
static int luaSound3D_PlayAndForget(lua_State* state)
{
#ifndef TRANSGAMING_NO_MILES
	Script L(state);

	int sound = L.GetInt(1,-1);
	if( GEngine->Audio && ( sound != -1 ) )
		GEngine->Audio->Sound3D_PlayAndForget( sound );
#endif
	return 0;
}
// =========================================================================
static int luaSound3D_Forget(lua_State* state)
{
#ifndef TRANSGAMING_NO_MILES
	Script L(state);
	int sound = L.GetInt(1,-1);
	if( GEngine->Audio && ( sound != -1 ) )
		GEngine->Audio->Sound3D_Forget( sound );
#endif
	return 0;
}
// =========================================================================
static int luaSound2D_Forget(lua_State* state)
{
#ifndef TRANSGAMING_NO_MILES
	Script L(state);
	int sound = L.GetInt(1,-1);
	if( GEngine->Audio && ( sound != -1 ) ) GEngine->Audio->Sound2D_Forget( sound );
#endif
	return 0;
}

// =========================================================================
static int luaSound3D_IsPlaying(lua_State* state)
{
	Script L(state);

	bool ret = false;
#ifndef TRANSGAMING_NO_MILES
	int sound = L.GetInt(1,-1);
	if( GEngine->Audio && ( sound != -1 ) )
		ret = GEngine->Audio->Sound3D_IsPlaying( sound );
#endif
	L.PushBool(ret);
	return 1;
}
// =========================================================================
static int luaSound3D_Pause(lua_State* state)
{
#ifndef TRANSGAMING_NO_MILES
	Script L(state);

	int sound = L.GetInt(1,-1);
	if( GEngine->Audio && ( sound != -1 ) )
		GEngine->Audio->Sound3D_Stop( sound );
#endif
	return 0;
}
// =========================================================================
static int luaSound3D_Resume(lua_State* state)
{
#ifndef TRANSGAMING_NO_MILES
	Script L(state);

	int sound = L.GetInt(1,-1);
	if( GEngine->Audio && ( sound != -1 ) )
		GEngine->Audio->Sound3D_Resume( sound );
#endif
	return 0;
}
// =========================================================================
static int luaSound3D_Stop(lua_State* state)
{
#ifndef TRANSGAMING_NO_MILES
	Script L(state);

	int sound = L.GetInt(1,-1);
	if( GEngine->Audio && ( sound != -1 ) )
		GEngine->Audio->Sound3D_Stop( sound );
#endif
	return 0;
}
// =========================================================================
static int luaSound3D_SetLoopCount(lua_State* state)
{
#ifndef TRANSGAMING_NO_MILES
	Script L(state);

	int sound = L.GetInt(1,-1);
	if( GEngine->Audio && ( sound != -1 ) )
		GEngine->Audio->Sound3D_SetLoopCount( sound, L.GetInt(2) );
#endif
	return 0;
}
// =========================================================================
static int luaSound3D_SetVolume(lua_State* state)
{
#ifndef TRANSGAMING_NO_MILES
	Script L(state);

	int sound = L.GetInt(1,-1);
	if( GEngine->Audio && ( sound != -1 ) )
		GEngine->Audio->Sound3D_SetVolume( sound, L.GetFloat(2)/100.0f, L.GetFloat(3,0.0f) );
#endif
	return 0;
}
// =========================================================================
static int luaSound3D_SetLowPass(lua_State* state)
{
#ifndef TRANSGAMING_NO_MILES
	Script L(state);
	int sound = L.GetInt(1,-1);
	if( GEngine->Audio && ( sound != -1 ) )
		GEngine->Audio->Sound3D_SetLowPass( sound, L.GetFloat(2) );
#endif
	return 0;
}
// =========================================================================
static int luaSound3D_GetLowPass(lua_State* state)
{
	Script L(state);
#ifdef TRANSGAMING_NO_MILES
	L.PushFloat( 1.0f );
#else
	int sound = L.GetInt(1,-1);
	L.PushFloat( ( GEngine->Audio && ( sound != -1 ) )?GEngine->Audio->Sound3D_GetLowPass( sound ):1.0f );
#endif
	return 1;
}
// =========================================================================
static int luaSound3D_SetPosition(lua_State* state)
{
#ifndef TRANSGAMING_NO_MILES
	Script L(state);

	int sound = L.GetInt(1,-1);
	if( GEngine->Audio && ( sound != -1 ) )
	{
		Vector v(L.GetFloat(2),L.GetFloat(3),L.GetFloat(4));
		GEngine->Audio->Sound3D_SetPosition( sound, v );
	}
#endif
	return 0;
}
// =========================================================================
static int luaSound3D_SetOrientation(lua_State* state)
{
#ifndef TRANSGAMING_NO_MILES
	Script L(state);

	int sound = L.GetInt(1,-1);
	if( GEngine->Audio && ( sound != -1 ) )
	{
		Vector v(L.GetFloat(2),L.GetFloat(3),L.GetFloat(4));
		Vector up(0,1,0);
		GEngine->Audio->Sound3D_SetOrientation( sound, v, up );
	}
#endif
	return 0;
}
// =========================================================================
static int luaSound3D_SetVelocity(lua_State* state)
{
#ifndef TRANSGAMING_NO_MILES
	Script L(state);

	int sound = L.GetInt(1,-1);
	if( GEngine->Audio && ( sound != -1 ) )
	{
		Vector v(L.GetFloat(2),L.GetFloat(3),L.GetFloat(4));
		GEngine->Audio->Sound3D_SetVelocity( sound, v );
	}
#endif
	return 0;
}
// =========================================================================
static int luaSound3D_SetHearingDistance(lua_State* state)
{
#ifndef TRANSGAMING_NO_MILES
	Script L(state);
	int sound = L.GetInt(1,-1);
	if( GEngine->Audio && ( sound != -1 ) )
		GEngine->Audio->Sound3D_SetHearingDistance( sound, L.GetFloat(2), L.GetFloat(3) );
#endif
	return 0;
}
// =========================================================================
static int luaSound3D_SetObstructed(lua_State* state)
{
#ifndef TRANSGAMING_NO_MILES
	Script L(state);

	int sound = L.GetInt(1,-1);
	if( GEngine->Audio && ( sound != -1 ) )
		GEngine->Audio->Sound3D_SetObstructed( sound, L.GetBool(2) );
#endif
	return 0;
}
// =========================================================================
static int luaSound3D_SetIntensity(lua_State* state)
{
#ifndef TRANSGAMING_NO_MILES
	Script L(state);

	int sound = L.GetInt(1,-1);
	if( GEngine->Audio && ( sound != -1 ) )
		GEngine->Audio->Sound3D_SetIntensity( sound, L.GetFloat(2), L.GetFloat(3,0.0f) );
#endif
	return 0;
}
// =========================================================================
static const luaL_reg sound3d_lib[] = {
{"Create", luaSound3D_Create},
{"Delete", luaSound3D_Delete},

{"Play", luaSound3D_Play},
{"PlayAndForget", luaSound3D_PlayAndForget},
{"Forget", luaSound3D_Forget},
{"Pause", luaSound3D_Pause},
{"Resume", luaSound3D_Resume},
{"Stop", luaSound3D_Stop},
{"IsPlaying", luaSound3D_IsPlaying},

{"SetLoopCount", luaSound3D_SetLoopCount},
{"SetVolume", luaSound3D_SetVolume},
{"SetLowPass", luaSound3D_SetLowPass},
{"GetLowPass", luaSound3D_GetLowPass},
{"SetPosition", luaSound3D_SetPosition},
{"SetOrientation", luaSound3D_SetOrientation},
{"SetVelocity", luaSound3D_SetVelocity},
{"SetHearingDistance", luaSound3D_SetHearingDistance},
{"SetObstructed", luaSound3D_SetObstructed},
{"SetIntensity", luaSound3D_SetIntensity},
{"DisableRandomize", luaSound3D_DisableRandomize},
{NULL, NULL}
};
// =========================================================================
// Sound2D
// =========================================================================
static int luaSound2D_Create(lua_State* state)
{
	Script L(state);

#ifdef TRANSGAMING_NO_MILES
	L.PushInt( -1);
#else
    if (!GEngine->Audio)
	{
		L.PushInt( -1);
		return 1;
	}

# ifdef TRANSGAMING
    snprintf(soundTmpBuf, MAX_PATH, soundFileFormatStr, L.GetString(1));

	L.PushInt( GEngine->Audio->Sound2D_CreateEx( soundTmpBuf, 0, 1.0f, L.GetBool( 2, false ), L.GetBool( 3, false ), false, 1.0f, L.GetBool( 4, false ) ) );
# else
	static char buf[MAX_PATH];


    sprintf(buf, soundFileFormatStr,L.GetString(1));

	L.PushInt( GEngine->Audio->Sound2D_CreateEx( buf, 0, 1.0f, L.GetBool( 2, false ), L.GetBool( 3, false ), false, 1.0f, L.GetBool( 4, false ) ) );
# endif
#endif
	return 1;
}
// =========================================================================
static int luaSound2D_Delete(lua_State* state)
{
#ifndef TRANSGAMING_NO_MILES
	Script L(state);
	int sound = L.GetInt(1,-1);
	if( GEngine->Audio && ( sound != -1 ) ) GEngine->Audio->Sound2D_Delete( sound );
#endif
	return 0;
}
// =========================================================================
static int luaSound2D_Play(lua_State* state)
{
#ifndef TRANSGAMING_NO_MILES
	Script L(state);

	int sound = L.GetInt(1,-1);
	if( GEngine->Audio && ( sound != -1 ) )
		GEngine->Audio->Sound2D_Play( sound );
#endif
	return 0;
}
// =========================================================================
static int luaSound2D_PlayAndForget(lua_State* state)
{
#ifndef TRANSGAMING_NO_MILES
	Script L(state);

	int sound = L.GetInt(1,-1);
	if( GEngine->Audio && ( sound != -1 ) )
		GEngine->Audio->Sound2D_PlayAndForget( sound );
#endif
	return 0;
}
// =========================================================================
static int luaSound2D_DisableRandomize(lua_State* state)
{
#ifndef TRANSGAMING_NO_MILES
	Script L(state);

	int sound = L.GetInt(1,-1);
	if( GEngine->Audio && ( sound != -1 ) )
		GEngine->Audio->Sound2D_NoSpeedRandomize( sound );
#endif
	return 0;
}
// =========================================================================
static int luaSound2D_IsPlaying(lua_State* state)
{
	Script L(state);

	bool ret = false;
#ifndef TRANSGAMING_NO_MILES
	int sound = L.GetInt(1,-1);
	if( GEngine->Audio && ( sound != -1 ) )
		ret = GEngine->Audio->Sound2D_IsPlaying( sound );
#endif
	L.PushBool( ret );
	return 1;
}
// =========================================================================
static int luaSound2D_Pause(lua_State* state)
{
#ifndef TRANSGAMING_NO_MILES
	Script L(state);

	int sound = L.GetInt(1,-1);
	if( GEngine->Audio && ( sound != -1 ) )
		GEngine->Audio->Sound2D_Stop( sound );
#endif
	return 0;
}
// =========================================================================
static int luaSound2D_Resume(lua_State* state)
{
#ifndef TRANSGAMING_NO_MILES
	Script L(state);

	int sound = L.GetInt(1,-1);
	if( GEngine->Audio && ( sound != -1 ) )
		GEngine->Audio->Sound2D_Resume( sound );
#endif
	return 0;
}
// =========================================================================
static int luaSound2D_Stop(lua_State* state)
{
#ifndef TRANSGAMING_NO_MILES
	Script L(state);

	int sound = L.GetInt(1,-1);
	if( GEngine->Audio && ( sound != -1 ) )
		GEngine->Audio->Sound2D_Stop( sound );
#endif
	return 0;
}
// =========================================================================
static int luaSound2D_SetLoopCount(lua_State* state)
{
#ifndef TRANSGAMING_NO_MILES
	Script L(state);

	int sound = L.GetInt(1,-1);
	if( GEngine->Audio && ( sound != -1 ) )
		GEngine->Audio->Sound2D_SetLoopCount( sound, L.GetInt(2) );
#endif
	return 0;
}
// =========================================================================
static int luaSound2D_SetVolume(lua_State* state)
{
#ifndef TRANSGAMING_NO_MILES
	Script L(state);

	int sound = L.GetInt(1,-1);
	if( GEngine->Audio && ( sound != -1 ) )
		GEngine->Audio->Sound2D_SetVolume( sound, L.GetFloat(2)/100.0f, L.GetFloat(3,0.0f) );
#endif
	return 0;
}
// =========================================================================
static int luaSound2D_SetLowPass(lua_State* state)
{
#ifndef TRANSGAMING_NO_MILES
	Script L(state);
	int sound = L.GetInt(1,-1);
	if( GEngine->Audio && ( sound != -1 ) )
		GEngine->Audio->Sound2D_SetLowPass( sound, L.GetFloat(2) );
#endif
	return 0;
}
// =========================================================================
static int luaSound2D_GetLowPass(lua_State* state)
{
	Script L(state);
	int sound = L.GetInt(1,-1);
#ifdef TRANSGAMING_NO_MILES
	L.PushFloat( 1.0f );
#else
	L.PushFloat( (GEngine->Audio&&(sound!=-1))?GEngine->Audio->Sound2D_GetLowPass( sound ):1.0f );
#endif
	return 1;
}

// =========================================================================
static int luaSound2D_SetSoundSpeed(lua_State* state)
{
#ifndef TRANSGAMING_NO_MILES
	Script L(state);
	int sound = L.GetInt(1,-1);
	if( GEngine->Audio && ( sound != -1 ) )
		GEngine->Audio->Sound2D_SetSoundSpeed( sound, L.GetFloat(2,1.0f) );
#endif
	return 1;
}
// =========================================================================
static const luaL_reg sound2d_lib[] = {
	{"Create", luaSound2D_Create},
	{"Delete", luaSound2D_Delete},

	{"Play", luaSound2D_Play},
	{"Forget", luaSound2D_Forget},
	{"PlayAndForget", luaSound2D_PlayAndForget},
	{"Pause", luaSound2D_Pause},
	{"Resume", luaSound2D_Resume},
	{"Stop", luaSound2D_Stop},
	{"IsPlaying", luaSound2D_IsPlaying},

	{"SetLoopCount", luaSound2D_SetLoopCount},
	{"SetVolume", luaSound2D_SetVolume},
	{"SetLowPass", luaSound2D_SetLowPass},
	{"GetLowPass", luaSound2D_GetLowPass},
	{"DisableRandomize", luaSound2D_DisableRandomize},
	{"SetSoundSpeed", luaSound2D_SetSoundSpeed},
	{NULL, NULL}
};
// =========================================================================
static int luaSnd_Setup3D(lua_State* state)
{
    Script L(state);
#ifndef TRANSGAMING
	Sound* sound = (Sound*) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeSound);
#else
    Sound* sound = (Sound*) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeSound);
#endif
    if( sound )
        sound->Setup3D(L.GetString(2),L.GetFloat(3,10.0f),L.GetFloat(4,20.0f),L.GetFloat(5,-1.0f),L.GetFloat(6,10.0f),L.GetBool(7,false));
    return 0;
}
// =========================================================================
static int luaSnd_Play(lua_State* state)
{
    Script L(state);
#ifndef TRANSGAMING
	Sound* sound = (Sound*) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeSound);
#else
    Sound* sound = (Sound*) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeSound);
#endif
    if (sound)
        sound->Play(L.GetFloat(2,-1.0f));
	return 0;
}
// =========================================================================
static int luaSnd_GetSound3DPtr(lua_State* state)
{
    Script L(state);
#ifndef TRANSGAMING
	Sound* sound = (Sound*) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeSound);
#else
    Sound* sound = (Sound*) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeSound);
#endif
	int snd = -1;
    if( sound ) snd = sound->GetSound3DIndex();
	L.PushInt( snd );
	return 1;
}
// =========================================================================
static int luaSnd_IsPlaying(lua_State* state)
{
	Script L(state);
#ifndef TRANSGAMING
	Entity *e = GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeSound);
#else
    Entity *e = GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeSound);
#endif
    if (e)
        L.PushBool(true);
    else
        L.PushBool(false);
	return 1;
}

// =========================================================================
static int luaSnd_Stop(lua_State* state)
{
    Script L(state);
#ifndef TRANSGAMING
	Sound* sound = (Sound*) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeSound);
#else
    Sound* sound = (Sound*) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeSound);
#endif
    if (sound)
        sound->Stop();
    return 0;
}
// =========================================================================
static int luaSnd_SetVelocityScaleFactor(lua_State* state)
{
    Script L(state);
#ifndef TRANSGAMING
	Sound* sound = (Sound*) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeSound);
#else
    Sound* sound = (Sound*) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeSound);
#endif
    if (sound)
        sound->SetVelocityScaleFactor(L.GetFloat(2));
    return 0;
}
// =========================================================================
static const luaL_reg snd_lib[] = {
    {"Setup3D", luaSnd_Setup3D},
    {"Play", luaSnd_Play},
    {"Stop", luaSnd_Stop},
	{"IsPlaying", luaSnd_IsPlaying},
    {"SetVelocityScaleFactor", luaSnd_SetVelocityScaleFactor},
	{"GetSound3DPtr", luaSnd_GetSound3DPtr},
	{NULL, NULL},
};
// =========================================================================
// Particle
// =========================================================================
static int luaParticle_Die(lua_State* state)
{
    Script L(state);
#ifndef TRANSGAMING
	ParticleEffect *p = (ParticleEffect *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeParticleFx);
#else
    ParticleEffect *p = (ParticleEffect *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeParticleFx);
#endif
    if (p)
    {
        if (p->Parent)
            p->Parent->UnregisterChild(p);
        for (int i=0;i<p->EmitterCount;i++)
        {
            p->Emitters[i].Emitter->Immortal = FALSE;
            p->Emitters[i].Emitter->Evolve = FALSE;
            p->Emitters[i].Emitter->Dying = TRUE;
        }
    }
    return 0;
}
// =========================================================================
static int luaParticle_SetEvolve(lua_State* state)
{
	Script L(state);
#ifndef TRANSGAMING
	ParticleEffect *p = (ParticleEffect *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeParticleFx);
#else
    ParticleEffect *p = (ParticleEffect *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeParticleFx);
#endif
	if (p)
	{
		for (int i=0;i<p->EmitterCount;i++)
		{
			bool state = L.GetBool(2,true);
			p->Emitters[i].Emitter->Evolve = state;
			if (state)
				p->Emitters[i].Emitter->Active = true;
		}
	}
	return 0;
}
// =========================================================================
static int luaParticle_AddEmitter(lua_State* state)
{
	int i = 0;
	Script L(state);
#ifndef TRANSGAMING
	ParticleEffect *p = (ParticleEffect *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeParticleFx);
#else
    ParticleEffect *p = (ParticleEffect *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeParticleFx);
#endif
	if (p)
		i = p->AddEmitter(GEngine->Video->ParticleSys.CreateEmitter(L.GetString(2)));

	L.PushInt(i);
	return 1;
}
// =========================================================================
static int luaParticle_SetupEmitter(lua_State* state)
{
	Script L(state);
#ifndef TRANSGAMING
	ParticleEffect *p = (ParticleEffect *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeParticleFx);
#else
    ParticleEffect *p = (ParticleEffect *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeParticleFx);
#endif
	if (p)
	{
		int i = L.GetInt(2);
		p->Emitters[i].Scale = L.GetFloat(3);
		p->Emitters[i].Pos = Vector(L.GetFloat(4),L.GetFloat(5),L.GetFloat(6));
		p->Emitters[i].Rotation = Vector(L.GetFloat(7),L.GetFloat(8),L.GetFloat(9));
		p->Emitters[i].SetupTransform();
	}
	return 0;
}
// =========================================================================
static int luaParticle_SetParentOffset(lua_State* state)
{
	Script L(state);
#ifndef TRANSGAMING
	ParticleEffect *p = (ParticleEffect *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeParticleFx);
#else
    ParticleEffect *p = (ParticleEffect *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeParticleFx);
#endif

	if (p)
	{
		Vector v =  Vector(L.GetFloat(2),L.GetFloat(3),L.GetFloat(4));
        int joint = -1;
        if (!L.IsNil(5))
        {
            if (L.GetType(5) == LUA_TNUMBER)
                joint = L.GetInt(5);
            else if (p->Parent && p->Parent->AsModel())
                joint = p->Parent->AsModel()->GetJointIndex(L.GetString(5));
        }
        Vector co =  Vector(L.GetFloat(6,0.0f),L.GetFloat(7,0.0f),L.GetFloat(8,0.0f));
        p->SetParentOffset(v,joint,co);
        if (!(L.IsNil(11))) {
			p->ParentRotation = true;
			p->ParentRotationOffset.FromEulerAnglesXYZ(Vector(L.GetFloat(9,0.0f),L.GetFloat(10,0.0f),L.GetFloat(11,0.0f)));
		}
	}
	return 0;
}
// =========================================================================
static int luaParticle_SetImmortal(lua_State* state)
{
	Script L(state);
#ifndef TRANSGAMING
	ParticleEffect *p = (ParticleEffect *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeParticleFx);
#else
    ParticleEffect *p = (ParticleEffect *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeParticleFx);
#endif
	if (p)
		p->IsImmortal = L.GetBool(2,true);
	return 0;
}

// =========================================================================
static int luaParticle_SetFixedTransform(lua_State* state)
{
	Script L(state);
#ifndef TRANSGAMING
	ParticleEffect *p = (ParticleEffect *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeParticleFx);
#else
    ParticleEffect *p = (ParticleEffect *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeParticleFx);
#endif

	if (p)
		p->FixedTransform = L.GetBool(2,false);

	return 0;
}

// =========================================================================
static int luaParticle_Restart(lua_State* state)
{
	Script L(state);
#ifndef TRANSGAMING
	ParticleEffect *p = (ParticleEffect *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeParticleFx);
#else
    ParticleEffect *p = (ParticleEffect *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeParticleFx);
#endif
	if (p)
		p->Restart();
	return 0;
}
/*static int luaParticle_ForceLastPosition(lua_State* state)
{
	Script L(state);
    ParticleEffect *p = (ParticleEffect *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
	if (p) {
		p->updateOnce = true;
	}
	return 0;
}*/
// =========================================================================
static const luaL_reg particle_lib[] = {
	{"AddEmitter", luaParticle_AddEmitter},
	{"SetupEmitter", luaParticle_SetupEmitter},
	{"SetEvolve", luaParticle_SetEvolve},
    {"Die", luaParticle_Die},
	{"SetParentOffset", luaParticle_SetParentOffset},
	{"SetImmortal", luaParticle_SetImmortal},
	{"Restart", luaParticle_Restart},
	{"SetFixedTransform",luaParticle_SetFixedTransform},
	{NULL, NULL}
};
// =========================================================================
// HUD
// =========================================================================
static int luaHUD_PrintXY(lua_State* state)
{
    if (!GEngine->Video) return 0;
	Script L(state);

	//TextureState st[2];
	//st[0].Set(0,TextureState::ETexOpColor,0,TextureState::ETexFilterPoint);
	//st[1].Set(1,TextureState::ETexOpDisable);
	//GEngine->GetRenderDevice()->SetTextureState(st,ARRAY_COUNT(st));

#ifdef TRANSGAMING
	unsigned char r = L.GetInt(4, 0x00);
	unsigned char g = L.GetInt(5, 0xff);
	unsigned char b = L.GetInt(6, 0x00);
	int color = 0xff000000 | b<<16 | g << 8 | r;
	GEngine->Video->Hud->SetRenderState();


    //make sure a font is set
    if (!GEngine->Video->Hud->isFontSet()){
        TRACE("luaHUD_PrintXY(): WARNING-> no font set!\n");
        GEngine->Video->Hud->SetFont(0);

        //no font is still set => fail
        if (!GEngine->Video->Hud->isFontSet())
            return 0;
    }

	const char* str = L.GetString(3, "");
	int xPos = L.GetInt(1);					// jesli xPos < 0 to centruj w x

	View * view = GEngine->Video->GetActiveView();
	if (xPos < 0) {
		int size = GEngine->Video->Hud->GetTextWidth(str);
		Viewport *pViewport = &view->LocalViewport;
		xPos = (pViewport->Width - size) >> 1;
	}

	int yPos = L.GetInt(2);
	if (yPos < 0) {
		int size = GEngine->Video->Hud->GetTextHeight(str);
		Viewport *pViewport = &view->LocalViewport;
		yPos = (pViewport->Height - size) >> 1;
	}

	GEngine->Video->Hud->Print(xPos,yPos,str, color);
#else
	unsigned char r = L.GetInt(5, 0x00);
	unsigned char g = L.GetInt(6, 0xff);
	unsigned char b = L.GetInt(7, 0x00);
	int color = 0xff000000 | r<<16 | g << 8 | b;
	GEngine->Video->Hud->SetRenderState();

	const char* f = L.GetString(4, NULL);
	int fsize = L.GetInt(8, 0);
	if (f != NULL)
		GEngine->Video->Hud->SetFont(f, fsize);
	else
		GEngine->Video->Hud->SetFont(0);

	const char* str = L.GetString(3);
	int xPos = L.GetInt(1);					// jesli xPos < 0 to centruj w x

	View * view = GEngine->Video->GetActiveView();
	if (xPos < 0) {
		int size = GEngine->Video->Hud->GetTextWidth(str);
		Viewport *pViewport = &view->LocalViewport;
		xPos = (pViewport->Width - size) >> 1;
	}

	int yPos = L.GetInt(2);
	if (yPos < 0) {
		int size = GEngine->Video->Hud->GetTextHeight(str);
		Viewport *pViewport = &view->LocalViewport;
		yPos = (pViewport->Height - size) >> 1;
	}

	GEngine->Video->Hud->Print(xPos,yPos,str, color);
#endif
	return 0;
}

// =========================================================================
static int luaHUD_DrawRect(lua_State* state)
{
    if (!GEngine->Video) return 0;
	Script L(state);
	GEngine->Video->Hud->DrawRect(L.GetFloat(1),L.GetFloat(2),L.GetFloat(3),L.GetFloat(4),L.GetInt(5));
	return 0;
}
// =========================================================================
static int luaHUD_DrawQuad(lua_State* state)
{
    if (!GEngine->Video) return 0;
	Script L(state);
	Texture* t = (Texture*) L.GetLightUD(1);
	DWORD color = L.GetInt(6,-1);

#ifdef TRANSGAMING_PS2
    if (t == NULL)
        t = GEngine->Video->MaterialSys->Textures.White;
#endif
	GEngine->Video->Hud->DrawQuad(t,L.GetFloat(2),L.GetFloat(3),L.GetFloat(4),L.GetFloat(5),color,L.GetFloat(7,0.0f),L.GetFloat(8,0.0f),L.GetFloat(9,1.0f),L.GetFloat(10,1.0f));
	return 0;
}
// =========================================================================
static int luaHUD_DrawQuadRGBA(lua_State* state)
{
    if (!GEngine->Video) return 0;
	Script L(state);
	Texture* t = (Texture*) L.GetLightUD(1);
#ifdef TRANSGAMING_PS2
    if (t == NULL)
        t = GEngine->Video->MaterialSys->Textures.White;
#endif
#ifdef	TRANSGAMING
	DWORD color = Color_TG(L.GetInt(6,255),L.GetInt(7,255),L.GetInt(8,255),L.GetInt(9,255)).dwVal;
#else
	DWORD color = Color(L.GetInt(6,255),L.GetInt(7,255),L.GetInt(8,255),L.GetInt(9,255)).dwVal;
#endif
	GEngine->Video->Hud->DrawQuad(t,L.GetFloat(2),L.GetFloat(3),L.GetFloat(4),L.GetFloat(5),color,L.GetFloat(10,0.01f),L.GetFloat(11,0.01f),L.GetFloat(12,0.99f),L.GetFloat(13,0.99f));
	return 0;
}
// =========================================================================
static int luaHUD_DrawQuadRotated(lua_State* state)
{
    if (!GEngine->Video) return 0;
	Script L(state);
	Texture* t = (Texture*) L.GetLightUD(1);
	POINT pt;
	pt.x = (int)L.GetFloat(7);
	pt.y = (int)L.GetFloat(8);

#ifdef TRANSGAMING_PS2
    if (t == NULL)
        t = GEngine->Video->MaterialSys->Textures.White;
#endif
#ifdef	TRANSGAMING
	DWORD color = Color_TG(L.GetInt(9,255),L.GetInt(10,255),L.GetInt(11,255),L.GetInt(12,255)).dwVal;
#else
	DWORD color = Color(L.GetInt(9,255),L.GetInt(10,255),L.GetInt(11,255),L.GetInt(12,255)).dwVal;
#endif
	GEngine->Video->Hud->DrawQuadRotated(t,L.GetFloat(2),L.GetFloat(3),L.GetFloat(4),L.GetFloat(5),L.GetFloat(6),pt,color);
	return 0;
}
// =========================================================================
static int luaHUD_GetTransparency(lua_State* state)
{
	Script L(state);
	L.PushInt( GEngine->Video->Hud->GetTransparency() );
	return 1;
}
// =========================================================================
static int luaHUD_SetTransparency(lua_State* state)
{
    if (!GEngine->Video) return 0;
	Script L(state);

	int trans = L.GetInt( 1, 100 );

	trans = int( ( float )trans / 100.0f * 255.0f );

	GEngine->Video->Hud->SetTransparency( trans );

	return 0;
}
// =========================================================================
static int luaHUD_DrawBossHealth(lua_State* state)
{
    if (!GEngine->Video) return 0;
	Script L(state);
	int health = L.GetInt( 1 );
	GEngine->Video->Hud->DrawBossHealth( health );
	return 0;
}
// =========================================================================
static int luaHUD_DrawBorder(lua_State* state)
{
    if (!GEngine->Video) return 0;
	Script L(state);
	GEngine->Video->Hud->DrawBorder(L.GetInt(1,0),L.GetInt(2,0),L.GetInt(3,1024),L.GetInt(4,768));
	return 0;
}
// =========================================================================
static int luaHUD_GetTextWidth(lua_State *state)
{
	if (!GEngine->Video) return 0;
	Script L(state);
	const char *text = L.GetString(1,"");
	L.PushInt( GEngine->Video->Hud->GetTextWidth(text) );
	return 1;
}
// =========================================================================
static int luaHUD_GetTextHeight(lua_State *state)
{
	if (!GEngine->Video) return 0;
	Script L(state);
	const char *txt = L.GetString( 1, NULL );
	if( txt )
		L.PushInt( GEngine->Video->Hud->GetTextHeight( txt ) );
	else
		L.PushInt( GEngine->Video->Hud->GetTextHeight() );
	return 1;
}
// =========================================================================
static int luaHUD_SetFont(lua_State *state)
{
	if (!GEngine->Video) return 0;
	Script L(state);
	const char *font = L.GetString(1, "");
	int         size = L.GetInt(2, 0);

    //TRACE("luaHUD_SetFont(): setting the current font to '%s' (size = %d)\n", font, size);
    //GScript.DumpError(-17, __FILE__, __FUNCTION__);
	GEngine->Video->Hud->SetFont(font, size);

	return 0;
}
// =========================================================================
#ifdef TRANSGAMING
static int luaHUD_removeFont(lua_State *state){
    if (!GEngine->Video)
        return 0;


    Script  L(state);
    const char *name = L.GetString(1, "");
    const char *tex =  L.GetString(2, NULL);


    GEngine->Video->Hud->removeFont(name, tex);
    return 0;
}

static int luaHUD_removeAllFonts(lua_State *state){
    if (!GEngine->Video)
        return 0;


    Script L(state);


    GEngine->Video->Hud->removeAllFonts();
    return 0;
}
#endif
// =========================================================================
static int luaHUD_PrepareString(lua_State *state)
{
	if (!GEngine->Video) return 0;
	Script L(state);

	const char *str = L.GetString(1,"");


	L.PushString( HUD_PrepareString(str) );
	return 1;
}
// =========================================================================
#ifndef TRANSGAMING_NO_DEDICATED_SERVER
static int luaHUD_StripColorInfo(lua_State *state)
{
	Script L(state);

#ifdef TRANSGAMING
    const char *str = L.GetString(1, "");
	char        desc[255];
	int         count = 0;


	desc[0] = 0;

	while (str[0]){
		if (*str == '#')
			str++;

		else
			desc[count++] = *str;

		str++;
	}

	desc[count] = 0;

	L.PushString(desc);
#else
    String s = L.GetString(1,"");

	char desc[255];
	desc[0] = 0;
	int count = 0;
	char *tmp = ( char *)s.AsChar();
	while( *tmp )
	{
		if( *tmp == '#' )
		{
			tmp++;
		}
		else
		{
			desc[count++] = *tmp;
		}

		tmp++;
	}

	desc[count] = 0;

	L.PushString( String( desc ).AsChar() );
#endif

	return 1;
}
#endif
// =========================================================================
static int luaHUD_ColorSubstr(lua_State *state)
{
	Script L(state);

#ifdef TRANSGAMING
	const char *str = L.GetString(1, "");
	int         maxCount = L.GetInt(2,100);
	char        desc[255];
	int         count = 0;
    int         realCnt = 0;


	desc[0] = 0;

	while (*str){
		if (*str == '#'){
			desc[count++] = '#';
			str++;
			desc[count++] = *str;
			str++;
		}

		desc[count++] = *str;
		realCnt++;
		str++;
		if (realCnt > maxCount)
			break;
	}

	desc[count] = 0;

	L.PushString(desc);
#else
	String s = L.GetString(1,"");
	int maxCount = L.GetInt(2,100);

	char desc[255];
	desc[0] = 0;
	int count = 0, realCnt = 0;
	char *tmp = ( char *)s.AsChar();
	while( *tmp )
	{
		if( *tmp == '#' )
		{
			desc[count++] = '#';
			tmp++;
			desc[count++] = *tmp;
			tmp++;
		}

		desc[count++] = *tmp;
		realCnt++;
		tmp++;
		if( realCnt > maxCount )
			break;
	}

	desc[count] = 0;

	L.PushString( String( desc ).AsChar() );
#endif

	return 1;
}
// =========================================================================
// eric-> HACK!!  This is the solution to a very strange problem on the PS2.
//   It seems that the hud_lib lua library was *just* on the verge of reaching
//   some kind of size limit (hash buckets, filled buffer, etc?) before the
//   SetFontSize() function was added.  When the function was added, it caused
//   the PS2 build to crash (memory clobber early on in the program).  The
//   solution was to register a dummy function alongside the SetFontSize()
//   function to force the [whatever] limit to be exceeded.
/*static int luaHUD_doStuff(lua_State *state){
    return 0;
}//*/

static const luaL_reg hud_lib[] = {
	{"PrintXY", luaHUD_PrintXY},
	{"DrawRect", luaHUD_DrawRect},
	{"DrawQuad", luaHUD_DrawQuad},
	{"DrawQuadRGBA", luaHUD_DrawQuadRGBA},
	{"DrawQuadRotated", luaHUD_DrawQuadRotated},
	{"SetTransparency", luaHUD_SetTransparency},
	{"GetTransparency", luaHUD_GetTransparency},
	{"DrawBossHealth", luaHUD_DrawBossHealth},
	{"DrawBorder", luaHUD_DrawBorder},
	{"GetTextWidth", luaHUD_GetTextWidth},
	{"GetTextHeight", luaHUD_GetTextHeight},
	{"SetFont", luaHUD_SetFont},
#ifdef TRANSGAMING
    {"RemoveFont", luaHUD_removeFont},
    {"RemoveAllFonts", luaHUD_removeAllFonts},
#endif
/*#ifdef TRANSGAMING
    {"SetFontSize", luaHUD_SetFontSize},
    {"DoStuff", luaHUD_doStuff},
#endif//*/
	{"PrepareString", luaHUD_PrepareString},
#ifndef TRANSGAMING_NO_DEDICATED_SERVER
	{"StripColorInfo", luaHUD_StripColorInfo},
#endif
	{"ColorSubstr", luaHUD_ColorSubstr},
	{NULL, NULL}
};
// =========================================================================
static int luaMaterial_Create(lua_State* state)
{
    Script L(state);
    if (!GEngine->Video) {L.PushNil();return 1;}

    const char *filename = L.GetString(1);


    Texture* tex = GEngine->Video->MaterialSys->CreateTexture(filename,L.GetInt(2,0));

    if (tex == NULL)
        TRACE("MATERIAL.Create(): ERROR-> could not load the texture '%s'\n", filename);

	L.PushLightUD(tex);
	return 1;
}
// =========================================================================
static int luaMaterial_Release(lua_State* state)
{
	Script L(state);
	Texture* m = (Texture*) L.GetLightUD(1);
	if (m && GEngine->Video)
		GEngine->Video->MaterialSys->ReleaseTexture(m);
	return 0;
}
// =========================================================================
static int luaMaterial_Replace(lua_State* state)
{
	Script L(state);
	if (GEngine->Video)
		GEngine->Video->MaterialSys->ReplaceTexture(L.GetString(1),L.GetString(2));
	return 0;
}
// =========================================================================
static int luaMaterial_Size(lua_State* state)
{
	Script L(state);
	Texture* m = (Texture*) L.GetLightUD(1);
	if (m)
	{
		L.PushInt(m->OriginalWidth);
		L.PushInt(m->OriginalHeight);
	}
    else
    {
        L.PushInt(-1);
        L.PushInt(-1);
    }
	return 2;
}
// =========================================================================
static int luaMaterial_SetPriority(lua_State* state)
{
	Script L(state);
	RenderDevice *dev = GEngine->GetRenderDevice();
	Texture* m = (Texture*) L.GetLightUD(1);
	if (dev && m && GEngine->Video)
		dev->SetTexturePriority(m,L.GetInt(2,1));
	return 0;
}
// =========================================================================
static const luaL_reg material_lib[] = {
	{"Create", luaMaterial_Create},
	{"Release", luaMaterial_Release},
	{"Replace", luaMaterial_Replace},
	{"Size", luaMaterial_Size},
	{"SetPriority", luaMaterial_SetPriority},
	{NULL, NULL}
};
// =========================================================================
// CAMERA
// =========================================================================
static int luaCamera_SetPos(lua_State* state)
{
	Script L(state);
    if (!GEngine->Video) return 0;
	View * view = GEngine->Video->GetActiveView();
	view->mCamera.SetPosition(Vector(L.GetFloat(1),L.GetFloat(2),L.GetFloat(3)));
	view->mCamera.InverseTransform();
	return 0;
}
// =========================================================================
static int luaCamera_EnableInterpolation(lua_State* state)
{
	Script L(state);
    if (!GEngine->Video) return 0;
	View * view = GEngine->Video->GetActiveView();
	view->mCamera.Interpolation = L.GetBool(1,true);
	return 0;
}
// =========================================================================
static int luaCamera_SetAng(lua_State* state)
{
	Script L(state);
    if (!GEngine->Video) return 0;
	View * view = GEngine->Video->GetActiveView();
    view->mCamera.SetRotation(Vector(L.GetFloat(1),L.GetFloat(2),L.GetFloat(3)));
	view->mCamera.InverseTransform();
	return 0;
}
// =========================================================================
static int luaCamera_SetAngRad(lua_State* state)
{
    Script L(state);
    if (!GEngine->Video) return 0;
    View * view = GEngine->Video->GetActiveView();
    view->mCamera.SetRotation(Vector(FRad2Deg(L.GetFloat(1)),FRad2Deg(L.GetFloat(2)),FRad2Deg(L.GetFloat(3))));
    view->mCamera.InverseTransform();
    return 0;
}
// =========================================================================
static int luaCamera_GetAng(lua_State* state)
{
	Script L(state);
    if (GEngine->Video)
    {
	    View *  view = GEngine->Video->GetActiveView();
        Vector  v = view->mCamera.GetRotation();


	    L.PushFloat(v.X);
	    L.PushFloat(v.Y);
	    L.PushFloat(v.Z);
    }
    else
    {
        L.PushFloat(0);
        L.PushFloat(0);
        L.PushFloat(0);
    }

	return 3;
}
// =========================================================================
static int luaCamera_GetAngRad(lua_State* state)
{
	Script L(state);
	View *  view = GEngine->Video->GetActiveView();
    Vector  v = view->mCamera.GetRotation();


	L.PushFloat(FDeg2Rad(v.X));
	L.PushFloat(FDeg2Rad(v.Y));
	L.PushFloat(FDeg2Rad(v.Z));
	return 3;
}
// =========================================================================
static int luaCamera_GetRotation(lua_State* state)
{
	Script L(state);
	View * view = GEngine->Video->GetActiveView();
	Vector v = view->mCamera.GetRotation();
	L.PushFloat(v.X);
	L.PushFloat(v.Y);
	L.PushFloat(v.Z);
	return 3;
}
// =========================================================================
static int luaCamera_GetRawRotation(lua_State* state)
{
	Script L(state);
	View * view = GEngine->Video->GetActiveView();
	Vector v = view->mCamera.GetRawRotation();
	L.PushFloat(v.X);
	L.PushFloat(v.Y);
	L.PushFloat(v.Z);
	return 3;
}
// =========================================================================
static int luaCamera_GetQuaternion(lua_State* state)
{
	Script L(state);
	View * view = GEngine->Video->GetActiveView();
	Camera &c = view->mCamera;

	//Matrix mat = YRotationMatrix(FDeg2Rad(-c.Rotation.X))*XRotationMatrix(FDeg2Rad(c.Rotation.Y));
	//Matrix mat =  YRotationMatrix(FDeg2Rad(c.Rotation.X))
	//              *XRotationMatrix(FDeg2Rad(c.Rotation.Y));



	Quaternion q(c.GetViewMatrix().Inverse());

	L.PushFloat(q.W);
	L.PushFloat(q.X);
	L.PushFloat(q.Y);
	L.PushFloat(q.Z);
	return 4;
}
// =========================================================================
static int luaCamera_GetPos(lua_State* state)
{
	Script L(state);
    if (GEngine->Video)
    {
	    View *  view = GEngine->Video->GetActiveView();
        Vector  v = view->mCamera.GetPosition();


	    L.PushFloat(v.X);
	    L.PushFloat(v.Y);
	    L.PushFloat(v.Z);
    }
    else
    {
        L.PushFloat(0);
        L.PushFloat(0);
        L.PushFloat(0);
    }
	return 3;
}
// =========================================================================
static int luaCamera_LookAt(lua_State* state)
{
	Script L(state);
    if (!GEngine->Video) return 0;
	View * view = GEngine->Video->GetActiveView();
	view->mCamera.LookAt(Vector(L.GetFloat(1),L.GetFloat(2),L.GetFloat(3)));
	return 0;
}
// =========================================================================
static int luaCamera_GetForwardVector(lua_State* state)
{
	Script L(state);
    if (GEngine->Video)
    {
	    View * view = GEngine->Video->GetActiveView();
	    Vector v = view->mCamera.GetForwardVector();
	    L.PushFloat(v.X);
	    L.PushFloat(v.Y);
	    L.PushFloat(v.Z);
    }
    else
    {
        L.PushFloat(0);
        L.PushFloat(0);
        L.PushFloat(0);
    }
	return 3;
}
// =========================================================================
static int luaCamera_GetRightVector(lua_State* state)
{
	Script L(state);
    if (GEngine->Video)
    {
	    View * view = GEngine->Video->GetActiveView();
	    Vector v = view->mCamera.GetRightVector();
	    L.PushFloat(v.X);
	    L.PushFloat(v.Y);
	    L.PushFloat(v.Z);
    }
    else
    {
        L.PushFloat(0);
        L.PushFloat(0);
        L.PushFloat(0);
    }
	return 3;
}
// =========================================================================
static int luaCamera_GetUpVector(lua_State* state)
{
	Script L(state);
    if (GEngine->Video)
    {
	    View * view = GEngine->Video->GetActiveView();
	    Vector v = view->mCamera.GetUpVector();
	    L.PushFloat(v.X);
	    L.PushFloat(v.Y);
	    L.PushFloat(v.Z);
    }
    else
    {
        L.PushFloat(0);
        L.PushFloat(0);
        L.PushFloat(0);
    }
	return 3;
}
// =========================================================================
static int luaCamera_SetPositionDisplacement(lua_State* state)
{
	Script L(state);
    if (!GEngine->Video) return 0;
	View * view = GEngine->Video->GetActiveView();
	view->mCamera.SetDispPosition(Vector(L.GetFloat(1, 0),L.GetFloat(2, 0),L.GetFloat(3, 0)));
	return 0;
}
// =========================================================================
static int luaCamera_SetRotationDisplacement(lua_State* state)
{
	Script L(state);
    if (!GEngine->Video) return 0;
	View * view = GEngine->Video->GetActiveView();
	view->mCamera.SetDispRotation(Vector(L.GetFloat(1, 0),L.GetFloat(2, 0),L.GetFloat(3, 0)));
	return 0;
}
// =========================================================================
static int luaCamera_SetTransform(lua_State* state)
{
    Script L(state);
    if (!GEngine->Video) return 0;
    Vector pos(L.GetFloat(1, 0),L.GetFloat(2, 0),L.GetFloat(3, 0));
    Quaternion rot(L.GetFloat(4, 1),L.GetFloat(5, 0),L.GetFloat(6, 0),L.GetFloat(7, 0));

    // nie sprawdzone
    View * view = GEngine->Video->GetActiveView();
    view->mCamera.SetViewMatrix(TranslationMatrix(-pos)
        *rot.ToMatrix()  *ScaleMatrix(Vector(1,1,-1)));

	return 0;
}
// =========================================================================
static int luaCamera_SetAdditionalRotation(lua_State* state)
{
	Script L(state);
	Quaternion rot(L.GetFloat(1, 1),L.GetFloat(2, 0),L.GetFloat(3, 0),L.GetFloat(4, 0));

	View * view = GEngine->Video->GetActiveView();
	Vector crot = view->mCamera.GetRotation();
	Vector cpos = view->mCamera.GetRotation();

	Matrix mat = TranslationMatrix(-view->mCamera.GetPosition())
		*rot.ToMatrix().GetTranspose()
		*YRotationMatrix(FDeg2Rad(crot.X))
		*ZRotationMatrix(FDeg2Rad(crot.Z))
		*XRotationMatrix(FDeg2Rad(crot.Y))
		*ScaleMatrix(Vector(1,1,-1));

	view->mCamera.AdditionalRotation = rot.ToMatrix().GetRotation();

	view->mCamera.SetViewMatrix(mat);
	return 0;
}

#ifdef TRANSGAMING
// =========================================================================
static int luaCamera_PreTranslate(lua_State* state)
{
	Script L(state);
    if (!GEngine->Video) return 0;
	View * view = GEngine->Video->GetActiveView();

	Vector v = Vector( L.GetFloat(1, 0), L.GetFloat(2, 0), L.GetFloat(3, 0) );
	Vector cpos = view->mCamera.GetPosition();

	// First we'll get the WorldToCamera matrix, and then set it's translation values to 0.0f.
	// We will still have the rotation values which is what we want.  Lastly, transform the
	// matrix by our movement vector.
	Matrix w2c = view->mCamera.WorldToCamera();
	w2c.SetTranslation(Vector( 0.0f, 0.0f, 0.0f ));
	Vector newpos = w2c.Transform(v);

	view->mCamera.SetPosition( Vector(cpos.X + newpos.X, cpos.Y + newpos.Y, cpos.Z + newpos.Z) );
	return 0;
}
#endif

// =========================================================================
static int luaCamera_UpdateViewport(lua_State* state)
{
	Script L(state);
    if (!GEngine->Video) return 0;
	View * view = GEngine->Video->GetActiveView();
	view->LocalViewport.Update( view->mCamera );
	return 0;
}
// =========================================================================
static const luaL_reg camera_lib[] = {
	{"SetPos", luaCamera_SetPos},
	{"GetPos", luaCamera_GetPos},
	{"SetAng", luaCamera_SetAng},
    {"SetAngRad", luaCamera_SetAngRad},
	{"GetAng", luaCamera_GetAng},
	{"GetAngRad", luaCamera_GetAngRad},
	{"LookAt", luaCamera_LookAt},
	{"GetForwardVector", luaCamera_GetForwardVector},
	{"GetRightVector", luaCamera_GetRightVector},
	{"GetUpVector", luaCamera_GetUpVector},
	{"GetRotation", luaCamera_GetRotation},
	{"GetRawRotation", luaCamera_GetRawRotation},
	{"GetQuaternion", luaCamera_GetQuaternion},
	{"SetPositionDisplacement", luaCamera_SetPositionDisplacement},
	{"SetRotationDisplacement", luaCamera_SetRotationDisplacement},
    {"SetTransform", luaCamera_SetTransform},
	{"SetAdditionalRotation", luaCamera_SetAdditionalRotation},
    {"EnableInterpolation", luaCamera_EnableInterpolation},
	{"UpdateViewport", luaCamera_UpdateViewport},
	{"PreTranslate", luaCamera_PreTranslate},
	{NULL, NULL}
};
// =========================================================================
// WAY POINTS
// =========================================================================
static int luaWayPoints_Add(lua_State* state)
{
	Script L(state);
    bool newWaypoints = (( GEngine->Video->GetActiveView()->Mode & View::EModeNewWaypoints ) != 0 );

	if( newWaypoints )
	{
		Waypoint2 wp;
		wp.Position = Vector(0.5f,0.5f,0.5f);
		wp.BigMonsters = false;
#ifndef TRANSGAMING
		wp.Set = GEngine->World->Paths2.GetValidCurrentSet();
#else
		wp.Set = GEngine->pcfWorld->Paths2.GetValidCurrentSet();
#endif
		wp.Border = false;
		wp.Disabled = false;
		wp.Selected = false;
#ifndef TRANSGAMING
		L.PushInt( GEngine->World->Paths2.AddWaypoint(wp) );
#else
		L.PushInt( GEngine->pcfWorld->Paths2.AddWaypoint(wp) );
#endif
	}
	else
	{
		Waypoint wp;
		wp.mPosition = Vector(0.5f,0.5f,0.5f);
#ifndef TRANSGAMING
		L.PushInt(GEngine->World->Paths.AddWaypoint(wp));
#else
		L.PushInt(GEngine->pcfWorld->Paths.AddWaypoint(wp));
#endif
	}
	return 1;
}
// =========================================================================
static int luaWayPoints_SetPos(lua_State* state)
{
	Script L(state);
	int idx = L.GetInt(1);

    bool newWaypoints = (( GEngine->Video->GetActiveView()->Mode & View::EModeNewWaypoints ) != 0 );

#ifndef TRANSGAMING
	if( newWaypoints )
		GEngine->World->Paths2.MoveWaypoint( idx, Vector(L.GetFloat(2),L.GetFloat(3),L.GetFloat(4)) );
	else
		GEngine->World->Paths.MoveWaypoint(idx,Vector(L.GetFloat(2),L.GetFloat(3),L.GetFloat(4)));
#else
	if( newWaypoints )
		GEngine->pcfWorld->Paths2.MoveWaypoint( idx, Vector(L.GetFloat(2),L.GetFloat(3),L.GetFloat(4)) );
	else
		GEngine->pcfWorld->Paths.MoveWaypoint(idx,Vector(L.GetFloat(2),L.GetFloat(3),L.GetFloat(4)));
#endif

	return 0;
}
// =========================================================================
static int luaWayPoints_GetPos(lua_State* state)
{
	Script L(state);
	int idx = L.GetInt(1);

    bool newWaypoints = (( GEngine->Video->GetActiveView()->Mode & View::EModeNewWaypoints ) != 0 );

	if( newWaypoints )
	{
#ifndef TRANSGAMING
		Waypoint2& wp = GEngine->World->Paths2[idx];
#else
		Waypoint2& wp = GEngine->pcfWorld->Paths2[idx];
#endif
		L.PushFloat(wp.Position.X);
		L.PushFloat(wp.Position.Y);
		L.PushFloat(wp.Position.Z);
	}
	else
	{
#ifndef TRANSGAMING
		WaypointSet *pWps = GEngine->World->Paths.GetCurrentWaypointSet();
#else
		WaypointSet *pWps = GEngine->pcfWorld->Paths.GetCurrentWaypointSet();
#endif
		if (!pWps) return 0;

		Waypoint wp = (*pWps)[idx];
		L.PushFloat(wp.mPosition.X);
		L.PushFloat(wp.mPosition.Y);
		L.PushFloat(wp.mPosition.Z);
	}
	return 3;
}
// =========================================================================
static int luaWayPoints_Pick(lua_State* state)
{
	Script L(state);
	int sx = L.GetInt(1);
	int sy = L.GetInt(2);
	float tolerance = L.GetFloat(3);

    bool newWaypoints = (( GEngine->Video->GetActiveView()->Mode & View::EModeNewWaypoints ) != 0 );

	int idx;

#ifndef TRANSGAMING
	if( newWaypoints )
		idx = GEngine->World->Paths2.PickWaypoint( GEngine->Video->GetActiveView()->LocalViewport, sx, sy, tolerance );
	else
		idx = GEngine->World->Paths.PickWaypoint(sx,sy,tolerance);
#else
	if( newWaypoints )
		idx = GEngine->pcfWorld->Paths2.PickWaypoint( GEngine->Video->GetActiveView()->LocalViewport, sx, sy, tolerance );
	else
		idx = GEngine->pcfWorld->Paths.PickWaypoint(sx,sy,tolerance);
#endif

	L.PushInt(idx);
	return 1;
}
// =========================================================================
static int luaWayPoints_Select(lua_State* state)
{
	Script L(state);
	RECT r;
	SetRect(&r,L.GetInt(1),L.GetInt(2),L.GetInt(3),L.GetInt(4));
	if (r.top>r.bottom) {int a = r.bottom; r.bottom = r.top; r.top = a;};
	if (r.left>r.right) {int a = r.right; r.right = r.left; r.left = a;};

    bool newWaypoints = (( GEngine->Video->GetActiveView()->Mode & View::EModeNewWaypoints ) != 0 );

	int idx;

#ifndef TRANSGAMING
	if( newWaypoints )
		idx = GEngine->World->Paths2.Selection( GEngine->Video->GetActiveView()->LocalViewport, L.GetFloat(5), &r, L.GetInt(6) );
	else
		idx = GEngine->World->Paths.Selection( L.GetFloat(5), &r, L.GetInt(6) );
#else
	if( newWaypoints )
		idx = GEngine->pcfWorld->Paths2.Selection( GEngine->Video->GetActiveView()->LocalViewport, L.GetFloat(5), &r, L.GetInt(6) );
	else
		idx = GEngine->pcfWorld->Paths.Selection( L.GetFloat(5), &r, L.GetInt(6) );
#endif

	L.PushInt(idx);
	return 1;
}
// =========================================================================
static int luaWayPoints_UnselectAll(lua_State* state)
{
	Script L(state);

    bool newWaypoints = (( GEngine->Video->GetActiveView()->Mode & View::EModeNewWaypoints ) != 0 );

#ifndef TRANSGAMING
	if( newWaypoints )
	{
		GEngine->World->Paths2.Selection( GEngine->Video->GetActiveView()->LocalViewport, 0, NULL, 0 );
		GEngine->World->Paths2.FloorSelection( GEngine->Video->GetActiveView()->LocalViewport, NULL, 0 );
	}
	else
	{
		GEngine->World->Paths.Selection( 0, NULL, 0 );
		GEngine->World->Paths.FloorSelection( NULL, 0 );
	}
#else
	if( newWaypoints )
	{
		GEngine->pcfWorld->Paths2.Selection( GEngine->Video->GetActiveView()->LocalViewport, 0, NULL, 0 );
		GEngine->pcfWorld->Paths2.FloorSelection( GEngine->Video->GetActiveView()->LocalViewport, NULL, 0 );
	}
	else
	{
		GEngine->pcfWorld->Paths.Selection( 0, NULL, 0 );
		GEngine->pcfWorld->Paths.FloorSelection( NULL, 0 );
	}
#endif
	return 0;
}
// =========================================================================
#ifdef TRANSGAMING_USE_EDITOR
static int luaWayPoints_MoveSelected(lua_State* state)
{
	Script L(state);
	Vector v(L.GetFloat(1),L.GetFloat(2),L.GetFloat(3));

    bool newWaypoints = (( GEngine->Video->GetActiveView()->Mode & View::EModeNewWaypoints ) != 0 );

#ifndef TRANSGAMING
	if( newWaypoints )
		GEngine->World->Paths2.Selected_Displace( v );
	else
		GEngine->World->Paths.DisplaceSelected( v );
#else
	if( newWaypoints )
		GEngine->pcfWorld->Paths2.Selected_Displace( v );
	else
		GEngine->pcfWorld->Paths.DisplaceSelected( v );
#endif

	return 0;
}
#endif
// =========================================================================
#ifdef TRANSGAMING_USE_EDITOR
static int luaWayPoints_RecalculateSelected(lua_State* state)
{
	Script L(state);

    bool newWaypoints = (( GEngine->Video->GetActiveView()->Mode & View::EModeNewWaypoints ) != 0 );

#ifndef TRANSGAMING
	if( newWaypoints )
		GEngine->World->Paths2.Selected_Recalculate();
	else
		GEngine->World->Paths.RecalculateSelected();
#else
	if( newWaypoints )
		GEngine->pcfWorld->Paths2.Selected_Recalculate();
	else
		GEngine->pcfWorld->Paths.RecalculateSelected();
#endif

	return 0;
}
#endif
// =========================================================================
#ifdef TRANSGAMING_USE_EDITOR
static int luaWayPoints_DeleteSelected(lua_State* state)
{
	Script L(state);

    bool newWaypoints = (( GEngine->Video->GetActiveView()->Mode & View::EModeNewWaypoints ) != 0 );

#ifndef TRANSGAMING
	if( newWaypoints )
		GEngine->World->Paths2.Selected_Remove();
	else
		GEngine->World->Paths.RemoveSelected();
#else
	if( newWaypoints )
		GEngine->pcfWorld->Paths2.Selected_Remove();
	else
		GEngine->pcfWorld->Paths.RemoveSelected();
#endif

	return 0;
}
#endif
// =========================================================================
static int luaWayPoints_FastPickCurrentSet(lua_State* state)
{
	Script L(state);
	int sx = L.GetInt(1);
	int sy = L.GetInt(2);
	float tolerance = L.GetFloat(3);
    bool newWaypoints = (( GEngine->Video->GetActiveView()->Mode & View::EModeNewWaypoints ) != 0 );
	if( newWaypoints )
#ifndef TRANSGAMING
		GEngine->World->Paths2.FastPickCurrentSet(GEngine->Video->GetActiveView()->LocalViewport, sx, sy, tolerance);
#else
		GEngine->pcfWorld->Paths2.FastPickCurrentSet(GEngine->Video->GetActiveView()->LocalViewport, sx, sy, tolerance);
#endif
	return 0;
}
// =========================================================================
static int luaWayPoints_EnableDisableSet(lua_State* state)
{
	Script L(state);
	int sx = L.GetInt(1);
	int sy = L.GetInt(2);
	float tolerance = L.GetFloat(3);
    bool newWaypoints = (( GEngine->Video->GetActiveView()->Mode & View::EModeNewWaypoints ) != 0 );
	if( newWaypoints )
#ifndef TRANSGAMING
		GEngine->World->Paths2.SwitchSetEnabling(GEngine->Video->GetActiveView()->LocalViewport, sx, sy, tolerance);
#else
		GEngine->pcfWorld->Paths2.SwitchSetEnabling(GEngine->Video->GetActiveView()->LocalViewport, sx, sy, tolerance);
#endif
	return 0;
}
// =========================================================================
static int luaWayPoints_SelectFloors(lua_State* state)
{
	Script L(state);
	RECT r;
	SetRect(&r,L.GetInt(1),L.GetInt(2),L.GetInt(3),L.GetInt(4));
	if (r.top>r.bottom) {int a = r.bottom; r.bottom = r.top; r.top = a;};
	if (r.left>r.right) {int a = r.right; r.right = r.left; r.left = a;};

    bool newWaypoints = (( GEngine->Video->GetActiveView()->Mode & View::EModeNewWaypoints ) != 0 );

#ifndef TRANSGAMING
	if( newWaypoints )
		GEngine->World->Paths2.FloorSelection( GEngine->Video->GetActiveView()->LocalViewport, &r, L.GetInt(5) );
	else
		GEngine->World->Paths.FloorSelection( &r, L.GetInt(5) );
#else
	if( newWaypoints )
		GEngine->pcfWorld->Paths2.FloorSelection( GEngine->Video->GetActiveView()->LocalViewport, &r, L.GetInt(5) );
	else
		GEngine->pcfWorld->Paths.FloorSelection( &r, L.GetInt(5) );
#endif

	return 0;
}
// =========================================================================
static int luaWayPoints_Load(lua_State* state)
{
	Script L(state);

	char wpName[MAX_PATH];
	char* s;


#ifndef TRANSGAMING
	strcpy(wpName,L.GetString(1));
	bool dontLoadPortals = L.GetBool(2,false);

    GEngine->World->Paths2.ClearContents();
	GEngine->World->Paths.ClearContents();
    GEngine->World->UseNewPaths = true;
#else
	strncpy(wpName, L.GetString(1), sizeof(wpName));
	bool dontLoadPortals = L.GetBool(2,false);

    TRACE("luaWayPoints_Load(): attempting to load way points from the file '%s'\n", wpName);
    GEngine->pcfWorld->Paths2.ClearContents();
	GEngine->pcfWorld->Paths.ClearContents();
    GEngine->pcfWorld->UseNewPaths = true;
#endif

	s = strstr(wpName,".wpm");
	if( s )
	{
#ifndef TRANSGAMING
		strcpy( s, ".wps" );

        if( GEngine->World->Paths2.LoadContents( wpName, 1.0f, dontLoadPortals ) )
		{
			GEngine->World->UseNewPaths = true;
			strcpy( s, ".fls" );
			GEngine->World->Paths2.LoadFloors( wpName );
			return 0;
		}
		else	// try to load old waypoints then
		{
            MessageBox(NULL, "Old waypoint format don't work with savegame!!", "Warning!!", MB_TOPMOST);
			GEngine->World->UseNewPaths = false;
			strcpy( s, ".wpm" );
			GEngine->World->Paths.LoadContents( wpName, 1.0f, dontLoadPortals );
			strcpy( s, ".flm" );
			GEngine->World->Paths.LoadFloors( wpName );
			return 0;
		}
#else
		strncpy( s, ".wps", MAX_PATH - (s - wpName) - 1);

        TRACE("luaWayPoints_Load(): loading path contents from '%s' (to replace a '.wpm' file)\n", wpName);
        if( GEngine->pcfWorld->Paths2.LoadContents( wpName, 1.0f, dontLoadPortals ) )
		{
			GEngine->pcfWorld->UseNewPaths = true;
			strncpy( s, ".fls", MAX_PATH - (s - wpName) - 1 );
			GEngine->pcfWorld->Paths2.LoadFloors( wpName );
			return 0;
		}
		else	// try to load old waypoints then
		{
            //MessageBox(NULL, "Old waypoint format don't work with savegame!!", "Warning!!", MB_TOPMOST);
            TRACE("[]::luaWayPoints_load(): WARNING-> old waypoint format doesn't work with savegames!\n");
			GEngine->pcfWorld->UseNewPaths = false;
			strncpy( s, ".wpm", MAX_PATH - (s - wpName) - 1 );
			GEngine->pcfWorld->Paths.LoadContents( wpName, 1.0f, dontLoadPortals );
			strncpy( s, ".flm", MAX_PATH - (s - wpName) - 1 );
			GEngine->pcfWorld->Paths.LoadFloors( wpName );
			return 0;
		}
#endif
	}
	s = strstr(wpName,".wps");

#ifndef TRANSGAMING
	if( s && GEngine->World->Paths2.LoadContents( wpName, 1.0f, dontLoadPortals ) )
	{
		GEngine->World->UseNewPaths = true;
		strcpy( s, ".fls" );
		GEngine->World->Paths2.LoadFloors( wpName );
	}
#else
    TRACE("luaWayPoints_Load(): loading path contents from '%s' (as a '.wps' file)\n", wpName);
	if( s && GEngine->pcfWorld->Paths2.LoadContents( wpName, 1.0f, dontLoadPortals ) )
	{
		GEngine->pcfWorld->UseNewPaths = true;
		strncpy( s, ".fls", MAX_PATH - (s - wpName) - 1 );
		GEngine->pcfWorld->Paths2.LoadFloors( wpName );
	}
#endif

	return 0;
}
// =========================================================================
static int luaWayPoints_GetLength(lua_State* state)
{
	Script L(state);

	static WaypointGPath path;
	static WaypointGPath2 path2;
	Vector from( L.GetFloat(1), L.GetFloat(2), L.GetFloat(3));
	Vector to( L.GetFloat(4), L.GetFloat(5), L.GetFloat(6));

	float len;

#ifndef TRANSGAMING
	if( GEngine->World->UseNewPaths )
	{
		path2.Empty();
		len = GEngine->World->Paths2.GetShortestPath( from, to, path2 );
	}
	else
	{
		path.Empty();
		len = GEngine->World->Paths.GetShortestPath( from, to, GAStarDistanceHeuristic, path );
	}
#else
	if( GEngine->pcfWorld->UseNewPaths )
	{
		path2.Empty();
		len = GEngine->pcfWorld->Paths2.GetShortestPath( from, to, path2 );
	}
	else
	{
		path.Empty();
		len = GEngine->pcfWorld->Paths.GetShortestPath( from, to, GAStarDistanceHeuristic, path );
	}
#endif

	L.PushFloat(len);
	return 1;
}
// =========================================================================
static int luaWayPoints_FlipZ(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	if( GEngine->World->UseNewPaths )
		GEngine->World->Paths2.FlipZ();
	else
		GEngine->World->Paths.FlipZ();
#else
	if( GEngine->pcfWorld->UseNewPaths )
		GEngine->pcfWorld->Paths2.FlipZ();
	else
		GEngine->pcfWorld->Paths.FlipZ();
#endif

	return 0;
}
// =========================================================================
static int luaWayPoints_GetRandomPoint(lua_State* state)
{
	Script L(state);

	srand((int)(GEngine->SysDriver.GetCurrentTime() * 1000.0f));

#ifndef TRANSGAMING
	if( GEngine->World->UseNewPaths )
	{
		int wp = GEngine->World->Paths2.NumberOfWaypoints();
		if( !wp ) return 0;
		wp = rand()%wp;
		L.PushFloat( GEngine->World->Paths2[wp].Position.X+1);
		L.PushFloat( GEngine->World->Paths2[wp].Position.Y);
		L.PushFloat( GEngine->World->Paths2[wp].Position.Z-1);
	}
	else
	{
		int zs = GEngine->World->Zones.Size();
		if (zs==0) return 0;
		zs = rand()%zs;

		WaypointSet &wps = GEngine->World->Paths.GetWaypointsInZone(zs);
		int wp = wps.NumberOfWaypoints();
		if (wp==0) return 0;
		wp = rand()%wp;
		L.PushFloat(wps[wp].mPosition.X+1);
		L.PushFloat(wps[wp].mPosition.Y);
		L.PushFloat(wps[wp].mPosition.Z-1);
	}
#else
	if( GEngine->pcfWorld->UseNewPaths )
	{
		int wp = GEngine->pcfWorld->Paths2.NumberOfWaypoints();
		if( !wp ) return 0;
		wp = rand()%wp;
		L.PushFloat( GEngine->pcfWorld->Paths2[wp].Position.X+1);
		L.PushFloat( GEngine->pcfWorld->Paths2[wp].Position.Y);
		L.PushFloat( GEngine->pcfWorld->Paths2[wp].Position.Z-1);
	}
	else
	{
		int zs = GEngine->pcfWorld->Zones.Size();
		if (zs==0) return 0;
		zs = rand()%zs;

		WaypointSet &wps = GEngine->pcfWorld->Paths.GetWaypointsInZone(zs);
		int wp = wps.NumberOfWaypoints();
		if (wp==0) return 0;
		wp = rand()%wp;
		L.PushFloat(wps[wp].mPosition.X+1);
		L.PushFloat(wps[wp].mPosition.Y);
		L.PushFloat(wps[wp].mPosition.Z-1);
	}
#endif

	return 3;
}
// =========================================================================
static int luaWayPoints_GetClosest(lua_State* state)
{
	Script L(state);

	Vector from( L.GetFloat(1), L.GetFloat(2), L.GetFloat(3));
	int zone = -1, index = -1;
	float distance = 10.0f;

#ifndef TRANSGAMING
	if( GEngine->World->UseNewPaths )
	{
		zone = 0;
		GEngine->World->Paths2.GetClosestWaypoint( from, index, distance );
	}
	else
		GEngine->World->Paths.GetClosestWaypoint( from, zone, index, distance );
#else
	if( GEngine->pcfWorld->UseNewPaths )
	{
		zone = 0;
		GEngine->pcfWorld->Paths2.GetClosestWaypoint( from, index, distance );
	}
	else
		GEngine->pcfWorld->Paths.GetClosestWaypoint( from, zone, index, distance );
#endif

	L.PushInt(zone);
	L.PushInt(index);
	return 2;
}
// =========================================================================
static int luaWayPoints_EnableWP(lua_State* state)
{
	Script L(state);
	int wp = L.GetInt(1);
	bool enable = L.GetBool(2,true);
#ifndef TRANSGAMING
	GEngine->World->Paths2.EnableWaypoint(wp,enable);
#else
	GEngine->pcfWorld->Paths2.EnableWaypoint(wp,enable);
#endif

	return 0;
}
static int luaWayPoints_IsEnabled(lua_State* state)
{
	Script L(state);
	int wp = L.GetInt(1);
#ifndef TRANSGAMING
	bool enable = GEngine->World->Paths2.WaypointEnabled(wp);
#else
	bool enable = GEngine->pcfWorld->Paths2.WaypointEnabled(wp);
#endif
	L.PushBool(enable);
	return 1;
}

// =========================================================================
static int luaWayPoints_GetPosition(lua_State* state)
{
	Script L(state);
	int zone = L.GetInt(1);
	int index = L.GetInt(2);
	Vector pos(0,0,0);

#ifndef TRANSGAMING
	if( GEngine->World->UseNewPaths )
		pos = GEngine->World->Paths2[index].Position;
#else
	if( GEngine->pcfWorld->UseNewPaths )
		pos = GEngine->pcfWorld->Paths2[index].Position;
#endif

#ifndef TRANSGAMING_NO_HAVOK
	else
#ifndef TRANSGAMING
		pos = GEngine->World->Paths.GetWaypointPosition(zone,index);
#else
		pos = GEngine->pcfWorld->Paths.GetWaypointPosition(zone,index);
#endif
#endif

	L.PushFloat(pos.X);
	L.PushFloat(pos.Y);
	L.PushFloat(pos.Z);
	return 3;
}
// =========================================================================
static int luaWayPoints_GetPathsNumber(lua_State* state)
{
	Script L(state);
	int zone = L.GetInt(1);
	int index = L.GetInt(2);
	int number;

#ifndef TRANSGAMING
	if( GEngine->World->UseNewPaths )
		number = GEngine->World->Paths2.GetWaypointPaths(index);
	else
		number = GEngine->World->Paths.GetWaypointPaths(zone,index);
#else
	if( GEngine->pcfWorld->UseNewPaths )
		number = GEngine->pcfWorld->Paths2.GetWaypointPaths(index);
	else
		number = GEngine->pcfWorld->Paths.GetWaypointPaths(zone,index);
#endif

	L.PushInt(number);
	return 1;
}
// =========================================================================
static int luaWayPoints_GetWaypointByPathNumber(lua_State* state)
{
	Script L(state);
	int zone = L.GetInt(1);
	int index = L.GetInt(2);
	int pathIndex = L.GetInt(3);
	int zone2 = -1;
	float distance;

#ifndef TRANSGAMING
	if( GEngine->World->UseNewPaths )
		GEngine->World->Paths2.GetWaypointPath( index, pathIndex, zone2, distance );
	else
		GEngine->World->Paths.GetWaypointPath( zone, index, pathIndex, zone2, distance );
#else
	if( GEngine->pcfWorld->UseNewPaths )
		GEngine->pcfWorld->Paths2.GetWaypointPath( index, pathIndex, zone2, distance );
	else
		GEngine->pcfWorld->Paths.GetWaypointPath( zone, index, pathIndex, zone2, distance );
#endif

	L.PushInt(zone2);
	L.PushFloat(distance);
	return 2;
}
// =========================================================================
static const luaL_reg waypoints_lib[] = {
	{"Add", luaWayPoints_Add},
	{"SetPos", luaWayPoints_SetPos},
	{"GetPos", luaWayPoints_GetPos},
	{"Pick", luaWayPoints_Pick},
	{"Select", luaWayPoints_Select},
	{"SelectFloors", luaWayPoints_SelectFloors},
#ifdef TRANSGAMING_USE_EDITOR
	{"MoveSelected",luaWayPoints_MoveSelected},
	{"DeleteSelected",luaWayPoints_DeleteSelected},
	{"RecalculateSelected",luaWayPoints_RecalculateSelected},
#endif
	{"FastPickCurrentSet",luaWayPoints_FastPickCurrentSet},
	{"EnableDisableSet",luaWayPoints_EnableDisableSet},
	{"GetLength",luaWayPoints_GetLength},
	{"UnselectAll",luaWayPoints_UnselectAll},
	{"Load",luaWayPoints_Load},
	{"FlipZ",luaWayPoints_FlipZ},
	{"GetRandomPoint",luaWayPoints_GetRandomPoint},
	{"GetClosest",luaWayPoints_GetClosest},
	{"EnableWP",luaWayPoints_EnableWP},
	{"IsEnabled",luaWayPoints_IsEnabled},
	{"GetPosition",luaWayPoints_GetPosition},
	{"GetPathsNumber",luaWayPoints_GetPathsNumber},
	{"GetWaypointByPathNumber",luaWayPoints_GetWaypointByPathNumber},
	{NULL, NULL}
};
// =========================================================================
// PATHFINDING
// =========================================================================
static int luaPathfinding_Create(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	if( GEngine->World->UseNewPaths )
	{
        m_setTag("ScriptAI");
		WaypointGPath2* path = new WaypointGPath2();
		path->Init( &GEngine->World->Paths2 );
		int index;
		if( GEngine->World->Paths2.FreePathIndices.Size() )
		{
			index = GEngine->World->Paths2.FreePathIndices[GEngine->World->Paths2.FreePathIndices.Size()-1];
			GEngine->World->Paths2.FreePathIndices.RemoveLast();
			GEngine->World->Paths2.WaypointPaths[index] = path;
		}
		else
			index = GEngine->World->Paths2.WaypointPaths.Add( path );
		L.PushInt( index );
	}
	else
	{
        m_setTag("ScriptAI");
		WaypointGPath *path = new WaypointGPath();
		path->Init(&GEngine->World->Paths, GAStarDistanceHeuristic );
		L.PushLightUD(path);
	}
#else
	if( GEngine->pcfWorld->UseNewPaths )
	{
        m_setTag("ScriptAI");
		WaypointGPath2* path = new WaypointGPath2();
		path->Init( &GEngine->pcfWorld->Paths2 );
		int index;
		if( GEngine->pcfWorld->Paths2.FreePathIndices.Size() )
		{
			index = GEngine->pcfWorld->Paths2.FreePathIndices[GEngine->pcfWorld->Paths2.FreePathIndices.Size()-1];
			GEngine->pcfWorld->Paths2.FreePathIndices.RemoveLast();
			GEngine->pcfWorld->Paths2.WaypointPaths[index] = path;
		}
		else
			index = GEngine->pcfWorld->Paths2.WaypointPaths.Add( path );
		L.PushInt( index );
	}
	else
	{
        m_setTag("ScriptAI");
		WaypointGPath *path = new WaypointGPath();
		path->Init(&GEngine->pcfWorld->Paths, GAStarDistanceHeuristic );
		L.PushLightUD(path);
	}
#endif
	return 1;
}
// =========================================================================
static int luaPathfinding_Release(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	if( GEngine->World->UseNewPaths )
	{
		if (L.IsNil(1)) return 0;
		int index = L.GetInt(1);
		WaypointGPath2* path = GEngine->World->Paths2.WaypointPaths[index];
		delete path;
		GEngine->World->Paths2.WaypointPaths[index] = 0;
		GEngine->World->Paths2.FreePathIndices.Add(index);
	}
#else
	if( GEngine->pcfWorld->UseNewPaths )
	{
		if (L.IsNil(1)) return 0;
		int index = L.GetInt(1);
		WaypointGPath2* path = GEngine->pcfWorld->Paths2.WaypointPaths[index];
		delete path;
		GEngine->pcfWorld->Paths2.WaypointPaths[index] = 0;
		GEngine->pcfWorld->Paths2.FreePathIndices.Add(index);
	}
#endif
	else
	{
		WaypointGPath* path = (WaypointGPath*)L.GetLightUD(1);
		if ( path ) delete path;
	}
	return 0;
}
// =========================================================================
static int luaPathfinding_GetShortest(lua_State* state)
{
	Script L(state);
	float len = 0;

#ifndef TRANSGAMING
	if( GEngine->World->UseNewPaths )
	{
		int index = L.GetInt(1);
		WaypointGPath2* path = GEngine->World->Paths2.WaypointPaths[index];
		if( path )
		{
			path->Empty();
			Vector from( L.GetFloat(2), L.GetFloat(3), L.GetFloat(4));
			Vector to( L.GetFloat(5), L.GetFloat(6), L.GetFloat(7));
			path->SetMoveConstraints( L.GetFloat(8,0.f), L.GetFloat(9,10000.f) );
			len = GEngine->World->Paths2.GetShortestPath( from, to, *path );
		}
	}
#else
	if( GEngine->pcfWorld->UseNewPaths )
	{
		int index = L.GetInt(1);
		WaypointGPath2* path = GEngine->pcfWorld->Paths2.WaypointPaths[index];
		if( path )
		{
			path->Empty();
			Vector from( L.GetFloat(2), L.GetFloat(3), L.GetFloat(4));
			Vector to( L.GetFloat(5), L.GetFloat(6), L.GetFloat(7));
			path->SetMoveConstraints( L.GetFloat(8,0.f), L.GetFloat(9,10000.f) );
			len = GEngine->pcfWorld->Paths2.GetShortestPath( from, to, *path );
		}
	}
#endif
	else
	{
		WaypointGPath *path = (WaypointGPath*)L.GetLightUD(1);
		if( path )
		{
			path->Empty();
			Vector from( L.GetFloat(2), L.GetFloat(3), L.GetFloat(4));
			Vector to( L.GetFloat(5), L.GetFloat(6), L.GetFloat(7));
#ifndef TRANSGAMING
			len = GEngine->World->Paths.GetShortestPath(from,to,GAStarDistanceHeuristic, *path);
#else
			len = GEngine->pcfWorld->Paths.GetShortestPath(from,to,GAStarDistanceHeuristic, *path);
#endif
		}
	}
	L.PushFloat(len);
	return 1;
}
// =========================================================================
static int luaPathfinding_GetNextPoint(lua_State* state)
{
	Script L(state);
	Vector pt(0,0,0), dir(0,0,0);

#ifndef TRANSGAMING
	if( GEngine->World->UseNewPaths )
#else
	if( GEngine->pcfWorld->UseNewPaths )
#endif
	{
		int index = L.GetInt(1);
#ifndef TRANSGAMING
		WaypointGPath2* path = GEngine->World->Paths2.WaypointPaths[index];
#else
		WaypointGPath2* path = GEngine->pcfWorld->Paths2.WaypointPaths[index];
#endif
		if( path )
			pt = path->Next(dir);
	}
	else
	{
		WaypointGPath* path = (WaypointGPath*)L.GetLightUD(1);
		if( path )
			pt = path->Next(dir);
	}

	L.PushFloat(pt.X);
	L.PushFloat(pt.Y);
	L.PushFloat(pt.Z);
	L.PushFloat(dir.X);
	L.PushFloat(dir.Y);
	L.PushFloat(dir.Z);
	return 6;
}
// =========================================================================
static int luaPathfinding_IsFinished(lua_State* state)
{
	Script L(state);
	int ret = 1;

#ifndef TRANSGAMING
	if( GEngine->World->UseNewPaths )
#else
	if( GEngine->pcfWorld->UseNewPaths )
#endif
	{
		int index = L.GetInt(1);
#ifndef TRANSGAMING
		WaypointGPath2* path = GEngine->World->Paths2.WaypointPaths[index];
#else
		WaypointGPath2* path = GEngine->pcfWorld->Paths2.WaypointPaths[index];
#endif
		if( path && !path->Finished() )
			ret = 0;
	}
	else
	{
		WaypointGPath* path = (WaypointGPath*)L.GetLightUD(1);
		if( path && !path->Finished() )
			ret = 0;
	}

	L.PushInt( ret );
	return 1;
}
// =========================================================================
static const luaL_reg pathfinding_lib[] = {
	{"Create",luaPathfinding_Create},
	{"Release",luaPathfinding_Release},
	{"GetShortest",luaPathfinding_GetShortest},
	{"GetNextPoint",luaPathfinding_GetNextPoint},
	{"IsFinished",luaPathfinding_IsFinished},
	{NULL, NULL}
};
// =========================================================================
static int luaPhysics_GetHavokBodyRotation(lua_State* state)
{
    Script L(state);
    void *e = L.GetLightUD(1);
#ifndef TRANSGAMING_NO_HAVOK
#ifndef TRANSGAMING
	Quaternion q = GEngine->World->Physics->GetHavokBodyRotation(e);
#else
    Quaternion q = GEngine->pcfWorld->Physics->GetHavokBodyRotation(e);
#endif

#else
    IdentityQuaternion q;

#endif

    L.PushFloat(q.W);
    L.PushFloat(q.X);
    L.PushFloat(q.Y);
    L.PushFloat(q.Z);
    return 4;
}
// =========================================================================
static int luaPhysics_GetHavokBodyVelocity(lua_State* state)
{
    Script L(state);
    void *e = L.GetLightUD(1);
#ifndef TRANSGAMING_NO_HAVOK
#ifndef TRANSGAMING
	Vector vel = GEngine->World->Physics->GetHavokBodyVelocity(e);
#else
    Vector vel = GEngine->pcfWorld->Physics->GetHavokBodyVelocity(e);
#endif

#else
    Vector vel(0, 0, 0);
#endif

    L.PushFloat(vel.X);
    L.PushFloat(vel.Y);
    L.PushFloat(vel.Z);
    L.PushFloat( sqrt(vel.X * vel.X + vel.Y * vel.Y + vel.Z * vel.Z));
    return 4;
}
// =========================================================================
static int luaPhysics_GetHavokBodyInfo(lua_State* state)
{
    Script L(state);
    void *rb = L.GetLightUD(1);
    void* result = 0;
    int joint = -1;
#ifndef TRANSGAMING_NO_HAVOK
    int res = GEngine->Physics->RigidBodyInfo( rb, &result, joint );
#else
    int res = 0;
#endif

    switch( res )
    {
    default:
    case 0:	// no info; unknown rigid body!!!
        L.PushInt(0);
        return 1;
    case 1:	// Physics Object
        L.PushInt(1);
        L.PushInt( result?((Entity*)result)->GIndex:0);	// Entity pointer
        return 2;
    case 2:	// Ragdoll
        L.PushInt(2);
        L.PushInt( result?((Entity*)result)->GIndex:0);	// Entity pointer
        L.PushInt(joint);			// Joint number
        return 3;
    };
}
// =========================================================================
static int luaPhysics_GetHavokBodyBarrierInfo(lua_State* state)
{
    Script L(state);
#ifndef TRANSGAMING_NO_HAVOK
	L.PushInt( GEngine->Physics->BarrierInfo( L.GetLightUD(1) ) );
#else
    L.PushInt(0);
#endif
	return 1;
}
// =========================================================================
static int luaPhysics_SetHavokBodyPosition(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
    Script L(state);
    void *e = L.GetLightUD(1);
    Vector v(L.GetFloat(2),L.GetFloat(3),L.GetFloat(4));

#ifndef TRANSGAMING
	GEngine->World->Physics->SetHavokBodyPosition(e,v);
#else
    GEngine->pcfWorld->Physics->SetHavokBodyPosition(e,v);
#endif

#endif
    return 0;
}
// =========================================================================
static int luaPhysics_GetHavokBodyPosition(lua_State* state)
{
    Script L(state);
#ifndef TRANSGAMING_NO_HAVOK
    void *e = L.GetLightUD(1);

#ifndef TRANSGAMING
	Vector v = GEngine->World->Physics->GetHavokBodyPosition(e);
#else
    Vector v = GEngine->pcfWorld->Physics->GetHavokBodyPosition(e);
#endif

#else
    Vector v(0, 0, 0);
#endif

    L.PushFloat(v.X);
    L.PushFloat(v.Y);
    L.PushFloat(v.Z);

    return 3;
}
// =========================================================================
static int luaPhysics_SetHavokBodyRotation(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
    Script L(state);
    void *e = L.GetLightUD(1);
    if (e)
    {
        Quaternion q(L.GetFloat(2),Vector(L.GetFloat(3),L.GetFloat(4),L.GetFloat(5)));

#ifndef TRANSGAMING
		GEngine->World->Physics->SetHavokBodyRotation(e,q);
#else
        GEngine->pcfWorld->Physics->SetHavokBodyRotation(e,q);
#endif

    }
#endif
    return 0;
}
// =========================================================================
static int luaPhysics_SetHavokBodyVelocity(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
    Script L(state);
    void *e = L.GetLightUD(1);
    if (e)
    {
        Vector v(L.GetFloat(2),L.GetFloat(3),L.GetFloat(4));
#ifndef TRANSGAMING
		GEngine->World->Physics->SetHavokBodyVelocity(e,v);
#else
        GEngine->pcfWorld->Physics->SetHavokBodyVelocity(e,v);
#endif

    }
#endif
    return 0;
}
// =========================================================================
static int luaPhysics_PinHavokBody(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
    Script L(state);
    void *e = L.GetLightUD(1);
    if (e)
#ifndef TRANSGAMING
		GEngine->World->Physics->PinHavokBody(e,L.GetBool(2,true));
#else
        GEngine->pcfWorld->Physics->PinHavokBody(e,L.GetBool(2,true));
#endif

#endif
    return 0;
}
// =========================================================================
static int luaPhysics_IsHavokBodyPinned(lua_State* state) {
    Script L(state);
#ifndef TRANSGAMING_NO_HAVOK
    void *e = L.GetLightUD(1);

#ifndef TRANSGAMING
	L.PushBool(GEngine->World->Physics->IsHavokBodyPinned(e));
#else
    L.PushBool(GEngine->pcfWorld->Physics->IsHavokBodyPinned(e));
#endif

#else
    L.PushBool(false);
#endif
	return 1;
}
// =========================================================================
static int luaPhysics_IsHavokBodyInWorld(lua_State* state)
{
    Script L(state);
#ifndef TRANSGAMING_NO_HAVOK
    void *e = L.GetLightUD(1);

#ifndef TRANSGAMING
	L.PushBool(GEngine->World->Physics->IsHavokBodyInWorld(e));
#else
    L.PushBool(GEngine->pcfWorld->Physics->IsHavokBodyInWorld(e));
#endif

#else
    L.PushBool(false);
#endif
	return 1;
}
// =========================================================================
static int luaPhysics_RemoveHavokBodyFromIS(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
    Script L(state);
    void *e = L.GetLightUD(1);

#ifndef TRANSGAMING
	GEngine->World->Physics->RemoveHavokBodyFromIS(e,L.GetBool(2,true));
#else
    GEngine->pcfWorld->Physics->RemoveHavokBodyFromIS(e,L.GetBool(2,true));
#endif

#endif
    return 0;
}
// =========================================================================
static int luaPhysics_GetHavokBodyActiveGroup(lua_State* state)
{
	Script L(state);
#ifndef TRANSGAMING_NO_HAVOK
	void *e = L.GetLightUD(1);

#ifndef TRANSGAMING
	BYTE b = GEngine->World->Physics->GetHavokBodyActiveGroup(e);
#else
	BYTE b = GEngine->pcfWorld->Physics->GetHavokBodyActiveGroup(e);
#endif

	L.PushInt(b);
#else
    L.PushInt(0);
#endif
    return 1;
}
// =========================================================================
static int luaPhysics_SetGravity(lua_State* state)
{
    Script L(state);
    if (GEngine && GEngine->Physics)
        GEngine->Physics->TWGravity = L.GetFloat(1);
    return 0;
}
// =========================================================================
static int luaPhysics_SetBunnyHopAcceleration(lua_State* state)
{
    Script L(state);
    if (GEngine && GEngine->Physics)
		GEngine->Physics->TWMPBunnyHopAcceleration = L.GetFloat(1);
    return 0;
}
// =========================================================================
static int luaPhysics_ActiveMeshGroupEnable(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
	Script L(state);
	unsigned char group = L.GetInt( 1, 0 );

#ifndef TRANSGAMING
	if( GEngine && GEngine->World && GEngine->World->Physics/* && group */)
		GEngine->World->Physics->ActiveMeshGroupEnable( group, L.GetBool( 2, false ) );
#else
	if( GEngine && GEngine->pcfWorld && GEngine->pcfWorld->Physics/* && group */)
		GEngine->pcfWorld->Physics->ActiveMeshGroupEnable( group, L.GetBool( 2, false ) );
#endif

#endif
    return 0;
}
// =========================================================================
static int luaPhysics_StaticMeshGroupEnable(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
	Script L(state);
	unsigned char group = L.GetInt( 1, 0 );

#ifndef TRANSGAMING
	if( GEngine && GEngine->World && GEngine->World->Physics)
		GEngine->World->Physics->StaticMeshesEnableByGroup( group, L.GetBool( 2, true ) );
#else
	if( GEngine && GEngine->pcfWorld && GEngine->pcfWorld->Physics)
		GEngine->pcfWorld->Physics->StaticMeshesEnableByGroup( group, L.GetBool( 2, true ) );
#endif

#endif
    return 0;
}
// =========================================================================
static int luaPhysics_ActiveMeshGroupStaticMeshEnable(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
	Script L(state);
	unsigned char group = L.GetInt( 1, 0 );

#ifndef TRANSGAMING
	if( GEngine && GEngine->World && GEngine->World->Physics/* && group */)
		GEngine->World->Physics->ActiveMeshGroupStaticMeshEnable( group, L.GetBool( 2, false ) );
#else
	if( GEngine && GEngine->pcfWorld && GEngine->pcfWorld->Physics/* && group */)
		GEngine->pcfWorld->Physics->ActiveMeshGroupStaticMeshEnable( group, L.GetBool( 2, false ) );
#endif

#endif
    return 0;
}
// =========================================================================
static int luaPhysics_ActiveMeshGroupRecurrentActivationEnable(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
	Script L(state);
	unsigned char group = L.GetInt( 1, 0 );

#ifndef TRANSGAMING
	if( GEngine && GEngine->World && GEngine->World->Physics/* && group */)
		GEngine->World->Physics->ActiveMeshGroupRecurrentActivationEnable( group, L.GetBool( 2, false ) );
#else
	if( GEngine && GEngine->pcfWorld && GEngine->pcfWorld->Physics/* && group */)
		GEngine->pcfWorld->Physics->ActiveMeshGroupRecurrentActivationEnable( group, L.GetBool( 2, false ) );
#endif

#endif
    return 0;
}
// =========================================================================
static int luaPhysics_ActiveMeshGroupActivate(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
	Script L(state);
	unsigned char group = L.GetInt( 1, 0 );

#ifndef TRANSGAMING
	if( GEngine && GEngine->World && GEngine->World->Physics )
		GEngine->World->Physics->ActiveMeshGroupActivate( group )
#else
	if( GEngine && GEngine->pcfWorld && GEngine->pcfWorld->Physics )
		GEngine->pcfWorld->Physics->ActiveMeshGroupActivate( group );
#endif

#endif
    return 0;
}
// =========================================================================
static int luaPhysics_ActiveMeshGroupSetActivationParams(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
	Script L(state);
	unsigned char group = L.GetInt( 1, 0 );

#ifndef TRANSGAMING
	if( GEngine && GEngine->World && GEngine->World->Physics )
		GEngine->World->Physics->ActiveMeshGroupSetActivationSettings( group, L.GetBool( 2, false ),
#else
	if( GEngine && GEngine->pcfWorld && GEngine->pcfWorld->Physics )
		GEngine->pcfWorld->Physics->ActiveMeshGroupSetActivationSettings( group, L.GetBool( 2, false ),
#endif
		L.GetFloat( 3, 0.6f ), L.GetFloat( 4, 8.0f ), L.GetFloat( 5, 150.0f ), L.GetFloat( 6, 5000.0f ),
		L.GetFloat( 7, 10.0f ), L.GetFloat( 8, 10.0f ), L.GetFloat( 9, 10.0f ) );
#endif
    return 0;
}
// =========================================================================
static int luaPhysics_StaticMeshEnable(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
	Script L(state);

#ifndef TRANSGAMING
	Entity *e = GEngine->World->GetEntityByIndex(L.GetInt(1));
	if( GEngine && GEngine->World && GEngine->World->Physics && e)
		GEngine->World->Physics->StaticMeshEnable( e, L.GetBool( 2, true ));
#else
	Entity *e = GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1));
	if( GEngine && GEngine->pcfWorld && GEngine->pcfWorld->Physics && e)
		GEngine->pcfWorld->Physics->StaticMeshEnable( e, L.GetBool( 2, true ));
#endif

#endif
	return 0;
}
// =========================================================================
static int luaPhysics_SetMaxRecursiveActivationDistance(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
	Script L(state);

#ifndef TRANSGAMING
	if( GEngine && GEngine->World && GEngine->World->Physics )
		GEngine->World->Physics->SetActiveMeshMaxRecursiveActivationDistance( L.GetFloat( 1, 1000000.0f ) );
#else
	if( GEngine && GEngine->pcfWorld && GEngine->pcfWorld->Physics )
		GEngine->pcfWorld->Physics->SetActiveMeshMaxRecursiveActivationDistance( L.GetFloat( 1, 1000000.0f ) );
#endif

#endif
	return 0;
}
// =========================================================================
static int luaPhysics_WarmUp(lua_State* state)
{
#ifndef TRANSGAMING_NO_HAVOK
    Script L(state);

#ifndef TRANSGAMING
	if( GEngine && GEngine->World && GEngine->World->Physics )
        GEngine->World->Physics->WarmUp(L.GetFloat(1,2.0f));
#else
    if( GEngine && GEngine->pcfWorld && GEngine->pcfWorld->Physics )
        GEngine->pcfWorld->Physics->WarmUp(L.GetFloat(1,2.0f));
#endif

#endif
    return 0;
}
// =========================================================================
static const luaL_reg physics_lib[] = {
//	{"CreateRigidConstraintBetweenEntities",luaPhysics_CreateRigidConstraintBetweenEntities},
//    {"CreateRigidConstraint",luaPhysics_CreateRigidConstraint},
    {"SetHavokBodyPosition",luaPhysics_SetHavokBodyPosition},
    {"GetHavokBodyPosition",luaPhysics_GetHavokBodyPosition},
    {"GetHavokBodyRotation",luaPhysics_GetHavokBodyRotation},
    {"GetHavokBodyVelocity",luaPhysics_GetHavokBodyVelocity},
    {"GetHavokBodyInfo",luaPhysics_GetHavokBodyInfo},
    {"GetHavokBodyBarrierInfo",luaPhysics_GetHavokBodyBarrierInfo},
    {"SetHavokBodyRotation",luaPhysics_SetHavokBodyRotation},
    {"SetHavokBodyVelocity",luaPhysics_SetHavokBodyVelocity},
    {"PinHavokBody",luaPhysics_PinHavokBody},
	{"IsHavokBodyPinned",luaPhysics_IsHavokBodyPinned},
	{"IsHavokBodyInWorld",luaPhysics_IsHavokBodyInWorld},
    {"RemoveHavokBodyFromIS",luaPhysics_RemoveHavokBodyFromIS},
	{"GetHavokBodyActiveGroup",luaPhysics_GetHavokBodyActiveGroup},
    {"SetGravity",luaPhysics_SetGravity},
    {"SetBunnyHopAcceleration",luaPhysics_SetBunnyHopAcceleration},
    {"ActiveMeshGroupEnable",luaPhysics_ActiveMeshGroupEnable},
    {"StaticMeshGroupEnable",luaPhysics_StaticMeshGroupEnable},
    {"ActiveMeshGroupStaticMeshEnable",luaPhysics_ActiveMeshGroupStaticMeshEnable},
    {"ActiveMeshGroupRecurrentActivationEnable",luaPhysics_ActiveMeshGroupRecurrentActivationEnable},
    {"ActiveMeshGroupActivate",luaPhysics_ActiveMeshGroupActivate},
    {"ActiveMeshGroupSetActivationParams",luaPhysics_ActiveMeshGroupSetActivationParams},
	{"StaticMeshEnable",luaPhysics_StaticMeshEnable},
	{"SetMaxRecursiveActivationDistance",luaPhysics_SetMaxRecursiveActivationDistance},
    {"WarmUp",luaPhysics_WarmUp},
	{NULL, NULL}
};
// =========================================================================
// REGION
// =========================================================================
static int luaRegion_BuildFromPoint(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	WorldRegion *wr = (WorldRegion *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeRegion);
#else
    WorldRegion *wr = (WorldRegion *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeRegion);
#endif

	if (wr)
	{
		Script table(state,2);
		DynamicArray<Vector> points;

		int n = table.GetCount();
		for (int ii=1;ii<=n;ii++)
		{
			Script k = table[ii];
			float x = k["X"].GetFloat();
			float y = k["Y"].GetFloat();
			float z = k["Z"].GetFloat();
			points.Add(Vector(x,y,z));
		}

		wr->CreateRegionFromPoints(points,1.0f);
	}
	return 0;
}
// =========================================================================
static const luaL_reg region_lib[] = {
	{"BuildFromPoint", luaRegion_BuildFromPoint},
	{NULL, NULL}
};
// =========================================================================
// Environment
// =========================================================================
static int luaEnvironment_SetAmbient(lua_State* state)
{
	Script L(state);
#ifndef TRANSGAMING
	Environment *e = (Environment *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeEnv);
#else
    Environment *e = (Environment *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeEnv);
#endif

	if( e)
	{
		if (L.GetBool(2))
			e->Defines |= Environment::EEnvAmbient;
		else
			e->Defines &= ~Environment::EEnvAmbient;
		e->AmbientColor = L.GetInt(3);
	}
	return 0;
}
// =========================================================================
static int luaEnvironment_SetDirLight(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Environment *e = (Environment *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeEnv);
#else
    Environment *e = (Environment *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeEnv);
#endif

	if( e)
	{
		if (L.GetBool(2))
			e->Defines |= Environment::EEnvDirLight;
		else
			e->Defines &= ~Environment::EEnvDirLight;

		Vector to;
		e->DirLight.SetDirection(Vector( L.GetFloat(3), L.GetFloat(4), L.GetFloat(5)));
		e->DirLight.Color  = L.GetInt(6);
		e->DirLight.Intensity = L.GetFloat(7);
		e->LightFadeTime = L.GetFloat(8);
	}
	return 0;
}
// =========================================================================
static int luaEnvironment_SetFog(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Environment *e = (Environment *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeEnv);
#else
    Environment *e = (Environment *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeEnv);
#endif

	if (e)
	{
		if (L.GetBool(2))
			e->Defines |= Environment::EEnvFog;
		else
			e->Defines &= ~Environment::EEnvFog;
		e->FogParams.Start = L.GetFloat(3);
		e->FogParams.End = L.GetFloat(4);
		e->FogParams.Density = L.GetFloat(5);
		e->FogParams.Color = L.GetInt(6);
	}
	return 0;

}

// =========================================================================

static int luaEnvironment_SetWater(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Environment *e = (Environment *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeEnv);
#else
    Environment *e = (Environment *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeEnv);
#endif

	if (e)
	{
		e->Water.FresnelBias		= L.GetFloat(2);
		e->Water.FresnelExponent	= L.GetFloat(3);
		e->Water.BumpHeight			= L.GetFloat(4);
		e->Water.WaveAmplitude		= L.GetFloat(5);
		e->Water.WaveFreq			= L.GetFloat(6);
#ifdef	TRANSGAMING
		e->Water.DeepWaterColor		= Color_TG(L.GetFloat(7)/255.0f,L.GetFloat(8)/255.0f,L.GetFloat(9)/255.0f);
		e->Water.ShallowWaterColor	= Color_TG(L.GetFloat(10)/255.0f,L.GetFloat(11)/255.0f,L.GetFloat(12)/255.0f);
#else
		e->Water.DeepWaterColor		= Color(L.GetFloat(7)/255.0f,L.GetFloat(8)/255.0f,L.GetFloat(9)/255.0f);
		e->Water.ShallowWaterColor	= Color(L.GetFloat(10)/255.0f,L.GetFloat(11)/255.0f,L.GetFloat(12)/255.0f);
#endif
		e->Water.WaveSpeed			= L.GetFloat(13);
		e->Water.WaterAmount		= L.GetFloat(14);
		e->Water.ReflectionAmount	= L.GetFloat(15);
		e->Water.WaterLevel			= L.GetFloat(16);

		bool reflect = L.GetBool(17);
		bool refract = L.GetBool(18);

		e->Water.ReflectFlags = 0;
		if (reflect)
			e->Water.ReflectFlags |= Entity::EReflectScene;
		if (refract)
			e->Water.ReflectFlags |= Entity::ERefractScene;

		e->Water.ReflectScale	= L.GetFloat(19);
		e->Water.RefractScale	= L.GetFloat(20);
		e->Water.PlaneShift		= L.GetFloat(21);
		e->Water.ReflectRadius	= L.GetFloat(22);
		e->Water.ReflectDist	= L.GetFloat(23);
		e->Water.ReflectSky		= L.GetBool(24,false);

		for (int i=0; i<2; ++i) {
			e->Water.Pan[i].X		= L.GetFloat(25+i*4);
			e->Water.Pan[i].Y		= L.GetFloat(26+i*4);
			e->Water.Tile[i].X		= L.GetFloat(27+i*4);
			e->Water.Tile[i].Y		= L.GetFloat(28+i*4);
		}

		//if (GEngine->pcfWorld->WaterMesh)
		//	GEngine->pcfWorld->WaterMesh->SetupWaterShader();
	}

	return 0;
}

// =========================================================================
static int luaEnvironment_AddLight(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Environment *e = (Environment *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeEnv);
#else
    Environment *e = (Environment *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeEnv);
#endif

	if (e)
	{
#ifndef TRANSGAMING
		Light *l = (Light *) GEngine->World->GetEntityByIndex(L.GetInt(2),Entity::ETypeLight);
#else
        Light *l = (Light *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(2),Entity::ETypeLight);
#endif
		if (l)
			e->AddLight(l);
	}
	return 0;

}
// =========================================================================
static int luaEnvironment_RemoveLight(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Environment *e = (Environment *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeEnv);
#else
    Environment *e = (Environment *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeEnv);
#endif

	if (e)
	{
#ifndef TRANSGAMING
		Light *l = (Light *) GEngine->World->GetEntityByIndex(L.GetInt(2),Entity::ETypeLight);
#else
        Light *l = (Light *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(2),Entity::ETypeLight);
#endif
		if (l)
			e->RemoveLight(l);
	}
	return 0;

}
// =========================================================================
static int luaEnvironment_RemoveLights(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Environment *e = (Environment *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeEnv);
#else
    Environment *e = (Environment *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeEnv);
#endif

	if (e)
	{
		e->RemoveLights();
	}
	return 0;

}
// =========================================================================
static int luaEnvironment_ResetReflectList(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Environment *e = (Environment *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeEnv);
#else
    Environment *e = (Environment *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeEnv);
#endif

	if (e) {
		e->Water.ReflectList.ClearArray();
	}
	return 0;

}
// =========================================================================
static int luaEnvironment_AddReflectMesh(lua_State* state)
{
	Script L(state);

#ifndef TRANSGAMING
	Environment *e = (Environment *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeEnv);
#else
    Environment *e = (Environment *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeEnv);
#endif

	if (e) {
		const char* s = L.GetString(2);
		Assert(s);
#ifndef TRANSGAMING
		Entity* entity = GEngine->World->GetEntityByName(s);
#else
		Entity* entity = GEngine->pcfWorld->GetEntityByName(s);
#endif

		if (!entity) {
			GLog.Print("WTF: entity \"%s\" NOT FOUND\n", s);
		}

		//Assert(entity);
		//Assert(entity->AsMesh());

        //WorldMesh* mesh = (WorldMesh*) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(2),Entity::ETypeMesh);
		//e->Water.ReflectList.Add(entity->AsMesh());

		if (entity && entity->AsMesh())
			e->Water.ReflectList.Add(entity->AsMesh());
	}
	return 0;

}
// =========================================================================
static const luaL_reg env_lib[] = {
	{"SetAmbient",	luaEnvironment_SetAmbient},
	{"SetFog",		luaEnvironment_SetFog},
	{"SetWater",	luaEnvironment_SetWater},
	{"SetDirLight",	luaEnvironment_SetDirLight},
	{"AddLight",	luaEnvironment_AddLight},
	{"RemoveLight", luaEnvironment_RemoveLight},
	{"RemoveLights",luaEnvironment_RemoveLights},
	{"ResetReflectList",luaEnvironment_ResetReflectList},
	{"AddReflectMesh",luaEnvironment_AddReflectMesh},
	{NULL, NULL}
};
// =========================================================================
static int luaFogVol_Setup( lua_State* state )
{
    Script L(state);

#ifndef TRANSGAMING
	Volume *e = (Volume *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeVolume);
#else
    Volume *e = (Volume *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeVolume);
#endif

    if (e)
    {
        e->VolColor = GScript.GetInt(2);
        e->VolEnd = GScript.GetFloat(3);
    }
    return 0;
}
// =========================================================================
static int luaFogVol_GetProperties( lua_State* state )
{
    Script L(state);

#ifndef TRANSGAMING
	Volume *e = (Volume *) GEngine->World->GetEntityByIndex(L.GetInt(1),Entity::ETypeVolume);
#else
    Volume *e = (Volume *) GEngine->pcfWorld->GetEntityByIndex(L.GetInt(1),Entity::ETypeVolume);
#endif

    if (e)
    {
        GScript.PushInt(e->VolColor.R);
        GScript.PushInt(e->VolColor.G);
        GScript.PushInt(e->VolColor.B);
        GScript.PushFloat(e->VolEnd);
        return 4;
    }
    return 0;
}
// =========================================================================
static const luaL_reg fogvol_lib[] = {
    {"Setup", luaFogVol_Setup},
    {"GetProperties", luaFogVol_GetProperties},
    {NULL, NULL}
};
// =========================================================================
static int luaGameSpy_SetServerInfo( lua_State* state )
{
    Script L(state);
    if (!GEngine->Net) return 0;

    GameSpyData2 *sd = &GEngine->Net->GameSpyInfo;

    sd->GameVersion  = L.GetString(1);
    sd->ServerName    = L.GetString(2);
    sd->Password      = L.GetString(3);
    sd->MapName       = L.GetString(4);
    sd->GameType      = L.GetString(5);
    sd->MaxPlayers    = L.GetInt(6);
    sd->MaxSpectators = L.GetInt(7);
    sd->FragLimit     = L.GetInt(8);
    sd->TimeLimit     = L.GetInt(9);
    sd->TimeElapsed   = L.GetInt(10);
	sd->GameState	  = L.GetString(11);
	sd->NumPlayers	  = L.GetInt(12);
	sd->NumSpectators = L.GetInt(13);
    return 0;
}
// =========================================================================
static int luaGameSpy_SetPlayerInfo( lua_State* state )
{
    if (!GEngine->Net) return 0;

    Script L(state);
    int i = L.GetInt(1);
    if (i<0 || i>=MAX_PLAYERS) return 0;

    GameSpyPlayerData2 *pd = &GEngine->Net->GameSpyInfo.Players[i];

    pd->Valid   = true;
    pd->Name   = L.GetString(2);
    pd->Score  = L.GetInt(3);
    pd->Ping   = L.GetInt(4);
    pd->Deaths = L.GetInt(5);
    pd->Team   = L.GetInt(6);
    return 0;
}
// =========================================================================
static int luaGameSpy_SetGameMode( lua_State* state )
{
    if (!GEngine->Net) return 0;

	// 1 - waiting
	// 2 - playing
	// 3 - exiting

    Script L(state);
    int i = L.GetInt(1);
	if( ( i < 1 ) || ( i > 3 ) )
	{
		GLog.Print( "Warning: invalid parameter given to GAMESPY.SetGameMode(): %d\n", i );
		return 0;
	}
	GEngine->Net->GameSpyInfo.SetMode( i );
    return 0;
}
// =========================================================================
static int luaGameSpy_RemoveAllPlayers( lua_State* state )
{
    if (!GEngine->Net) return 0;

    for (int i=0; i<MAX_PLAYERS; i++)
        GEngine->Net->GameSpyInfo.Players[i].Valid = false;

    return 0;
}
// =========================================================================
static const luaL_reg gamespy_lib[] = {
    {"SetServerInfo"   , luaGameSpy_SetServerInfo},
    {"SetPlayerInfo"   , luaGameSpy_SetPlayerInfo},
    {"SetGameMode"     , luaGameSpy_SetGameMode},
    {"RemoveAllPlayers", luaGameSpy_RemoveAllPlayers},
    {NULL, NULL}
};
// =========================================================================

static int luaVectorRotate(lua_State* state) {
	Script L(state);
	Vector v(L.GetFloat(1),L.GetFloat(2),L.GetFloat(3));
	Matrix m;
	Vector r(L.GetFloat(4),L.GetFloat(5),L.GetFloat(6));
	m.FromEulerAnglesXYZ(r);
	v = m.Transform(v);
	L.PushFloat(v.X);
	L.PushFloat(v.Y);
	L.PushFloat(v.Z);
	return 3;
}
// =========================================================================
static int luaVectorRotateByQuat(lua_State* state) {
	Script L(state);
	Vector v(L.GetFloat(1),L.GetFloat(2),L.GetFloat(3));
	Quaternion q(L.GetFloat(4),L.GetFloat(5),L.GetFloat(6),L.GetFloat(7));
	v = q * v;
	L.PushFloat(v.X);
	L.PushFloat(v.Y);
	L.PushFloat(v.Z);
	return 3;
}
// =========================================================================
static int luaVectorInverseRotateByQuat(lua_State* state) {
    Script L(state);
    Vector v(L.GetFloat(1),L.GetFloat(2),L.GetFloat(3));
    Quaternion q(L.GetFloat(4),L.GetFloat(5),L.GetFloat(6),L.GetFloat(7));
    q.Normalize();
    v = q.UnitInverse() * v;
    L.PushFloat(v.X);
    L.PushFloat(v.Y);
    L.PushFloat(v.Z);
    return 3;
}
// =========================================================================
static int luaVectorAngle( lua_State *state )
{
	Script L( state );
	Vector v1( L.GetFloat( 1 ), L.GetFloat( 2 ), L.GetFloat( 3 ) );
	Vector v2( L.GetFloat( 4 ), L.GetFloat( 5 ), L.GetFloat( 6 ) );
	v1.Normalize();
	v2.Normalize();

	float angle = acosf( v1.Dot( v2 ) );

	L.PushFloat( angle );

	return 1;
}
// =========================================================================
#ifndef TRANSGAMING
static int luaProfiler_LOGIC1(lua_State* state) {
	Script L(state);
	GProfiler.SetState( UPDATE_LOGIC1 );
	return 0;
}
static int luaProfiler_LOGIC2a(lua_State* state) {
	Script L(state);
	GProfiler.SetState( UPDATE_LOGIC2a );
	return 0;
}
static int luaProfiler_LOGIC2b(lua_State* state) {
	Script L(state);
	GProfiler.SetState( UPDATE_LOGIC2b );
	return 0;
}
static int luaProfiler_LOGIC2c(lua_State* state) {
	Script L(state);
	GProfiler.SetState( UPDATE_LOGIC2c );
	return 0;
}

static int luaProfiler_LOGIC3a(lua_State* state) {
	Script L(state);
	GProfiler.SetState( UPDATE_LOGIC3a );
	return 0;
}
static int luaProfiler_LOGIC3b(lua_State* state) {
	Script L(state);
	GProfiler.SetState( UPDATE_LOGIC3b );
	return 0;
}
static int luaProfiler_LOGIC3c(lua_State* state) {
	Script L(state);
	GProfiler.SetState( UPDATE_LOGIC3c );
	return 0;
}

static int luaProfiler_LOGIC4a(lua_State* state) {
	Script L(state);
	GProfiler.SetState( UPDATE_LOGIC4a );
	return 0;
}

static int luaProfiler_LOGIC5(lua_State* state) {
	Script L(state);
	GProfiler.SetState( UPDATE_LOGIC5);
	return 0;
}
#endif

// =========================================================================
static int luaGetRegistry(lua_State* state) {
	lua_getregistry(state);
	return 1;
}
// =========================================================================
static int luaCreatePlayer(lua_State* state)
{
	Script L(state);
    L.PushInt(GEngine->Game->CreatePlayer(L.GetString(1,""),L.GetBool(2,false))->GIndex);
	return 1;
}
// =========================================================================
static int luaGetEngineVersionString(lua_State* state)
{
	Script L(state);
	char ver[] = { '1', '.', '4', 0 };
	L.PushString( ver );
    return 1;
}
// =========================================================================
static int luaIsDedicatedServer(lua_State* state)
{
    Script L(state);
#ifndef TRANSGAMING_NO_DEDICATED_SERVER
    if (GEngine && GEngine->GameState == PCFSystem::DedicatedServer )
        L.PushBool(true);
    else
#endif
        L.PushBool(false);

    return 1;
}
// =========================================================================
static int luaAddBitFlag(lua_State* state)
{
    Script L(state);
    int i = L.GetInt(1);
    i |= L.GetInt(2);
    L.PushInt(i);
    return 1;
}
// =========================================================================
static int luaIsBitFlag(lua_State* state)
{
    Script L(state);
    L.PushBool( L.GetInt(1) & L.GetInt(2) );
    return 1;
}
// =========================================================================
static int luaRemoveBitFlag(lua_State* state)
{
    Script L(state);
    int i = L.GetInt(1);
    i &= ~L.GetInt(2);
    L.PushInt(i);
    return 1;
}
// =========================================================================
static int luaReplaceBitFlag(lua_State* state)
{
    Script L(state);
    int i = L.GetInt(1);
    i &= ~L.GetInt(2);
    i |= L.GetInt(3);
    L.PushInt(i);
    return 1;
}
// =========================================================================
static int luaIsFinalBuild(lua_State* state)
{
    Script L(state);
    #ifdef FINAL_BUILD
    L.PushBool(true);
    #else
    L.PushBool(false);
    #endif
    return 1;
}
// =========================================================================
static int luaIsConstPhysicsTick(lua_State* state)
{
    Script L(state);
    L.PushBool(true);
    return 1;
}
// =========================================================================
static int luaIsMPDemo(lua_State* state)
{
    Script L(state);
#ifdef TRANSGAMING
    L.PushBool(false);
#else
    #ifdef MPDEMO
    L.PushBool(true);
    #else
    L.PushBool(false);
    #endif
#endif
    return 1;
}

// =========================================================================
static int luaIsNewNetcode(lua_State* state)
{
	Script L(state);
	L.PushBool(true);
	return 1;
}
// =========================================================================
ENGINE_API bool IsPKInstalled()
{

#ifdef TRANSGAMING
    return true;
#else
# ifdef STAND_ALONE_SERVER
	return true;
# endif

	ScriptObject cfg = GScript.Globals()["Cfg"];
	String lang = cfg["Language"].GetString();
	if( lang != "polish" )
		return true;

	DWORD mode = EFindFiles;
	DynamicArray<String> files;

	GFileMan.FindFiles("../Data/Maps/1x0*.mpk",files,mode);

	if( files.Size() > 3 )
	{
		GFileMan.FindFiles("../Data/Maps/2x0*.mpk",files,mode);
		if( files.Size() > 3 )
		{
			GFileMan.FindFiles("../Data/Maps/3x0*.mpk",files,mode);

			if( files.Size() > 3 )
			{
				return true;
			}
		}
	}

	return false;
#endif
}
// =========================================================================
ENGINE_API bool IsBooHInstalled()
{
    // eric-> the ONLY difference between the BooH game and the normal/black edition
    //   game is the splash screen that is loaded and which menus are displayed after
    //   choosing the game difficulty and after clicking on the back button from the
    //   game map.
#ifndef TRANSGAMING
# ifdef STAND_ALONE_SERVER
	return true;
# endif

	ScriptObject cfg = GScript.Globals()["Cfg"];
	String lang = cfg["Language"].GetString();

	DWORD mode = EFindFiles;
	DynamicArray<String> files;

	GFileMan.FindFiles("../Data/Maps/C6L*.mpk",files,mode);

	if( files.Size() > 3 )
	{
		return true;
	}
#endif
	return false;
}
// =========================================================================
ENGINE_API bool IsBlackEdition()
{
#ifdef TRANSGAMING
    //return access("../Data/Textures/HUD/Menu_black.dds", 0) == 0;

    // eric-> we'll always claim this copy IS NOT the black edition of the game.
    //   The only differences between the two editions are the background textures
    //   that get loaded and some other small textures that get loaded.  The only
    //   behavioural change that occurs as a result of being black edition is that
    //   the credits menu returns to the the options menu in black edition, and the
    //   main menu in normal edition.
    return false;
#else
    DWORD mode = EFindFiles;
	DynamicArray<String> files;

	GFileMan.FindFiles("../Data/Textures/HUD/Menu_black.dds",files,mode);

	if( files.Size() > 0 )
		return true;

	return false;
#endif
}

// =========================================================================
static int luaIsPKInstalled(lua_State* state)
{
	Script L(state);
	L.PushBool(IsPKInstalled());
	return 1;
}
// =========================================================================
static int luaIsBooHInstalled(lua_State* state)
{
	Script L(state);
	L.PushBool(IsBooHInstalled());
	return 1;
}
// =========================================================================
static int luaIsBlackEdition(lua_State* state)
{
	Script L(state);
	L.PushBool(IsBlackEdition());
	return 1;
}

///---------------------------------------------------------------------------
/// \brief Print the Lua-to-C API stack.
///
/// This is a modified version of code from the Lua Programming Manual 2ed.
/// p.225
///---------------------------------------------------------------------------
static int luaPrintApiStack(lua_State * L)
{
	int top = lua_gettop(L);
	for (int i = 1; i <= top; ++i)
	{
		int t = lua_type(L, i);
		switch (t)
		{
		case LUA_TSTRING:
			printf("'%s'", lua_tostring(L, i));
			break;

		case LUA_TBOOLEAN:
			printf(lua_toboolean(L, i) ? "true" : "false" );
			break;

		case LUA_TNUMBER:
			printf("%g", lua_tonumber(L, i));
			break;

		default:
			printf("%s", lua_typename(L, t));
			break;
		}
		// print stack on one line, with seperators.
		printf("  ");
	}
	printf("\n");
}

//----------------------------------------------------------------------------
// AAARGH! lua_istring returns true for BOTH strings and numbers.
// lua_isnumber returns true for BOTH strings and numbers.

// lua_tostring and lua_tolstring convert the tested value to a string
// so, you can clobber your table keys and kill the iteration!
// In short, calling lua_istring before lua_to*string is not good enough!

// Never call lua_tostring or tolstring during table iteration, on the table keys,
// unless lua_type(key) is LUA_TSTRING

static int isCopyable(lua_State * L, int keyStackId, int valStackId)
{
	// Only deepcopy tables...
	if ( LUA_TTABLE != lua_type(L, valStackId) )
	{
		return false;
	}

	Assert( lua_checkstack(L, 1) );
	const bool hasMetatable = lua_getmetatable(L, valStackId);
	if (hasMetatable)
	{
		lua_pop(L, 1);	// metatable was pushed onto stack.
		return false;
	}

	// So far, so good. Now check against key naming convention to see
	// if we should copy by value.

	const int keyType = lua_type(L, keyStackId);
	if ( keyType != LUA_TSTRING )
	{
		// We can't perform any string checks on this, since we don't
		// want to mutate the key into a string -- this would mess
		// up our containing loop iteration.

		return true;
	}

	Assert( keyType == LUA_TSTRING );
	const char * str = lua_tostring(L, keyStackId);

	//no name!
	Assert( str );
    if (!str)
	{
        return false;
	}

	const size_t strLength = strlen(str);

    //potentially a reference member => skip
    if (strLength > 0 && str[0] == '_')
	{
        if (strLength > 2 && str[1] == 'r' && str[2] == '_')
            return false;

        else if (strLength > 3 && str[1] == 'O' && str[2] == 'b' && str[3] == 'j')
            return false;
    }

    //potentially a static or reference object => skip
    else if (strLength > 1)
	{
        if (str[0] == 'r' && str[1] == '_')
            return false;

        if (str[0] == 's' && str[1] == '_')
            return false;

        else if (strLength > 2 && str[0] == 'O' && str[1] == 'b' && str[2] == 'j')
            return false;
    }

	// Okay -- copy by value!
    return true;
}

//----------------------------------------------------------------------------
// Copy a table.
// Expects the source table to be on top of the stack.
//----------------------------------------------------------------------------
static int luaShallowCopy(lua_State *L)
{
	// STACK bot> src <top
	int src = lua_gettop(L);
	Assert( lua_istable(L, src) );

	lua_newtable(L);

	// STACK bot> src, dst <top
	int dst = lua_gettop(L);

	// Start iterating src at first index.
	lua_pushnil(L);
	// STACK bot> src, dst, nil < top

	// lua_next pops current key from stack, and pushes next
	// key, and next value.
	while (lua_next(L, src))
	{
		// STACK bot> src, dst, src_key, src_val <top

		// lua_settable pops k and v from the stack, but we
		// need k in order to continue iterating.

		lua_pushvalue(L, -2);

		// STACK bot> src, dst, src_key, src_val, src_key <top

		lua_insert(L, -2);

		// STACK bot> src, dst, src_key, src_key, src_val <top

		// src key is at index -2 and src value is at index -1
		// this is exactly the placement lua_settable needs.

		lua_settable(L, dst);

		// STACK bot> src, dst, src_key <top
	}

	Assert( lua_gettop(L) == dst );
	return 1;
}

///---------------------------------------------------------------------------
/// \brief Copy a Lua object (table) by value, except for static and reference
/// \brief members, that are copied by reference.
///
/// @param lua stack top [-1] is the object to be copied
/// @return lua stack top [-1] will be the copy of the given object.
/// @see isCopyable for the naming convention.
///
/// People Can Fly uses tables as objects, along with a naming convention to
/// determine which members are per-instance, and which are shared. This
/// function copies a PCF object, attempting to give the new object its own
/// copy of any sub-tables, except for ones with name prefixes that indicate
/// that the member is shared between all instances.
///---------------------------------------------------------------------------
static int luaClone(lua_State *L)
{
	const int initial_top = lua_gettop(L);

	//if not t then return nil end
    if (lua_isnoneornil(L, -1))
	{
		Assert( lua_checkstack(L, 1) );
        lua_pushnil(L);

        return 1;
    }

	// If it's not a table, then just duplicate it.
	if ( !lua_istable(L, -1) )
	{
		Assert( lua_checkstack(L, 1) );
		lua_pushvalue(L, -1);

		return 1;
	}

	// STACK bot> src <top
	int src = lua_gettop(L);
	Assert( lua_checkstack(L, 1) );
	lua_newtable(L);

	// STACK bot> src, dst <top
	int dst = lua_gettop(L);

	// STACK bot> src, dst <top
	const int top_before_while = lua_gettop(L);

	// Start iterating src at first index.
	Assert( lua_checkstack(L, 1) );
	lua_pushnil(L);
	// STACK bot> src, dst, nil < top

	// lua_next pops current key from stack, and pushes next
	// key, and next value.
	Assert( lua_checkstack(L, 1) );
	while (lua_next(L, src))
	{
		// STACK bot> src, dst, src_key, src_val <top
		Assert( lua_gettop(L) == dst + 2 );

		// lua_settable pops k and v from the stack, but we
		// need k in order to continue iterating. So dup k.

		Assert( lua_checkstack(L, 1) );
		lua_pushvalue(L, -2);

		// STACK bot> src, dst, src_key, src_val, src_key <top

		lua_insert(L, -2);

		// STACK bot> src, dst, src_key, src_key, src_val <top
		Assert( lua_gettop(L) == 3 + dst );

		// Either copy a reference to, or deep copy this value,
		// depending on whether it's a table, and the name of the key.

		if (isCopyable(L, -2, -1))
		{
			// COPY BY VALUE
			// STACK bot> src, dst, src_key, src_key, src_val <top

			Assert( lua_checkstack(L, 1) );
			luaClone(L);	// copy src_val table

			// STACK bot> src, dst, src_key, src_key, src_val, dst_val <top
			Assert( lua_gettop(L) == 4 + dst );

			// don't need source val, we've copied it into dest val.
			lua_remove(L, -2);

			// STACK bot> src, dst, src_key, src_key, dst_val <top

			lua_settable(L, dst);

			// STACK bot> src, dst, src_key <top
			Assert( lua_gettop(L) == 1 + dst );
		}
		else
		{
			// COPY BY REFERENCE
			// STACK bot> src, dst, src_key, src_key, src_val <top

			Assert( lua_checkstack(L, 1) );
			lua_settable(L, dst);	// consumes src_key and src_val

			// STACK bot> src, dst, src_key <top
			Assert( lua_gettop(L) == 1 + dst );
		}

		// STACK bot> src, dst, src_key <top
		Assert( lua_gettop(L) == dst + 1 );
	}
	// STACK bot> src, dst <top
	Assert( lua_gettop(L) == dst );

	return 1;
}

//----------------------------------------------------------------------------
// Want:
// floatmod(2.6, -1) => 0.6
// floatmod(-2.6, 1) => -0.6
// floatmod(10.1, 5) => 0.1
//
static float floatmod(float num, float denom)
{
	const float sign = (num >= 0.f) ? 1.f : -1.f;

	const float abs_denom = fabs(denom);
	const float factor = fabs(num) / abs_denom;

	const float remainder = factor - floor(factor);
	return sign * remainder * abs_denom;
}

//----------------------------------------------------------------------------
/// @param float angle a1 (stack index 1)
/// @param float angle a2 (stack index 2)
static int luaAngDist(lua_State * L)
{
	const float a2 = lua_tonumber(L, -1);
	const float a1 = lua_tonumber(L, -2);

//  local d1 = math.mod( math.abs(a2-a1) , 2*math.pi)
	const float d1 = floatmod( fabs(a2 - a1),  F_TWOPI);

//	local d2=2*math.pi-d1
	const float d2 = F_TWOPI - d1;

//	local m=math.min(d1,d2)
	float m = (d1 < d2) ? d1 : d2;

//	if (d1<d2) then
	if ( d1 < d2)
	{
//		if(a2>=a1) then m=d1 else m=-d1 end
		if (a2 >= a1)
			m = d1;
		else
			m = -d1;
	}
//	else
	else
	{
//		if(a2>=a1) then m=-d2 else m=d2 end
		if (a2 >= a1)
			m = -d2;
		else
			m = d2;
	}

//    return math.mod(m,math.pi*2)
	lua_checkstack(L, 1);
	lua_pushnumber(L, floatmod(m, F_TWOPI));
	return 1;
}

///---------------------------------------------------------------------------
/// \brief Iterate the supplied AiBrain's goals (states), looking for the one
/// \brief that's currently rated best.
///
/// @param lua stack (-1) is the AiBrain object to use. Essentially, "this".
/// @return the number of returned values (1)
///			lua stack (-1) will be the key of the highest-rated goal in this
///			AiBrain's _goals table.
///
/// This iteration was very slow in Lua. There was a surprising amount of
/// time spent in the Lua version of the function, even though all the work
/// was done in its children. Now we do the iteration in C++, but still pass
/// the real work on to the Lua Evaluate() functions...
///---------------------------------------------------------------------------
static int luaAiBrainEvaluateGoals(lua_State * L)
{
	// make sure we got a parameter of the correct type.
	Assert( lua_gettop(L) > 0 );
	Assert( lua_istable(L, -1) );

	// STACK ... aibrain <top
	//luaPrintApiStack(L);

	const int idAiBrain = lua_gettop(L);

	lua_pushstring( L, "_goals" );

	// STACK ... aibrain "_goals" <top
	//luaPrintApiStack(L);

	lua_gettable( L, -2 );

	// STACK ... aibrain aibrain._goals <top
	//luaPrintApiStack(L);

	const int idGoals = lua_gettop(L);

	//local eval = -1
	//local currentEval = -1
	//local goal = nil

	float highestScore = -1.f;
	float currentScore = -1.f;
	int highestScoredKey = -1;

	//for i,v in self._goals do

	// Start iterating src at first index.
	lua_pushnil(L);

	// STACK ... aibrain aibrain._goals nil
	//luaPrintApiStack(L);

	// lua_next pops current key from stack, and pushes next
	// key, and next value.
	while (lua_next(L, idGoals))
	{
		// STACK ... aibrain aibrain._goals cur_key cur_goal <top
		//luaPrintApiStack(L);

		const int idCurGoal = lua_gettop(L);

		//	currentEval = v:Evaluate(self)

		// Use lua_pcall to call the Evaluate lua function. Need to set up
		// function pointer and args first...

		lua_pushstring( L, "Evaluate" );
		// STACK ... aibrain aibrain._goals cur_key cur_goal "Evaluate" <top
		//luaPrintApiStack(L);

		lua_gettable( L, -2 );
		// STACK ... aibrain aibrain._goals cur_key cur_goal cur_goal.Evaluate <top
		//luaPrintApiStack(L);

		// Evaluate uses object-oriented calling style, so first we push a self (this)
		// argument.

		lua_pushvalue( L, -2 );
		// STACK ... aibrain aibrain._goals cur_key cur_goal cur_goal.Evaluate cur_goal <top
		//luaPrintApiStack(L);

		lua_pushvalue( L, -6 );
		// STACK ... aibrain aibrain._goals cur_key cur_goal cur_goal.Evaluate cur_goal aibrain <top
		//luaPrintApiStack(L);

		const int numArgs = 2;
		const int numResults = 1;

		lua_call( L, numArgs, numResults );
		// STACK ... aibrain aibrain._goals cur_key cur_goal currentEval <top
		//luaPrintApiStack(L);

		currentScore = lua_tonumber(L, -1);

		//	if currentEval > eval then
		if ( currentScore > highestScore )
		{
		//		eval = currentEval
		//		goal = i

			highestScore = currentScore;
			highestScoredKey = (int)lua_tonumber(L, -3);	// cur_key
		}
		// STACK ... aibrain aibrain._goals cur_key cur_goal currentEval <top
		//luaPrintApiStack(L);

		lua_pop(L, 2);
		// STACK ... aibrain aibrain._goals cur_key <top
	}
	// STACK ... aibrain aibrain._goals <top

	if ( highestScoredKey > 0 )
	{
		lua_pushnumber(L, highestScoredKey);
		// STACK ... aibrain aibrain._goals best_key <top
		//luaPrintApiStack(L);
	}
	else
	{
		lua_pushnil(L);
	}
	// STACK ... aibrain aibrain._goals (best_key or nil) <top
	//luaPrintApiStack(L);

	// The Lua function return mechanism expects to find the the function,
	// followed by its return values on the stack. Remove our temporary stuff.

	lua_remove(L, -2);
	// STACK ... aibrain (best_goal or nil) <top
	//luaPrintApiStack(L);

	return 1;
}


//----------------------------------------------------------------------------
/// Use this function to help time function-call overhead.
/// @params None
/// @return cumulative time in seconds executing body of every invocation of
///			this function.
///

#ifdef TRANSGAMING_PSP
extern "C" float PSP_GetCurrentTimeInSeconds();
#elif defined(TRANSGAMING_PS2)
extern "C" float PS2_GetCurrentTimeInSeconds();
#endif

static int luaTimeBody(lua_State * L)
{
	static float bodySeconds = 0.f;

	float (*timeInSecondsFnc)() = NULL;
#ifdef TRANSGAMING_PSP
	timeInSecondsFnc = PSP_GetCurrentTimeInSeconds;
#elif TRANSGAMING_PS2
	timeInSecondsFnc = PS2_GetCurrentTimeInSeconds;
#else
	lua_pushnumber(L, 0.f);
	return 1;
#endif

	const char * s1 = lua_tostring(L, 1);
	const char * s2 = lua_tostring(L, 2);

	const float t0 = timeInSecondsFnc();
	printf("%s %s\n", s1, s2);
	const float dt = timeInSecondsFnc() - t0;
	bodySeconds += dt;

	lua_pushnumber(L, bodySeconds);
	return 1;
}

//----------------------------------------------------------------------------
static int luaIsPSP(lua_State* state){
    Script L(state);

#if defined( TRANSGAMING_PSP ) || defined( TRANSGAMING_WINDOWS )
	L.PushBool(true);
#else
	L.PushBool(false);
#endif
    return 1;
}

static int luaIsPS2(lua_State* state){
    Script L(state);

#ifdef TRANSGAMING_PS2
	L.PushBool(true);
#else
	L.PushBool(false);
#endif
    return 1;
}

#ifdef TRANSGAMING
static int luaIsDebug(lua_State *state){
    Script L(state);

#ifdef DEBUG
    L.PushBool(true);
#else
    L.PushBool(false);
#endif
    return 1;
}

static int luaIsSilent(lua_State *state){
    Script L(state);

#ifdef TRANSGAMING_SILENT_DEBUG
    L.PushBool(true);
#else
    L.PushBool(false);
#endif
    return 1;
}

static int luaIsLogging(lua_State *state){
    Script L(state);

#ifdef DEBUG
    L.PushBool(false);
#elif defined(TRANSGAMING_LOGGING)
    L.PushBool(true);
#else
    L.PushBool(false);
#endif
    return 1;
}
#endif

#ifdef CD_CHECK_ON

// =========================================================================
// WARNING: This is a fake CD-Check method
static int luaCdCheck1(lua_State* state)
{
    TRACE("[engineToLua.cpp]::luaCdCheck1(): checking CD label\n");
	char label[13];
	label[6]  = PK_CD_LABEL_06;
	label[8]  = PK_CD_LABEL_08;
	Script L(state);
	label[9]  = PK_CD_LABEL_09;
	label[10] = PK_CD_LABEL_10;
	label[0]  = PK_CD_LABEL_00;
	char dbits[110], vol[256];
	label[2]  = PK_CD_LABEL_02;
	label[4]  = PK_CD_LABEL_04;
	label[11] = PK_CD_LABEL_11;
	DWORD mf, sf, sno;
	label[3]  = PK_CD_LABEL_03;
	label[7]  = PK_CD_LABEL_07;
	GetLogicalDriveStrings( 110, dbits );
	label[5]  = PK_CD_LABEL_05;
	label[1]  = PK_CD_LABEL_01;

	for( int i = 0; *(dbits+i); i+= 4 )
		if( GetDriveType( dbits+i ) == DRIVE_CDROM )
		{
			GetVolumeInformation( dbits+i, vol, sizeof(vol), &sno, &mf, &sf, NULL, NULL );
			if( ( vol[0] == 'P' ) &&
				( vol[1] == 'K' ) &&
				( vol[2] == '_' ) &&
				( ( ( vol[3] == '1' ) && ( vol[4] == 0 ) ) ||
				  ( ( vol[5] == '_' ) && ( vol[6] == '1' ) && ( vol[7] == 0 ) ) ) )
			{
				L.PushString( dbits+i );
				return 1;
			}

			if( ( vol[0] == 'P' ) &&
				( vol[1] == 'K' ) &&
				( vol[2] == '_' ) &&
				( vol[3] == 'P' ) &&
				( vol[4] == 'L' ) &&
				( vol[5] == '_' ) &&
				( vol[6] == '1' ) &&
				( vol[7] == 0 ) )
			{
				L.PushString( dbits+i );
				return 1;
			}
/*
			if( stricmp( label, vol ) == 0 )
			{
				L.PushString( dbits+i );
				return 1;
			}
*/
		}
	L.PushNil();
	return 1;
}

#endif

static int luaGetGCCount(lua_State* state) {
	Script L(state);
	L.PushFloat(L.GetGCCount());
	return 1;
}

// =========================================================================
#pragma warning( default : 4311 4312 4244 4800)
// =========================================================================
void Engine2Lua()
{
#ifdef TRANSGAMING
    GScript.RegisterFunction("_ERRORMESSAGE", luaErrorMsg);
#endif
	GScript.RegisterFunction("_ALERT",luaMsgBox);
	GScript.RegisterFunction("Log",luaLOG);
	GScript.RegisterFunction("ToLightUD",luaToLightUD);
	GScript.RegisterFunction("ToNumber",luaToNumber);
#ifdef CD_CHECK_ON
	GScript.RegisterFunction("GetDriveLetter",luaCdCheck1);
#endif

	GScript.RegisterFunction("GetGCCount",luaGetGCCount);

    GScript.RegisterFunction("AddBitFlag",luaAddBitFlag);
    GScript.RegisterFunction("RemoveBitFlag",luaRemoveBitFlag);
    GScript.RegisterFunction("ReplaceBitFlag",luaReplaceBitFlag);
    GScript.RegisterFunction("IsBitFlag",luaIsBitFlag);

    GScript.RegisterFunction("IsDedicatedServer",luaIsDedicatedServer);
    GScript.RegisterFunction("IsFinalBuild",luaIsFinalBuild);
    GScript.RegisterFunction("IsConstPhysicsTick",luaIsConstPhysicsTick);
	GScript.RegisterFunction("IsMPDemo",luaIsMPDemo);
	GScript.RegisterFunction("IsNewNetcode", luaIsNewNetcode);
	GScript.RegisterFunction("IsPKInstalled", luaIsPKInstalled);
	GScript.RegisterFunction("IsBooHInstalled", luaIsBooHInstalled);
	GScript.RegisterFunction("IsBlackEdition", luaIsBlackEdition);
    GScript.RegisterFunction("IsPSP", luaIsPSP);
    GScript.RegisterFunction("IsPS2", luaIsPS2);
#ifdef TRANSGAMING
    GScript.RegisterFunction("IsDebug", luaIsDebug);
    GScript.RegisterFunction("IsSilent", luaIsSilent);
    GScript.RegisterFunction("IsLogging", luaIsLogging);
	GScript.RegisterFunction("PrintApiStack", luaPrintApiStack);
	GScript.RegisterFunction("Clone", luaClone);
	GScript.RegisterFunction("AngDist", luaAngDist);
    GScript.RegisterFunction("ShallowCopy", luaShallowCopy);
	GScript.RegisterFunction("TimeBody", luaTimeBody);
	GScript.RegisterFunction("AiBrainEvaluateGoals", luaAiBrainEvaluateGoals);
#endif


	GScript.RegisterFunction("DoFile",luaDoFile);
	GScript.RegisterFunction("DoString",luaDoString);
	GScript.RegisterFunction("MsgBox",luaMsgBox);
	GScript.RegisterFunction("Exit",luaExit);
	GScript.RegisterFunction("getregistry",luaGetRegistry);
#ifdef CD_CHECK_ON
	GScript.RegisterFunction("LabelOk",luaCdCheck3);
#endif
#ifndef TRANSGAMING
	GScript.RegisterFunction("GetCDLabel",luaCDGetLabel);
#endif
	GScript.RegisterFunction("EulerToQuat",luaEulerToQuat);
	GScript.RegisterFunction("NormalXToQuat",luaNormalXToQuat);
	GScript.RegisterFunction("NormalYToQuat",luaNormalYToQuat);
	GScript.RegisterFunction("NormalZToQuat",luaNormalZToQuat);
	GScript.RegisterFunction("QuatToEuler",luaQuatToEuler);
	GScript.RegisterFunction("RotateQuatByAxisAngle",luaRotateQuatByAxisAngle);
	GScript.RegisterFunction("VectorRotate", luaVectorRotate);
	GScript.RegisterFunction("VectorRotateByQuat", luaVectorRotateByQuat);
#ifdef CD_CHECK_ON
	GScript.RegisterFunction("GetCDLetter",luaCdCheck2);
#endif
    GScript.RegisterFunction("VectorInverseRotateByQuat", luaVectorInverseRotateByQuat);
	GScript.RegisterFunction("VectorAngle", luaVectorAngle);

	GScript.RegisterFunction("GetPlayerSpeed",luaEntity_GetPlayerSpeed);
	GScript.RegisterFunction("SetPlayerSpeed",luaEntity_SetPlayerSpeed);
    GScript.RegisterFunction("CreatePlayer",luaCreatePlayer);

    GScript.RegisterFunction("GetEngineVersionString",luaGetEngineVersionString);

#ifndef TRANSGAMING
	GScript.RegisterFunction("luaProfiler_LOGIC1", luaProfiler_LOGIC1);
	GScript.RegisterFunction("luaProfiler_LOGIC2a", luaProfiler_LOGIC2a);
	GScript.RegisterFunction("luaProfiler_LOGIC2b", luaProfiler_LOGIC2b);
	GScript.RegisterFunction("luaProfiler_LOGIC2c", luaProfiler_LOGIC2c);
	GScript.RegisterFunction("luaProfiler_LOGIC3a", luaProfiler_LOGIC3a);
	GScript.RegisterFunction("luaProfiler_LOGIC3b", luaProfiler_LOGIC3b);
	GScript.RegisterFunction("luaProfiler_LOGIC3c", luaProfiler_LOGIC3c);
	GScript.RegisterFunction("luaProfiler_LOGIC4a", luaProfiler_LOGIC4a);

	GScript.RegisterFunction("luaProfiler_LOGIC5", luaProfiler_LOGIC5);
#endif

#ifdef TRANSGAMING
    GScript.RegisterLibrary("DEBUG", debug_lib);
    GScript.RegisterLibrary("EDITOR", editor_lib);
    GScript.RegisterLibrary("MEMORY", memory_lib);
#endif
	GScript.RegisterLibrary("INP",input_lib);
	GScript.RegisterLibrary("MOUSE",mouse_lib);
	GScript.RegisterLibrary("MDL",model_lib);
	GScript.RegisterLibrary("MESH",mesh_lib);
	GScript.RegisterLibrary("ENTITY",entity_lib);
	GScript.RegisterLibrary("ENVIRONMENT",env_lib);
	GScript.RegisterLibrary("WORLD",world_lib);
	GScript.RegisterLibrary("R3D",render_lib);
	GScript.RegisterLibrary("LIGHT",light_lib);
	GScript.RegisterLibrary("BILLBOARD",billboard_lib);
	GScript.RegisterLibrary("FS",file_system_lib);
	GScript.RegisterLibrary("SOUND",sound_lib);
	GScript.RegisterLibrary("SOUND3D",sound3d_lib);
	GScript.RegisterLibrary("SOUND2D",sound2d_lib);
    GScript.RegisterLibrary("SND",snd_lib);
	GScript.RegisterLibrary("PLAYER",player_lib);
	GScript.RegisterLibrary("NET",net_lib);
	GScript.RegisterLibrary("HUD",hud_lib);
	GScript.RegisterLibrary("CAM",camera_lib);
	GScript.RegisterLibrary("WPT",waypoints_lib);
	GScript.RegisterLibrary("PATH",pathfinding_lib);
	GScript.RegisterLibrary("MATERIAL",material_lib);
	GScript.RegisterLibrary("REGION",region_lib);
	GScript.RegisterLibrary("PARTICLE",particle_lib);
	GScript.RegisterLibrary("PHYSICS",physics_lib);
    GScript.RegisterLibrary("FOGVOL",fogvol_lib);
    GScript.RegisterLibrary("GAMESPY",gamespy_lib);
	GScript.RegisterLibrary("LANG",languages_lib);
	GScript.RegisterLibrary("VARRAY",varray_lib);

	PainMenu_RegisterLUALib();
	MagicBoard_RegisterLUALib();
	MPStats_RegisterLUALib();
	Console_RegisterLUALib();
}
// =========================================================================
