// $Header: /Game/Source/Engine/Render/Material.cpp 262   04-11-02 17:09 Ulkai $

#include "Engine.h"

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

#include "FileSystem/GFileManager.h"
extern ENGINE_API GFileManager  	GFileMan;

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;

#include "Render/Video.h"
#include "Render/Material.h"
#include "Engine/World.h"

//static LogBuffer MLog;

Texture::~Texture()
{
    //TRACE("Texture::~Texture(): destroying the texture object '%s'\n", FileName.AsChar());
    ReleaseData();
}

MaterialScript::MaterialScript(){
#ifdef TRANSGAMING
    strBuf = NULL;
    strBufLen = 0;
#endif

	CurPos = 0;
	LastToken = 0;
	LastDelim = 0;
	CurDelim = 0;
	CurToken[0] = 0;
	CharNum = 0;
	InQuote = false;

	MaterialSys = NULL;
}

MaterialScript::MaterialScript(MaterialSystem* matSys) : MaterialSys(matSys){
#ifdef TRANSGAMING
    strBuf = NULL;
    strBufLen = 0;
#endif

	CurPos = 0;
	LastToken = 0;
	LastDelim = 0;
	CurDelim = 0;
	CurToken[0] = 0;
	CharNum = 0;
	InQuote = false;
}

MaterialScript::~MaterialScript(){
#ifdef TRANSGAMING
    if (strBuf)
        delete []strBuf;
#endif
}

BOOL MaterialScript::Load(const char* fileName)
{
    TRACE("MaterialScript::Load(): loading the material '%s'\n", fileName);
	GFile* f = GFileMan.CreateFileReader(fileName);
	if (f) 
	{
#ifdef TRANSGAMING
        if (strBuf)
            delete []strBuf;

        m_setTag("MaterialScript::Load");
        strBufLen = f->GetSize() + 1;
        strBuf = new char[strBufLen];

        f->Read(strBuf, strBufLen);
        strBuf[strBufLen - 1] = 0;
#else
        StrBuf.Resize(f->GetSize()+1);
		f->Read(&StrBuf[0],StrBuf.Size());
		StrBuf[StrBuf.Size()-1]=0;
#endif
        GFileMan.Close(f);

		CurDelim = 0;
		CharNum  = 0;
		CurToken[0] = 0;
		InQuote = false;
		CurPos = 0;
		LastToken = 0;
		LastDelim = 0;

		Parse();

#ifdef TRANSGAMING
        delete []strBuf;
        strBuf = NULL;
        strBufLen = 0;
#endif

		return TRUE;
	}

	return FALSE;
}

int MaterialScript::ParseTexArg()
{
	if (NextToken())
	{
		if (!strcmp("previous",CurToken))
			return TextureState::ETexArgPrevious;
		if (!strcmp("diffuse",CurToken))
			return TextureState::ETexArgDiffuse;
		if (!strcmp("color",CurToken))
			return TextureState::ETexArgColor;
		if (!strcmp("texture",CurToken))
			return TextureState::ETexArgTexture;
		if (!strcmp("previous_ar",CurToken))
			return TextureState::ETexArgPrevious_AR;
	}

	return EInvalidIndex;
}

int MaterialScript::ParseTexOp()
{
	if (NextToken())
    {
        if (!stricmp("modulate",CurToken))
            return TextureState::ETexOpModulate;
        if (!stricmp("modulate2x",CurToken))
            return TextureState::ETexOpModulate2x;
        if (!stricmp("add",CurToken))
            return TextureState::ETexOpAdd;
        if (!stricmp("addsigned",CurToken))
            return TextureState::ETexOpAddSigned;
        if (!stricmp("addsigned2x",CurToken))
            return TextureState::ETexOpAddSigned2x;
        if (!stricmp("blenddiffuse",CurToken))
            return TextureState::ETexOpBlendDiffuse;
        if (!stricmp("blendtexture",CurToken))
            return TextureState::ETexOpBlendTexture;
        if (!stricmp("blendcolor",CurToken))
            return TextureState::ETexOpBlendColor;
        if (!stricmp("blendprevious",CurToken))
            return TextureState::ETexOpBlendPrevious;
        if (!stricmp("modulatealphaadd",CurToken))
            return TextureState::ETexOpModulateAlphaAdd;
		if (!stricmp("previousmodulate",CurToken))
			return TextureState::ETexOpPreviousModulate;
	}

    return EInvalidIndex;
}

int MaterialScript::ParseTexGen(const char* token)
{
	if (!strcmp("position",token))
		return TextureState::ETexCoordCameraSpacePos;
	if (!strcmp("normal",token))
		return TextureState::ETexCoordCameraSpaceNormal;
	if (!strcmp("reflect",token))
		return TextureState::ETexCoordCameraSpaceReflect;
	
	if (!strcmp("worldspace",token))
		return TextureState::ETexCoordWorldSpace;
	if (!strcmp("worldorient",token))
		return TextureState::ETexCoordWorldOrient;

	return EInvalidIndex;
}

int MaterialScript::ParseTexAddr(const char* token)
{
	if (!strcmp("wrap",token))
		return TextureState::ETexAddrWrap;
	if (!strcmp("clamp",token))
		return TextureState::ETexAddrClamp;
	if (!strcmp("mirror",token))
		return TextureState::ETexAddrMirror;

	return EInvalidIndex;
}

int MaterialScript::ParseTexFilter(const char* token)
{
	if (!strcmp("point",token))
		return TextureState::ETexFilterPoint;
	if (!strcmp("bilinear",token))
		return TextureState::ETexFilterBilinear;
	if (!strcmp("bilinear_nomips",token))
		return TextureState::ETexFilterBilinearNoMips;

	return EInvalidIndex;
}

int MaterialScript::ParseCompareFunc()
{
	if (NextToken())
	{
		if (!stricmp("never",CurToken))
			return RenderState::EFuncNever;
		if (!stricmp("less",CurToken))
			return RenderState::EFuncLess;
		if (!stricmp("equal",CurToken))
			return RenderState::EFuncEqual;
		if (!stricmp("lessequal",CurToken))
			return RenderState::EFuncLessEqual;
		if (!stricmp("greater",CurToken))
			return RenderState::EFuncGreater;
		if (!stricmp("notequal",CurToken))
			return RenderState::EFuncNotEqual;
		if (!stricmp("greaterequal",CurToken))
			return RenderState::EFuncGreaterEqual;
		if (!stricmp("always",CurToken))
			return RenderState::EFuncAlways;
	}

	return EInvalidIndex;
}

int MaterialScript::ParseStencilOp()
{
	if (NextToken())
	{
		if (!stricmp("keep",CurToken))
			return RenderState::EStencilOpKeep;
		if (!stricmp("zero",CurToken))
			return RenderState::EStencilOpZero;
		if (!stricmp("replace",CurToken))
			return RenderState::EStencilOpReplace;
		if (!stricmp("incrsat",CurToken))
			return RenderState::EStencilOpIncrSat;
		if (!stricmp("decrsat",CurToken))
			return RenderState::EStencilOpDecrSat;
		if (!stricmp("invert",CurToken))
			return RenderState::EStencilOpInvert;
		if (!stricmp("incr",CurToken))
			return RenderState::EStencilOpIncr;
		if (!stricmp("decr",CurToken))
			return RenderState::EStencilOpDecr;
	}

	return EInvalidIndex;
}

int MaterialScript::ParseCullMode()
{
	if (NextToken())
	{
		if (!stricmp("none",CurToken))
			return RenderState::ECullNone;
		if (!stricmp("ccw",CurToken))
			return RenderState::ECullCCW;
		if (!stricmp("cw",CurToken))
			return RenderState::ECullCW;
	}

	return EInvalidIndex;
}

int MaterialScript::ParseFillMode()
{
	if (NextToken())
	{
		if (!stricmp("solid",CurToken))
			return RenderState::EFillSolid;
		if (!stricmp("lines",CurToken))			
			return RenderState::EFillLines;
		if (!stricmp("points",CurToken))
			return RenderState::EFillPoints;
	}

	return EInvalidIndex;
}


int MaterialScript::ParseBlendMode()
{
	if (NextToken())
	{
		if (!stricmp("none",CurToken))
			return RenderState::EBlendNone;
		if (!stricmp("alpha",CurToken))
			return RenderState::EBlendAlpha;
		if (!stricmp("add",CurToken))
			return RenderState::EBlendAdd;
		if (!stricmp("modulate",CurToken))
			return RenderState::EBlendModulate;
		if (!stricmp("modulate2x",CurToken))
			return RenderState::EBlendModulate2x;
		if (!stricmp("invmodulate",CurToken))
			return RenderState::EBlendInvModulate;
		if (!stricmp("filter",CurToken))
			return RenderState::EBlendFilter;
		if (!stricmp("translucent",CurToken))
			return RenderState::EBlendTranslucent;
		if (!stricmp("subtract",CurToken))
			return RenderState::EBlendSubtract;
		if (!stricmp("revsubtract",CurToken))
			return RenderState::EBlendRevSubtract;
		if (!stricmp("desttranslucent",CurToken))
			return RenderState::EBlendDestTranslucent;
		if (!stricmp("destalpha",CurToken))
			return RenderState::EBlendDestAlpha;
	}

	return EInvalidIndex;
}

static DWORD GetComponent(const char* s)
{
	if (!stricmp(s,"s2"))
		return RenderDevice::ERegShort2;
	if (!stricmp(s,"s4"))
		return RenderDevice::ERegShort4;
	if (!stricmp(s,"s2n"))
		return RenderDevice::ERegShort2N;
	if (!stricmp(s,"s4n"))
		return RenderDevice::ERegShort4N;
	if (!stricmp(s,"us2n"))
		return RenderDevice::ERegUShort2N;
	if (!stricmp(s,"us4n"))
		return RenderDevice::ERegUShort4N;

	if (!stricmp(s,"ub4"))
		return RenderDevice::ERegUByte4;
	if (!stricmp(s,"ub4n"))
		return RenderDevice::ERegUByte4N;

	if (!stricmp(s,"ud3"))
		return RenderDevice::ERegUDec3;
	if (!stricmp(s,"d3n"))
		return RenderDevice::ERegDec3N;

	return 0;
}

static DWORD ParseStreamDef(const char* s)
{
	if (!stricmp("ntu",s))
		return MeshVertex::ShaderStreamNTU;
	if (!stricmp("tu2",s))
		return MeshVertex::ShaderStreamTU2;

	DWORD d=0;
	int len = strlen(s);
	for (int i=0; i<len; ++i) 
	{
		switch (s[i]) 
		{
		case '1': d |= SetShaderComponent(i,RenderDevice::ERegFloat1); break;
		case '2': d |= SetShaderComponent(i,RenderDevice::ERegFloat2); break;
		case '3': d |= SetShaderComponent(i,RenderDevice::ERegFloat3); break;
		case '4': d |= SetShaderComponent(i,RenderDevice::ERegFloat4); break;
		case 'C': d |= SetShaderComponent(i,RenderDevice::ERegColor);  break;
		case 'c': d |= SetShaderComponent(i,RenderDevice::ERegColor);  break;

		default:
			if (i == 2)
			{
				DWORD Component = GetComponent(s+i);
				Assert(Component);

				d |= SetShaderComponent(i, Component);
			}
			else
			{
				Assert(0==0xbaadbeef);
			}

			i = len; // force break outta for loop
			break;
		}
	}

	return d;
}

static int FindSlot(RenderPass* pass, int stage)
{
	for (int i=0; i<RenderPass::MaxTextures; ++i)
	{
		if (pass->TexState[i].Stage == stage)
			return i;
	}

	if (pass->TexCount < RenderPass::MaxTextures)
	{		
		int slot = pass->TexCount++;
		pass->TexState[slot].Stage = stage;
        pass->TexState[slot].CoordIndex = (BYTE)(stage+1);
		return slot;
	}

	return -1;
}

