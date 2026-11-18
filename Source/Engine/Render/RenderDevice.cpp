#include "Engine.h"

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

#include "Render/RenderDevice.h"

#ifdef TRANSGAMING_PS2
#include "VideoSystem_PS2.h"
#endif
#ifdef TRANSGAMING_PSP
#include "VideoSystem_PSP.h"
#endif

#ifndef TRANSGAMING
HMODULE RenderDeviceManager::hDeviceModule=NULL;
#endif

RenderDevice* RenderDeviceManager::CreateRenderDevice(int deviceType)
{
#if defined( TRANSGAMING_PS2 )
    m_setTag("Video");
	return new VideoSystemPS2;
#elif defined( TRANSGAMING_PSP )
    m_setTag("Video");
	return new VideoSystemPSP;
#else
    if(deviceType == ED3DRenderDevice) {
		
		char dir[MAX_PATH];
		GetModuleFileName(NULL,dir,MAX_PATH);
# ifdef TRANSGAMING
        char tmpBuf[MAX_PATH];

        String::GetPath(dir, tmpBuf, MAX_PATH);
		dir[::strlen(dir) - strlen(tmpBuf)]=0;
# else
        String fileName=String::BaseName(dir);
		dir[::strlen(dir)-fileName.Len()]=0;
# endif
		strcat(dir,"D3Dev.dll");

        // create d3d device
        hDeviceModule=LoadLibrary(dir);
        if(hDeviceModule) {
            CreateDeviceProc createProc=(CreateDeviceProc) GetProcAddress(hDeviceModule,"CreateDevice");
            if(createProc) {
                return (createProc)();
            }
            GLog.Print("FATAL ERROR: could not find CreateDevice entry\n");
        }
        else {
            GLog.Print("FATAL ERROR: could not load D3Dev.dll\n");
        }
    }
    else {
    }

    return 0;
#endif
}

void RenderDeviceManager::DestroyRenderDevice(RenderDevice* rdevPtr)
{
#ifndef TRANSGAMING
    if(hDeviceModule) {
        DestroyDeviceProc destroyProc=(DestroyDeviceProc) GetProcAddress(hDeviceModule,"DestroyDevice");
        if(destroyProc) {
            (destroyProc)(rdevPtr);
        }
        else {
            GLog.Print("WARNING: could not find DestroyDevice entry, leaks!\n");
        }

        FreeLibrary(hDeviceModule);
        hDeviceModule=NULL;
    }
#endif
}

FogState FogState::UseVSFog() const
{
	FogState state(*this);
	state.Mode = EFogNone;
	return state;
}

#ifndef TRANSGAMING
static const char* StrCompareFunc(const int func)
{
	switch (func)
	{
	case RenderState::EFuncNever:			return "never";
	case RenderState::EFuncLess:			return "less";
	case RenderState::EFuncEqual:			return "equal";
	case RenderState::EFuncLessEqual:		return "lessequal";
	case RenderState::EFuncGreater:			return "greater";
	case RenderState::EFuncNotEqual:		return "notequal";
	case RenderState::EFuncGreaterEqual:	return "greaterequal";
	case RenderState::EFuncAlways:			return "always";
	}

	return "ERR";
}

static const char* StrBlendMode(const int mode)
{
	switch (mode)
	{
	case RenderState::EBlendNone:				return "none";
	case RenderState::EBlendAlpha:				return "alpha";
	case RenderState::EBlendAdd:				return "add";
	case RenderState::EBlendModulate:			return "modulate";
	case RenderState::EBlendModulate2x:			return "modulate2x";
	case RenderState::EBlendFilter:				return "filter";
	case RenderState::EBlendTranslucent:		return "translucent";
	case RenderState::EBlendInvModulate:		return "invmodulate";
	case RenderState::EBlendSubtract:			return "subtract";
	case RenderState::EBlendRevSubtract:		return "revsubtract";
	case RenderState::EBlendDestTranslucent:	return "desttranslucent";
	case RenderState::EBlendDestAlpha:			return "destalpha";
	}
	return "ERR";
}