BOOL MaterialScript::ParsePassToken(RenderPass* pass, const char* token)
{
	if (!stricmp("depthtest",token) && NextToken())
	{
		if (!stricmp("true",CurToken) || !strcmp("1",CurToken))
			pass->State.SetDepthTest(1);
		else if (!stricmp("false",CurToken) || !strcmp("0",CurToken))
			pass->State.SetDepthTest(0);
		else
            TRACE("MaterialScript::ParsePassToken(): WARNING-> weird value for depthtest: '%s', ignoring\n",CurToken);

		return TRUE;
	}

	if (!stricmp("depthwrite",token) && NextToken())
	{
		if (!stricmp("true",CurToken) || !strcmp("1",CurToken))
			pass->State.SetDepthWrite(1);
		else if (!stricmp("false",CurToken) || !strcmp("0",CurToken))
			pass->State.SetDepthWrite(0);
		else
			TRACE("MaterialScript::ParsePassToken(): WARNING-> weird value for depthwrite: '%s', ignoring\n",CurToken);

		return TRUE;
	}

	if (!stricmp("colorwrite",token) && NextToken())
	{
		if (!stricmp("true",CurToken) || !strcmp("1",CurToken))
			pass->State.SetColorWrite(1);
		else if (!stricmp("false",CurToken) || !strcmp("0",CurToken))
			pass->State.SetColorWrite(0);
		else
			TRACE("MaterialScript::ParsePassToken(): WARNING-> weird value for colorwrite: '%s', ignoring\n",CurToken);

		return TRUE;
	}

	if (!stricmp("alphawrite",token) && NextToken())
	{
		if (!stricmp("true",CurToken) || !strcmp("1",CurToken))
			pass->State.SetAlphaWrite(1);
		else if (!stricmp("false",CurToken) || !strcmp("0",CurToken))
			pass->State.SetAlphaWrite(0);
		else
			TRACE("MaterialScript::ParsePassToken(): WARNING-> weird value for alphawrite: '%s', ignoring\n",CurToken);

		return TRUE;
	}

	if (!stricmp("stenciltest",token) && NextToken())
	{
		if (!stricmp("true",CurToken) || !strcmp("1",CurToken))
			pass->State.SetStencilTest(1);
		else if (!stricmp("false",CurToken) || !strcmp("0",CurToken))
			pass->State.SetStencilTest(0);
		else
			TRACE("MaterialScript::ParsePassToken(): WARNING-> weird value for stenciltest: '%s', ignoring\n",CurToken);

		return TRUE;
	}

	if (!stricmp("alphatest",token) && NextToken())
	{
		if (!stricmp("true",CurToken) || !strcmp("1",CurToken))
			pass->State.SetAlphaTest(1);
		else if (!stricmp("false",CurToken) || !strcmp("0",CurToken))
			pass->State.SetAlphaTest(0);
		else
			TRACE("MaterialScript::ParsePassToken(): WARNING-> weird value for alphatest: '%s', ignoring\n",CurToken);

		return TRUE;
	}

	if (!stricmp("lighting",token) && NextToken())
	{
		if (!stricmp("true",CurToken) || !strcmp("1",CurToken))
			pass->State.SetLighting(1);
		else if (!stricmp("false",CurToken) || !strcmp("0",CurToken))
			pass->State.SetLighting(0);
		else
			TRACE("MaterialScript::ParsePassToken(): WARNING-> weird value for lighting: '%s', ignoring\n",CurToken);

		return TRUE;
	}

	if (!stricmp("specular",token) && NextToken())
	{
		if (!stricmp("true",CurToken) || !strcmp("1",CurToken))
			pass->State.SetSpecularEnable(1);
		else if (!stricmp("false",CurToken) || !strcmp("0",CurToken))
			pass->State.SetSpecularEnable(0);
		else
			TRACE("MaterialScript::ParsePassToken(): WARNING-> weird value for specular: '%s', ignoring\n",CurToken);

		return TRUE;
	}

	// normalize normals

	if (!stricmp("depthfunc",token))
	{
		int depthFunc = ParseCompareFunc();
		if (EInvalidIndex != depthFunc)
			pass->State.SetDepthFunc(depthFunc);
		else
			TRACE("MaterialScript::ParsePassToken(): WARNING-> weird value for depthfunc: '%s', ignoring\n",CurToken);

		return TRUE;
	}

	if (!stricmp("alphafunc",token))
	{
		int alphaFunc = ParseCompareFunc();
		if (EInvalidIndex != alphaFunc)
			pass->State.SetAlphaFunc(alphaFunc);
		else
			TRACE("MaterialScript::ParsePassToken(): WARNING-> weird value for alphaFunc: '%s', ignoring\n",CurToken);

		return TRUE;
	}

	if (!stricmp("alpharef",token) && NextToken())
	{
		pass->State.SetAlphaRef(atoi(CurToken));

		return TRUE;
	}

	if (!stricmp("stencilfunc",token))
	{
		int func = ParseCompareFunc();
		if (EInvalidIndex != func)
			pass->State.SetStencilFunc(func);
		else
			TRACE("MaterialScript::ParsePassToken(): WARNING-> weird value for stencilfunc: '%s', ignoring\n",CurToken);

		return TRUE;
	}

	if (!stricmp("stencilref",token) && NextToken())
	{
		pass->State.SetStencilRef(atoi(CurToken));

		return TRUE;
	}

	if (!stricmp("stencilmask",token) && NextToken())
	{
		pass->State.SetStencilMask(atoi(CurToken));

		return TRUE;
	}

	if (!stricmp("stencilfailop",token))
	{
		int op = ParseStencilOp();
		if (EInvalidIndex != op)
			pass->State.SetStencilFailOp(op);
		else
			TRACE("MaterialScript::ParsePassToken(): WARNING-> weird value for stencilfailop: '%s', ignoring\n",CurToken);

		return TRUE;
	}

	if (!stricmp("stencilzfailop",token))
	{
		int op = ParseStencilOp();
		if (EInvalidIndex != op)
			pass->State.SetStencilZFailOp(op);
		else
			TRACE("MaterialScript::ParsePassToken(): WARNING-> weird value for stencilzfailop: '%s', ignoring\n",CurToken);

		return TRUE;
	}

	if (!stricmp("stencilpassop",token))
	{
		int op = ParseStencilOp();
		if (EInvalidIndex != op)
			pass->State.SetStencilPassOp(op);
		else
			TRACE("MaterialScript::ParsePassToken(): WARNING-> weird value for stencilpassop: '%s', ignoring\n",CurToken);

		return TRUE;
	}

	if (!stricmp("blend",token))
	{
		int blendMode = ParseBlendMode();
		if (EInvalidIndex != blendMode)
			pass->State.SetBlendMode(blendMode);
		else
			TRACE("MaterialScript::ParsePassToken(): WARNING-> weird value for blend: '%s', ignoring\n",CurToken);

		return TRUE;
	}

	if (!stricmp("cull",token))
	{
		int cullMode = ParseCullMode();
		if (EInvalidIndex != cullMode)
			pass->State.SetCullMode(cullMode);
		else
			TRACE("MaterialScript::ParsePassToken(): WARNING-> weird value for cullmode: '%s', ignoring\n",CurToken);

		return TRUE;
	}

	if (!stricmp("fill",token))
	{
		int fillMode = ParseFillMode();
		if (EInvalidIndex != fillMode)
			pass->State.SetFillMode(fillMode);
		else
			TRACE("MaterialScript::ParsePassToken(): WARNING-> weird value for fillMode: '%s', ignoring\n",CurToken);

		return TRUE;
	}

	if (!stricmp("texop",token))
	{
		if (NextToken() && CurDelim == '[' && NextToken())
		{
			int stage = atoi(CurToken);
			int slot = FindSlot(pass,stage);			

			if (NextToken() && CurDelim == ']')
			{
                BYTE* destOp  = &pass->TexState[slot].Op;
                BYTE* destArg = &pass->TexState[slot].Arg;

				if (NextToken() && (!stricmp(".a",CurToken) || !stricmp(".alpha",CurToken)))
				{
                    destOp  = &pass->TexState[slot].AlphaOp;
                    destArg = &pass->TexState[slot].AlphaArg;
				}
				else
					Rewind();
                
                if (NextToken() && CurDelim == '=' && NextToken())
                {

    				if (!stricmp("disable",CurToken))
	    				*destOp = TextureState::ETexOpDisable;
					else
		    		if (!stricmp("texture",CurToken))
                    {
                        int op = ParseTexOp();
                        if (EInvalidIndex != op)
                        {
                            int arg = ParseTexArg();
                            if (EInvalidIndex != arg)
                            {
                                *destOp  = BYTE(op);
                                *destArg = BYTE(arg);
                            }
                        }   
                        else
						{
							Rewind();
					        *destOp = TextureState::ETexOpTexture;
						}
                    }
					else
				    if (!stricmp("diffuse",CurToken))
					    *destOp = TextureState::ETexOpDiffuse;
					else
    				if (!stricmp("color",CurToken))
	    				*destOp = TextureState::ETexOpColor;


					return TRUE;
                }
			}
		}
	}

	if (!strcmp("texgen", token))
	{
		if (NextToken() && CurDelim == '[' && NextToken())
		{
			int stage = atoi(CurToken);
			int slot = FindSlot(pass,stage);			

			if (NextToken() && CurDelim == ']' && NextToken() && CurDelim == '=')
			{
				while (NextToken())
				{
					int arg;
					
					arg = ParseTexGen(CurToken);
					if (EInvalidIndex != arg)
					{
						pass->TexState[slot].CoordIndex |= arg;
						pass->TexState[slot].Transform = TextureState::ETexTransform3;
						continue;
					}

					Rewind();
					return TRUE;
				}
			}
		}
	}	

	if (!strcmp("texenv",token))
	{
		if (NextToken() && CurDelim == '[' && NextToken())
		{
			int stage = atoi(CurToken);
			int slot = FindSlot(pass,stage);			

			if (NextToken() && CurDelim == ']' && NextToken() && CurDelim == '=')
			{
				while (NextToken())
				{
					int arg;
					
					arg = ParseTexAddr(CurToken);
					if (EInvalidIndex != arg)
					{
						pass->TexState[slot].AddrMode = (BYTE) arg;
						continue;
					}

					arg = ParseTexFilter(CurToken);
					if (EInvalidIndex != arg)
					{
						pass->TexState[slot].Filter = (BYTE) arg;
						continue;
					}

					break;
				}

				Rewind();
				return TRUE;
			}
		}
	}

	if (!strcmp("tile",token))
	{
		if (NextToken() && CurDelim == '[' && NextToken())
		{
			int stage = atoi(CurToken);
			int slot = FindSlot(pass,stage);			

			if (NextToken() && CurDelim == ']' && NextToken() && CurDelim == '=')
			{
				float tile[2] = { 1,1 };

				if (NextToken())
					tile[0] = (float) atof(CurToken);
				if (NextToken())
					tile[1] = (float) atof(CurToken);

				if (tile[0] != 1 || tile[1] != 1) 
				{
					pass->TexState[slot].TileU = tile[0];
					pass->TexState[slot].TileV = tile[1];
					pass->TexState[slot].Transform = TextureState::ETexTransform2;
					pass->TexState[slot].Xform = TextureState::EXformUseShader;
				}
			}
		}
	}

	if (!strcmp("rot",token))
	{
		if (NextToken() && CurDelim == '[' && NextToken())
		{
			int stage = atoi(CurToken);
			int slot = FindSlot(pass,stage);			

			if (NextToken() && CurDelim == ']' && NextToken() && CurDelim == '=')
			{
				float rot = 0;

				if (NextToken())
					rot = (float) atof(CurToken);

				if (rot != 0) 
				{
					pass->TexState[slot].Rot = rot;
					pass->TexState[slot].Transform = TextureState::ETexTransform2;
					pass->TexState[slot].Xform = TextureState::EXformUseShader;
				}
			}
		}
	}

	if (!strcmp("mat",token))
	{
		if (NextToken() && CurDelim == '[' && NextToken())
		{
			int stage = atoi(CurToken);
			int slot = FindSlot(pass,stage);			

			if (NextToken() && CurDelim == ']' && NextToken() && CurDelim == '=')
			{

				float mat[8] = {1,0,0,0,0,1,0,0};

				for (int i=0; i<8; i++)
				{
					if (NextToken())
						mat[i] = (float) atof(CurToken);
				}

				pass->TexState[slot].Mat.SetIdentity(); // = GIdentityMatrix;
				pass->TexState[slot].Mat.X = Plane(mat[0],mat[1],mat[2],mat[3]);
				pass->TexState[slot].Mat.Y = Plane(mat[4],mat[5],mat[6],mat[7]);
				pass->TexState[slot].Transform = TextureState::ETexTransform2;
				pass->TexState[slot].Xform = TextureState::EXformUseShader;
				pass->TexState[slot].Flags |= TextureState::EXformUseMat;
			}
		}
	}

	if (!strcmp("pan",token))
	{
		if (NextToken() && CurDelim == '[' && NextToken())
		{
			int stage = atoi(CurToken);
			int slot = FindSlot(pass,stage);			

			if (NextToken() && CurDelim == ']' && NextToken() && CurDelim == '=')
			{
				float pan[2] = { 0,0 };

				if (NextToken())
					pan[0] = (float) atof(CurToken);
				if (NextToken())
					pan[1] = (float) atof(CurToken);

				if (pan[0] != 0 || pan[1] != 0) 
				{
					pass->TexState[slot].PanU = pan[0];
					pass->TexState[slot].PanV = pan[1];
					pass->TexState[slot].Transform = TextureState::ETexTransform2;
					pass->TexState[slot].Xform = TextureState::EXformUseShader;
				}
			}
		}
	}

	if (!strcmp("xform",token))
	{
		if (NextToken() && CurDelim == '[' && NextToken())
		{
			int stage = atoi(CurToken);
			int slot = FindSlot(pass,stage);			

			if (NextToken() && CurDelim == ']' && NextToken() && CurDelim == '=' && NextToken())
			{
				if (CurToken[0] == '$')
				{
					if (!strcmp("alphaxform",CurToken+1))
					{
						pass->TexState[slot].Xform = TextureState::EXformInAlpha;
						pass->TexState[slot].Transform = TextureState::ETexTransform2;
					}
					else if (!strcmp("blendxform",CurToken+1))
					{
						pass->TexState[slot].Xform = TextureState::EXformInBlend;
						pass->TexState[slot].Transform = TextureState::ETexTransform2;
					}
					else if (!strcmp("detailxform",CurToken+1))
					{
						pass->TexState[slot].Xform = TextureState::EXformInDetail;
						pass->TexState[slot].Transform = TextureState::ETexTransform2;
					}
					else if (!strcmp("identity",CurToken+1))
						pass->TexState[slot].Xform = TextureState::EXformIdentity;
					else if (!strcmp("proj",CurToken+1))
						pass->TexState[slot].Transform	= TextureState::ETexTransformProj;
					else if (!strcmp("none",CurToken+1)) {
						pass->TexState[slot].Transform = TextureState::ETexTransformNone;
						pass->TexState[slot].Xform = TextureState::EXformNone;
					}
				}
				else
				{
					TRACE("MaterialScript::ParsePassToken(): ERROR-> xform type \"%s\" is invalid\n", CurToken);
					return FALSE;
				}
			}
		}
	}

	if (!stricmp("coordIndex",token))
	{
		if (NextToken() && CurDelim == '[' && NextToken())
		{
			int stage = atoi(CurToken);
			int slot = FindSlot(pass,stage);			

			if (NextToken() && CurDelim == ']' && NextToken() && CurDelim == '=' && NextToken())
			{
				int coordIndex = slot;

				if (NextToken())
				{
					coordIndex = atoi(CurToken);
					pass->TexState[slot].CoordIndex = (coordIndex&0x7) | (pass->TexState[slot].CoordIndex&~0x7);
				}
			}
		}
	}

	if (!strcmp("color",token))
	{
		if (NextToken() && CurDelim == '=' && NextToken())
		{
			if (CurDelim == '(')
			{
				float c[4] = {1,1,1,1};
				for (int i=0; i<4; ++i)
				{
					if (NextToken())
					{
						c[i] = (float) atof(CurToken);
						if (NextToken())
							if ((i==3 && CurDelim == ')') || (CurDelim == ','))
								continue;
					}

					return FALSE;
				}

#ifdef	TRANSGAMING
				pass->State.SetConstColor(Color_TG(c).dwVal);
#else
				pass->State.SetConstColor(Color(c).dwVal);
#endif
				return TRUE;
			}
			else
			{
				TRACE("MaterialScript::ParsePassToken(): ERROR-> syntax was weird while parsing color ...\n");
				return FALSE;
			}
		}
	}

	if (!strcmp("map",token))
	{
		if (NextToken() && CurDelim == '[' && NextToken())
		{
			int stage = atoi(CurToken);
			int slot = FindSlot(pass,stage);			

			if (NextToken() && CurDelim == ']' && NextToken() && CurDelim == '=' && NextToken())
			{
				if (CurToken[0] == '$')
				{
					if (!strcmp("colormap",CurToken+1))
						pass->TexMaps[slot].Type = RenderPass::EMatInColorMap;
					else if (!strcmp("lightmap",CurToken+1))
						pass->TexMaps[slot].Type = RenderPass::EMatInLightMap;
					else if (!strcmp("normalmap",CurToken+1))
						pass->TexMaps[slot].Type = RenderPass::EMatInNormalMap;
					else if (!strcmp("detailmap",CurToken+1))
						pass->TexMaps[slot].Type = RenderPass::EMatInDetailMap;
					else if (!strcmp("cubemap",CurToken+1))
						pass->TexMaps[slot].Type = RenderPass::EMatInCubeMap;
					else if (!strcmp("blendmap",CurToken+1))
						pass->TexMaps[slot].Type = RenderPass::EMatInBlendMap;
					else if (!strcmp("alphamap",CurToken+1))
						pass->TexMaps[slot].Type = RenderPass::EMatInAlphaMap;
					else if (!strcmp("emissivemap",CurToken+1))
						pass->TexMaps[slot].Type = RenderPass::EMatInEmissiveMap;
					else if (!strcmp("glossmap",CurToken+1))
						pass->TexMaps[slot].Type = RenderPass::EMatInGlossMap;
					else if (!strcmp("envcubemap",CurToken+1))
						pass->TexMaps[slot].Type = RenderPass::EMatInEnvCubeMap;
					else if (!strcmp("warpmap",CurToken+1))
						pass->TexMaps[slot].Type = RenderPass::EMatInWarpMap;
#ifndef TRANSGAMING
                    else if (!strcmp("normcubemap",CurToken+1)) 
					{
						pass->TexMaps[slot].Type = RenderPass::EMatUseTexture;
						pass->TexMaps[slot].TexPtr = MaterialSys->Textures.NormCube;
					}
#endif
                    else if (!strcmp("attenmap",CurToken+1)) {
						pass->TexMaps[slot].Type = RenderPass::EMatUseTexture;
						pass->TexMaps[slot].TexPtr = MaterialSys->Textures.AttenMap;
					} else if (!strcmp("fbtex",CurToken+1)) {
						pass->TexMaps[slot].Type = RenderPass::EMatInFBTex0;
						pass->TexMaps[slot].TexPtr = 0;
					} else if (!strcmp("fbtex1",CurToken+1)) {
						pass->TexMaps[slot].Type = RenderPass::EMatInFBTex1;
						pass->TexMaps[slot].TexPtr = 0;
					} else if (!strcmp("fbtex2",CurToken+1)) {
						pass->TexMaps[slot].Type = RenderPass::EMatInFBTex2;
						pass->TexMaps[slot].TexPtr = 0;
					} else if (!strcmp("notex",CurToken+1)) {
						pass->TexMaps[slot].Type   = RenderPass::EMatUseTexture;
						pass->TexMaps[slot].TexPtr = MaterialSys->Textures.NoTex;
					}
				}
				else
				{
					pass->TexMaps[slot].Type = RenderPass::EMatUseTexture;
					if (strstr(CurToken,"cube"))
						pass->TexMaps[slot].TexPtr = MaterialSys->CreateCubeMap(CurToken);
					else if (strstr(CurToken,"dudv"))
						pass->TexMaps[slot].TexPtr = MaterialSys->CreateTexture(CurToken,Texture::ETexDuDv);
					else
						pass->TexMaps[slot].TexPtr = MaterialSys->CreateTexture(CurToken);
				}

				return TRUE;
			}
		}
	}

	if (!stricmp("vshader",token))
	{
        int slot = 0;


        //check if the vertex shader has an index
        if (NextToken() && CurDelim == '[' && NextToken())
        {
            slot = atoi(CurToken);

            //advance the token beyond the index block
            NextToken();
            Assert(CurDelim == ']');
        }

        //no index => rewind to the previous token and parse it as slot 0
        else{
            slot = 0;

            Rewind();
        }

        //make sure the slot is in the proper range
        Assert(slot >= 0 && slot <= 7);

                
        if (NextToken() && CurDelim == '=' && NextToken())
		{
			if (!stricmp("NTU",CurToken)) {
				pass->VertShader[slot] = MaterialSys->Shaders.VS_NTU;
			}
			else if (!stricmp("TU2",CurToken)) {
				pass->VertShader[slot] = MaterialSys->Shaders.VS_TU2;
			}
			else if (!stricmp("fixedPos",CurToken)) {
				pass->VertShader[slot] = MaterialSys->Shaders.FixedPos;
				return TRUE;
			}
			else if (!stricmp("fixedSimple",CurToken)) {
				pass->VertShader[slot] = MaterialSys->Shaders.FixedSimple;
				return TRUE;
			}
			else if (!stricmp("fixedTU2",CurToken)) {
#ifdef TRANSGAMING_PS2
                pass->VertShader[slot] = MaterialSys->Shaders.Fixed_CTU2;
#else
				pass->VertShader[slot] = MaterialSys->Shaders.Fixed_TU2;
#endif
				return TRUE;
			}
#ifdef TRANSGAMING_PS2
            else if (!stricmp("fixedCTU2", CurToken)){
                pass->VertShader[slot] = MaterialSys->Shaders.Fixed_CTU2;
                return TRUE;
            }
            else if (!stricmp("fixedDCTU1", CurToken)){
                pass->VertShader[slot] = MaterialSys->Shaders.Fixed_DCTU1;
                return TRUE;
            }
            else if (!stricmp("fixed4DCTU1", CurToken)){
                pass->VertShader[slot] = MaterialSys->Shaders.Fixed_4DCTU1;
                return TRUE;
            }
            else if (!stricmp("fixedDTU1", CurToken)){
                pass->VertShader[slot] = MaterialSys->Shaders.Fixed_DTU1;
                return TRUE;
            }
            else if (!stricmp("fixed4DTU1", CurToken)){
                pass->VertShader[slot] = MaterialSys->Shaders.Fixed_4DTU1;
                return TRUE;
            }
#endif
            else if (!stricmp("fixedMT0_TU2", CurToken)){
                pass->VertShader[slot] = MaterialSys->Shaders.FixedMT0_TU2;
                return TRUE;
            }
            else if (!stricmp("fixedMT1_TU2", CurToken)){
                pass->VertShader[slot] = MaterialSys->Shaders.FixedMT1_TU2;
                return TRUE;
            }
			else if (!stricmp("fixedNTU",CurToken)) {
				pass->VertShader[slot] = MaterialSys->Shaders.Fixed_NTU;
				return TRUE;
			}
			else {
				StreamDef def(MeshVertex::ShaderStreamNTU);
				//String shaderName(CurToken);
                char shaderName[MAX_PATH];


                snprintf(shaderName, MAX_PATH, "%s.vso", CurToken);

				if (NextToken() && !stricmp("def",CurToken)) {
					if (!NextToken())
						return FALSE;

					def.Streams[0] = ParseStreamDef(CurToken);
					if (NextToken() && CurDelim == ',') {
						if (!NextToken())
							return FALSE;

						def.Streams[1] = ParseStreamDef(CurToken);
						++def.StreamCount;
					} else {
						Rewind();					
					}
				}
				else {
					Rewind();
				}

                //TRACE("MaterialScript::ParsePassToken(): loading shader '%s.vso'\n", shaderName);
				pass->VertShader[slot] = MaterialSys->LoadShader
				(
					shaderName,         //(shaderName+".vso").AsChar(),
					RenderDevice::EShaderVertex,
					&def
				);

			}

			return TRUE;
		}

        else{
            TRACE("MaterialScript::ParsePassToken(): ERROR-> invalid token! {token = '%s', delimiter = '%c'}\n", CurToken, CurDelim);
            Assert(!"Bad token!");
        }
	}

	if (!stricmp("fshader",token))
	{
		if (NextToken() && CurDelim == '=' && NextToken())
		{
			if (!stricmp("0",CurToken) || !stricmp("null",CurToken)) {
				pass->FragShader = EInvalidIndex;
			} else {
                char shaderName[MAX_PATH];


                snprintf(shaderName, MAX_PATH, "%s.pso", CurToken);

                //TRACE("MaterialScript::ParsePassToken(): loading shader '%s'\n", shaderName);
				pass->FragShader = MaterialSys->LoadShader
				(
					shaderName,         //(String(CurToken)+".pso").AsChar(),
					RenderDevice::EShaderPixel,
					0
				);
			}
		}
	}

	if (!stricmp("fx",token))
	{
		if (NextToken() && CurDelim == '=' && NextToken())
		{
			StreamDef def(MeshVertex::ShaderStreamNTU);
			//String fxName(CurToken);
            char fxName[MAX_PATH];


            strncpy(fxName, CurToken, MAX_PATH);


			if (NextToken() && !stricmp("def",CurToken)) {
				if (!NextToken())
					return FALSE;

				def.Streams[0] = ParseStreamDef(CurToken);
				if (NextToken() && CurDelim == ',') {
					if (!NextToken())
						return FALSE;

					def.Streams[1] = ParseStreamDef(CurToken);
					++def.StreamCount;
				} else {
					Rewind();					
				}
			}
			else {
				Rewind();
			}

			FXEntry* Fe = MaterialSys->FindEffect(fxName);   //fxName.AsChar());
			if (Fe)
			{
				for (int i=0; i<Fe->Techniques.Size(); ++i)
				{
					RenderTechnique* T = &Fe->Techniques[i];
					if (!stricmp(T->Name.AsChar(), fxName))  //fxName.AsChar()))
					{
					    RenderDevice* dev = GEngine->Video->RenderDev;

						const EffectPass& P = T->Passes[0];
						pass->VertShader[0] = P.VertShader ? dev->CreateShader(RenderDevice::EShaderVertex,P.VertShader,def.Streams,def.StreamCount)
														: EInvalidIndex;

                        //make sure to clear out all other vertex shaders
                        for (int j = 1; j < RenderPass::MaxShaderCount; j++)
                            pass->VertShader[j] = EInvalidIndex;

						pass->FragShader = P.FragShader ? dev->CreateShader(RenderDevice::EShaderPixel,P.FragShader,0,0)
														: EInvalidIndex;

						break;
					}
				}
			}
			else
			{
				TRACE("MaterialScript::ParsePassToken(): WARNING-> fx \"%s\" WAS NOT FOUND\n", fxName);   //fxName.AsChar());
			}
		}
	}

	if (!stricmp("target",token))
	{
		if (NextToken() && CurDelim == '=' && NextToken())
		{
			if (!stricmp("$fb",CurToken))
				pass->Flags |= RenderPass::EPass_RTargetFB;
			else if (!stricmp("$fbtex1",CurToken))
				pass->Flags |= RenderPass::EPass_RTargetTexFB1;
			else if (!stricmp("$fbtex2",CurToken))
				pass->Flags |= RenderPass::EPass_RTargetTexFB2;
			else 
			{
				TRACE("MaterialScript::ParsePassToken(): ERROR-> invalid target \"%s\"\n", CurToken);
			}
		}
		else
		{
			TRACE("MaterialScript::ParsePassToken(): ERROR-> expecting token after \"target\"\n");
		}
	}

	return TRUE;
}

#ifndef TRANSGAMING
static const char* StrTexOp(const int op)
{
	switch (op)
	{
	case TextureState::ETexOpDisable:			return "disable";
	case TextureState::ETexOpTexture:			return "texture";
	case TextureState::ETexOpDiffuse:			return "diffuse";
	case TextureState::ETexOpColor:				return "color";
	case TextureState::ETexOpModulate:			return "modulate";
	case TextureState::ETexOpModulate2x:		return "modulate2x";
	case TextureState::ETexOpAdd:				return "add";
	case TextureState::ETexOpAddSigned:			return "addsigned";
	case TextureState::ETexOpAddSigned2x:		return "addsigned2x";
	case TextureState::ETexOpBlendDiffuse:		return "blenddiffuse";
	case TextureState::ETexOpBlendTexture:		return "blendtexture";
	case TextureState::ETexOpBlendColor:		return "blendcolor";
	case TextureState::ETexOpBlendPrevious: 	return "blendprevious";
	case TextureState::ETexOpModulateAlphaAdd:	return "modulatealphaadd";
	}

	return "*";
}

static const char* StrTexArg(const int arg)
{
	switch (arg)
	{
	case TextureState::ETexArgPrevious:	return "previous";
	case TextureState::ETexArgDiffuse:	return "diffuse";
	case TextureState::ETexArgColor:	return "color";
	case TextureState::ETexArgTexture:	return "texture";
	}

	return "*";
}

static const char* StrAddrModeArg(const int arg)
{
	switch (arg)
	{
	case TextureState::ETexAddrWrap:	return "wrap";
	case TextureState::ETexAddrClamp:	return "clamp";
	case TextureState::ETexAddrMirror:	return "mirror";
	}

	return "";
}

static const char* StrFilterArg(const int arg)
{
	switch (arg)
	{
	case TextureState::ETexFilterPoint:				return "point";
	case TextureState::ETexFilterBilinear:			return "bilinear";
	case TextureState::ETexFilterBilinearNoMips:	return "bilinearnomips";
	}

	return "";
}

void DumpTexStates(const RenderPass& pass)
{
	for (int i=0; i<pass.TexCount; ++i)
	{
		const TextureState& t=pass.TexState[i];
		MLog.Print("\t\t[%d] stage(%d): c(%s %s) a(%s %s) | %s %s\n",
					i,pass.TexState[i].Stage,
					StrTexOp(t.Op),StrTexArg(t.Arg),
					StrTexOp(t.AlphaOp),StrTexArg(t.AlphaArg),
					StrAddrModeArg(t.AddrMode),StrFilterArg(t.Filter));
	}
}

static void Dump(const Material* mat, const char *matName)
{
#if 0
	MLog.Print("=====================================================\n");
	MLog.Print("Material %s\n",matName);
	for (int i=0; i<mat->PassCount; ++i)
	{
		const RenderPass& pass=mat->Passes[i];
		MLog.Print("Pass[%d]: TexCount: %d\n",i,pass.TexCount);
		MLog.Print("\tState:\n");
		pass.State.DumpState(MLog);

		if (pass.TexCount)
		{
			MLog.Print("\tTexState:\n");
			DumpTexStates(pass);
		}
	}
#endif
}
#endif

// ==============================================================================================

BOOL MaterialScript::ParseMaterial()
{
	int depth = 0;
    m_setTag("TextureMaterial");
	Material* mat = new Material();
	int passCount = 0;
	RenderPass* curPass=0;
	//String matName;
    char matName[MAX_PATH];
	bool isCopy=false;
	int hwclass = RenderDevice::EHWClassUnsupported;
	int matCopyFound = true;

	// parse hw technique
	if (NextToken() && CurDelim=='<' && NextToken())
	{
		if (!stricmp(CurToken,"tnl")) 
		{
			hwclass = RenderDevice::EHWClassGenericTnL;
		} 
		else if (!stricmp(CurToken,"nv20")) 
		{
			hwclass = RenderDevice::EHWClassNV20;
		}
		else if (!stricmp(CurToken,"nv25")) 
		{
			hwclass = RenderDevice::EHWClassNV25;
		}
		else if (!stricmp(CurToken,"r200")) 
		{
			hwclass = RenderDevice::EHWClassR200;
		}
		else if (!stricmp(CurToken,"nv30")) 
		{
			hwclass = RenderDevice::EHWClassNV30;
		}
		else if (!stricmp(CurToken,"nv40")) 
		{
			hwclass = RenderDevice::EHWClassNV40;
		}


		if (NextToken() && CurDelim == '>')
		{
		}
	}
	else 
	{
		Rewind();
	}

	// parse material
	while (NextToken())
	{
		if (CurDelim == '{')
		{
			++depth;
		}
		else
		if (CurDelim == '}')
		{
			--depth;
			if (0 == depth)
			{
				if (!isCopy)
					mat->PassCount = passCount;

				if (!MaterialSys->FindMaterial(matName) &&         //matName.AsChar()) && 
					hwclass <= GEngine->Video->RenderDev->HWClass &&
					matCopyFound) 
				{
					// make sure that the last TexStateOp is always set to texopdisable
					for (int iPass=0; iPass<mat->PassCount; iPass++)
					{
						RenderPass *pass = &mat->Passes[iPass];

						if (pass->TexCount == 0 || pass->TexCount == RenderPass::MaxTextures)
							continue;

						TextureState &texstate = pass->TexState[pass->TexCount-1];

						if (texstate.Op != TextureState::ETexOpDisable)
						{
							int slot = FindSlot(pass, texstate.Stage+1);

							pass->TexState[slot].Op	= TextureState::ETexOpDisable;
						}

						if (texstate.AlphaOp != TextureState::ETexOpDisable)
						{
							int slot = FindSlot(pass, texstate.Stage+1);

							pass->TexState[slot].AlphaOp = TextureState::ETexOpDisable;
						}
					}

					// make sure that all texture transforms but projected are disabled for programmable pipeline
					// projected is needed for ps 1.1-1.3
					for (int iPass=0; iPass<mat->PassCount; iPass++)
					{
						RenderPass *pass = &mat->Passes[iPass];
                        for (int i = 0; i < RenderPass::MaxShaderCount; i++){
						    if (pass->VertShader[i] != MaterialSys->Shaders.FixedPos &&
							    pass->VertShader[i] != MaterialSys->Shaders.FixedSimple &&
							    pass->VertShader[i] != MaterialSys->Shaders.Fixed_TU2 &&
#ifdef TRANSGAMING_PS2
                                pass->VertShader[i] != MaterialSys->Shaders.Fixed_CTU2 &&
                                pass->VertShader[i] != MaterialSys->Shaders.Fixed_DCTU1 &&
                                pass->VertShader[i] != MaterialSys->Shaders.Fixed_DTU1 &&
                                pass->VertShader[i] != MaterialSys->Shaders.Fixed_4DTU1 &&
                                pass->VertShader[i] != MaterialSys->Shaders.Fixed_4DCTU1 &&
#endif
                                pass->VertShader[i] != MaterialSys->Shaders.FixedMT0_TU2 &&
                                pass->VertShader[i] != MaterialSys->Shaders.FixedMT1_TU2 &&
							    pass->VertShader[i] != MaterialSys->Shaders.Fixed_NTU)
						    {
							    for (int iStage=0; iStage<pass->TexCount; iStage++)
								    pass->TexState[iStage].Transform &= ~0xf;
						    }
                        }
					}


#ifndef TRANSGAMING
					Dump(mat, matName);    //matName.AsChar());
#endif

					MaterialSys->MatHash[matName] = mat;
				}
				else 
				{
					delete mat;
				}

				return TRUE;
			}
		}
		else
		{
			if (0 == depth)
			{
				//matName = CurToken;
                strncpy(matName, CurToken, MAX_PATH);

				if (NextToken() && !stricmp("copy",CurToken) && NextToken())
				{
					Material** ptr = MaterialSys->MatHash.Find(CurToken);
					if(ptr)
					{
						(*mat) = (**ptr);
						isCopy = true;
					}
					else
					{
						if (hwclass <= GEngine->Video->RenderDev->HWClass)
                            TRACE("MaterialScript::ParseMaterial(): ERROR-> while parsing shader \"%s\": copy '%s' -> not found\n",
								matName, CurToken);    //matName.AsChar(), CurToken);

						matCopyFound = false;
					}
				}
				else
				{
					Rewind();
					mat->PassCount = 0;
				}
			}
			else
			if (1 == depth && matName[0])   //matName.Size())
			{
				if (!stricmp("pass",CurToken))
				{
					const int index = passCount;
                    Assert(passCount < Material::MaxPasses);
					curPass=&mat->Passes[passCount++];

					if (NextToken() && !stricmp("copy",CurToken) && NextToken())
					{
						if (!stricmp("previous",CurToken))
						{
							if (index>0)
								(*curPass) = mat->Passes[index-1];
							else
                                TRACE("MaterialScript::ParseMaterial(): WARNING-> cannot copy previous pass while at pass 0!\n");
						}
						else
						{
							// TODO:
						}
					}
					else // no copy, so init to some defaults
					{
						Rewind();
						if (!isCopy) 
						{
							//memset(curPass,0,sizeof(RenderPass));
                            for (int i=0; i<RenderPass::MaxTextures; ++i)
							{
								curPass->TexState[i].Stage		= EInvalidIndex;                                                           
								curPass->TexState[i].Op			= TextureState::ETexOpDisable;
								curPass->TexState[i].AlphaOp	= TextureState::ETexOpDisable;
							}

							curPass->VertShader[0] = MaterialSys->Shaders.VS_NTU;
                            for (int i = 1; i < RenderPass::MaxShaderCount; i++)
                                curPass->VertShader[i] = EInvalidIndex;

							curPass->FragShader = EInvalidIndex;
						}
					}
				}
				else if (!stricmp("setflag",CurToken))
				{
					if (NextToken())
					{
						if (!stricmp("trans",CurToken))
							mat->Flags |= EMF_SortTrans;
						else if (!stricmp("warp",CurToken))
							//mat->Flags |= (EMF_SortTrans|EMF_WarpScreen);
							mat->Flags |= EMF_WarpScreen;
						else if (!stricmp("reflect",CurToken))
							mat->Flags |= EMF_Reflect;
						else if (!stricmp("refract",CurToken))
							mat->Flags |= EMF_Refract;
						else if (!stricmp("water",CurToken))
							mat->Flags |= EMF_Water;
					}
				}
				else
				{
					TRACE("MaterialScript::ParseMaterial(): ERROR-> while parsing shader \"%s\": uknown token: \"%s\"\n",
						matName, CurToken);    //matName.AsChar(), CurToken);
				}
			}
			else
			if (2 == depth)
			{
				if (curPass)
				{
					if (!ParsePassToken(curPass,CurToken))
					{
						TRACE("MaterialScript::ParseMaterial(): ERROR-> while parsing shader \"%s\"\n", matName);    //matName.AsChar());
						return FALSE;
					}
				}
				else
				{
					TRACE("MaterialScript::ParseMaterial(): ERROR-> while parsing shader \"%s\": uknown token: \"%s\"\n",
						matName, CurToken);    //matName.AsChar(), CurToken);
				}
			}
		}
	}

	if (mat)
		delete mat;

	return FALSE;
}