static const char* StrStencilOp(const int op)
{
	switch (op)
	{
	case RenderState::EStencilOpKeep:		return "keep";
	case RenderState::EStencilOpZero:		return "zero";
	case RenderState::EStencilOpReplace:	return "replace";
	case RenderState::EStencilOpIncrSat:	return "incrsat";
	case RenderState::EStencilOpDecrSat:	return "decrsat";
	case RenderState::EStencilOpInvert:		return "invert";
	case RenderState::EStencilOpIncr:		return "incr";
	case RenderState::EStencilOpDecr:		return "decr";
	}

	return "ERR";
}

static const char* StrFillMode(const int mode)
{
	switch (mode)
	{
	case RenderState::EFillSolid:	return "solid";
	case RenderState::EFillLines:	return "lines";
	case RenderState::EFillPoints:	return "points";
	}
	return "ERR";
}

static const char* StrCullMode(const int mode)
{
	switch (mode)
	{
	case RenderState::ECullNone:	return "none";
	case RenderState::ECullCCW:		return "ccw";
	case RenderState::ECullCW:		return "cw";
	}

	return "ERR";
}

void RenderState::DumpState(LogBuffer& MLog) const 
{
	RenderState emptyState;

	if (DepthFunc != emptyState.DepthFunc)
		MLog.Print("\t\tSetDepthFunc %s\n", StrCompareFunc(DepthFunc));

	if (DepthTest != emptyState.DepthTest)
		MLog.Print("\t\tSetDepthTest: %d\n",DepthTest);

	if (DepthWrite != emptyState.DepthWrite)
		MLog.Print("\t\tSetDepthWrite: %d\n",DepthWrite);

	if (StencilTest != emptyState.StencilTest)
		MLog.Print("\t\tSetStencilTest: %s\n",StrCompareFunc(StencilTest));

	if (StencilFailOp != emptyState.StencilFailOp)
		MLog.Print("\t\tSetStencilFailOp: %s\n",StrStencilOp(StencilFailOp));

	if (StencilZFailOp != emptyState.StencilZFailOp)
		MLog.Print("\t\tSetStencilZFailOp: %s\n",StrStencilOp(StencilZFailOp));

	if (StencilPassOp != emptyState.StencilPassOp)
		MLog.Print("\t\tSetStencilPassOp: %s\n",StrStencilOp(StencilPassOp));

	if (AlphaFunc != emptyState.AlphaFunc)
		MLog.Print("\t\tSetAlphaFunc: %s\n",StrCompareFunc(AlphaFunc));

	if (AlphaTest != emptyState.AlphaTest)
		MLog.Print("\t\tSetAlphaTest: %d\n",AlphaTest);

	if (AlphaRef != emptyState.AlphaRef)
		MLog.Print("\t\tSetAlphaRef: %d\n",AlphaRef);

	if (Lighting != emptyState.Lighting)
		MLog.Print("\t\tSetLighting: %d\n",Lighting);

	if (NormalizeNormals != emptyState.NormalizeNormals)
		MLog.Print("\t\tSetNormalizeNormals: %d\n",NormalizeNormals);

	if (SpecularEnable != emptyState.SpecularEnable)
		MLog.Print("\t\tSetSpecularEnable: %d\n",SpecularEnable);

	if (BlendMode != emptyState.BlendMode)
		MLog.Print("\t\tSetBlendMode: %s\n",StrBlendMode(BlendMode));

	if (FillMode != emptyState.FillMode)
		MLog.Print("\t\tSetFillMode: %s\n",StrFillMode(FillMode));

	if (CullMode != emptyState.CullMode)
		MLog.Print("\t\tSetCullMode: %s\n",StrCullMode(CullMode));

	if (ConstColor != emptyState.ConstColor)
		MLog.Print("\t\tSetConstColor: %08x\n",ConstColor);

	if (ColorWrite != emptyState.ColorWrite || AlphaWrite != emptyState.AlphaWrite)
		MLog.Print("\t\tSetColorWrite: RGB:%d A:%d\n",ColorWrite,AlphaWrite);

	if (AmbientColor != emptyState.AmbientColor)
		MLog.Print("\t\tSetAmbientColor: %08x\n",AmbientColor);
}
#endif