void MaterialScript::Parse()
{
	while (NextToken())
	{
		if (!stricmp(CurToken,"shader"))
		{
			if (!ParseMaterial())
				break;
		}
	}
}

BOOL MaterialScript::EndToken()
{
	if (CharNum)
	{
		CurToken[CharNum]=0;
		CharNum=0;
		strlwr(CurToken);
		return TRUE;
	}

	return FALSE;
}

void MaterialScript::Rewind()
{
	CurDelim	= LastDelim;
	CurPos		= LastToken;
}

BOOL MaterialScript::NextToken()
{
	LastDelim = CurDelim;
	LastToken = CurPos;

	CurDelim = 0;
	bool InComment = false;

#ifdef TRANSGAMING
	while (CurPos<strBufLen)
	{
		const char c = strBuf[CurPos++];

		if (c == '/' && CurPos < strBufLen && strBuf[CurPos] == '/')
#else
	while (CurPos<StrBuf.Size())
	{
		const char c = StrBuf[CurPos++];

		if (c == '/' && CurPos<StrBuf.Size() && StrBuf[CurPos]=='/')
#endif
		{
			if (EndToken())
				--CurPos;
			return TRUE;
			CurPos++;
			InComment = true;
			continue;
		}

		if (InComment)
		{
			if (c=='\n' || c=='\r')
				InComment = false;
			continue;
		}
	
		if (c == '"')
		{
			if (!InQuote)
			{
				InQuote = true;
				continue;
			}

			InQuote = false;
			return EndToken();
		}
		
		if (!InQuote)
		{
			switch (c)
			{
			case '=':
			case '(':
			case ')':
			case '[':
			case ']':
			case '{':
			case '}':
			case '<':
			case '>':
			case ',':
				CurDelim = c;
				if (EndToken())
					--CurPos;
				return TRUE;

			case '\t':
			case ' ':
			case '\r':
			case '\n':
				if (EndToken())
					return TRUE;

				continue;
			default:
				break;
			}			
		}

		CurToken[CharNum++] = c;

	}

	return EndToken();
}
// ============================================================

Material::Material() 
:	Flags(0), RefCount(0), PassCount(0)
{
}

// ============================================================

MaterialInput::MaterialInput(int input, Texture** maps)
{
	if (input&EInputColor)		ColorMap	= *maps++;
	if (input&EInputLight)		LightMap	= *maps++;
#ifndef TRANSGAMING
	if (input&EInputNormal)		NormalMap	= *maps++;
    if (input&EInputDetail)		DetailMap	= *maps++;
#endif
	if (input&EInputCube)		CubeMap		= *maps++;
	if (input&EInputBlend)		BlendMap	= *maps++;
	if (input&EInputAlpha)		AlphaMap	= *maps++;
	if (input&EInputEmissive)	EmissiveMap	= *maps++;
	if (input&EInputGloss)		GlossMap	= *maps++;
	if (input&EInputEnvCubeMap)	EnvCubeMap	= *maps++;
	if (input&EInputWarpMap)	WarpMap		= *maps++;
}

MaterialInput::MaterialInput(int input, Texture** maps, MaterialDef** mats)
{
	if (input&EInputColor)		ColorMap	= *maps++;
	if (input&EInputLight)		LightMap	= *maps++;
#ifndef TRANSGAMING
    if (input&EInputNormal)		NormalMap	= *maps++;
	if (input&EInputDetail)		DetailMap	= *maps++;
#endif
	if (input&EInputCube)		CubeMap		= *maps++;
	if (input&EInputBlend)		BlendMap	= *maps++;
	if (input&EInputAlpha)		AlphaMap	= *maps++;
	if (input&EInputEmissive)	EmissiveMap	= *maps++;
	if (input&EInputGloss)		GlossMap	= *maps++;
	if (input&EInputEnvCubeMap)	EnvCubeMap	= *maps++;
	if (input&EInputWarpMap)	WarpMap		= *maps++;

	if (input&EDefBlend)		BlendDef	= *mats++;
	if (input&EDefAlpha)		AlphaDef	= *mats++;
#ifndef TRANSGAMING
	if (input&EDefDetail)		DetailDef	= *mats++;
#endif
}

// ============================================================

void RenderPass::Activate()
{
	RenderDevice* dev = GEngine->Video->RenderDev;

	if (Flags)
	{		
		if (Flags & EPass_RTargetFB)
			dev->SetFBRenderTarget();
#ifdef	TRANSGAMING
		else if (Flags & EPass_RTargetTexFB1)
			dev->SetRenderTarget(GEngine->pcfWorld->TexFB[1], NULL);
		else if (Flags & EPass_RTargetTexFB2)
			dev->SetRenderTarget(GEngine->pcfWorld->TexFB[2], NULL);
#else
		else if (Flags & EPass_RTargetTexFB1)
			dev->SetRenderTarget(GEngine->World->TexFB[1], NULL);
		else if (Flags & EPass_RTargetTexFB2)
			dev->SetRenderTarget(GEngine->World->TexFB[2], NULL);
#endif
	}


    //make sure the base stage shader is explicitly set
	dev->SetShader(VertShader[0], RenderDevice::EShaderVertex0);

    //set all remaining shader stages until a NULL shader is encountered
    //NOTE that a missing or invalid shader name will effectively disable
    //  all further stage shaders (they will use the shader for stage 0).
    for (int i = 1; i < MaxShaderCount && VertShader[i] != EInvalidIndex; i++)
        dev->SetShader(VertShader[i], i + RenderDevice::EShaderVertexBase);


    dev->SetShader(FragShader,RenderDevice::EShaderPixel);
    dev->SetState(State);
}

void RenderPass::SetInput(MaterialInput* matIn)
{
	RenderDevice* dev = GEngine->Video->RenderDev;
    MaterialSystem* matSys = GEngine->Video->MaterialSys;

	for (int i=0; i<TexCount; ++i)
    {
		switch (TexMaps[i].Type)
        {
        case EMatUseTexture 	: TexState[i].TexId = TexMaps[i].TexPtr->DeviceId; break;
		case EMatInColorMap 	: TexState[i].TexId = matIn->ColorMap		? matIn->ColorMap->DeviceId	 		: 0; break;
		case EMatInLightMap 	: TexState[i].TexId = matIn->LightMap		? matIn->LightMap->DeviceId  		: 0; break;
#ifndef TRANSGAMING
		case EMatInNormalMap	: TexState[i].TexId = matIn->NormalMap		? matIn->NormalMap->DeviceId 		: 0; break;
		case EMatInDetailMap	: TexState[i].TexId = matIn->DetailMap		? matIn->DetailMap->DeviceId 		: 0; break;
#endif
		case EMatInCubeMap  	: TexState[i].TexId = matIn->CubeMap		? matIn->CubeMap->DeviceId   		: 0; break;
		case EMatInBlendMap 	: TexState[i].TexId = matIn->BlendMap		? matIn->BlendMap->DeviceId  		: 0; break;
		case EMatInAlphaMap 	: TexState[i].TexId = matIn->AlphaMap		? matIn->AlphaMap->DeviceId			: 0; break;
		case EMatInEmissiveMap	: TexState[i].TexId = matIn->EmissiveMap	? matIn->EmissiveMap->DeviceId		: 0; break;
		case EMatInGlossMap		: TexState[i].TexId = matIn->GlossMap		? matIn->GlossMap->DeviceId			: 0; break;
		case EMatInEnvCubeMap	: TexState[i].TexId = matIn->EnvCubeMap		? matIn->EnvCubeMap->DeviceId		: 0; break;
		case EMatInWarpMap		: TexState[i].TexId = matIn->WarpMap		? matIn->WarpMap->DeviceId			: 0; break;
#ifdef	TRANSGAMING
		case EMatInFBTex0		: TexState[i].TexId = GEngine->pcfWorld->TexFB[0] ? GEngine->pcfWorld->TexFB[0]->DeviceId : 0; break;
		case EMatInFBTex1		: TexState[i].TexId = GEngine->pcfWorld->TexFB[1] ? GEngine->pcfWorld->TexFB[1]->DeviceId : 0; break;
		case EMatInFBTex2		: TexState[i].TexId = GEngine->pcfWorld->TexFB[2] ? GEngine->pcfWorld->TexFB[2]->DeviceId : 0; break;
#else
		case EMatInFBTex0		: TexState[i].TexId = GEngine->World->TexFB[0] ? GEngine->World->TexFB[0]->DeviceId : 0; break;
		case EMatInFBTex1		: TexState[i].TexId = GEngine->World->TexFB[1] ? GEngine->World->TexFB[1]->DeviceId : 0; break;
		case EMatInFBTex2		: TexState[i].TexId = GEngine->World->TexFB[2] ? GEngine->World->TexFB[2]->DeviceId : 0; break;
#endif
        default					: TexState[i].TexId = 0; break;                       
        }

		switch (TexState[i].Xform)
		{
		case TextureState::EXformNone: 
			break;
		case TextureState::EXformIdentity: 
			TexState[i].MatStore.SetIdentity();     // = IdentityMatrix(); 
			break;
		case TextureState::EXformUseShader: 
			// unchanged - read from shader script
			break;
		case TextureState::EXformInBlend: 
			TexState[i].TileU = matIn->BlendDef->Tile.X;
			TexState[i].TileV = matIn->BlendDef->Tile.Y;
			TexState[i].PanU  = matIn->BlendDef->Pan.X;
			TexState[i].PanV  = matIn->BlendDef->Pan.Y;
			TexState[i].Rot   = matIn->BlendDef->Rot;
			TexState[i].TickMatStore(matSys->Time);
			break;
		case TextureState::EXformInAlpha:
			TexState[i].TileU = matIn->AlphaDef->Tile.X;
			TexState[i].TileV = matIn->AlphaDef->Tile.Y;
			TexState[i].PanU  = matIn->AlphaDef->Pan.X;
			TexState[i].PanV  = matIn->AlphaDef->Pan.Y;
			TexState[i].Rot   = matIn->AlphaDef->Rot;
			TexState[i].TickMatStore(matSys->Time);
			break;
#ifndef TRANSGAMING
		case TextureState::EXformInDetail:
			TexState[i].TileU = matIn->DetailDef->Tile.X;
			TexState[i].TileV = matIn->DetailDef->Tile.Y;
			TexState[i].PanU  = matIn->DetailDef->Pan.X;
			TexState[i].PanV  = matIn->DetailDef->Pan.Y;
			TexState[i].Rot   = matIn->DetailDef->Rot;
			TexState[i].TickMatStore(matSys->Time);
			break;
#endif
		}

		if (TexState[i].Xform!=TextureState::EXformNone)
		{
			dev->SetShaderMemory(24+i*3,&TexState[i].MatStore.X,1);
			dev->SetShaderMemory(25+i*3,&TexState[i].MatStore.Y,1);
			dev->SetShaderMemory(26+i*3,&TexState[i].MatStore.W,1);
		}
	}

   	dev->SetTextureState(TexState,TexCount);
}

// ============================================================

void Texture::ReleaseData()
{
    FreeData();
    Flags &= ~ETexDataValid;
	Size = 0;
}

void Texture::FreeData(){
    if (DataFile){
		free(DataFile);

        DataFile = NULL;
    }
}

BOOL Texture::Load(const char* fileName)
{
	BOOL success=FALSE;
    GFile* file = GFileMan.CreateFileReader(fileName);
    if(file) 
    {
		ReleaseData();
		Size=file->GetSize();
		if(Size>0)
		{
            m_setTag("Texture");
#ifdef TRANSGAMING
            TRACE("Texture::Load(): loading texture from '%s' (size = %d)\n", fileName, Size);
#endif
			DataFile = (BYTE *)malloc(Size);
			file->Read(DataFile,Size);

            Flags |= ETexDataValid;
		}

        success=TRUE;
		GFileMan.Close(file);
    }

    return success;
}

#ifndef TRANSGAMING
void Texture::GenerateAttenDst2(int size)
{
}

void Texture::GenerateAttenExp(int size)
{
#if 0
	Release();

	Width = Height = size;
	DataFile = (BYTE*) GMem.Alloc(size*size);
    Flags |= Texture::ERawP8 | Texture::ETexDataValid;
#endif

    m_setTag("Texture");
	float** e = (float**) malloc(size*size*sizeof(float));
	Assert(e);

	float range = 2.5f;
	float fsz = (float) size;

	for (int j=0; j<size; ++j)
	{
		float fj = -1.f + (2.f*j+1.f)/fsz;
		float efj = fj * range;
		efj = (float) exp(-(efj*efj));

		for (int i=0; i<size; ++i)
		{
			float fi = -1.f + (2.f*i+1.f)/fsz;
			float efi = fi * range;
			efi = (float) exp(-(efi*efi));
			e[i][j] = efi*efj;
		}
	}

	free(e);
}
#endif

// =================================================================
#ifndef TRANSGAMING
// Tool functions for computing specular exponent approxiamation function params.
// This is based on gamasutra article.

// Table containing multiply instruction modifier for each instruction (1, 2 or 4)
int Multiplier[] = { 4, 4, 4, 4, 4, 4, 4, 4 };

// Compute value of k at instruction i given a table of multiply instruction modifiers
double ComputeK( int i )
{
   if( i == 0 )
      return Multiplier[i];

   double Temp = ComputeK( i-1 );
   return Multiplier[i] * Temp * Temp;
}

// Compute maximum absolute values for A and B given some m and a multiplier table
// LogM: log of m in base 2 (number of instructions - 1)
double MaxAB( int LogM )
{
   double m = pow( 2.0, LogM );           // Find the value of m
   double K = ComputeK( LogM );         // Compute K
   return pow( K, 1.0/m );
}

// Compute values of A' and B' given A and B and a multiplier table
// LogM: log of m in base 2 (number of instructions - 1)
void ComputeApBp( int LogM, double A, double B, double &APrime, double &BPrime )
{
   double Temp = 1.0/MaxAB( LogM );     // Note that k -1/m = 1/MaxAB

   APrime = A * Temp;
   BPrime = B * Temp;
}
#endif

// =================================================================

MaterialSystem::MaterialSystem() : Time(0), FreezeRefs(0)
{
	/*
	double ap,bp;
	FILE *f = fopen("out.psh","wt");
	double A[] = { 1.336, 1.683, 2.033, 2.385, 3.802, 6.645};
	double B[] = {-0.336,-0.683,-1.033,-1.385,-2.802,-5.645};
	int pow[] = {3,4,5,6,10,18};
	for (int i=0; i<6; i++)
	{
		ComputeApBp(1,A[i],B[i],ap,bp);
		fprintf(f,";def c2,%f,%f,%f,%f // ^%d\n",ap,ap,ap,ap,pow[i]);
		fprintf(f,";def c3,%f,%f,%f,%f\n",bp,bp,bp,bp);
	}

	fclose(f);
	*/

#ifndef TRANSGAMING
	const VertexComponent PackedNTU[] = 
	{
		VertexComponent(0, 0,	RenderDevice::ERegFloat3,	RenderDevice::EUsagePosition),
		VertexComponent(0, 12,	RenderDevice::ERegDec3N,	RenderDevice::EUsageNormal),
		VertexComponent(0, 16,	RenderDevice::ERegDec3N,	RenderDevice::EUsageTangent),
		VertexComponent(0, 24,	RenderDevice::ERegFloat2,	RenderDevice::EUsageTexCoord0),
		VertexComponent()
	};

	const VertexComponent PackedTU2[] = 
	{
		VertexComponent(0, 0,	RenderDevice::ERegFloat3,	RenderDevice::EUsagePosition),
		VertexComponent(0, 12,	RenderDevice::ERegDec3N,	RenderDevice::EUsageNormal),
		VertexComponent(0, 16,	RenderDevice::ERegDec3N,	RenderDevice::EUsageTangent),
		VertexComponent(0, 20,	RenderDevice::ERegFloat2,	RenderDevice::EUsageTexCoord0),
		VertexComponent(0, 28,	RenderDevice::ERegShort2N,	RenderDevice::EUsageTexCoord1),
		VertexComponent()
	};

	memcpy(Decls.PackedNTU, PackedNTU, sizeof(VertexComponent)*ARRAY_COUNT(PackedNTU));
	memcpy(Decls.PackedTU2, PackedTU2, sizeof(VertexComponent)*ARRAY_COUNT(PackedTU2));
#endif


    Shaders.FixedPos =          EInvalidIndex;
    Shaders.FixedSimple =       EInvalidIndex;
    Shaders.Fixed_TU2 =         EInvalidIndex;
    Shaders.Fixed_NTU =         EInvalidIndex;
#ifdef TRANSGAMING_PS2
    Shaders.Fixed_DCTU1 =       EInvalidIndex;
    Shaders.Fixed_DTU1 =        EInvalidIndex;
    Shaders.Fixed_CTU2 =        EInvalidIndex;
    Shaders.Fixed_4DTU1 =       EInvalidIndex;
    Shaders.Fixed_4DCTU1 =      EInvalidIndex;
    //Shaders.Fixed_4NTU =        EInvalidIndex;
#endif
    Shaders.FixedMT0_TU2 =      EInvalidIndex;
    Shaders.FixedMT1_TU2 =      EInvalidIndex;

    Shaders.VS_NTU =            EInvalidIndex;
    Shaders.VS_TU2 =            EInvalidIndex;


    for (int j = 0; j < Entity::MaxInfluences; j++){
        for (int i = 0; i < Entity::MaxLights + 1; i++)
            Shaders.SKIN[j][i] = EInvalidIndex;
    }

    Textures.NoTex =            NULL;
    Textures.White =            NULL;
    Textures.Gray =             NULL;
    Textures.Black =            NULL;
    Textures.AttenMap =         NULL;
    Textures.IllumMap =         NULL;
#ifndef TRANSGAMING
    Textures.NormCube =         NULL;
#endif
    Textures.Flashlight =       NULL;
    Textures.FlashFalloff =     NULL;
    Textures.ShadowFalloff =    NULL;
    

    Material*   NullMat =       NULL;
}

MaterialSystem::~MaterialSystem()
{
    for(int i=0; i<MatHash.Size(); ++i)
        delete MatHash.GetPair(i).Value;

	ReleaseEffects();
    for(int i=0; i<TexHash.Size(); ++i)
	{
        Texture *texture = TexHash.GetPair(i).Value;

        if (texture == NULL)
            continue;

		ReleaseTexture(texture, true);
        delete texture;
	}
}

void MaterialSystem::Tick(float timeDelta)
{
	Time += timeDelta;

	for (int mat=0; mat<MatHash.Size(); ++mat)
	{
		Material *material = MatHash.GetPair(mat).Value; 
		for (int pass=0; pass<material->PassCount; ++pass)
		{
			for (int tex=0; tex<RenderPass::MaxTextures; ++tex)
			{
				material->Passes[pass].TexState[tex].TickMatStore(Time);
			}
		}
	}

	for (int tex=0; tex<AniTex.Size(); ++tex)
	{
		Texture *t = AniTex[tex];

		switch (t->AnimType)
		{
		case Texture::ETexAnimAdvance:
			t->DeviceId = t->FrameIds[int(Time*t->Fps) % t->NumFrames];
			break;
		}
	}
}

#ifndef TRANSGAMING
int MaterialSystem::SetTextureLOD()
{
	GLog.Print("Setting textures quality...\n");
	RenderDevice* dev = GEngine->Video->RenderDev;

    for(int i=0; i<TexHash.Size(); ++i)
	{
		Texture* texture = TexHash.GetPair(i).Value;

        if (texture == NULL)
            continue;

		int rawBytes = texture->Flags & (Texture::ETexRaw32|Texture::ETexRaw8|Texture::ETexRaw32Alpha);
		if (!texture->isDataValid() || rawBytes || (texture->Flags & Texture::ETexAnim))
			continue;

        if (texture->NumFrames) {
            for (int i=0; i<texture->NumFrames; ++i)
                dev->ReleaseResource(RenderDevice::EResTexture,&texture->FrameIds[i],1);
        } else {
            dev->ReleaseResource(RenderDevice::EResTexture,&texture->DeviceId,1);
        }

	}

    for(int i=0; i<TexHash.Size(); ++i)
	{
		Texture* texture = TexHash.GetPair(i).Value;

        if (texture == NULL)
            continue;

        int rawBytes = texture->Flags & (Texture::ETexRaw32|Texture::ETexRaw8|Texture::ETexRaw32Alpha);
		if (!texture->isDataValid() || rawBytes || (texture->Flags & Texture::ETexAnim))
			continue;

		dev->CreateTexture(texture);
	}

	return 1;
}
#endif

int MaterialSystem::ReloadTextures()
{
	RenderDevice* dev = GEngine->Video->RenderDev;
	GLog.Print("Reloading textures...\n");

    for(int i=0; i<TexHash.Size(); ++i)
	{
		Texture* texture = TexHash.GetPair(i).Value;

        if (texture == NULL)
            continue;

        int rawBytes = texture->Flags & (Texture::ETexRaw32|Texture::ETexRaw8|Texture::ETexRaw32Alpha);
		if (!texture->isDataValid() || rawBytes)
			continue;

        if (!texture->NumFrames) {
            dev->ReleaseResource(RenderDevice::EResTexture,&texture->DeviceId,1);
			texture->ReleaseData();
			texture->Load(texture->FileName.AsChar());
            dev->CreateTexture(texture);
        }
	}

	return 1;
}

void MaterialSystem::SetTexFiltering()
{
#ifndef TRANSGAMING
	ScriptObject cfg = GScript.Globals()["Cfg"];
	String filter = cfg["TextureFiltering"].GetString();

	bool ani = !strcmp(filter.AsChar(), "Anisotropic");
	bool tri = !strcmp(filter.AsChar(), "Trilinear");
#endif

	for( int i=0; i<MatHash.Size(); i++ )
	{
		Material *mat = MatHash.GetPair(i).Value;
		for( int p=0; p<mat->PassCount; p++ )
		{
			RenderPass *pass = &mat->Passes[p];
			for( int s=0; s<pass->TexCount; s++ )
			{
				TextureState *state = &pass->TexState[s];
				if( state->Filter != TextureState::ETexFilterBilinearNoMips )
				{
#ifndef TRANSGAMING
					if (ani)
					{
						state->Filter = TextureState::ETexFilterAnisotropic;
					}
					else if (tri)
					{
						state->Filter = TextureState::ETexFilterTrilinear;
					}
					else
#endif
					{
						state->Filter = TextureState::ETexFilterBilinear;
					}
				}
			}
		}
	}
}

void MaterialSystem::CreateWarpTexture()
{
    m_setTag("Texture");
	Texture* T = new Texture(Texture::ETexRegular);

	const int Size = 16;
	T->Size		= Size * Size * 4;
    m_setTag("Texture");
	T->DataFile		= (BYTE*) malloc(T->Size);
	T->Width	= Size;
	T->Height	= Size;

    T->Flags	= /*Texture::ETexInvalid|*/Texture::ETexRaw32|/*Texture::ETexNoMipMaps|*/Texture::ETexSystem | Texture::ETexDataValid;

    //create a colour 32-bit noise texture
	DWORD* P = (DWORD*) T->DataFile;
	for (int y=0; y<Size; ++y, P += Size)
	{
		for (int x=0; x<Size; ++x)
		{
			const FLOAT FX = (rand()/(FLOAT)RAND_MAX);
			const FLOAT FY = (rand()/(FLOAT)RAND_MAX);

			const DWORD DX = DWORD(FX * 255.0F);
			const DWORD DY = DWORD(FY * 255.0F);

			// argb (G=X, B=Y)
			//const DWORD C = ((DX << 8) & 0xFF00) | (DY & 0xFF);
			const DWORD C = (DX<<16) | (DY<<8) | 0xFF;

			P[x] = C;
		}
	}

	RenderDevice* Device = GEngine->Video->RenderDev;
	Device->CreateTexture(T);

	Assert(T->DeviceId != EInvalidIndex);
	TexHash["warp"] = T;
}

BOOL MaterialSystem::Init()
{
	RenderDevice* dev = GEngine->Video->RenderDev;

#ifndef TRANSGAMING
	MLog.Open("shader.log");
#endif
    
#ifndef TRANSGAMING
	// Set texture quality (MUST match enum ETexLODx enum in render device!!!)
	char *names[] = {"TextureQualityArchitecture","TextureQualityWeapons","TextureQualityCharacters","TextureQualitySkies"};
	ScriptObject cfg = GScript.Globals()["Cfg"];

	for (int i=0; i<RenderDevice::ETexLODNumEntries; ++i)
	{
		int texQ = 0;
		if( !cfg[names[i]].IsNil() )
		{
			texQ = cfg[names[i]].GetInt();
		}
		dev->TexLOD[i] = texQ;
	}
    
 	SetTextureLOD();
#endif

	DWORD type;

	
	// for rendering lines 
	type = RenderDevice::EVertexPos;
	Shaders.FixedPos = dev->CreateShader(RenderDevice::EShaderVertex,0,&type,1);

	// for rendering HUD
	type = RenderDevice::ESimpleVertex;
	Shaders.FixedSimple = dev->CreateShader(RenderDevice::EShaderVertex,0,&type,1);

#ifdef TRANSGAMING_PS2
//	type = RenderDevice::ERenderVertexNC1;
    type = RenderDevice::ERenderVertexNC; // this is messy
    Shaders.Fixed_TU2 =  dev->CreateShader(RenderDevice::EShaderVertex,0,&type,1);


    //type = RenderDevice::ERenderVertexN1;
    //Shaders.Fixed_NTU =  dev->CreateShader(RenderDevice::EShaderVertex,0,&type,1);

    {   //fully deinterleaved RN1 vertex format
        DWORD type2[] = {RenderDevice::EVertexPos4, RenderDevice::EVertexNormal, RenderDevice::EVertexTU1};
        Shaders.Fixed_NTU =  dev->CreateShader(RenderDevice::EShaderVertex, 0, type2, ARRAY_COUNT(type2));
    }

    {   //partially deinterleaved vertex format for the world meshes
        DWORD type2[] = {RenderDevice::ERenderVertexC2, RenderDevice::EVertexNormal};
        Shaders.Fixed_CTU2 = dev->CreateShader(RenderDevice::EShaderVertex, 0, type2, ARRAY_COUNT(type2));
    }

    {   //deinterleaved vertex format for the particle systems.
        DWORD type2[] = { RenderDevice::EVertexPos, RenderDevice::EVertexDiffuse, RenderDevice::EVertexTU1 };
        Shaders.Fixed_DCTU1 = dev->CreateShader(RenderDevice::EShaderVertex, 0, type2, ARRAY_COUNT(type2));
    }

    {   //deinterleaved vertex format for the trail systems.
        DWORD type2[] = { RenderDevice::EVertexPos, RenderDevice::EVertexTU1 };
        Shaders.Fixed_DTU1 = dev->CreateShader(RenderDevice::EShaderVertex, 0, type2, ARRAY_COUNT(type2));
    }

    {   //deinterleaved vertex format for the hud systems.
        DWORD type2[] = { RenderDevice::EVertexPos4, RenderDevice::EVertexTU1 };
        Shaders.Fixed_4DTU1 = dev->CreateShader(RenderDevice::EShaderVertex, 0, type2, ARRAY_COUNT(type2));
    }

    {   //deinterleaved vertex format for the particle systems.  
        DWORD type2[] = { RenderDevice::EVertexPos4, RenderDevice::EVertexDiffuse, RenderDevice::EVertexTU1 };
        Shaders.Fixed_4DCTU1 = dev->CreateShader(RenderDevice::EShaderVertex, 0, type2, ARRAY_COUNT(type2));
    }

    {   //deinterleaved MT vertex format for the lightmapped geometry
#ifdef TRANSGAMING_KEEP_NORMALS
        DWORD type2[] = {RenderDevice::EVertexPos4, RenderDevice::EVertexNormal, RenderDevice::EVertexTU1, RenderDevice::EVertexTU1};
#else
        DWORD type2[] = {RenderDevice::EVertexPos4, RenderDevice::EVertexTU1, RenderDevice::EVertexTU1};
#endif
        Shaders.FixedMT0_TU2 = dev->CreateShader(RenderDevice::EShaderVertex, 0, type2, ARRAY_COUNT(type2));
        Shaders.FixedMT1_TU2 = Shaders.FixedMT0_TU2;
    }

    /*{   //deinterleaved vertex format for the animated meshes.
        DWORD type2[] = { RenderDevice::EVertexPos4, RenderDevice::EVertexNormal, RenderDevice::EVertexTU1 };
        Shaders.Fixed_4NTU = dev->CreateShader(RenderDevice::EShaderVertex, 0, type2, ARRAY_COUNT(type2));
    }//*/

    //these should both be null
    Shaders.VS_NTU = EInvalidIndex;
    Shaders.VS_TU2 = EInvalidIndex;
#elif defined TRANSGAMING_PSP
//	type = RenderDevice::ERenderVertexNC1;
    type = RenderDevice::ERenderVertexNC;
    Shaders.Fixed_TU2 =  dev->CreateShader(RenderDevice::EShaderVertex,0,&type,1);

	type = RenderDevice::ERenderVertexN1;
    Shaders.Fixed_NTU =  dev->CreateShader(RenderDevice::EShaderVertex,0,&type,1);

# ifdef TRANSGAMING_KEEP_NORMALS
    //used for multitextured lit geometry stage 0
    type =  RenderDevice::EVertexBoneFloat | 
            RenderDevice::EVertexBone1 |
            RenderDevice::EVertexTUFloat |
            RenderDevice::EVertexTU1 | 
            RenderDevice::EVertexDiffuse | 
            RenderDevice::EVertexNormal |
            RenderDevice::EVertexPos |
            RenderDevice::EVertexForceClip | 
            RenderDevice::EVertexMultitexture;
    Shaders.FixedMT0_TU2 =  dev->CreateShader(RenderDevice::EShaderVertex,0,&type,1);

    //used for multitextured lit geometry stage 1
    type =  RenderDevice::EVertexBoneFloat |
            RenderDevice::EVertexBone3 |
            RenderDevice::EVertexTUUShort |
            RenderDevice::EVertexTU1 |
            RenderDevice::EVertexNormal |
            RenderDevice::EVertexPos |
            RenderDevice::EVertexForceClip | 
            RenderDevice::EVertexMultitexture;
    Shaders.FixedMT1_TU2 =  dev->CreateShader(RenderDevice::EShaderVertex,0,&type,1);
# else
    //used for multitextured unlit geometry stage 0
    type =  RenderDevice::EVertexBoneFloat | 
            RenderDevice::EVertexBone1 |
            RenderDevice::EVertexTUFloat |
            RenderDevice::EVertexTU1 | 
            RenderDevice::EVertexDiffuse | 
            RenderDevice::EVertexPos |
            RenderDevice::EVertexForceClip | 
            RenderDevice::EVertexMultitexture;
    Shaders.FixedMT0_TU2 =  dev->CreateShader(RenderDevice::EShaderVertex,0,&type,1);

    //used for multitextured unlit geometry stage 1
    type =  RenderDevice::EVertexBoneFloat |
            RenderDevice::EVertexBone3 |
            RenderDevice::EVertexTUUShort |
            RenderDevice::EVertexTU1 |
            RenderDevice::EVertexPos |
            RenderDevice::EVertexForceClip | 
            RenderDevice::EVertexMultitexture;
    Shaders.FixedMT1_TU2 =  dev->CreateShader(RenderDevice::EShaderVertex,0,&type,1);
# endif

    Shaders.VS_NTU = EInvalidIndex;
    Shaders.VS_TU2 = EInvalidIndex;
#else
    type = RenderDevice::EMeshVertex;
    Shaders.Fixed_TU2 =  dev->CreateShader(RenderDevice::EShaderVertex,0,&type,1);

    type = RenderDevice::EMeshVertexNTU;
    Shaders.Fixed_NTU =  dev->CreateShader(RenderDevice::EShaderVertex,0,&type,1);

    Shaders.VS_NTU = LoadShader("ntu",RenderDevice::EShaderVertex,&StreamDef(MeshVertex::ShaderStreamNTU));
    Shaders.VS_TU2 = LoadShader("tu2",RenderDevice::EShaderVertex,&StreamDef(MeshVertex::ShaderStreamTU2));
#endif


    char name[MAX_PATH];
	
	for (int i=0; i<Entity::MaxInfluences; i++)
	{
		for (int j=0; j<=Entity::MaxLights; j++)
		{
            snprintf(name, MAX_PATH, "palskin.bones%d.lights%d.vso", i+1, j);

#ifdef TRANSGAMING
            StreamDef s1(MeshVertex::ShaderStreamNTU, VertexWeights::ShaderStream);


			Shaders.SKIN[i][j] = LoadShader(name, RenderDevice::EShaderVertex, &s1);

			if (Shaders.SKIN[i][j]==EInvalidIndex)
				Shaders.SKIN[i][j] = LoadShader("palskin",RenderDevice::EShaderVertex,&s1);
#else
			Shaders.SKIN[i][j] = LoadShader(name, RenderDevice::EShaderVertex, &StreamDef(MeshVertex::ShaderStreamNTU,VertexWeights::ShaderStream));

			if (Shaders.SKIN[i][j]==EInvalidIndex)
			{
				Shaders.SKIN[i][j] = LoadShader("palskin",RenderDevice::EShaderVertex,&StreamDef(MeshVertex::ShaderStreamNTU,VertexWeights::ShaderStream));
			}
#endif
		}
	}

    // parse config file and build a list of texture paths
	TexturePath = "../data/textures/";

    Texture* texture;
#ifdef TRANSGAMING
    char tmpFilename[MAX_PATH];
#endif

    m_setTag("Texture");
	texture = new Texture();
	texture->Flags	  = Texture::ETexInvalid|Texture::ETexNoMipMaps|Texture::ETexSystem;

#ifdef TRANSGAMING
# ifdef DEBUG
    if (TextureOnDisk("special/notex-debug", tmpFilename, MAX_PATH)){
        texture->FileName = tmpFilename;
		texture->Load(tmpFilename);
    }
# else
    if (TextureOnDisk("special/notex", tmpFilename, MAX_PATH)){
        texture->FileName = tmpFilename;
		texture->Load(tmpFilename);
    }
# endif
#else
	if (TextureOnDisk("special/notex", texture->FileName))
		texture->Load(texture->FileName.AsChar());
#endif
	if (texture->isDataValid())
		dev->CreateTexture(texture);
	else
		texture->DeviceId = EInvalidIndex;

	TexHash["notex"] = texture;
	Textures.NoTex = texture;

    m_setTag("Texture");
	texture = new Texture();
	texture->Flags	  = Texture::ETexInvalid|Texture::ETexNoMipMaps|Texture::ETexSystem;
#ifdef TRANSGAMING
    if (TextureOnDisk("special/white", tmpFilename, MAX_PATH)){
        texture->FileName = tmpFilename;
		texture->Load(tmpFilename);
    }
#else
	if (TextureOnDisk("special/white", texture->FileName))
		texture->Load(texture->FileName.AsChar());
#endif
	if (texture->isDataValid())
		dev->CreateTexture(texture);
	else
		texture->DeviceId = EInvalidIndex;

	TexHash["white"] = texture;
	Textures.White = texture;

    m_setTag("Texture");
	texture = new Texture();
	texture->Flags	  = Texture::ETexInvalid|Texture::ETexNoMipMaps|Texture::ETexSystem;
#ifdef TRANSGAMING
    if (TextureOnDisk("special/gray", tmpFilename, MAX_PATH)){
        texture->FileName = tmpFilename;
		texture->Load(tmpFilename);
    }
#else
	if (TextureOnDisk("special/gray", texture->FileName))
		texture->Load(texture->FileName.AsChar());
#endif
	if (texture->isDataValid())
		dev->CreateTexture(texture);
	else
		texture->DeviceId = EInvalidIndex;

	TexHash["gray"] = texture;
	Textures.Gray = texture;

    m_setTag("Texture");
	texture = new Texture();
	texture->Flags	  = Texture::ETexInvalid|Texture::ETexNoMipMaps|Texture::ETexSystem;
#ifdef TRANSGAMING
    if (TextureOnDisk("special/black", tmpFilename, MAX_PATH)){
        texture->FileName = tmpFilename;
		texture->Load(tmpFilename);
    }
#else
	if (TextureOnDisk("special/black", texture->FileName))
		texture->Load(texture->FileName.AsChar());
#endif
	if (texture->isDataValid())
		dev->CreateTexture(texture);
	else
		texture->DeviceId = EInvalidIndex;

	TexHash["black"] = texture;
	Textures.Black = texture;


	// null material
    m_setTag("TextureMaterial");
	Material* mat = new Material();
	mat->PassCount = 0;
	MatHash["null"] = mat;
	NullMat = mat;


	Textures.AttenMap = CreateTexture("Special/atten.tga",Texture::ETexNoMipMaps|Texture::ETexSystem|Texture::ETexNoLOD);
    Textures.IllumMap = CreateTexture("Special/illum.tga",Texture::ETexNoMipMaps|Texture::ETexSystem|Texture::ETexNoLOD);
	Textures.Flashlight = CreateTexture("Special/flashlight",Texture::ETexNoMipMaps|Texture::ETexSystem|Texture::ETexNoLOD);
	Textures.FlashFalloff = CreateTexture("Special/flashfalloff",Texture::ETexNoMipMaps|Texture::ETexSystem|Texture::ETexNoLOD);
	Textures.ShadowFalloff = CreateTexture("Special/shadowfalloff",Texture::ETexNoMipMaps|Texture::ETexSystem|Texture::ETexNoLOD);

#ifndef TRANSGAMING
    m_setTag("TextureCubeMap");
	texture = new Texture(Texture::ETexNormCubeMap);
	texture->Width = 128;
	dev->CreateTexture(texture);
    TexHash["normcube"] = texture;
	Textures.NormCube = texture;
#endif

    //create a 16x16 random noise texture
	CreateWarpTexture();


	LoadEffects();
	LoadScripts();
	ReleaseEffects();

#ifndef TRANSGAMING
#ifdef DEBUG
	MLog.Print(" ================ MATERIAL LIST =====================\n");
	for (int i=0; i<MatHash.Size(); ++i)
	{
		MLog.Print("Material[%d]: %s\n",i,MatHash.GetPair(i).Key.AsChar());
	}
#endif
#endif

	SetTexFiltering();

    return TRUE;
}

void MaterialSystem::ReleaseEffects()
{
    for(int i=0; i<FXList.Size(); ++i)
	{
		FXEntry* Fe = &FXList[i];
        delete Fe->FX;
	}

	FXList.ClearArray();
}

int MaterialSystem::CreateShader(const char* name, int type)
{
#if 0
	if (type == RenderDevice::EShaderVertex)
	{
		return LoadShader
		(
			(String(name)+String(".vso")).AsChar(),
			RenderDevice::EShaderVertex,
			&StreamDef(MeshVertex::ShaderStreamNTU)
		);
	}

	return 0;
#endif

#if 1
# ifdef TRANSGAMING
    const int   BUFSIZE = 512;
    char        buf[BUFSIZE];
    int         result;
    StreamDef   sd(MeshVertex::ShaderStreamNTU);


    snprintf(buf, BUFSIZE, "%s.%s", name, (type == RenderDevice::EShaderVertex) ? "vso" : "pso");

    result = LoadShader(buf, type, (type == RenderDevice::EShaderVertex ? &sd : 0));    


	return result;
# else
	return LoadShader
	(
		(String(name) + ((type == RenderDevice::EShaderVertex) ? String(".vso") : String(".pso"))).AsChar(),
		type,
		(type == RenderDevice::EShaderVertex ? &StreamDef(MeshVertex::ShaderStreamNTU) : 0)
	);
# endif
#endif
}

Material* MaterialSystem::CreateMaterial(const char* name)
{
    guard(MaterialSystem::CreateMaterial);
    Assert(name);
    //TRACE("MaterialSystem::CreateMaterial(): creating the material named '%s'\n", name);

    Material* material;
    Material** ptr = MatHash.Find(name);
    if (ptr){
        //TRACE("MaterialSystem::CreateMaterial(): material already exists!\n");
        material = (*ptr);
    }
    else
    {
        m_setTag("TextureMaterial");
        material = new Material();
        MatHash[name] = material;
    }

	material->RefCount++;
    return material;
    unguard;
}

Material* MaterialSystem::FindMaterial(const char* name) const
{
    guard(MaterialSystem::FindMaterial);
    Assert(name);
    //TRACE("MaterialSystem::FindMaterial(): searching for the material '%s'\n", name);

    Material** ptr = MatHash.Find(name);
    if (ptr)
	{
        Material* material = (*ptr);
		material->RefCount++;
        //TRACE("MaterialSystem::FindMaterial(): found it!\n");
	    return material;
	}
    else
    {
		return NULL;
    }

    unguard;
}

FXEntry* MaterialSystem::FindEffect(const char* name) const
{
	Assert(name);
	FXEntry** ptr = FXHash.Find(name);
	if (ptr)
	{
		FXEntry* Fe = (*ptr);
		return Fe;
	}

	return NULL;
}	

Texture* MaterialSystem::FindTexture(const char* name) const
{
    char tmpName[MAX_PATH];


    strncpy(tmpName, name, MAX_PATH);
    strlwr(tmpName);

	Texture** ptr = TexHash.Find(tmpName);  //String(name).ToLower().AsChar());
	return ptr ? *ptr : NULL;
}

Texture* MaterialSystem::ReplaceTexture(const char* oldTexName, const char* newTexName)
{
	Texture* texture = FindTexture(oldTexName);
	if (!texture)
		return NULL;

	if (texture->NumFrames)
		return NULL;

#ifdef TRANSGAMING
	char newNameOnDisk[MAX_PATH];

    if (!TextureOnDisk(newTexName, newNameOnDisk, MAX_PATH))
		return NULL;

	if (texture->FileName.EqualsNoCase(newNameOnDisk))
		return texture;
#else
	String newNameOnDisk;

    if (!TextureOnDisk(newTexName,newNameOnDisk))
		return NULL;

	if (texture->FileName.EqualsNoCase(newNameOnDisk))
		return texture;
#endif

	RenderDevice* dev = GEngine->Video->RenderDev;

    TRACE("MaterialSystem::ReplaceTexture(): replacing the texture '%s' with '%s'\n", oldTexName, newTexName);

	dev->ReleaseResource(RenderDevice::EResTexture,&texture->DeviceId,1);
	texture->ReleaseData();
	texture->FileName = newNameOnDisk;
	texture->Load(texture->FileName.AsChar());
	dev->CreateTexture(texture);

	return texture;
}



#ifdef TRANSGAMING
bool MaterialSystem::TextureOnDiskExt(const char *name, char *retname, size_t len) const
#else
bool MaterialSystem::TextureOnDiskExt(const char *name, String &retname) const
#endif
{
#if defined(TRANSGAMING_PSP)
    snprintf(retname, len, "%s%s.gim", TexturePath.AsChar(), name);
    strlwr(retname);

    if (GFileMan.FileExist(retname))
        return true;

    else
        retname[strlen(retname) - 4] = 0;
#elif defined(TRANSGAMING_PS2)
    snprintf(retname, len, "%s%s.x", TexturePath.AsChar(), name);
    strlwr(retname);

    if (GFileMan.FileExist(retname))
        return true;

    else
        retname[strlen(retname) - 4] = 0;
#else
    const String ext[4]={ ".dds", ".tga", ".bmp" };
    for (int i=0; i<3; ++i) 
    {
        retname = TexturePath + name + ext[i];
		retname.ToLower();
        if (GFileMan.FileExist(retname.AsChar()))
            return true;
    }

    retname = TexturePath;
	retname += name;
#endif
    return false;
}

#ifdef TRANSGAMING
bool MaterialSystem::TextureOnDisk(const char *name, char *retname, size_t len) const
{
    const char *    baseName;
    char            buf[MAX_PATH];
    bool            exist = false;


    snprintf(buf, MAX_PATH, "%s%s", TexturePath.AsChar(), name);

    exist = TextureOnDiskExt(name, retname, len);

    // additional searching this texture in items, models and level paths
    baseName = String::GetFilename(name);

    // items
    if (!exist){
        snprintf(buf, MAX_PATH, "items/%s", baseName);
        exist = TextureOnDiskExt(buf, retname, len);    
    }
    // models
    if (!exist){
        snprintf(buf, MAX_PATH, "models/%s", baseName);
        exist = TextureOnDiskExt(buf, retname, len);    
    }
    // current level
    if (!exist && GEngine->pcfWorld && !GEngine->pcfWorld->Name.IsNull()){
        snprintf(buf, MAX_PATH, "levels/%s/%s", GEngine->pcfWorld->Name.AsChar(), baseName);
        //TRACE("MaterialSystem::TextureOnDisk(): checking for the texture '%s%s.gim'\n", TexturePath.AsChar(), buf);
        exist = TextureOnDiskExt(buf, retname, len);
    }

	return exist;
}
#else
bool MaterialSystem::TextureOnDisk(const char *name, String& retname) const
{
    char    buf[MAX_PATH];
    bool    exist = false;
    retname = TexturePath + name;

    exist = TextureOnDiskExt(name,retname);

    // additional searching this texture in items, models and level paths
    String basename = String::BaseName(name);
    // items
    if (!exist){
        snprintf(buf, MAX_PATH, "items/%s", basename.AsChar());
        exist = TextureOnDiskExt(buf, retname);    
    }
    // models
    if (!exist){
        snprintf(buf, MAX_PATH, "models/%s", basename.AsChar());
        exist = TextureOnDiskExt(buf, retname);    
    }
    // current level
    if (!exist && GEngine->World && GEngine->World->Name.Len()){
        snprintf(buf, MAX_PATH, "levels/%s/%s", GEngine->World->Name.AsChar(), basename.AsChar());
        exist = TextureOnDiskExt(String("levels/") + GEngine->World->Name + String("/") + basename,retname);
    }

	return exist;
}
#endif

bool MaterialSystem::IsTextureOnDisk(const char *name) const
{
#ifdef TRANSGAMING
    char buf[MAX_PATH];

    return TextureOnDisk(name, buf, MAX_PATH);
#else
	String retname;
    return TextureOnDisk(name, retname);
#endif
}

Texture* MaterialSystem::CreateCubeMap(const char* name, DWORD flags, int lodset)
{
	guard(MaterialSystem::CreateCubeMap);
	Assert(name);

    Texture* texture;
    Texture** ptr = TexHash.Find(name);
	if(ptr) 
	{
        texture = (*ptr);

        Assert(*ptr);
        if ((*ptr) == NULL){
            TRACE("MaterialSystem::CreateCubeMap(): ERROR-> a null texture was retrieved from the hash table under the name '%s'!\n", name);

            texture = new Texture();
            TexHash.ChangeValue(name, texture);
        }

		if (texture == Textures.NoTex)
			return texture;
	}
    else
    {
        m_setTag("TextureCubeMap");
        texture = new Texture();
        TexHash[name] = texture;
    }

    //MLog.Print("CreateTexture: '%s', DeviceId=%d\n",name,texture->DeviceId);

	RenderDevice* dev=GEngine->Video->RenderDev;


    //TRACE("MaterialSystem::CreateCubeMap(): checking to see if '%s' needs to be reloaded {deviceId = %d, invalid = %d}\n", name, texture->DeviceId, (texture->Flags & Texture::ETexInvalid) ? 1 : 0);
	if(texture->DeviceId == EInvalidIndex && !(texture->Flags & Texture::ETexInvalid))
    {
        // load texture
        // if FileName set - use it
        // else try dds,tga,bmp and set FileName to one which succeeded

		//texture->DeviceId = EInvalidIndex;

		if(!texture->isDataValid())
		{
#ifdef TRANSGAMING
			if(!texture->FileName.Size())
			{
                char texName[MAX_PATH];
                char tmpFilename[MAX_PATH];


                String::StripExtension(name, texName, MAX_PATH);
                texture->FileName = tmpFilename;

				if (!TextureOnDisk(texName, tmpFilename, MAX_PATH) || !texture->Load(tmpFilename))
				{
#ifdef ANNOYING_GRAPHICS_LOGS
					MLog.Print("WARNING-> texture '%s' was NOT FOUND\n",texture->FileName.AsChar());
#endif
					texture->FileName.Clear();
					texture->Flags|=Texture::ETexInvalid;
					texture->DeviceId=EInvalidIndex;
				}

			}
			else
				texture->Load(texture->FileName.AsChar());
#else
            String texName(String::StripExtension(name));

			if(!texture->FileName.Size())
			{
				if (!TextureOnDisk(texName.AsChar(),texture->FileName) || !texture->Load(texture->FileName.AsChar()))
				{
#ifdef ANNOYING_GRAPHICS_LOGS
					MLog.Print("WARNING-> texture '%s' was NOT FOUND\n",texture->FileName.AsChar());
#endif
					texture->FileName.ClearArray();
					texture->Flags|=Texture::ETexInvalid;
					texture->DeviceId=EInvalidIndex;
				}
			}
			else
            {
				texture->Load(texture->FileName.AsChar());
            }
#endif
		}

		if(texture->isDataValid())
		{
            texture->Flags|=flags;
			texture->Type=Texture::ETexCubeMap;
#ifndef TRANSGAMING
			texture->LevelOfDetail = lodset;
#endif
			dev->CreateTexture(texture);
		}

        // if cannot load, set Invalid=TRUE
        // else, upload to device
        // if failed set Invalid=TRUE
    }

	texture->RefCount++;
    return texture;
	unguard;
}

/*
static int NextPowerOf2(int x)
{
	for (int i=0; i<32; ++i)
	{
		const int a = (1<<i);
		if (a >= x)
			return a;
	}

	return -1;
}
*/

Texture* MaterialSystem::CreateTexture(const char* texturename, DWORD flags, int lodset)
{
	guard(MaterialSystem::CreateTexture);

#ifdef TRANSGAMING
    char name[MAX_PATH];

    String::StripExtension(texturename, name, MAX_PATH);
    strlwr(name);

	String strippedName = name;

    Texture* texture;
    Texture** ptr = TexHash.Find(strippedName);
#else
	String name = String::StripExtension(texturename);
	name.ToLower();

	String strippedName(String::StripExtension(name));

    Texture* texture;
    Texture** ptr = TexHash.Find(strippedName);
#endif

	if(ptr) 
	{
        //TRACE("MaterialSystem::CreateTexture(): found the texture '%s' in the hash table at %p\n", strippedName.AsChar(), *ptr);
        texture = (*ptr);

        Assert(*ptr);
        if ((*ptr) == NULL){
            TRACE("MaterialSystem::CreateTexture(): ERROR-> a null texture was retrieved from the hash table under the name '%s'!\n", name);

            texture = new Texture();
            TexHash.ChangeValue(name, texture);
        }

		if (texture == Textures.NoTex)
			return texture;
	}
    else
    {
        //GLog.Print( "Create texture: %s.\n",texturename );
        //TRACE("MaterialSystem::CreateTexture(): creating a new texture to hold '%s'\n", texturename);
        m_setTag("Texture");
        texture = new Texture();
        texture->Flags |= flags;        
    }

    //MLog.Print("CreateTexture: '%s', DeviceId=%d\n",name,texture->DeviceId);

	RenderDevice* dev=GEngine->Video->RenderDev;
#ifdef TRANSGAMING
    char tmpFilename[MAX_PATH];
#endif

    //TRACE("MaterialSystem::CreateTexture(): checking to see if '%s' needs to be reloaded {deviceId = %d, invalid = %d}\n", texturename, texture->DeviceId, (texture->Flags & Texture::ETexInvalid) ? 1 : 0);
	if (texture->DeviceId == EInvalidIndex && !(texture->Flags & Texture::ETexInvalid))
    {

		bool anim = false;
        // load texture
        // if FileName set - use it
        // else try dds,tga,bmp and set FileName to one which succeeded

		//texture->DeviceId = EInvalidIndex;

		if(!texture->isDataValid())
		{
			if(!texture->FileName.Size() || texture->Flags&Texture::ETexAnim )
			{
#ifdef TRANSGAMING
                char    texName[MAX_PATH];
                size_t  texNameLen;

                
                String::StripExtension(name, texName, MAX_PATH);
                texNameLen = strlen(texName);

                if (texNameLen > 3 && String::StrSuffix(texName, "_00"))
				{
                    //chop off the '_00' at the end of the filename
                    texName[texNameLen - 3] = 0;
                    // try animated frames
                    char name[MAX_PATH];
#else
                String texName(String::StripExtension(name));
				if (texName.Size()>3 && !strcmp(texName.AsChar()+(texName.Size()-4),"_00"))
				{
					texName = texName.SubStr(texName.Size()-4);
                    // try animated frames
                    String name;
#endif
                    //String fileName;
                    int i;

					anim = 1;
					texture->NumFrames = 0;

					for (i=0; i<MAX_TEXTURE_FRAMES; ++i) 
					{
#ifdef TRANSGAMING
                        snprintf(name, MAX_PATH, "%s_%02d", texName, i);

                        if (!TextureOnDisk(name, tmpFilename, MAX_PATH))
                            break;

                        texture->FileName = tmpFilename;
                        if (!texture->Load(tmpFilename))
                            break;
#else
						name = texName + String::Sprintf("_%02d",i);

                        if (!TextureOnDisk(name.AsChar(),texture->FileName))
                            break;

                        if (!texture->Load(texture->FileName.AsChar()))
                            break;
#endif

#ifndef TRANSGAMING
						texture->LevelOfDetail = lodset;
#endif
                        dev->CreateTexture(texture);
                        texture->FrameIds[i] = texture->DeviceId;
                    }

#ifdef TRANSGAMING
                    snprintf(name, MAX_PATH, "%s_00", texName);
					TextureOnDisk(name, tmpFilename, MAX_PATH);
#else
					TextureOnDisk((texName + "_00").AsChar(),texture->FileName); // 
#endif

                    if (i>0) 
					{
						texture->Flags|= Texture::ETexAnim|Texture::ETexNoLOD;
                        texture->DeviceId = texture->FrameIds[0];
                        texture->NumFrames = i;
#ifdef TRANSGAMING
						char *s = strstr(texName, "fps");
#else
						char *s = strstr(texName.AsChar(), "fps");
#endif
						if (s)
						{
							int fps = atoi(s+3);
							if (fps)
							{
								texture->AnimType = Texture::ETexAnimAdvance;
								texture->Fps = fps;
								AniTex.AddUnique(texture);
#ifdef ANNOYING_GRAPHICS_LOGS
							    MLog.Print("anitexture '%s' automatic animated at %d fps\n",texName.AsChar(), fps);
#endif
							}
						}

						/*
						texture->Flags |= Texture::ETexAnim3D;
						texture->Type = Texture::ETexVolume;
						texture->Depth = NextPowerOf2(texture->NumFrames);

						dev->CreateTexture(texture);
						*/
                    }
                    else 
					{
#ifdef ANNOYING_GRAPHICS_LOGS
					    //MLog.Print("WARNING-> anitexture '%s' was NOT FOUND\n",fileName);
#endif
					    texture->FileName.Clear();
					    texture->Flags|=Texture::ETexInvalid;
                        texture->DeviceId=EInvalidIndex;
                        TRACE("Material::CreateTexture(): ERROR-> could not load the animated texture '%s'\n", texturename);
                    }
                }
				
				if (!anim) 
				{
#ifdef TRANSGAMING
					if (!TextureOnDisk(texName, tmpFilename, MAX_PATH) || (texture->FileName = tmpFilename, !texture->Load(tmpFilename)))
#else
					if (!TextureOnDisk(texName.AsChar(),texture->FileName) || !texture->Load(texture->FileName.AsChar()))
#endif
					{
#ifdef ANNOYING_GRAPHICS_LOGS
						MLog.Print("WARNING-> texture '%s' was NOT FOUND\n",texture->FileName.AsChar());
#endif
						if (!ptr) 
						{
							texture->ReleaseData();
							delete texture;
                            TRACE("Material::CreateTexture(): ERROR-> could not load the texture '%s'\n", texturename);
						}
						return Textures.NoTex;
					}
				}
			}
			else
            {
                //MLog.Print("loading (precached filename): %s\n",texture->FileName.AsChar());
				texture->Load(texture->FileName.AsChar());
            }
		}

        //TRACE("MaterialSystem::CreateTexture(): checking texture '%s' for validity {dataFile = %p, anim = %d}\n", texturename, texture->DataFile, anim);
		if(!anim && texture->isDataValid())
		{
            texture->Flags|=flags;
#ifndef TRANSGAMING
			texture->LevelOfDetail = lodset;
#endif
			dev->CreateTexture(texture);
			//MLog.Print("Texture created: [%d] (%d,%d) %d bytes\n",
			//			texture->DeviceId,texture->Width,texture->Height,texture->PixelBytes);
		}

        //TRACE("MaterialSystem::CreateTexture(): deviceId for '%s' is %d\n", texturename, texture->DeviceId);

        // if cannot load, set Invalid=TRUE
        // else, upload to device
        // if failed set Invalid=TRUE
    }

	texture->RefCount++;
    if (!ptr){
		TexHash[strippedName] = texture;
        //TRACE("MaterialSystem::CreateTexture(): hashing the texture '%s' under the key '%s' (len = %d)\n", texture->FileName.AsChar(), strippedName.AsChar(), strippedName.Size());
    }

    return texture;
	unguard;
}

Texture* MaterialSystem::CreateEmptyTexture(const char *name, int width, int height, int bpp, DWORD flags)
{
	guard(MaterialSystem::CreateEmptyTexture);
	Assert(name);

#ifdef TRANSGAMING
    char tmpBuf[MAX_PATH];

    String strippedName = String::StripExtension(name, tmpBuf, MAX_PATH);

    Texture* texture = NULL;
    Texture** ptr = TexHash.Find(strippedName);
#else
	String strippedName(String::StripExtension(name));

    Texture* texture = NULL;
    Texture** ptr = TexHash.Find(strippedName);
#endif
	if(ptr) 
	{
        texture = (*ptr);

        Assert(*ptr);
        if ((*ptr) == NULL){
            TRACE("MaterialSystem::CreateEmptyTexture(): ERROR-> a null texture was retrieved from the hash table under the name '%s'!\n", name);

            if( bpp == 32 ){
                m_setTag("Texture");
			    texture = new Texture( Texture::ETexRegular, Texture::ETexNoMipMaps | Texture::ETexRaw32 );
            }
            else{
                m_setTag("Texture");
			    texture = new Texture( Texture::ETexRegular, Texture::ETexNoMipMaps | Texture::ETexRaw8 );
            }


            TexHash.ChangeValue(name, texture);
        }

		if (texture == Textures.NoTex)
			return texture;
	}
    else
    {
        if( bpp == 32 ){
            m_setTag("Texture");
			texture = new Texture( Texture::ETexRegular, Texture::ETexNoMipMaps | Texture::ETexRaw32 );
        }
        else{
            m_setTag("Texture");
			texture = new Texture( Texture::ETexRegular, Texture::ETexNoMipMaps | Texture::ETexRaw8 );
        }
    }

	RenderDevice *dev = GEngine->Video->RenderDev;

    //TRACE("MaterialSystem::CreateEmptyTexture(): checking to see if '%s' needs to be reloaded {deviceId = %d, invalid = %d}\n", name, texture->DeviceId, (texture->Flags & Texture::ETexInvalid) ? 1 : 0);
	if (texture->DeviceId == EInvalidIndex && !(texture->Flags & Texture::ETexInvalid) )
	{
		texture->Width = width;
		texture->Height = height;
		texture->Size = width * height * bpp / 8;

        //free the old buffer if it had some data!
        if (texture->DataFile)
            free(texture->DataFile);

        m_setTag("Texture");
		texture->DataFile = ( BYTE *)malloc( texture->Size );
		texture->FileName = name;
        texture->Flags |= flags | Texture::ETexDataValid | Texture::ETexDataEmpty;
		memset( texture->DataFile, 0, texture->Size );
		dev->CreateTexture( texture );
	}

	if( texture->DeviceId == EInvalidIndex )
		GLog.Print( "Error creating texture.\n" );

	texture->RefCount++;
    if (!ptr){
		TexHash[strippedName] = texture;
        //TRACE("MaterialSystem::CreateEmptyTexture(): hashing the texture '%s' under the key '%s'\n", texture->FileName.AsChar(), strippedName.AsChar());
    }

	return texture;
	unguard;
}

void MaterialSystem::ReleaseMaterial(Material* material, bool forceRelease)
{
	if (material)
	{
		material->RefCount--;
		if (material->RefCount<=0)
		{
			material->RefCount = 0;
            // TODO: delete material, remove from hash            
		}
	}
}

void MaterialSystem::ReleaseTexture(Texture* texture, bool forceRelease)
{
    if (texture) {
		texture->RefCount--;
        TRACE("MaterialSystem::ReleaseTexture(): releasing texture '%s' {refCount = %d}\n", texture->FileName.AsChar(), texture->RefCount);

		if(forceRelease || (texture->RefCount<=0 && !(texture->Flags & Texture::ETexSystem))) {
			if (FreezeRefs) {
				//GLog.Print("FREEZE: %s\n", texture->FileName.AsChar());
				return;
			}

			FreeTexture(texture);
        }
    }
}

void MaterialSystem::FreeTexture(Texture* texture)
{
	RenderDevice* dev=GEngine->Video->RenderDev;

    if (texture->NumFrames) 
	{
        for (int i=0; i<texture->NumFrames; ++i)
            dev->ReleaseResource(RenderDevice::EResTexture,&texture->FrameIds[i],1);

		if (texture->AnimType != Texture::ETexAnimNone)
		{
			int id = AniTex.Find(texture);

			if (id != EInvalidIndex)
			{
				AniTex.FastRemove(id);
			}
		}
    } 
	else 
	{
        dev->ReleaseResource(RenderDevice::EResTexture,&texture->DeviceId,1);
    }
#ifdef ANNOYING_GRAPHICS_LOGS
    if (texture->FileName.Size() > 0)
        GLog.Print( "Release texture: %s.\n",texture->FileName.AsChar() );
#endif
	texture->ReleaseData();
	/*texture->FileName.Clear();
	texture->Flags &= Texture::ETexAnim; // store anim flag
	texture->DeviceId = 0;
	texture->RefCount = 0;
	texture->AnimType = Texture::ETexAnimNone;//*/

    //EnumerateTextures();

    //create a hash key for the texture so it can be removed from the table
    TRACE("MaterialSystem::FreeTexture(): freeing the texture '%s'\n", texture->FileName.AsChar());
    String  key;


    TexHash.KeyOf(texture, key);
    TRACE("MaterialSystem::FreeTexture(): checking for the key '%s'\n", key.AsChar());

#ifdef DEBUG
    //make sure the correct texture is to be removed from the hash table!
    Texture **tmp = TexHash.Find(key);

    Assert((*tmp) == texture);
    //TRACE("MaterialSystem::FreeTexture(): texture '%s' matches the one stored in the hash table\n", texture->FileName.AsChar());
#endif

    //stupid ass backwards non-functional hash table remove function 
    //needs to have the data members of the hash pair manually reset
    //to invalid values or else access violations could occur when
    //using the [broken] enumeration system.
    int index = TexHash.Remove(key);


    TexHash.GetPair(index).Key = "";
    TexHash.GetPair(index).Value = NULL;
    //TRACE("MaterialSystem::FreeTexture(): removed '%s' from the texture hash table\n", texture->FileName.AsChar());


    delete texture;
    //TRACE("MaterialSystem::FreeTexture(): deleted the texture '%s'\n", key.AsChar());
}

void MaterialSystem::CleanupFrozenStuff()
{
	if (FreezeRefs) {
		for(int i=0; i<TexHash.Size(); ++i) {
			Texture* texture = TexHash.GetPair(i).Value;

            if (texture == NULL)
                continue;
            
            if (!(texture->Flags & Texture::ETexSystem) && (texture->RefCount <= 0)) {
				//GLog.Print("FREEZE cleanup: %s\n", texture->FileName.AsChar());
				FreeTexture(texture);
			}
		}
		
		FreezeRefs = 0;
	}
}

int MaterialSystem::LoadShader(const char* fileName, int type, StreamDef* streams)
{
    // eric-> CreateShader() will always return EInvalidIndex if the second parameter
    //   is non-NULL (which is always the case in this function) => always return
    //   EInvalidIndex from this function instead of doing the file I/O

#ifndef TRANSGAMING
    String path("../Data/Shaders/");
    String file(fileName);

    file = String::StripExtension(file);
	if (type == RenderDevice::EShaderVertex) {
        file += ".vso";
	}
	else if (type == RenderDevice::EShaderPixel) {
        file += ".pso";
	}
    else 
        return EInvalidIndex;

	GFile* f = GFileMan.CreateFileReader((path+file).AsChar());
	if (f) 
	{
        DWORD tmp[2048];
		f->Read(tmp,f->GetSize());
        GFileMan.Close(f);

        RenderDevice* dev = GEngine->Video->RenderDev;
        Assert(dev);
        return streams ? dev->CreateShader(type,tmp,streams->Streams,streams->StreamCount)
                       : dev->CreateShader(type,tmp,0,0);
    }
    else
    {
        GLog.Print("	WARNING-> shader \"%s\" NOT FOUND!\n",fileName);
    }
#endif

    return EInvalidIndex;
}

/*
int MaterialSystem::LoadVertexShader(const char* fileName, const VertexComponent* Decl)
{
    String path("../Data/Shaders/");
    String file(fileName);

    file = String::StripExtension(file) + ".vso";

	GFile* f = GFileMan.CreateFileReader((path+file).AsChar());
	if (f) 
	{
        DWORD tmp[2048];
		f->Read(tmp,f->GetSize());
        GFileMan.Close(f);

        RenderDevice* dev = GEngine->Video->RenderDev;
        Assert(dev);
        return dev->CreateVertexShader(tmp,Decl);
    }
    else
    {
        GLog.Print("	WARNING-> vertex shader \"%s\" NOT FOUND!\n",fileName);
    }

    return EInvalidIndex;
}
*/

RenderEffect* MaterialSystem::LoadEffect(const char* FileName)
{
	//char FullPath[1024] = "../Data/Shaders/Effects/";
	//strcat(FullPath, String::StripExtension(FileName).AsChar());
	//strcat(FullPath, ".fxo");

    // eric-> the CreateEffect() function is dummied out and always returns NULL.
    //   Just return NULL immediately instead.
#ifndef TRANSGAMING
	RenderEffect* FX = NULL;
	GFile* f = GFileMan.CreateFileReader(FileName);
	if (f) 
	{
		const long32 Size = f->GetSize();
        m_setTag("TextureEffect");
        BYTE* EffectData = new BYTE[Size];
		Assert(EffectData);
		f->Read(EffectData,Size);
        GFileMan.Close(f);

        RenderDevice* Device = GEngine->Video->RenderDev;
		FX = Device->CreateEffect(0, EffectData, UINT(Size));

		delete [] EffectData;
	}

	return FX;
#else
    return NULL;
#endif
}

static int GetHWClassForString(const char* s)
{
	if (!stricmp("NV20",s))
		return RenderDevice::EHWClassNV20;
	if (!stricmp("NV25",s))
		return RenderDevice::EHWClassNV25;
	if (!stricmp("R200",s))
		return RenderDevice::EHWClassR200;
	if (!stricmp("NV30",s))
		return RenderDevice::EHWClassNV30;
	if (!stricmp("NV40",s))
		return RenderDevice::EHWClassNV40;


	return RenderDevice::EHWClassGenericTnL;
}

void MaterialSystem::LoadScripts()
{
	DynamicArray<String> files;
#ifdef TRANSGAMING
    const int   BUFSIZE = 512;
    char        buf[BUFSIZE];
    int         len = snprintf(buf, BUFSIZE, "../Data/Shaders/Scripts/");


    strncpy(&buf[len], "*.shader", BUFSIZE - len);
	GFileMan.FindFiles(buf, files);
#else
	String path("../Data/Shaders/Scripts/");
	GFileMan.FindFiles(String(path+"*.shader").AsChar(),files);
#endif

	// first parse default.shader, so that inheritance works
	int idx = EInvalidIndex;
	for (int i=0; i<files.Size(); ++i)
	{
#ifdef TRANSGAMING
		if (!stricmp(files[i].AsChar(), "default.shader")){
			MaterialScript s(this);


            snprintf(&buf[len], BUFSIZE - len, "%s", files[i].AsChar());

			s.Load(buf);
			idx = i;

			break;
		}
#else
		if (files[i] == "default.shader")
		{
			MaterialScript s(this);
			s.Load((path+files[i]).AsChar());
			idx = i;
			break;
		}
#endif
	}

	for (int i=0; i<files.Size(); ++i)
	{
#ifdef TRANSGAMING
        if (i != idx) {
			MaterialScript s(this);


            snprintf(&buf[len], BUFSIZE - len, "%s", files[i].AsChar());
			s.Load(buf);
		}
#else
        if (i != idx) {
			MaterialScript s(this);
			s.Load((path+files[i]).AsChar());
		}
#endif
	}

    // eric-> this loop will never have any side-effects because FX->GetTechniqueString() always
    //   returns 0
#ifndef TRANSGAMING_PSP
	// see which FX materials can be auto-created
    for(int i=0; i<FXList.Size(); ++i)
	{
		FXEntry* Fe = &FXList[i];
		RenderEffect* FX = Fe->FX;

		for (int j=0; j<Fe->Techniques.Size(); ++j)
		{
			const RenderTechnique& T = Fe->Techniques[j];

			if (!T.NumPasses)
				continue;

			String HWClassDef;
			if (FX->GetTechniqueString(T.Index, "HWClass", HWClassDef))
			{
				const int DevHWClass = GEngine->GetRenderDevice()->HWClass;

				Assert(HWClassDef.Size()>0);
				char* s = strstr(HWClassDef.AsChar(),"-");
				if (s)
				{
					s[0] = 0;
					const int MinHWClass = GetHWClassForString(HWClassDef.AsChar());
					const int MaxHWClass = GetHWClassForString(s+1);
					if (DevHWClass < MinHWClass || DevHWClass > MaxHWClass)
					{
						/*MLog.Print("INFO: Skipping FX Tech \"%s\" because DevHWClass (%d) not in [%d-%d]\n", 
									T.Name.AsChar(), DevHWClass, MinHWClass, MaxHWClass);//*/
						continue;
					}						
				}
				else
				{
					const int HWClass = GetHWClassForString(HWClassDef.AsChar());
					if (DevHWClass < HWClass)
					{
						/*MLog.Print("INFO: Skipping FX Tech \"%s\" because DevHWClass (%d) less than %d (required)\n", 
									T.Name.AsChar(), DevHWClass, HWClass);//*/
						continue;
					}
				}
			}

			String BaseMat;
			if (FX->GetTechniqueString(T.Index, "Material", BaseMat))
			{
				BaseMat.ToLower();
				Material* Base = FindMaterial(BaseMat.AsChar());
				if (Base)
				{
                    m_setTag("TextureMaterial");
					Material* M = new Material();
					(*M) = (*Base);

					RenderDevice* dev = GEngine->Video->RenderDev;
					StreamDef def(MeshVertex::ShaderStreamNTU);

					String SDef;
					if (FX->GetTechniqueString(T.Index, "StreamDef", SDef))
					{
						def.Streams[0] = ParseStreamDef(SDef.AsChar());
					}

					const EffectPass& Pass = T.Passes[0];
					M->Passes[0].VertShader[0] = dev->CreateShader(RenderDevice::EShaderVertex,Pass.VertShader,def.Streams,def.StreamCount);
					M->Passes[0].FragShader =    dev->CreateShader(RenderDevice::EShaderPixel,Pass.FragShader,0,0);

					MatHash[T.Name.LowerCase()] = M;
					//MLog.Print("FX material (auto): %s inherits %s\n", T.Name.AsChar(), BaseMat.AsChar());
				}
				else
				{
					//MLog.Print("ERROR-> FX material references %s, which doesn't exist\n", BaseMat.AsChar());
				}
			}
		}
	}
#endif
}

void MaterialSystem::LoadEffects()
{
    // eric-> the LoadEffect() function always returns NULL and all other side-effects
    //   of this function are based upon the return value of LoadEffect() => dummy out
    //   this entire function
#ifndef TRANSGAMING
	DynamicArray<String> Files;
	char FullPath[1024] = "../Data/Shaders/Effects/";
	int len = strlen(FullPath);

	strcat(FullPath,"*.fxo");
	GFileMan.FindFiles(FullPath,Files);

	MLog.Print("============== FX Materials =================\n");
	for (int i=0; i<Files.Size(); ++i)
	{
        TRACE("MaterialSystem::LoadEffects(): loading the effect file '%s'\n", Files[i].AsChar());
		FullPath[len] = 0;
		strcat(FullPath, Files[i].AsChar());

		FXEntry* Fe = FXList.Create();

		RenderEffect* FX = LoadEffect(FullPath);
		MLog.Print("FX: %s\n", FullPath);

		Fe->FX = FX;
# ifdef TRANSGAMING
		if ( FX )
		{
# endif
		DynamicArray<RenderTechnique>& Techniques = Fe->Techniques;

		if (FX->EnumTechniques(Techniques))
		{
			for (int j=0; j<Techniques.Size(); ++j)
			{
				const RenderTechnique& T = Techniques[j];

				if (!T.NumPasses)
					continue;
				
				MLog.Print("\t Tech: %s (%d)\n", T.Name.AsChar(), T.NumPasses);

				// auto created FX materials are NOT added to FX hash
				// to prevent circular dependencies
				String BaseMat;
				if (FX->GetTechniqueString(T.Index, "Material", BaseMat))
				{
					MLog.Print("\t\t using default material: %s\n", BaseMat.AsChar());
				}
				else
					FXHash.Insert(T.Name.LowerCase(), Fe);
			}
		}
# ifdef TRANSGAMING
		}
# endif
	}
#endif
}

#ifndef TRANSGAMING
void MaterialSystem::SetTextureAnimNone(const char* name)
{
	Texture *tex = FindTexture(name);

	if (tex && (tex->AnimType != Texture::ETexAnimNone))
	{
		int id = AniTex.Find(tex);

		if (id != EInvalidIndex)
		{
			AniTex.FastRemove(id);
			tex->AnimType = Texture::ETexAnimNone;
		}
	}
}
#endif

void MaterialSystem::SetTextureAnimAdvance(const char* name, float fps)
{
	Texture *tex = FindTexture(name);

	if (tex)
	{
		AniTex.AddUnique(tex);
		tex->AnimType = Texture::ETexAnimAdvance;
		tex->Fps = fps;
	}
}

#ifdef DEBUG
void MaterialSystem::EnumerateTextures(){
    int count = 0;


    //count up the total number of valid textures
    for (int i=0; i<TexHash.Size(); i++){
        Texture *tex = TexHash.GetPair(i).Value;        

        if (tex == NULL)
            continue;

        count++;
    }


    TRACE("MaterialSystem::EnumerateTextures(): enumerating ~%d textures\n", count);
    count = 0;

    for (int i=0; i<TexHash.Size(); i++){
        Texture *tex = TexHash.GetPair(i).Value;        

        if (tex == NULL)
            continue;

        if (tex->DeviceId != EInvalidIndex)
            TRACE("  %2d) id = %3d, hash = '%s', filename = '%s', refCount = %d, deviceId = %d, size = <%d, %d> (%d bytes), \n", count, i, TexHash.GetPair(i).Key.AsChar(), tex->FileName.AsChar(), tex->RefCount, tex->DeviceId, tex->Width, tex->Height, tex->Size);

        else
            TRACE("* %2d) id = %3d, hash = '%s', filename = '%s', refCount = %d, deviceId = %d, size = <%d, %d> (%d bytes), \n", count, i, TexHash.GetPair(i).Key.AsChar(), tex->FileName.AsChar(), tex->RefCount, tex->DeviceId, tex->Width, tex->Height, tex->Size);


        count++;
    }  
}
#endif

#ifndef TRANSGAMING
void MaterialSystem::PreLoadAll() const
{
	RenderDevice* dev = GEngine->GetRenderDevice();

	for (int i=0; i<TexHash.Size(); i++)
	{
        Texture *tex = TexHash.GetPair(i).Value;

        if (texture == NULL)
            continue;        

        dev->PreLoadTexture(tex);
	}
}
#endif
